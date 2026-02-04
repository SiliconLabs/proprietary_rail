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

#include "em_prs.h"

#include "sl_common.h"
#include "sl_rail_time_synchronization_config.h"
#include "sl_radioprs_config_framedet_prs.h"
#include "sl_radioprs_config_syncsent_prs.h"

#include "sl_rail.h"
#include "sl_rail_util_init.h"
#include "app_log.h"
#include "app_init.h"
#include "app_process.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define DUMMY_DATA       0x96
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
SL_RAIL_DECLARE_FIFO_BUFFER(tx_fifo, SL_TIME_SYNCHRONIZATION_TX_FIFO_SIZE);
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
void app_init(void)
{
  sl_rail_status_t status;
  // Get RAIL handle, used later by the application
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Fill up dummy payload to avoid clock synchronization failure
  // if there is no symbol change within the payload
  memset(tx_fifo, DUMMY_DATA, SL_TIME_SYNCHRONIZATION_TX_FIFO_SIZE);

  // Calculate the position of the next second in microseconds
  // and start a debug timer
  sl_rail_time_t next_second =
    ((sl_rail_get_time(rail_handle) / ONE_SECOND_IN_US) + 1)
    * ONE_SECOND_IN_US;
  sl_rail_set_timer(rail_handle,
                    next_second,
                    SL_RAIL_TIME_ABSOLUTE,
                    timer_callback);

  // Start the Rx
  status = sl_rail_start_rx(rail_handle,
                            SL_TIME_SYNCHRONIZATION_DEFAULT_CHANNEL,
                            NULL);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    app_log_error("sl_rail_start_rx() failed!\n");
  }

  // Combine SYNCSENT and FRAMEDET PRS channels to a single PRS channel
  PRS_Combine(RADIOPRS_SYNCSENT_PRS_CHANNEL,
              RADIOPRS_FRAMEDET_PRS_CHANNEL,
              prsLogic_A_OR_B);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
