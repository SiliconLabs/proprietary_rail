/***************************************************************************//**
 * @file
 * @brief app_init.h
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

#ifndef APP_INIT_H
#define APP_INIT_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "sl_rail_rf_generator_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
typedef struct packet_t{
  uint8_t id;
  uint8_t channel;
  uint16_t length;
  uint8_t payload[SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH];
}packet_t;

// An element of the sequence array that is transmitted
typedef struct packet_sequence_element_t{
  packet_t *packet;
  RAIL_Time_t delay;
}packet_sequence_element_t;

// States of the application state machine
typedef enum {
  S_IDLE, // No active transmission
  S_SCHEDULE_NEXT_TX, // Schedules the next Tx of the sequence
  S_TX_STARTED, // Logs the started event if enabled
  S_WAITING_FOR_TX, // Waiting for the scheduled Tx
  S_TX_ABORT_PENDING, // Aborts the sequence transmission
}state_t;
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * The function is used for application initialization.
 *
 * @param None
 * @returns RAIL_Handle_t RAIL handle
 *****************************************************************************/
RAIL_Handle_t app_init(void);

#endif // APP_INIT_H
