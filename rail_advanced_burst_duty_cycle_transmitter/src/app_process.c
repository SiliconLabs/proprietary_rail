/***************************************************************************//**
 * @file
 * @brief app_process.c
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
#include "sl_rail.h"
#include "sl_rail_util_init.h"
#include "sl_rail_advanced_burst_dc_transmitter_config.h"
#include "sl_simple_button_instances.h"
#include "sl_rail_packet_streamer.h"

#include "app_process.h"
#include "app_log.h"
#include "app_assert.h"

#include "em_gpcrc.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief Callback, called if data payload is requested by packet streamer
 * @return the pointer to the data frame to transmit
 *****************************************************************************/
static uint8_t *data_prepare_packet();

/******************************************************************************
 * @brief Callback, called if burst payload is requested by packet streamer
 * @param[in] time_until_anchor Time until the anchor timestamp
 * @return the pointer to the burst frame to transmit
 *****************************************************************************/
static uint8_t *burst_prepare_packet(uint32_t time_until_anchor);

/******************************************************************************
 * @brief Callback, called if transmission is finished by packet streamer
 * @param[in] error_code Error code of the transmission
 *****************************************************************************/
static void app_cb_send_done(sl_status_t error_code);

/******************************************************************************
 * @brief Function to calculate CRC of the given packet
 * @param[in] packet Pointer to the packet data
 * @param[in] packet_len Length of the packet data
 * @return Calculated CRC value
 *****************************************************************************/
static uint16_t calculate_crc(const uint8_t *packet, uint8_t packet_len);

/******************************************************************************
 * @brief This function loads the FIFO with the data packet.
 * @param[in] rail_handle RAIL handle
 *****************************************************************************/
static void load_data_packet_to_fifo(sl_rail_handle_t rail_handle);

/******************************************************************************
 * @brief This function loads the FIFO with the padded sync word.
 * @param[in] rail_handle RAIL handle
 *****************************************************************************/
static void write_sync_to_fifo(sl_rail_handle_t rail_handle);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// This variable is used to configure the rail_packet_streamer during different
// transmission types
volatile packet_streamer_info_t packet_info_streamer_ldc;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// Frame of the burst packets
static burst_frame_t burst_frame = {
  .length = sizeof(burst_frame_t)
            - (sizeof(((burst_frame_t){}).length)
               + sizeof(((burst_frame_t){}).crc)),
};

// Frame of the data packet
static volatile data_frame_t data_frame = {
  .length = sizeof(data_frame_t)
            - (sizeof(((data_frame_t){}).length)
               + sizeof(((data_frame_t){}).crc)),
  .data =
  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f }
};

// Application state variable
static volatile app_state_t state = S_IDLE;

// Flag to indicate to go into transfer mode
static volatile bool start_wakeup_flag = false;

// Flag indicating if an error occured during calibration
static volatile bool calibration_error_flag = false;

// Flag indicating if the transmission has been finished.
static volatile bool send_done_flag = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(void)
{
  if (calibration_error_flag) {
    calibration_error_flag = false;
    app_log_error("sl_rail_calibrate was unable to perform calibration!");
  }
  if (send_done_flag) {
    state = S_IDLE;
    send_done_flag = false;
    app_log_info("Transmission sequence has been finished.\n");
  }
  if (start_wakeup_flag && (state == S_IDLE)) {
    start_wakeup_flag = false;
    state = S_BURST_TX;

    packet_info_streamer_ldc.frame_len = sizeof(burst_frame_t);
    // Calculate the number of burst packets based on the burst duration
    packet_info_streamer_ldc.repeat =
      SL_ADVANCED_BURST_DC_TRANSMITTER_BURST_LEN_MS * 1000
      / ((sizeof(burst_frame_t)
          + packet_info_streamer_ldc.
          syncword_len / 8
          + packet_info_streamer_ldc.
          preamble_length / 8)
         * packet_info_streamer_ldc.byte_time);
    // we count the first packet as the first repeat, so we need to
    // decrease the repeat count by 1
    packet_info_streamer_ldc.repeat--;
    packet_info_streamer_ldc.time_anchor_enabled = true;

    packet_streamer_configure_packet(&packet_info_streamer_ldc);

    sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
      SL_RAIL_UTIL_HANDLE_INST0);

    /*
     * Note that the below call will trigger multiple
     * packet_streamer_cb_request_payload until the FIFO is full with
     * packets. The API will call startTx after the FIFO is full and return
     * only after that.
     */
    app_assert(packet_streamer_tx(rail_handle) == SL_RAIL_STATUS_NO_ERROR,
               "packet_streamer_tx return status failed.");
  }
}

/******************************************************************************
 * @brief RAIL callback, called if a RAIL event occurs
 * @param[in] rail_handle RAIL handle
 * @param[in] events events triggered by RAIL
 *****************************************************************************/
void sl_rail_util_on_event(sl_rail_handle_t rail_handle,
                           sl_rail_events_t events)
{
  if (state == S_BURST_TX) {
    packet_streamer_on_rail_event(rail_handle, events);
  } else {
    if (events & SL_RAIL_EVENTS_TX_COMPLETION) {
      sl_rail_set_fixed_length(rail_handle, SL_RAIL_SET_FIXED_LENGTH_INVALID);
      // switch back to normal length handling.
      // The return value is not useful when called with
      // SL_RAIL_SET_FIXED_LENGTH_INVALID parameter.
      if (events & SL_RAIL_EVENT_TX_PACKET_SENT) {
        app_cb_send_done(SL_STATUS_OK);
      } else {
        if (events & SL_RAIL_EVENT_TX_ABORTED) {
          app_cb_send_done(SL_STATUS_ABORT);
        } else if (events & SL_RAIL_EVENT_TX_UNDERFLOW) {
          app_cb_send_done(SL_STATUS_TRANSMIT_UNDERFLOW);
        } else {
          app_cb_send_done(SL_STATUS_FAIL);
        }
      }
    }
  }

  if (events & SL_RAIL_EVENT_CAL_NEEDED) {
    // This is a Tx app only, but the calibration is present for good practice
    if (sl_rail_calibrate(rail_handle,
                          NULL,
                          SL_RAIL_CAL_ALL_PENDING) != SL_RAIL_STATUS_NO_ERROR) {
      // Raise the calibration error flag
      calibration_error_flag = true;
    }
  }
}

/******************************************************************************
 * @brief Button callback, called if a Button event occurs
 * @param[in] handle Pointer to the button instance that triggered the event
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (handle == SL_SIMPLE_BUTTON_INSTANCE(0)) {    // Send button
      start_wakeup_flag = true;
    }
  }
}

bool packet_streamer_cb_tx_done(packet_streamer_tx_status_t status_code,
                                int32_t burst_counter,
                                sl_rail_time_t anchor_time)
{
  (void)burst_counter;

  if (status_code == TX_SUCCESS) {
    state = S_DATA_TX;
    anchor_time -= SL_ADVANCED_BURST_DC_TRANSMITTER_SCHEDULE_DELAY_US;
    sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
      SL_RAIL_UTIL_HANDLE_INST0);
    sl_rail_scheduled_tx_config_t config = {
      .mode = SL_RAIL_TIME_ABSOLUTE,
      .when = anchor_time,
      .tx_during_rx = SL_RAIL_SCHEDULED_TX_DURING_RX_ABORT_TX,
      // should be don't care
    };
    sl_rail_status_t status = sl_rail_start_scheduled_tx(rail_handle,
                                                         packet_info_streamer_ldc.channel,
                                                         SL_RAIL_TX_OPTIONS_DEFAULT,
                                                         &config,
                                                         NULL);
    if (status == SL_RAIL_STATUS_NO_ERROR) {
      status = sl_rail_reset_fifo(rail_handle, true, false);
      app_assert(status == SL_RAIL_STATUS_NO_ERROR,
                 "sl_rail_reset_fifo return status failed");
      uint8_t sync_bytecount = (packet_info_streamer_ldc.syncword_len + 7) >> 3;
      uint16_t length_set = sl_rail_set_fixed_length(rail_handle,
                                                     sizeof(data_frame)
                                                     + sync_bytecount);
      app_assert(length_set
                 == (sizeof(data_frame) + sync_bytecount),
                 "sl_rail_set_fixed_length did not set the expected length");
      load_data_packet_to_fifo(rail_handle);
      return true;
    } else {
      return false;   // we shouldn't yield the radio at this point. ScheduleTx
                      // should yield itself until scheduled tx
    }
  } else if (status_code == TX_ERROR_ABORTED) {
    app_cb_send_done(SL_STATUS_ABORT);
  } else if (status_code == TX_ERROR_UNDERFLOW) {
    app_cb_send_done(SL_STATUS_TRANSMIT_UNDERFLOW);
  } else {
    app_cb_send_done(SL_STATUS_FAIL);
  }
  return true;
}

void packet_streamer_cb_payload_done(uint8_t *payload)
{
  (void)payload;
  // we ignore this with burst. It is only important when we tx multiple packets
  // with a complex payload
}

void packet_streamer_cb_request_payload(uint16_t payload_len,
                                        uint32_t time_until_anchor,
                                        uint8_t * *payload)
{
  (void)payload_len;
  *payload = burst_prepare_packet(time_until_anchor);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static uint8_t *data_prepare_packet()
{
  data_frame.data[0]++;
  data_frame.crc =
    calculate_crc(((uint8_t *)(&data_frame)) + sizeof(data_frame.length),
                  data_frame.length);
  return (uint8_t *)&data_frame;
}

static uint8_t *burst_prepare_packet(uint32_t time_until_anchor)
{
  burst_frame.time = time_until_anchor;
  burst_frame.crc =
    calculate_crc(((uint8_t *)(&burst_frame)) + sizeof(burst_frame.length),
                  burst_frame.length);

  return (uint8_t *)&burst_frame;
}

static void app_cb_send_done(sl_status_t error_code)
{
  app_assert(error_code == SL_STATUS_OK,
             "app_cb_send_done is called with an error.");
  send_done_flag = true;
}

static uint16_t calculate_crc(const uint8_t *packet, uint8_t packet_len)
{
  uint8_t offset = 0;
  while (packet_len - offset > 3) { // if we have 4B or more left, we use
                                    // GPCRC's 32bit input, as it is faster
    GPCRC_InputU32(GPCRC, *((uint32_t *)(packet + offset)));
    offset += 4;
  }
  while (packet_len - offset > 0) { // for the last 3 or less bytes, we use the
                                    // 8bit input
    GPCRC_InputU8(GPCRC, *(packet + offset));
    offset++;
  }
  return GPCRC_DataReadByteReversed(GPCRC); // This is a fixed CRC option for
                                            // the receiver node
}

static void load_data_packet_to_fifo(sl_rail_handle_t rail_handle)
{
  write_sync_to_fifo(rail_handle);

  // Writing the packet
  uint8_t *payload = data_prepare_packet();
  uint16_t bytes_written = sl_rail_write_tx_fifo(rail_handle, payload,
                                                 sizeof(data_frame),
                                                 false);
  app_assert(bytes_written == sizeof(data_frame),
             "sl_rail_write_tx_fifo did not write the expected number of bytes");
}

static void write_sync_to_fifo(sl_rail_handle_t rail_handle)
{
  // pre-pad syncword with preamble pattern - in LSB first
  uint32_t padded_syncword = packet_info_streamer_ldc.syncword;
  uint8_t target_bytecount = (packet_info_streamer_ldc.syncword_len + 7) >> 3;
  for (int i = 0;
       i
       < ((target_bytecount * 8) - packet_info_streamer_ldc.syncword_len)
       / packet_info_streamer_ldc.preamble_pattern_len;
       i++) {
    padded_syncword <<=
      packet_info_streamer_ldc.preamble_pattern_len;
    padded_syncword |=
      packet_info_streamer_ldc.preamble_pattern;
  }

  uint16_t bytes_written = sl_rail_write_tx_fifo(rail_handle,
                                                 (uint8_t *)&padded_syncword,
                                                 target_bytecount,
                                                 false);
  app_assert(bytes_written == target_bytecount,
             "sl_rail_write_tx_fifo did not write the expected number of bytes");
}
