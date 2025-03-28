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
#include "rail.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "sl_rail_tutorial_downloading_messages_config.h"
#include "app_log.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static uint8_t payload[SL_TUTORIAL_DOWNLOADING_MESSAGES_PAYLOAD_LENGTH] =
{ SL_TUTORIAL_DOWNLOADING_MESSAGES_PAYLOAD_LENGTH - 1, 0x01, 0x02, 0x03, 0x04,
  0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x00 };

static volatile bool send_packet = false;

// This buffer is not the RAIL Rx FIFO, but an application buffer.
static uint8_t rx_buffer[SL_TUTORIAL_DOWNLOADING_MESSAGES_BUFFER_LENGTH];
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  static RAIL_RxPacketHandle_t packet_handle;
  static RAIL_RxPacketInfo_t packet_info;
  static RAIL_RxPacketDetails_t packet_details;

  if (send_packet) {
    send_packet = false;
    // Increment the last byte of the payload
    payload[SL_TUTORIAL_DOWNLOADING_MESSAGES_PAYLOAD_LENGTH - 1]++;
    // If the radio configuration's payload settings doesn't match the
    // SL_TUTORIAL_DOWNLOADING_MESSAGES_PAYLOAD_LENGTH, the FIFO might get
    // corrupt and a FIFO reset may be required in a production quality code
    // before writing anything to it.
    uint16_t size = RAIL_WriteTxFifo(rail_handle,
                                     payload,
                                     SL_TUTORIAL_DOWNLOADING_MESSAGES_PAYLOAD_LENGTH,
                                     false);
    if (size != SL_TUTORIAL_DOWNLOADING_MESSAGES_PAYLOAD_LENGTH) {
      // Turn led1 ON on WriteTxFifo Error
      sl_led_toggle(&sl_led_led1);
    }
    RAIL_Status_t status = RAIL_StartTx(rail_handle,
                                        SL_TUTORIAL_DOWNLOADING_MESSAGES_DEFAULT_CHANNEL,
                                        RAIL_TX_OPTIONS_DEFAULT,
                                        NULL);
    if (status != RAIL_STATUS_NO_ERROR) {
      sl_led_toggle(&sl_led_led1);
    }
  }

  packet_handle = RAIL_GetRxPacketInfo(rail_handle,
                                       RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE,
                                       &packet_info);
  if (packet_handle != RAIL_RX_PACKET_HANDLE_INVALID) {
    RAIL_CopyRxPacket(rx_buffer, &packet_info);
    RAIL_Status_t status =
      RAIL_GetRxPacketDetails(rail_handle, packet_handle, &packet_details);
    if (status != RAIL_STATUS_NO_ERROR) {
      sl_led_toggle(&sl_led_led1);
    }
    status = RAIL_ReleaseRxPacket(rail_handle, packet_handle);
    if (status != RAIL_STATUS_NO_ERROR) {
      sl_led_toggle(&sl_led_led1);
    }

    app_log("Rx");
    for (int i = 0; i < packet_info.packetBytes; i++) {
      app_log(" 0x%02X", rx_buffer[i]);
    }
    app_log("; RSSI=%d dBm\n", packet_details.rssi);
  }
}

/******************************************************************************
 * Button callback, called if a Button event occurs
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    send_packet = true;
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  if (events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      sl_led_toggle(&sl_led_led0);
    } else {
      sl_led_toggle(&sl_led_led1); // all other events in
                                   // RAIL_EVENTS_TX_COMPLETION are errors
    }
  }
  if (events & RAIL_EVENTS_RX_COMPLETION) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      if (RAIL_HoldRxPacket(rail_handle) == RAIL_RX_PACKET_HANDLE_INVALID) {
        sl_led_toggle(&sl_led_led1);
      }
      sl_led_toggle(&sl_led_led0);
    } else {
      sl_led_toggle(&sl_led_led1); // all other events in
                                   // RAIL_EVENTS_RX_COMPLETION are errors
    }
  }
  if (events & RAIL_EVENT_CAL_NEEDED) {
    RAIL_Status_t status = RAIL_Calibrate(rail_handle,
                                          NULL,
                                          RAIL_CAL_ALL_PENDING);
    if (status != RAIL_STATUS_NO_ERROR) {
      sl_led_toggle(&sl_led_led1);
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
