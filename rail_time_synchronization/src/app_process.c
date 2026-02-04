/***************************************************************************//**
 * @file
 * @brief app_process.c
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_rail_time_synchronization_config.h"

#include "sl_common.h"
#include "sl_component_catalog.h"
#include "sl_simple_button_instances.h"
#include "sl_cli_instances.h"

#include "app_log.h"
#include "app_process.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define PREAMBLE_AND_SYNCWORD_TOTAL (             \
    SL_TIME_SYNCHRONIZATION_PREABMLE_LENGTH_TOTAL \
    +                                             \
    SL_TIME_SYNCHRONIZATION_SYNCWORD_LENGTH_TOTAL)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// Flag indicating synchronization packet to be send
volatile bool sync_packet_transmit_pend = false;

extern sl_rail_fifo_buffer_align_t tx_fifo[];

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// Flag indicating transmission already started
static volatile bool tx_started = false;

static sl_rail_time_t time_preamble_and_syncword_duration_us;
static sl_rail_time_t time_sync_word_end_position_master_us;
static sl_rail_time_t time_tx_started_position_master_us;
static sl_rail_time_t time_sync_word_end_position_slave_us;
static sl_rail_packet_time_stamp_t packet_timestamp;

static int32_t local_times_difference = 0;

static volatile bool tx_packet_sent = false;
static volatile bool rx_packet_received = false;
static volatile bool tx_packet_failure = false;
static volatile bool rx_packet_failure = false;
static volatile bool calibration_error_flag = false;

static uint8_t rx_payload[SL_TIME_SYNCHRONIZATION_PACKET_LENGTH] = { 0 };

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Pressing a button starts the synchronization process
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (handle == &sl_button_btn0) {
    if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
      sync_packet_transmit_pend = true;
    }
  }
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(void)
{
  sl_rail_status_t status;

  if (calibration_error_flag) {
    app_log_error("Calibration failed!\n");
    calibration_error_flag = false;
  }

  // Wait until user requests synchronization
  if (sync_packet_transmit_pend) {
    // Get RAIL handle, used later by the application
    sl_rail_handle_t rail_handle =
      sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

    app_log_info("Synchronization started.\n");

    // Clear flag
    sync_packet_transmit_pend = false;

    // Load the first part of the payload
    if (sl_rail_set_tx_fifo(rail_handle,
                            tx_fifo,
                            SL_TIME_SYNCHRONIZATION_TX_FIFO_SIZE,
                            SL_TIME_SYNCHRONIZATION_PACKET_OFFSET,
                            0) != SL_RAIL_STATUS_NO_ERROR) {
      app_log_error("sl_rail_set_tx_fifo() failed!\n");
    }

    // Send the first packet of the synchronization process
    status = sl_rail_start_tx(rail_handle,
                              SL_TIME_SYNCHRONIZATION_DEFAULT_CHANNEL,
                              SL_RAIL_TX_OPTIONS_DEFAULT,
                              NULL);
    if (status != SL_RAIL_STATUS_NO_ERROR) {
      app_log_error("sl_rail_start_tx() failed!\n");
    }

    // Changing the channel this value might change; recalculate
    time_preamble_and_syncword_duration_us = (PREAMBLE_AND_SYNCWORD_TOTAL
                                              * ONE_SECOND_IN_US)
                                             / sl_rail_get_bit_rate(rail_handle);

    // Blocking wait until Tx started
    while (!tx_started) {}

    // Clear flag
    tx_started = false;

    // Modify the payload during the transmission
    // WARNING: This should be done before transmitting
    //   SL_TIME_SYNCHRONIZATION_PACKET_OFFSET
    // bytes of the payload!
    sl_rail_write_tx_fifo(rail_handle,
                          (uint8_t *)&time_sync_word_end_position_master_us,
                          4 * sizeof(uint8_t),
                          false);

    // Blocking wait until Tx finished
    // WARNING: Make sure all events of SL_RAIL_EVENTS_TX_COMPLETION are enabled
    // otherwise the while loop may never exit
    while (!tx_packet_sent && !tx_packet_failure) {}

    if (tx_packet_failure) {
      app_log_error("Packet Tx failed!\n");
    }

    if (tx_packet_sent) {
      app_log_info("Synchronization packet was sent.\n");
    }

    // Clear flags
    tx_packet_failure = false;
    tx_packet_sent = false;
  }

  if (rx_packet_received) {
    time_sync_word_end_position_slave_us = packet_timestamp.packet_time;

    // Acquire the timestamp from the received packet
    time_sync_word_end_position_master_us = *(uint32_t *)(rx_payload
                                                          +
                                                          SL_TIME_SYNCHRONIZATION_PACKET_OFFSET);

    // Compensate the propagation delay
    time_sync_word_end_position_master_us +=
      SL_TIME_SYNCHRONIZATION_PROPAGATION_DELAY_US;

    // Calculate the difference of local times marking the time of the
    // synchronization packet's syncword end
    local_times_difference = time_sync_word_end_position_master_us
                             - time_sync_word_end_position_slave_us;

    app_log_info("Synchronization finished successfully.\n");
    // Clear flag
    rx_packet_received = false;
  }

  if (rx_packet_failure) {
    app_log_error("Packet reception failed!\n");

    // Clear flag
    rx_packet_failure = false;
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(sl_rail_handle_t rail_handle,
                           sl_rail_events_t events)
{
  if (events & SL_RAIL_EVENT_TX_STARTED) {
    // Set flag
    tx_started = true;

    sl_rail_tx_packet_details_t packet_details;
    packet_details.time_sent.total_packet_bytes = SL_RAIL_TX_STARTED_BYTES;

    // Get the time when transmission started
    sl_rail_get_tx_time_preamble_start(rail_handle,
                                       &packet_details);

    time_tx_started_position_master_us = packet_details.time_sent.packet_time;
    // Calculate timestamp when syncword transmission ends
    time_sync_word_end_position_master_us = time_tx_started_position_master_us
                                            +
                                            time_preamble_and_syncword_duration_us;
  }

  // Catch all Tx completion events and set flags accordingly
  if (events & SL_RAIL_EVENTS_TX_COMPLETION) {
    if (events & SL_RAIL_EVENT_TX_PACKET_SENT) {
      tx_packet_sent = true;
    } else {
      tx_packet_failure = true;
    }
  }
  // Catch all Rx completion events and set flags accordingly
  if (events & SL_RAIL_EVENTS_RX_COMPLETION) {
    if (events & SL_RAIL_EVENT_RX_PACKET_RECEIVED) {
      rx_packet_received = true;

      // Get the latest received packet details
      sl_rail_rx_packet_info_t rx_packet_info;
      sl_rail_rx_packet_handle_t rx_packet_handle = sl_rail_get_rx_packet_info(
        rail_handle,
        SL_RAIL_RX_PACKET_HANDLE_NEWEST,
        &rx_packet_info);

      sl_rail_rx_packet_details_t rx_packet_details;
      rx_packet_details.time_received.total_packet_bytes =
        rx_packet_info.packet_bytes;
      rx_packet_details.time_received.time_position =
        SL_RAIL_PACKET_TIME_AT_SYNC_END_USED_TOTAL;

      sl_rail_get_rx_packet_details(rail_handle,
                                    rx_packet_handle,
                                    &rx_packet_details);

      // Get the timestamp of the received packet' syncword end
      sl_rail_get_rx_time_sync_word_end(rail_handle, &rx_packet_details);

      // Copy the received packet's payload to the buffer
      sl_rail_copy_rx_packet(rail_handle, rx_payload, &rx_packet_info);

      packet_timestamp = rx_packet_details.time_received;
    } else {
      rx_packet_failure = true;
    }
  }

  if (events & SL_RAIL_EVENT_CAL_NEEDED) {
    if (SL_RAIL_STATUS_NO_ERROR != sl_rail_calibrate(rail_handle,
                                                     NULL,
                                                     SL_RAIL_CAL_ALL_PENDING)) {
      calibration_error_flag = true;
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

void timer_callback(sl_rail_handle_t rail_handle)
{
  sl_rail_time_t current_time = sl_rail_get_time(rail_handle);
  sl_rail_time_t next_local_second_boundary = ((current_time / ONE_SECOND_IN_US)
                                               + 1)
                                              * ONE_SECOND_IN_US;
  sl_rail_time_t next_second_boundary = next_local_second_boundary;
  if (local_times_difference != 0) {
    // Calculate the next second boundary in the remote's time
    sl_rail_time_t next_remote_second_boundary = 0;
    if (local_times_difference < 0) {
      uint32_t unsigned_difference = -local_times_difference;
      next_remote_second_boundary = next_local_second_boundary
                                    + (unsigned_difference % ONE_SECOND_IN_US)
                                    - ONE_SECOND_IN_US
                                    - (
        SL_TIME_SYNCHRONIZATION_PROPAGATION_DELAY_US);
    } else {
      uint32_t unsigned_difference = local_times_difference;
      next_remote_second_boundary = next_local_second_boundary
                                    - (unsigned_difference % ONE_SECOND_IN_US);
    }

    // Choose the sooner of the two boundaries
    next_second_boundary = SL_MIN(next_local_second_boundary,
                                  next_remote_second_boundary);

    if (next_second_boundary < current_time) {
      next_second_boundary = next_local_second_boundary;
    }
  }

  // Set the timer for the next interrupt
  sl_rail_set_timer(rail_handle,
                    next_second_boundary,
                    SL_RAIL_TIME_ABSOLUTE,
                    timer_callback);

  if (local_times_difference != 0) {
    app_log_debug("Local time: %lu us, Remote time %lu us\n",
                  current_time,
                  current_time + local_times_difference);
  } else {
    app_log_debug("Local time: %lu us\n", current_time);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
