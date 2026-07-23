/***************************************************************************//**
 * @file
 * @brief app_init.c
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
#include "sl_rail_util_init.h"
#include "sl_rail_advanced_burst_dc_receiver_config.h"
#include "sl_common.h"
#include "app_log.h"
#include "app_assert.h"
#include "sl_rail_sleepdc.h"
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

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief The function is used for some basic initialization relates to the app.
 *****************************************************************************/
void app_init(void)
{
  sl_rail_status_t status;

  // Get RAIL handle, used later by the application
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  sl_rail_timer_sync_config_t timer_sync_config = SL_RAIL_TIMER_SYNC_DEFAULT;
  status = sl_rail_config_sleep(rail_handle, &timer_sync_config);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_config_sleep return status failed.");

  sl_rail_sleepdc_init();
  sl_status_t status_sleepdc = sl_rail_sleepdc_set_mode(WAKEUP_BURST,
                                                        SL_ADVANCED_BURST_DC_RECEIVER_DEFAULT_CHANNEL);
  app_assert(status_sleepdc == SL_STATUS_OK,
             "sl_rail_sleepdc_set_mode return status failed.");

  // Welcome message
  app_log("Advanced Burst Duty Cycle Receiver example\n"
          "Press PB0 (right) to Enable/Disable sleeping\n"
          "Sleeping is enabled\n");
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
