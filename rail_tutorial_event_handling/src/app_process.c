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
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "sl_rail_tutorial_event_handling_config.h"

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
static uint8_t payload[SL_TUTORIAL_EVENT_HANDLING_PAYLOAD_LENGTH] =
{ SL_TUTORIAL_EVENT_HANDLING_PAYLOAD_LENGTH - 1, 0x01, 0x02, 0x03, 0x04,
  0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x00 };
static volatile bool send_packet = false;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(void)
{
  if (send_packet) {
    // Get RAIL handle, used later by the application
    sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
      SL_RAIL_UTIL_HANDLE_INST0);
    send_packet = false;
    // Increment the last byte of the payload
    payload[SL_TUTORIAL_EVENT_HANDLING_PAYLOAD_LENGTH - 1]++;
    // If the radio configuration's payload settings doesn't match the
    // SL_TUTORIAL_EVENT_HANDLING_PAYLOAD_LENGTH, the FIFO might get corrupt and
    // a FIFO reset may be required in a production quality code before writing
    // anything to it.
    uint16_t size = sl_rail_write_tx_fifo(rail_handle,
                                          payload,
                                          SL_TUTORIAL_EVENT_HANDLING_PAYLOAD_LENGTH,
                                          false);
    if (size != SL_TUTORIAL_EVENT_HANDLING_PAYLOAD_LENGTH) {
      // Turn led1 ON on WriteTxFifo Error
      sl_led_toggle(&sl_led_led1);
    }
    sl_rail_status_t status = sl_rail_start_tx(rail_handle,
                                               SL_TUTORIAL_EVENT_HANDLING_DEFAULT_CHANNEL,
                                               SL_RAIL_TX_OPTIONS_DEFAULT,
                                               NULL);
    if (status != SL_RAIL_STATUS_NO_ERROR) {
      sl_led_toggle(&sl_led_led1);
    }
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
void sl_rail_util_on_event(sl_rail_handle_t rail_handle,
                           sl_rail_events_t events)
{
  (void) rail_handle;

  if (events & SL_RAIL_EVENTS_TX_COMPLETION) {
    if (events & SL_RAIL_EVENT_TX_PACKET_SENT) {
      sl_led_toggle(&sl_led_led0);
    } else {
      sl_led_toggle(&sl_led_led1); // all other events in
                                   // SL_RAIL_EVENTS_TX_COMPLETION are errors
    }
  }
  if (events & SL_RAIL_EVENTS_RX_COMPLETION) {
    if (events & SL_RAIL_EVENT_RX_PACKET_RECEIVED) {
      sl_led_toggle(&sl_led_led0);
    } else {
      sl_led_toggle(&sl_led_led1); // all other events in
                                   // SL_RAIL_EVENTS_RX_COMPLETION are errors
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
