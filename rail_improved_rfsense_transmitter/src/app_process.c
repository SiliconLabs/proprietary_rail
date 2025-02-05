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
#include "sl_rail_improved_rfsense_transmitter_config.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "app_process.h"
#include "app_log.h"
#include "app_assert.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * This function starts a transmission sequence
 *****************************************************************************/
static void start_rfsense_tx(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// A flag indicating that the RAIL timer has elapsed.
volatile bool send_timer_elapsed = false;

// Flag indicating if an error occured during calibration
static volatile bool calibration_error_flag = false;

// Flag indicating if an error occured during transmission
static volatile bool tx_error_flag = false;

// Array containing the channel indexes used to transmit the packets
static uint8_t array_of_channels[] =
  SL_IMPROVED_RFSENSE_TRANSMITTER_ARRAY_OF_CHANNELS;

// This variable indicates the actual active channel's index in
// array_of_channels
static volatile uint32_t active_channel_index = sizeof(array_of_channels);
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  if (calibration_error_flag) {
    calibration_error_flag = false;
    app_log_error("RAIL_Calibrate was unable to perform calibration!");
  }
  if (tx_error_flag) {
    tx_error_flag = false;
    app_log_error("Packet transmission was not successful! Increase TX delay!");
  }
  if (send_timer_elapsed) {
    send_timer_elapsed = false;

    if (active_channel_index < sizeof(array_of_channels)) {
      app_log("Ch: %u\n", array_of_channels[active_channel_index]);
      RAIL_Status_t status = RAIL_StartTx(rail_handle,
                                          array_of_channels[active_channel_index],
                                          RAIL_TX_OPTION_RESEND,
                                          NULL);
      app_assert(status == RAIL_STATUS_NO_ERROR,
                 "RAIL_StartTx return status failed.");
      active_channel_index++;
    } else {
      sl_led_turn_off(&sl_led_led0);
    }
    RAIL_SetTimer(rail_handle,
                  SL_IMPROVED_RFSENSE_TRANSMITTER_TX_DELAY_US,
                  RAIL_TIME_DELAY,
                  &RAILCb_TimerExpired);
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
      calibration_error_flag = true;
    }
  }
  if (events & RAIL_EVENTS_TX_COMPLETION) {
    if (!(events & RAIL_EVENT_TX_PACKET_SENT)) {
      tx_error_flag = true;
    }
  }
}

/******************************************************************************
 * Button callback, called if a Button event occurs
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (handle == &sl_button_btn0) {
      start_rfsense_tx();
    }
  }
}

/******************************************************************************
 * Timer callback, called if RAIL timer has elapsed
 *****************************************************************************/
void RAILCb_TimerExpired(RAIL_Handle_t railHandle)
{
  (void) railHandle;

  send_timer_elapsed = true;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * This function starts a transmission sequence
 *****************************************************************************/
static void start_rfsense_tx(void)
{
  if (active_channel_index == sizeof(array_of_channels)) {
    sl_led_turn_on(&sl_led_led0);
    active_channel_index = 0;
  }
}
