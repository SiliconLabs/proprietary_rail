/***************************************************************************//**
 * @file app_init.c
 * @brief Source file containing startup configurations
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided \'as-is\', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 *
 * EXPERIMENTAL QUALITY
 * This code has been minimally tested to ensure that it builds with the
 * specified dependency versions and is suitable as a demonstration for
 * evaluation purposes only.
 * This code will be maintained at the sole discretion of Silicon Labs.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_rail_util_init.h"
#include "app_init.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define RX_FIFO_THRESHOLD (RX_FIFO_SIZE / 4)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
RAIL_DataConfig_t dataConfig = { .rxMethod = FIFO_MODE };
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static uint8_t rx_fifo[RX_FIFO_SIZE];
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  RAIL_Status_t rail_status;
  uint16_t threshold;
  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  threshold = RAIL_SetRxFifoThreshold(rail_handle, RX_FIFO_THRESHOLD);
  app_assert(threshold == RX_FIFO_THRESHOLD);

  dataConfig.rxSource = RX_DATA_SOURCE;
  rail_status = RAIL_ConfigData(rail_handle, &dataConfig);
  app_assert(rail_status == RAIL_STATUS_NO_ERROR);

  return rail_handle;
}

RAIL_Status_t RAILCb_SetupRxFifo(RAIL_Handle_t railHandle)
{
  uint16_t rxFifoSize = RX_FIFO_SIZE;
  RAIL_Status_t status = RAIL_SetRxFifo(railHandle, &rx_fifo[0], &rxFifoSize);
  if (rxFifoSize != RX_FIFO_SIZE) {
    // We set up an incorrect FIFO size
    return RAIL_STATUS_INVALID_PARAMETER;
  }
  if (status == RAIL_STATUS_INVALID_STATE) {
    // Allow failures due to multiprotocol
    return RAIL_STATUS_NO_ERROR;
  }
  return status;
}
