/***************************************************************************//**
 * @file
 * @brief app_init.c
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

#include "sl_common.h"
#include "sl_rail_util_init.h"
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
SL_RAIL_DECLARE_FIFO_BUFFER(tx_fifo, SL_TUTORIAL_EVENT_HANDLING_BUFFER_LENGTH);
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization relates to the app.
 *****************************************************************************/
void app_init(void)
{
  // Get RAIL handle, used later by the application
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  sl_rail_status_t status = sl_rail_set_tx_fifo(rail_handle,
                                                tx_fifo,
                                                SL_TUTORIAL_EVENT_HANDLING_BUFFER_LENGTH,
                                                0,
                                                0);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    // Turn led1 ON on set_tx_fifo Error
    sl_led_toggle(&sl_led_led1);
  }
  status = sl_rail_config_events(rail_handle,
                                 SL_RAIL_EVENTS_ALL,
                                 SL_RAIL_EVENTS_TX_COMPLETION
                                 | SL_RAIL_EVENTS_RX_COMPLETION);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    // Turn led1 ON on config_events Error
    sl_led_toggle(&sl_led_led1);
  }
  status = sl_rail_start_rx(rail_handle,
                            SL_TUTORIAL_EVENT_HANDLING_DEFAULT_CHANNEL,
                            NULL);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    // Turn led1 ON on start_rx Error
    sl_led_toggle(&sl_led_led1);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
