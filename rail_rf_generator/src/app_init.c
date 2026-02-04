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

#include "app_init.h"
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
// These packets are used as default ones in the application
packet_t packets[SL_RF_GENERATOR_NUMBER_OF_PACKETS] = { 0 };

// This array contains the packets with timing information for transmission
packet_sequence_element_t sequence[SL_RF_GENERATOR_SEQUENCE_LENGTH];

// Payload that is used for the initial packet data
uint8_t payload[SL_RF_GENERATOR_PAYLOAD_LENGTH] = {
  0x0F, 0x0E, 0x11, 0x22, 0x33, 0x44, 0x55, 0x0F,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
};

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// RAIL Tx fifo
SL_RAIL_DECLARE_FIFO_BUFFER(tx_fifo, SL_RF_GENERATOR_FIFO_LENGTH);
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
void app_init(void)
{
  // Get RAIL handle, used later by the application
  sl_rail_handle_t rail_handle
    = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  sl_rail_status_t status = sl_rail_set_tx_fifo(rail_handle,
                                                tx_fifo,
                                                SL_RF_GENERATOR_FIFO_LENGTH,
                                                0,
                                                0);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_set_tx_fifo() failed to allocate a large enough fifo\n");

  // Initializing packets
  for (uint8_t i = 0; i < SL_RF_GENERATOR_NUMBER_OF_PACKETS; ++i)
  {
    packets[i].id = i;
    packets[i].channel = SL_RF_GENERATOR_DEFAULT_CHANNEL;
    packets[i].length = SL_RF_GENERATOR_PAYLOAD_LENGTH;
    memcpy(packets[i].payload, payload, SL_RF_GENERATOR_PAYLOAD_LENGTH);
  }

  // Initializing sequence
  for (uint8_t i = 0; i < SL_RF_GENERATOR_SEQUENCE_LENGTH; ++i)
  {
    sequence[i].packet = &packets[i];
    sequence[i].delay = SL_RF_GENERATOR_PACKET_DELAY_US;
  }

  app_log("RF Generator\n");
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
