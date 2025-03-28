/**************************************************************************/ /**
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
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sl_common.h"
#include "sl_rail_util_init.h"

#include "app_init.h"
#include "app_assert.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void rf_generator_init(RAIL_Handle_t rail_handle);
static void init_packets();
static void init_sequence();

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
packet_t packets[NUMBER_OF_PACKETS] = { 0 };
packetSequenceElement_t sequence[SEQUENCE_LENGTH];

uint8_t payload[PACKET_LENGTH] = {
  0x0F, 0x0E, 0x11, 0x22, 0x33, 0x44, 0x55, 0x0F,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
};

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
SL_ALIGN(RAIL_FIFO_ALIGNMENT)
static uint8_t tx_fifo[RAIL_FIFO_SIZE]
SL_ATTRIBUTE_ALIGN(RAIL_FIFO_ALIGNMENT);
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle
    = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  rf_generator_init(rail_handle);

  app_log_info("RF Generator\n");

  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static void rf_generator_init(RAIL_Handle_t rail_handle)
{
  uint16_t allocated_tx_fifo_size = 0;
  allocated_tx_fifo_size = RAIL_SetTxFifo(rail_handle,
                                          tx_fifo,
                                          0,
                                          RAIL_FIFO_SIZE);
  app_assert(allocated_tx_fifo_size == RAIL_FIFO_SIZE,
             "RAIL_SetTxFifo() failed to allocate a large enough fifo (%d bytes instead of %d bytes)\n",
             allocated_tx_fifo_size,
             RAIL_FIFO_SIZE);

  init_packets();
  init_sequence();
}

static void init_packets()
{
  for (uint8_t i = 0; i < NUMBER_OF_PACKETS; ++i)
  {
    packets[i].id = i;
    packets[i].length = PACKET_LENGTH;
    memcpy(packets[i].payload, payload, PACKET_LENGTH);
  }
}

static void init_sequence()
{
  for (uint8_t i = 0; i < NUMBER_OF_PACKETS; ++i)
  {
    sequence[i].packet = &packets[i % NUMBER_OF_PACKETS];
    sequence[i].delay = PACKET_DELAY;
  }
}
