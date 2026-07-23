/***************************************************************************//**
 * @file
 * @brief sl_rail_packet_streamer.c
 * @details Interface to stream packets without gaps in between
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Experimental Quality
 * This code has not been formally tested and is provided as-is. It is not
 * suitable for production environments. In addition, this code will not be
 * maintained and there may be no bug maintenance planned for these resources.
 * Silicon Labs may update projects from time to time.
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_rail_advanced_burst_dc_transmitter_config.h"
#include "sl_rail_packet_streamer.h"
#include "sl_rail.h"
#include "sl_common.h"

#include "app_assert.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/******************************************************************************
 * @def PACKET_STREAMER_REQUIRED_RAIL_EVENTS
 * @brief Required RAIL events for the packet streamer
 *****************************************************************************/
#define PACKET_STREAMER_REQUIRED_RAIL_EVENTS SL_RAIL_EVENT_TX_STARTED \
  | SL_RAIL_EVENT_TX_FIFO_ALMOST_EMPTY | SL_RAIL_EVENTS_TX_COMPLETION \
  | SL_RAIL_EVENT_SCHEDULER_STATUS

/******************************************************************************
 * @def PACKET_STREAMER_IDLE
 * @brief Internal constant to keep track of the packet streamer status in
 * runtime based on the burst counter. This value means that the packet streamer
 * is not sending any packets.
 *****************************************************************************/
#define PACKET_STREAMER_IDLE                 -1

/******************************************************************************
 * @struct packet_info_late_t
 * @brief This struct is used to control the packet streamer during the
 * transmission.
 *****************************************************************************/
typedef struct {
  // The time it takes to send the whole packet in microseconds.
  // It is the length of the data frame or the burst frames combined.
  uint32_t full_packet_time;
  // The length of the preamble used in the packet, in bytes.
  uint32_t actual_preamble_length_byte;
  // The pattern used for the preamble.
  uint32_t long_preamble_pattern;
  // The sync word pre-padded with the preamble pattern, in LSB first format.
  uint32_t padded_syncword;
  // This is to track how many packets we have left to send in the burst mode.
  int32_t burst_counter;
  // This is the time when the Tx starts.
  sl_rail_time_t start_time;
  // this is to track the status of the transmissions. The final value of the Tx
  // status is calculated based on this variable and packet_streamer_tx_status_t
  // values.
  int8_t start_time_accuracy;
} packet_info_late_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief This function writes the preamble and sync word to the FIFO.
 * @param[in, out] continue_write This is true if this function is called for
 * not the first time for the same packet. It is true after the call if we're in
 * the middle of writing the preamble/sync but we ran out of FIFO space.
 * @param[in] rail_handle RAIL handle
 *****************************************************************************/
static void write_preamble_and_sync_to_fifo(bool *continue_write,
                                            sl_rail_handle_t rail_handle);

/******************************************************************************
 * @brief This function loads the FIFO with the packet data.
 * @param[in] rail_handle RAIL handle
 * @return It returns true whether we're in the middle of writing a packet, and
 * thus have more to write, but we're out of FIFO space.
 *****************************************************************************/
static bool write_packet_to_fifo(sl_rail_handle_t rail_handle);

/******************************************************************************
 * @brief This function loads the FIFO with the necessary amount of burst packet
 * data. The packets are put after each other without gaps as a big packet to
 * transmit, so the timing is consistent between the packets in the burst. The
 * function handles preamble, syncword, payload and crc. This is a blocking
 * call.
 * @param[in] first_write This is true if this function is called for the first
 * time for the current packet.
 * @param[in] rail_handle RAIL handle
 *****************************************************************************/
static void load_fifo(bool first_write, sl_rail_handle_t rail_handle);

/******************************************************************************
 * @brief This function sets up the tx, including resetting the FIFO, turns off
 * preamble length and writing the first chunk of data to the FIFO.
 * @param[in] rail_handle RAIL handle
 *****************************************************************************/
static void tx_setup(sl_rail_handle_t rail_handle);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
volatile packet_streamer_info_t *packet_info;

// if the tx API is called from RAIL event handler, this must be volatile
volatile packet_info_late_t packet_info_late = {
  .full_packet_time = 0,
  .burst_counter = PACKET_STREAMER_IDLE,
};
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static uint8_t *payload;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void packet_streamer_init(sl_rail_handle_t rail_handle)
{
  sl_rail_status_t status;

  status = sl_rail_config_events(rail_handle,
                                 PACKET_STREAMER_REQUIRED_RAIL_EVENTS,
                                 PACKET_STREAMER_REQUIRED_RAIL_EVENTS);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_config_events return status failed");

  // Configure FIFO threshold at 10%.
  // When the number of bytes in the transmit FIFO falls below the configured
  // threshold, the packet streamer will be notified to load more data into the
  // FIFO. Note, that this limits the maximum throughput of the packet streamer,
  // as we need to wait for the callback to load more data into the FIFO when
  // the FIFO is almost empty.
  uint16_t threshold = SL_ADVANCED_BURST_DC_TRANSMITTER_BUFFER_LENGTH / 10;
  uint16_t threshold_set =
    sl_rail_set_tx_fifo_threshold(rail_handle, threshold);
  app_assert(threshold_set == threshold,
             "sl_rail_set_tx_fifo_threshold did not set the expected threshold");

  // Turn off preamble (used with burst mode, so we can handle preamble/sync in
  // software)
  status = sl_rail_set_tx_alt_preamble_length(rail_handle, 0);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_set_tx_alt_preamble_length return status failed");
}

void packet_streamer_configure_packet(
  volatile packet_streamer_info_t *packet_configuration)
{
  packet_info = packet_configuration;

  packet_info_late.actual_preamble_length_byte = packet_info->preamble_length
                                                 / 8;
  uint32_t remaining_preamble_bits = packet_info->preamble_length % 8;

  // pre-pad syncword with preamble pattern - in LSB first
  packet_info_late.padded_syncword = packet_info->syncword;
  for (int i = 0;
       i < (32 - packet_info->syncword_len) / packet_info->preamble_pattern_len;
       i++) {
    packet_info_late.padded_syncword <<= packet_info->preamble_pattern_len;
    packet_info_late.padded_syncword |= packet_info->preamble_pattern;
  }
  if (remaining_preamble_bits >= (uint32_t)(32 - packet_info->syncword_len)) {
    remaining_preamble_bits -= 32 - packet_info->syncword_len;
  } else {
    uint8_t remaining_syncbits = 32 - packet_info->syncword_len
                                 - remaining_preamble_bits;
    remaining_preamble_bits = 0;
    packet_info_late.actual_preamble_length_byte -= remaining_syncbits / 8;
    // if there's still preamble bits we're missing, we ignore that. We transmit
    // at most 7 bits more preamble then needed, but never less
  }

  // fill preamble pattern variable with 32b preamble pattern, for faster fifo
  // loads
  packet_info_late.long_preamble_pattern = 0;
  for (int i = 0; i < 32; i += packet_info->preamble_pattern_len) {
    packet_info_late.long_preamble_pattern |= packet_info->preamble_pattern
                                              << i;
  }

  // preamble, 4B sync (pre-padded with preamble), payload with CRC and
  // everything
  packet_info_late.full_packet_time = (uint32_t)packet_info->byte_time
                                      * (packet_info_late.
                                         actual_preamble_length_byte + 4
                                         + packet_info->frame_len);
}

sl_rail_status_t packet_streamer_tx(sl_rail_handle_t rail_handle)
{
  if (packet_info_late.burst_counter != PACKET_STREAMER_IDLE) {
    return SL_RAIL_STATUS_INVALID_STATE;
  }

  packet_info_late.burst_counter = packet_info->repeat + 1;
  tx_setup(rail_handle);
  sl_rail_scheduler_info_t sched_info = {
    .priority = packet_info->priority,
    .slip_time = packet_info->slip_time,
    .transaction_time = packet_info_late.full_packet_time
                        * packet_info_late.burst_counter,
  };
  packet_info_late.start_time = sl_rail_get_time(rail_handle);
  // If the status of the Tx does not change in the future, the status will be
  // TX_ERROR_UNDERFLOW
  packet_info_late.start_time_accuracy = -2;
  // Note: The radio cfg has CRC configured. However, we stop transmitting in
  // FIFO underflow, so CRC will be never added to the tx frame
  return sl_rail_start_tx(rail_handle,
                          packet_info->channel,
                          SL_RAIL_TX_OPTION_ALT_PREAMBLE_LEN,
                          &sched_info);
}

sl_rail_status_t packet_streamer_tx_at(sl_rail_handle_t rail_handle,
                                       sl_rail_time_t at)
{
  if (packet_info_late.burst_counter != PACKET_STREAMER_IDLE) {
    return SL_RAIL_STATUS_INVALID_STATE;
  }

  sl_rail_scheduler_info_t sched_info = {
    .priority = packet_info->priority,
    .slip_time = packet_info->slip_time,
    .transaction_time = packet_info_late.full_packet_time
                        * packet_info_late.burst_counter,
  };
  packet_info_late.start_time = sl_rail_get_time(rail_handle);
  // If the status of the Tx does not change in the future, the status will be
  // TX_ERROR_UNDERFLOW
  packet_info_late.start_time_accuracy = -2;
  packet_info_late.burst_counter = packet_info->repeat + 1;
  // Note: The radio cfg has CRC configured. However, we stop transmitting in
  // FIFO underflow, so CRC will be never added to the tx frame
  sl_rail_scheduled_tx_config_t config = {
    .mode = SL_RAIL_TIME_ABSOLUTE,
    .when = at,
    .tx_during_rx = SL_RAIL_SCHEDULED_TX_DURING_RX_ABORT_TX,
    // should be don't care
  };
  sl_rail_status_t status = sl_rail_start_scheduled_tx(rail_handle,
                                                       packet_info->channel,
                                                       SL_RAIL_TX_OPTION_ALT_PREAMBLE_LEN,
                                                       &config,
                                                       &sched_info);
  if (status == SL_RAIL_STATUS_NO_ERROR) {
    tx_setup(rail_handle);
  }
  return status;
}

void packet_streamer_on_rail_event(sl_rail_handle_t rail_handle,
                                   sl_rail_events_t events)
{
  if (packet_info_late.burst_counter == PACKET_STREAMER_IDLE) {
    return;
  }

  if (events & SL_RAIL_EVENT_TX_STARTED) {
    sl_rail_tx_packet_details_t packet_details =
    { .time_sent.total_packet_bytes = SL_RAIL_TX_STARTED_BYTES };
    sl_rail_status_t error = sl_rail_get_tx_time_preamble_start(rail_handle,
                                                                &packet_details);
    if (error != SL_RAIL_STATUS_NO_ERROR) {
      packet_info_late.start_time = sl_rail_get_time(rail_handle);
      // The status of the Tx will be TX_ERROR_UNKNOWN
      packet_info_late.start_time_accuracy = -1;
    } else {
      packet_info_late.start_time = packet_details.time_sent.packet_time;
      // The status of the Tx will be TX_SUCCESS
      packet_info_late.start_time_accuracy = 0;
    }
  }

  if (events & SL_RAIL_EVENT_TX_FIFO_ALMOST_EMPTY) {
    load_fifo(false, rail_handle);
  }
  if (events & SL_RAIL_EVENTS_TX_COMPLETION) {
    sl_rail_set_fixed_length(rail_handle, SL_RAIL_SET_FIXED_LENGTH_INVALID);
    // switch back to normal length handling.
    // The return value is not useful when called with
    // SL_RAIL_SET_FIXED_LENGTH_INVALID parameter.
    if ((events & SL_RAIL_EVENT_TX_UNDERFLOW)
        && (packet_info_late.burst_counter <= 0)) {
      // we might call APIs from this callback, so restore to idle early on
      uint32_t burst_counter_temp = packet_info_late.burst_counter;
      packet_info_late.burst_counter = PACKET_STREAMER_IDLE;
      bool yield = packet_streamer_cb_tx_done(
        TX_SUCCESS - packet_info_late.start_time_accuracy,
        burst_counter_temp,
        packet_info_late.start_time
        + packet_info_late.full_packet_time
        * (packet_info->repeat + 1)
        + packet_info->time_anchor_after_burst);
      if (yield) {
        sl_rail_status_t status = sl_rail_yield_radio(rail_handle);
        app_assert(status == SL_RAIL_STATUS_NO_ERROR,
                   "sl_rail_yield_radio return status failed");
      }
    } else {
      packet_streamer_tx_status_t status_code = TX_SUCCESS;
      if (events & SL_RAIL_EVENT_TX_ABORTED) {
        status_code = TX_ERROR_ABORTED;
      } else if (events & SL_RAIL_EVENT_TX_UNDERFLOW) {
        status_code = TX_ERROR_UNDERFLOW;
      } else { // the other errors seems highly unlikely. We should handle DMP
               // errors in the actual app though
        status_code = TX_ERROR_UNKNOWN;
      }
      bool yield = packet_streamer_cb_tx_done(status_code,
                                              packet_info_late.burst_counter,
                                              packet_info_late.start_time);
      packet_info_late.burst_counter = PACKET_STREAMER_IDLE;
      if (yield) {
        sl_rail_status_t status = sl_rail_yield_radio(rail_handle);
        app_assert(status == SL_RAIL_STATUS_NO_ERROR,
                   "sl_rail_yield_radio return status failed");
      }
    }
  }
  if (events & SL_RAIL_EVENT_SCHEDULER_STATUS) {
    sl_rail_scheduler_status_t status;
    sl_rail_status_t ret_status = sl_rail_get_scheduler_status(rail_handle,
                                                               &status,
                                                               NULL);
    app_assert(ret_status == SL_RAIL_STATUS_NO_ERROR,
               "sl_rail_get_scheduler_status return status failed");
    if (status
        & (SL_RAIL_SCHEDULER_STATUS_UNSUPPORTED
           | SL_RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED
           | SL_RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL
           | SL_RAIL_SCHEDULER_STATUS_INTERNAL_ERROR)) {
      if (status & SL_RAIL_SCHEDULER_TASK_SINGLE_TX) {
        packet_streamer_tx_status_t status_code = TX_SUCCESS;
        if (status == SL_RAIL_SCHEDULER_SINGLE_TX_INTERRUPTED) {
          // this is the only error case where we likely transmitted some
          status_code = TX_ERROR_ABORTED;
        } else {
          status_code = TX_ERROR_DMP;
        }
        bool yield = packet_streamer_cb_tx_done(status_code,
                                                packet_info_late.burst_counter,
                                                packet_info_late.start_time);
        packet_info_late.burst_counter = PACKET_STREAMER_IDLE;
        if (yield) {
          ret_status = sl_rail_yield_radio(rail_handle);
          app_assert(status == SL_RAIL_STATUS_NO_ERROR,
                     "sl_rail_yield_radio return status failed");
        }
      } else {
        // TODO drop some error code before reset
        // Not sure what happened. We received an unanticipated DMP error code
        app_assert(false, "Unexpected DMP error code received.");
      }
    }
  }
}

SL_WEAK bool packet_streamer_cb_tx_done(packet_streamer_tx_status_t status_code,
                                        int32_t burst_counter,
                                        sl_rail_time_t anchor_time)
{
  (void)status_code;
  (void)burst_counter;
  (void)anchor_time;
  return true;
}

SL_WEAK void packet_streamer_cb_payload_done(uint8_t *payload)
{
  (void)payload;
}

SL_WEAK void packet_streamer_cb_request_payload(uint16_t payload_len,
                                                uint32_t time_until_anchor,
                                                uint8_t * *payload)
{
  (void)payload_len;
  (void)time_until_anchor;
  *payload = NULL;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void write_preamble_and_sync_to_fifo(bool *continue_write,
                                            sl_rail_handle_t rail_handle)
{
  static uint32_t remaining_preamble_bytes = 0;
  static bool sync_written = false;
  if (!(*continue_write)) {
    remaining_preamble_bytes = packet_info_late.actual_preamble_length_byte;
    sync_written = false;
  } else if ((remaining_preamble_bytes == 0) && sync_written) {
    *continue_write = false; // we have nothing to write
    return;
  }
  while (sl_rail_get_tx_fifo_space_available(rail_handle) >= 4
         && remaining_preamble_bytes >= 4) {
    sl_rail_write_tx_fifo(rail_handle,
                          (uint8_t *)&(packet_info_late.long_preamble_pattern),
                          4,
                          false);
    remaining_preamble_bytes -= 4;
  }

  // we only start the last chunk if we have enough room for the remaining
  // preamble and padded sync word
  if (sl_rail_get_tx_fifo_space_available(rail_handle)
      >= remaining_preamble_bytes + 4) {
    uint16_t bytes_written = sl_rail_write_tx_fifo(rail_handle,
                                                   (uint8_t *)&(packet_info_late
                                                                .
                                                                long_preamble_pattern),
                                                   remaining_preamble_bytes,
                                                   false);
    app_assert(bytes_written == remaining_preamble_bytes,
               "sl_rail_write_tx_fifo did not write the expected number of bytes");

    remaining_preamble_bytes = 0;
    bytes_written = sl_rail_write_tx_fifo(rail_handle,
                                          (uint8_t *)&(packet_info_late.
                                                       padded_syncword),
                                          4,
                                          false);
    app_assert(bytes_written == 4,
               "sl_rail_write_tx_fifo did not write the expected number of bytes");
    sync_written = true;
    *continue_write = false;
    return;
  }

  // we didn't finish the write, but we're out of fifo space
  *continue_write = true;
  return;
}

static bool write_packet_to_fifo(sl_rail_handle_t rail_handle)
{
  if (sl_rail_get_tx_fifo_space_available(rail_handle)
      >= packet_info->frame_len) {
    uint32_t time_until_anchor = 0;
    if (packet_info->time_anchor_enabled) {
      time_until_anchor = (packet_info_late.burst_counter - 1)
                          * packet_info_late.full_packet_time
                          + packet_info->time_anchor_after_burst;
    }
    packet_streamer_cb_request_payload(packet_info->frame_len,
                                       time_until_anchor,
                                       &payload);
    uint16_t bytes_written = sl_rail_write_tx_fifo(rail_handle, payload,
                                                   packet_info->frame_len,
                                                   false);
    app_assert(bytes_written == packet_info->frame_len,
               "sl_rail_write_tx_fifo did not write the expected number of bytes");

    return false;
  } else {
    return true;
  }
}

static void load_fifo(bool first_write, sl_rail_handle_t rail_handle)
{
  static bool mid_packet;
  static bool dummy_written;
  if (first_write) {
    mid_packet = false;
    dummy_written = false;
  }
  while (packet_info_late.burst_counter > 0) {
    // write as much preamble as we can
    write_preamble_and_sync_to_fifo(&mid_packet, rail_handle);

    // if we finished, try writing the packet
    if (!mid_packet) {
      mid_packet = write_packet_to_fifo(rail_handle);
    }

    // if that is finished as well, we can continue with the next packet
    if (!mid_packet) {
      packet_info_late.burst_counter--;
      if (packet_info_late.burst_counter > 0) {
        // do not notify the upper layer if we have no more payload request
        // coming
        packet_streamer_cb_payload_done(payload);
      }
    }

    uint16_t remaining_bytes = sl_rail_get_tx_fifo_space_available(rail_handle);

    // we break the loop if we're out of space.
    // For preamble/sync, the worst case
    // is 4B sync + 3B preamble.
    // The preamble can be interrupted every 4B, so the worst case is always
    // sync+remainder
    if ((mid_packet && (remaining_bytes < 7))
        || (!mid_packet && (remaining_bytes < packet_info->frame_len))) {
      // for payload we need the full payload length
      break;
    }
  }
  if ((packet_info_late.burst_counter <= 0) && !dummy_written
      && (sl_rail_get_tx_fifo_space_available(rail_handle) > 0)) {
    // data is complete. Write a sacrificial byte to avoid corruption in the
    // last bit in certain modulations (known chip bug, the hw aborts ASAP,
    // ignoring the propagation delay of the tx chain)
    uint8_t dummy = 0;
    uint16_t bytes_written =
      sl_rail_write_tx_fifo(rail_handle, &dummy, 1, false);
    app_assert(bytes_written == 1,
               "sl_rail_write_tx_fifo did not write the expected number of bytes");

    dummy_written = true;
  }
}

static void tx_setup(sl_rail_handle_t rail_handle)
{
  sl_rail_status_t status = sl_rail_reset_fifo(rail_handle, true, false);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_reset_fifo return status failed");

  uint16_t length_set = sl_rail_set_fixed_length(rail_handle, 0);
  app_assert(length_set == 0,
             "sl_rail_set_fixed_length did not set the expected length");

  load_fifo(true, rail_handle);
}
