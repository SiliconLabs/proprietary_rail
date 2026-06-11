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
#include "sl_common.h"
#include "sl_rail_util_init.h"

#include "app_log.h"
#include "app_assert.h"
#include "app_init.h"
#include "app_process.h"
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
 * The function is used for some basic initialization relates to the app.
 *****************************************************************************/
void app_init(void)
{
  // Get RAIL handle, used later by the application
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  for (uint8_t i = 0; i < SL_IR_CALIBRATION_NUM_PROTOCOLS; i++) {
    app_state.slots[i].values =
      (sl_rail_ir_cal_values_t)SL_RAIL_IR_CAL_VALUES_UNINIT;
  }

  app_log("\nIR calibration example\n");
  app_log("Protocols compiled in   : %hu\n", SL_IR_CALIBRATION_NUM_PROTOCOLS);
  app_log("Active config index     : %hhu\n", app_state.config_index);
  app_log("Active channel          : %hu\n", app_state.channel);
  app_log("auto_ir_cal             : %s\n", auto_ir_cal ? "on" : "off");
  app_log("iterate_all_phys        : %s\n",
          iterate_all_phys ? "on" : "off");
  app_log("perform_ir_cal linked   : %s\n",
          SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL ? "yes" : "no");

  sl_rail_status_t status = sl_rail_start_rx(rail_handle,
                                             app_state.channel,
                                             NULL);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_start_rx failed: 0x%X\n",
             (unsigned)status);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
