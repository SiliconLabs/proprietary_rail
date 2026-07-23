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
#include "sl_power_manager.h"
#include "sl_rail_advanced_burst_dc_receiver_config.h"
#include "sl_simple_button_instances.h"
#include "sl_rail_sleepdc.h"
#include "app_process.h"
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
// State variable of application state machine
volatile app_state_t state = S_IDLE;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// Flag indicating if an error occured during calibration
static volatile bool calibration_error_flag = false;

// This is an application buffer that holds received data.
static uint8_t rx_buffer[SL_ADVANCED_BURST_DC_RECEIVER_BUFFER_LENGTH];

// Store the reason to log after interrupt
static volatile sl_rail_sleepdc_event_t callback_reason;

// Flag indicating if rail_sleepdc_cb_wakeup_status_irq was called
static volatile bool wakeup_status_irq_triggered_flag = false;

// Flag indicating if sleeping is enabled
static volatile bool sleep_enabled_flag = true;
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
  if (wakeup_status_irq_triggered_flag) {
    wakeup_status_irq_triggered_flag = false;
    if (callback_reason == SL_RAIL_SLEEPDC_EVENT_DATA_RECEIVED) {
      sl_rail_handle_t rail_handle =
        sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
      sl_rail_rx_packet_info_t packet_info;
      sl_rail_rx_packet_handle_t packet_handle = sl_rail_get_rx_packet_info(
        rail_handle,
        SL_RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE,
        &packet_info);
      if (packet_handle != SL_RAIL_RX_PACKET_HANDLE_INVALID) {
        app_assert(sl_rail_copy_rx_packet(rail_handle,
                                          rx_buffer,
                                          &packet_info) == SL_RAIL_STATUS_NO_ERROR,
                   "sl_rail_copy_rx_packet return status failed.");
        sl_rail_status_t status = sl_rail_release_rx_packet(rail_handle,
                                                            packet_handle);
        app_assert(status == SL_RAIL_STATUS_NO_ERROR,
                   "sl_rail_release_rx_packet return status failed.");

        app_log("Data message was received:\n");
        for (uint16_t i = 0; i < packet_info.packet_bytes; i++) {
          app_log("%#04x ", rx_buffer[i]);
        }
        app_log("\n");
      }
    } else if (callback_reason != SL_RAIL_SLEEPDC_EVENT_WAKEUP) {
      app_log_info(
        "RAIL sleep duty cycle status: %d\n",
        callback_reason);
    } else {
    }
  }
  sl_rail_sleepdc_process();
}

void sl_rail_sleepdc_cb_wakeup_event_irq(sl_rail_sleepdc_event_t reason)
{
  callback_reason = reason;
  wakeup_status_irq_triggered_flag = true;
}

void sl_rail_sleepdc_cb_wakeup_received()
{
  app_log_info("RAIL sleep duty cycle wakeup received\n");
}

void sl_rail_sleepdc_cb_wakeup_aborted(sl_rail_sleepdc_event_t reason)
{
  app_log_info(
    "RAIL sleep duty cycle aborted with error %d, attempting restart\n",
    reason);
  sl_status_t status = sl_rail_sleepdc_set_mode(WAKEUP_BURST,
                                                SL_ADVANCED_BURST_DC_RECEIVER_DEFAULT_CHANNEL);
  app_log_info("RAIL sleep duty cycle started with %ld\n", status);
}

/***************************************************************************//**
 * @brief Check if the MCU can sleep at that time. This function is called (in
 * sl_power_manager_handler.c) when the system is about to go sleeping, with the
 * interrupts disabled. It allows the software to cancel going to sleep in case
 * of a last-minute event occurred (window between the function call and
 * interrupt disable).
 * @return  True, if the system can go to sleep.
 *          False, otherwise.
 * @note  This function is called with the interrupt disabled and it MUST NOT be
 *        re-enabled.
 ******************************************************************************/
bool app_is_ok_to_sleep(void)
{
  return sleep_enabled_flag;
}

/******************************************************************************
 * @brief Button callback, called if a Button event occurs
 * @param[in] handle Pointer to the button instance that triggered the event
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (handle == SL_SIMPLE_BUTTON_INSTANCE(0)) {    // Sleep enable button
      sleep_enabled_flag = !sleep_enabled_flag;
    }
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
  sl_rail_sleepdc_util_on_event(rail_handle, events);
  if (events & SL_RAIL_EVENT_CAL_NEEDED) {
    if (sl_rail_calibrate(rail_handle,
                          NULL,
                          SL_RAIL_CAL_ALL_PENDING) != SL_RAIL_STATUS_NO_ERROR) {
      // Raise the calibration error flag
      calibration_error_flag = true;
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
