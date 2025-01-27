/***************************************************************************//**
 * @file
 * @brief app_process.c
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_rail_address_filtering_config.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "app_log.h"
#include "app_assert.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
uint8_t payload[SL_ADDRESS_FILTERING_PAYLOAD_LENGTH] =
{ 0xFA, 0xFB, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE };

/// Match table for address matching is shown below.
///
///  F |      |             F     I     E     L     D     1
///    | ADDR |            0xFB   0xB0   0xB1   0xB2   0xB3
///    |      |          |  D/C | Adr0 | Adr1 | Adr2 | Adr3 |
///  F |------|----------|------|------|------|------|------|
///  I | 0xFA | __ D/C__ |    0 |    0 |    1 |    0 |    1 |
///  E | 0xA0 | __Adr0__ |    1 |    0 |    0 |    1 |    1 |
///  L | 0xA1 | __Adr1__ |    0 |    1 |    0 |    0 |    0 |
///  D | 0xA2 | __Adr2__ |    1 |    0 |    0 |    0 |    0 |
///  0 | 0xA3 | __Adr3__ |    0 |    1 |    1 |    0 |    0 |

// Two dimensional array to store the addresses (4 for each field).
// Addresses are paired by the second index.
// This example only changes the TX payload but not the RX address filter.
const uint8_t addresses[2][5] = { SL_ADDRESS_FILTERING_FIELD_ZERO_ADDRESSES,
                                  SL_ADDRESS_FILTERING_FIELD_ONE_ADDRESSES };
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// Event indicating flags
// Flag indicating if the transmission of a message is finished
static volatile bool tx_complete = false;
// Flag indicating if a message is received
static volatile bool receive_flag = false;
// Flag indicating if an error occured during message reception
static volatile bool receive_error_flag = false;
// Flag indicating if an error occured during calibration
static volatile bool calibration_error_flag = false;
// Flag indicating if a message was filtered out
static volatile bool filter_flag = false;
// Flag indicating if an address change was requested
static volatile bool address_change_pending = false;

// This variable denotes the current address in use for the Tx payload, with the
// range indicating the maximum number of available address variations plus a
// don't-care option (25).
// The index of the first address array is determined by address_number / 5, and
// the second index is address_number % 5.
static volatile uint8_t address_number = 0;

static volatile bool tx_request = false; // go into transfer mode

// This buffer is not the RAIL RX FIFO, but an application buffer.
static uint8_t rx_buffer[SL_ADDRESS_FILTERING_BUFFER_LENGTH];
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  // Refresh the display based on any filter or receive flags raised
  if (filter_flag) {
    filter_flag = false;
    app_log("WRONG address, message was filtered out!\n");
  }
  if (receive_flag) {
    receive_flag = false;
    RAIL_RxPacketInfo_t packet_info;
    RAIL_RxPacketHandle_t packet_handle = RAIL_GetRxPacketInfo(rail_handle,
                                                               RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE,
                                                               &packet_info);
    if (packet_handle != RAIL_RX_PACKET_HANDLE_INVALID) {
      RAIL_CopyRxPacket(rx_buffer, &packet_info);
      RAIL_Status_t status = RAIL_ReleaseRxPacket(rail_handle, packet_handle);
      app_assert(status == RAIL_STATUS_NO_ERROR,
                 "RAIL_ReleaseRxPacket return status failed.");

      app_log("RIGHT address, message was received:\n");
      for (uint8_t i = 0; i < packet_info.packetBytes; i++) {
        app_log("%#04x ", rx_buffer[i]);
      }
      app_log("\n");
    }
  }
  if (receive_error_flag) {
    receive_error_flag = false;
    app_log_error("Rx error occured!");
  }
  if (calibration_error_flag) {
    calibration_error_flag = false;
    app_log_error("RAIL_Calibrate was unable to perform calibration!");
  }
  // If an address change was changed, reconfigure the packet's address fields,
  // the first two bytes
  if (address_change_pending) {
    address_change_pending = false;
    uint8_t i = (uint8_t) address_number / sizeof(addresses[0]);
    uint8_t j = address_number % sizeof(addresses[0]);
    payload[0] = addresses[0][i];
    payload[1] = addresses[1][j];
    app_log(
      "Address changed! New address:\n"
      "field0: %#04x field1: %#04x\n",
      payload[0], payload[1]);
  }
  if (tx_request) {
    tx_request = false;
    RAIL_Status_t status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
    app_assert(status == RAIL_STATUS_NO_ERROR,
               "RAIL_Idle return status failed.");
    // If the radio configuration's payload settings doesn't match the
    // SL_ADDRESS_FILTERING_PAYLOAD_LENGTH, the FIFO might get corrupt and a
    // FIFO reset may be required in a production quality code before
    // writing anything to it.
    uint16_t size = RAIL_WriteTxFifo(rail_handle,
                                     payload,
                                     SL_ADDRESS_FILTERING_PAYLOAD_LENGTH,
                                     false);
    app_assert(size == SL_ADDRESS_FILTERING_PAYLOAD_LENGTH,
               "RAIL_WriteTxFifo was unable to write the requested data.");
    status = RAIL_StartTx(rail_handle,
                          SL_ADDRESS_FILTERING_DEFAULT_CHANNEL,
                          RAIL_TX_OPTIONS_DEFAULT,
                          NULL);
    app_assert(status == RAIL_STATUS_NO_ERROR,
               "RAIL_StartTx was unable to start the transmission.");
    // Wait for TX complete
    while (!tx_complete) {
    }
    app_log("The packet was sent:\n");
    for (uint8_t i = 0; i < SL_ADDRESS_FILTERING_PAYLOAD_LENGTH; i++) {
      app_log("%#04x ", payload[i]);
    }
    app_log("\n");
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  if (events & RAIL_EVENT_CAL_NEEDED) {
    // Calibrate if necessary
    if (RAIL_Calibrate(rail_handle,
                       NULL,
                       RAIL_CAL_ALL_PENDING) != RAIL_STATUS_NO_ERROR) {
      // Raise the calibration error flag
      calibration_error_flag = true;
    }
  }
  if (events & RAIL_EVENTS_RX_COMPLETION) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      if (RAIL_HoldRxPacket(rail_handle) != RAIL_RX_PACKET_HANDLE_INVALID) {
        sl_led_toggle(&sl_led_led1);
        // Raise the receive flag
        receive_flag = true;
      }
    } else if (events & RAIL_EVENT_RX_ADDRESS_FILTERED) {
      // Raise the filter flag
      filter_flag = true;
    } else {
      // Raise the receive error flag
      receive_error_flag = true;
    }
  }
  if (events & RAIL_EVENTS_TX_COMPLETION) {
    sl_led_toggle(&sl_led_led0);
    tx_complete = true;
  }
}

/******************************************************************************
 * Button callback, called if a Button event occurs
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (handle == &sl_button_btn0) {    // Send button
      tx_request = true;
    } else if (handle == &sl_button_btn1) {  // Switch address button
      if (((unsigned int)address_number + 1)
          < (sizeof(addresses[0]) * sizeof(addresses[0]))) {
        address_number++;
      } else {
        address_number = 0;
      }
      address_change_pending = true;
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
