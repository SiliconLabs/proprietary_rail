/***************************************************************************//**
 * @file
 * @brief app_init.h
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_INIT_H
#define APP_INIT_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define RAIL_FIFO_SIZE    256
#define NUMBER_OF_PACKETS 6
#define PACKET_LENGTH     16
#define MAX_PACKET_LENGTH 512
#define PACKET_DELAY      2000000
#define SEQUENCE_LENGTH   NUMBER_OF_PACKETS

typedef struct packet_t{
  uint8_t id, channel;
  uint16_t length;
  uint8_t payload[MAX_PACKET_LENGTH];
}packet_t;

typedef struct packetSequenceElement_t{
  packet_t *packet;
  RAIL_Time_t delay;
}packetSequenceElement_t;

typedef enum {
  S_IDLE,
  S_TX_SEQUENCE,
  S_TX_STARTED,
  S_TX,
  S_TX_ABORTED,
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
