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
 *    claim that you wrsote the original software. If you use this software
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

#include "sl_rail_util_init.h"
#include "sl_rail_improved_rfsense_transmitter_config.h"
#include "sl_simple_led_instances.h"
#include "sl_common.h"

#include "pa_curve_types_efr32.h"
#include "pa_conversions_efr32.h"

#include "app_process.h"
#include "app_log.h"
#include "app_assert.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define ONE_SECOND_US 1000000
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
SL_RAIL_DECLARE_FIFO_BUFFER(tx_fifo,
                            SL_IMPROVED_RFSENSE_TRANSMITTER_FIFO_LENGTH) = { 0x55, 0x6F, 0xB1 };
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization relates to the app.
 *****************************************************************************/
void app_init(void)
{
  sl_rail_status_t status;

  // Get RAIL handle, used later by the application
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  uint16_t size = sl_rail_set_fixed_length(rail_handle,
                                           SL_IMPROVED_RFSENSE_TRANSMITTER_PAYLOAD_LENGTH);
  app_assert(size == SL_IMPROVED_RFSENSE_TRANSMITTER_PAYLOAD_LENGTH,
             "sl_rail_set_fixed_length was unable to set the required size.");

  status = sl_rail_set_tx_fifo(rail_handle,
                               tx_fifo,
                               SL_IMPROVED_RFSENSE_TRANSMITTER_FIFO_LENGTH,
                               SL_IMPROVED_RFSENSE_TRANSMITTER_PAYLOAD_LENGTH,
                               0);

  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_set_tx_fifo was unable to set the required size.");

  // Set the Tx power to maximum to achieve maximum range
  status = sl_rail_set_tx_power_dbm(rail_handle,
                                    SL_RAIL_TX_POWER_MAX);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_set_tx_power_dbm return status failed.");

  // Welcome message
  app_log("Improved RF Sense Transmitter example\n");

  status = sl_rail_set_timer(rail_handle, ONE_SECOND_US, SL_RAIL_TIME_DELAY,
                             &sl_rail_cb_timer_expired);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_set_timer return status failed.");
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
