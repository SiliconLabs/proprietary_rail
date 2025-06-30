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

#include "sl_rail_util_init.h"
#include "sl_rail_direct_to_buffer_config.h"
#include "sl_common.h"

#include "app_init.h"
#include "app_log.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define RX_FIFO_THRESHOLD (SL_DIRECT_TO_BUFFER_RX_FIFO_SIZE / 2)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// Rx FIFO allocated for RAIL
SL_ALIGN(RAIL_FIFO_ALIGNMENT)
static uint8_t rx_fifo[SL_DIRECT_TO_BUFFER_RX_FIFO_SIZE]
SL_ATTRIBUTE_ALIGN(RAIL_FIFO_ALIGNMENT);
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  RAIL_Status_t rail_status;

  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Sets Rx fifo threshold for Rx FIFO Almost Full event to the half of
  // the Rx FIFO size
  if (RX_FIFO_THRESHOLD
      != RAIL_SetRxFifoThreshold(rail_handle, RX_FIFO_THRESHOLD)) {
    app_log_warning("Rx FIFO threshold is not configured correctly!\n");
  }

  // Configure data capture method
  // Note: this is a workaround for an issue present if the user
  // wants to configure the data mode in rail_util_init
  RAIL_DataConfig_t data_config = {
    .rxMethod = FIFO_MODE,
    .rxSource = SL_DIRECT_TO_BUFFER_RX_SOURCE,
  };
  rail_status = RAIL_ConfigData(rail_handle, &data_config);

  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("RAIL_ConfigData failed with status %d\n", rail_status);
  }

  // FIFO mode data collection starts with entering Rx state
  rail_status =
    RAIL_StartRx(rail_handle, SL_DIRECT_TO_BUFFER_DEFAULT_CHANNEL, NULL);

  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_StartRx failed with status %d\n", rail_status);
  }

  app_log_info(
    "RAIL Direct to Buffer Example\nPush BTN0 to print data collected\n");

  return rail_handle;
}

RAIL_Status_t RAILCb_SetupRxFifo(RAIL_Handle_t rail_handle)
{
  uint16_t rx_fifo_size = SL_DIRECT_TO_BUFFER_RX_FIFO_SIZE;
  RAIL_Status_t status = RAIL_SetRxFifo(rail_handle, &rx_fifo[0],
                                        &rx_fifo_size);
  if (rx_fifo_size != SL_DIRECT_TO_BUFFER_RX_FIFO_SIZE) {
    // We set up an incorrect FIFO size
    return RAIL_STATUS_INVALID_PARAMETER;
  }
  if (status == RAIL_STATUS_INVALID_STATE) {
    // Allow failures due to multiprotocol
    return RAIL_STATUS_NO_ERROR;
  }
  return status;
}
