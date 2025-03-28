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
#include "sl_rail_address_filtering_config.h"
#include "sl_common.h"

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
extern uint8_t payload[SL_ADDRESS_FILTERING_PAYLOAD_LENGTH];
extern const uint8_t addresses[2][5];
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
SL_ALIGN(RAIL_FIFO_ALIGNMENT)
static uint8_t tx_fifo[SL_ADDRESS_FILTERING_BUFFER_LENGTH]
SL_ATTRIBUTE_ALIGN(RAIL_FIFO_ALIGNMENT);

// Using two address fields, one byte each, with zero offsets (packet should
// start with 1-1 byte address fields)
static const RAIL_AddrConfig_t addr_config =
{ .offsets = { 0, 0 }, .sizes = { 1, 1 },
  .matchTable = SL_ADDRESS_FILTERING_MATCH_TABLE };
// 0000 0000 0110 0000 1000 1011 0011 0100

/// Match table for address matching is shown below.
///
///  F |      |             F     I     E     L     D     1
///    | ADDR |            0xFB   0xB0   0xB1   0xB2   0xB3
///    |      |          |  D/C | Adr0 | Adr1 | Adr2 | Adr3 |
///  F |------|----------|---->-|------|------|------|------|
///  I | 0xFA | __ D/C__ |    0 |    0 |    1 |    0 |    1 |
///  E | 0xA0 | __Adr0__ |    1 |    0 |    0 |    1 |    1 |
///  L | 0xA1 | __Adr1__ |    0 |    1 |    0 |    0 |    0 |
///  D | 0xA2 | __Adr2__ |    1 |    0 |    0 |    0 |    0 |
///  0 | 0xA3 | __Adr3__ |    0 |    1 |    1 |    0 |    0 |
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization relates to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  RAIL_Status_t status;

  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Configuring and setting the address filter
  status = RAIL_ConfigAddressFilter(rail_handle, &addr_config);
  app_assert(status == RAIL_STATUS_NO_ERROR,
             "RAIL_ConfigAddressFilter return status failed.");
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      status =
        RAIL_SetAddressFilterAddress(rail_handle,
                                     i,
                                     j,
                                     &addresses[i][j + 1],
                                     true);
      // The addresses array contains the don't care option, so the index is
      // increased with 1
      app_assert(status == RAIL_STATUS_NO_ERROR,
                 "RAIL_SetAddressFilterAddress return status failed.");
    }
  }
  bool enabled = RAIL_EnableAddressFilter(rail_handle, true);
  app_assert(enabled == false, "Address filter was enabled to start with.");

  // Welcome message
  app_log(
    "Simple address filter example\n"
    "Receive is on.\n"
    "PB1 (left) to CHANGE address\n"
    "PB0 (right) to SEND packet\n"
    "Current address:\n"
    "field0: %#04x field1: %#04x\n",
    payload[0],
    payload[1]);

  uint16_t size = RAIL_SetTxFifo(rail_handle,
                                 tx_fifo,
                                 0,
                                 SL_ADDRESS_FILTERING_BUFFER_LENGTH);
  app_assert(size == SL_ADDRESS_FILTERING_BUFFER_LENGTH,
             "RAIL_SetTxFifo was unable to set the required size.");

  status = RAIL_StartRx(rail_handle,
                        SL_ADDRESS_FILTERING_DEFAULT_CHANNEL,
                        NULL);
  app_assert(status == RAIL_STATUS_NO_ERROR,
             "RAIL_StartRx was unable to start the reception.");

  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
