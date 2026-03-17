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
#include "sl_component_catalog.h"
#include "sl_rail.h"
#include "sl_rail_util_init.h"
#include "sl_simple_button_instances.h"

#include "btl_errorcode.h"
#include "btl_interface.h"

#include "sl_rail_ota_dfu_host_config.h"

#include "app_log.h"
#include "app_process.h"
#include "ota_dfu_host.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
// Segment payload length includes the 2-byte segment ID.
#define SEGMENT_PAYLOAD_LENGTH  (SL_OTA_DFU_HOST_SEGMENT_LENGTH + 2)
#define ACK_PAYLOAD_LENGTH      1

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
extern uint32_t last_segment_id;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

static volatile sl_rail_ota_dfu_host_state_t ota_dfu_state =
  sl_rail_ota_dfu_host_state_idle;

static volatile sl_rail_ota_dfu_host_ack_state_t ack_state =
  sl_rail_ota_dfu_ack_pending;

// Set when a calibration error occurs.
static volatile bool calibration_error_flag = false;

static uint8_t rx_buffer[ACK_PAYLOAD_LENGTH];
static uint8_t tx_payload[SEGMENT_PAYLOAD_LENGTH];

static volatile bool segment_transmitted_flag = false;

static uint16_t segment_id = 0;
static uint8_t retry_counter = 0;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine. Called repeatedly.
 *****************************************************************************/
void app_process_action(void)
{
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  switch (ota_dfu_state) {
    case sl_rail_ota_dfu_host_state_idle:
      // Transition out of idle is triggered by the button handler.
      break;

    case sl_rail_ota_dfu_host_state_prepare_next_segment:
      bootloader_readStorage(SL_OTA_DFU_HOST_DEFAULT_SLOT_ID,
                             segment_id * SL_OTA_DFU_HOST_SEGMENT_LENGTH,
                             &tx_payload[2],
                             SL_OTA_DFU_HOST_SEGMENT_LENGTH);
      tx_payload[0] = (segment_id >> 8) & 0xFF;
      tx_payload[1] = segment_id & 0xFF;
      app_log("Progress: %lu%%\n", (segment_id * 100) / last_segment_id);
      if (segment_id == last_segment_id) {
        // Mark the final segment in the header.
        tx_payload[0] ^= 0x80;
      }
      ota_dfu_state = sl_rail_ota_dfu_host_state_transmit_segment;
      retry_counter = SL_OTA_DFU_HOST_SEGMENT_RETRY_COUNT;
      break;

    case sl_rail_ota_dfu_host_state_transmit_segment:
      if (retry_counter == 0) {
        app_log_warning(
          "Connection to the target was lost. Aborting transfer.\n");
        ota_dfu_state = sl_rail_ota_dfu_host_state_idle;
        break;
      }
      retry_counter--;
      sl_rail_set_fixed_length(rail_handle, SEGMENT_PAYLOAD_LENGTH);
      sl_rail_write_tx_fifo(rail_handle,
                            tx_payload,
                            SEGMENT_PAYLOAD_LENGTH,
                            true);
      sl_rail_start_tx(rail_handle,
                       SL_OTA_DFU_HOST_DEFAULT_CHANNEL,
                       SL_RAIL_TX_OPTION_WAIT_FOR_ACK,
                       NULL);
      ota_dfu_state = sl_rail_ota_dfu_host_state_transmit_segment_pending;
      break;

    case sl_rail_ota_dfu_host_state_transmit_segment_pending:
      if (segment_transmitted_flag) {
        segment_transmitted_flag = false;
        ota_dfu_state = sl_rail_ota_dfu_host_state_transmit_wait_ack;
        sl_rail_set_fixed_length(rail_handle, ACK_PAYLOAD_LENGTH);
      }
      break;

    case sl_rail_ota_dfu_host_state_transmit_wait_ack:

      switch (ack_state) {
        case sl_rail_ota_dfu_ack_pending:
          // Wait for the ACK.
          break;

        case sl_rail_ota_dfu_ack_ok:
          ack_state = sl_rail_ota_dfu_ack_pending;
          if (segment_id == last_segment_id) {
            app_log("Image transfer completed.\n");
            ota_dfu_state = sl_rail_ota_dfu_host_state_idle;
          } else {
            ota_dfu_state = sl_rail_ota_dfu_host_state_prepare_next_segment;
            segment_id++;
          }
          break;

        case sl_rail_ota_dfu_ack_fail:
          ack_state = sl_rail_ota_dfu_ack_pending;
          app_log("ACK failure. Resending segment.\n");
          ota_dfu_state = sl_rail_ota_dfu_host_state_transmit_segment;
          break;

        case sl_rail_ota_dfu_ack_timeout:
          ack_state = sl_rail_ota_dfu_ack_pending;
          app_log("ACK timeout. Resending segment.\n");
          ota_dfu_state = sl_rail_ota_dfu_host_state_transmit_segment;
          break;

        default:
          break;
      }
      break;

    default:
      break;
  }

  if (calibration_error_flag) {
    calibration_error_flag = false;
    app_log_warning("sl_rail_calibrate was unable to perform calibration!");
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(sl_rail_handle_t rail_handle,
                           sl_rail_events_t events)
{
  if (events & SL_RAIL_EVENTS_RX_COMPLETION) {
    if (events & SL_RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Read the single-byte payload directly in the event handler.
      sl_rail_rx_packet_info_t packet_info;
      sl_rail_rx_packet_handle_t packet_handle;
      packet_handle = sl_rail_get_rx_packet_info(rail_handle,
                                                 SL_RAIL_RX_PACKET_HANDLE_NEWEST,
                                                 &packet_info);
      if (packet_handle != SL_RAIL_RX_PACKET_HANDLE_INVALID) {
        sl_rail_copy_rx_packet(rail_handle, rx_buffer, &packet_info);
        if (rx_buffer[0] & SL_OTA_DFU_HOST_ACK_OK) {
          ack_state = sl_rail_ota_dfu_ack_ok;
        } else {
          ack_state = sl_rail_ota_dfu_ack_fail;
        }
      } else {
        ack_state = sl_rail_ota_dfu_ack_fail;
      }
    } else {
      ack_state = sl_rail_ota_dfu_ack_fail;
    }
  }

  if (events & SL_RAIL_EVENT_RX_ACK_TIMEOUT) {
    ack_state = sl_rail_ota_dfu_ack_timeout;
  }

  if (events & SL_RAIL_EVENTS_TX_COMPLETION) {
    segment_transmitted_flag = true;
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

/******************************************************************************
 * Button callback. Called when a button event occurs.
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if ((sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)
      && (ota_dfu_state == sl_rail_ota_dfu_host_state_idle)) {
    segment_id = 0;
    ota_dfu_state = sl_rail_ota_dfu_host_state_prepare_next_segment;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
