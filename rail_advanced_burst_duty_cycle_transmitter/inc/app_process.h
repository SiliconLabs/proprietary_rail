/***************************************************************************//**
 * @file
 * @brief app_process.h
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

#ifndef APP_PROCESS_H
#define APP_PROCESS_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_common.h"
#include "sl_rail_advanced_burst_dc_transmitter_config.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/******************************************************************************
 * @typedef app_state_t
 * @brief Application state used in the main state machine
 *****************************************************************************/
typedef enum {
  S_IDLE,               // No active transmission
  S_BURST_TX,           // Burst packets are being transmitted
  S_DATA_TX,            // Data packet is being transmitted
} app_state_t;

/******************************************************************************
 * @typedef burst_frame_t
 * @brief Burst frame used
 *****************************************************************************/
SL_PACK_START(1)
typedef struct {
  uint8_t length;
  uint32_t time;          // Timestamp. Time between the end of this frame
                          // and the anchor point
  uint16_t crc;           // CRC
} SL_ATTRIBUTE_PACKED burst_frame_t;
SL_PACK_END()

/******************************************************************************
 * @typedef data_frame_t
 * @brief Data frame used
 *****************************************************************************/
SL_PACK_START(1)
typedef struct {
  uint8_t length;
  uint8_t data[SL_ADVANCED_BURST_DC_TRANSMITTER_DATA_PAYLOAD_LEN];
  uint16_t crc;           // CRC
} SL_ATTRIBUTE_PACKED data_frame_t;
SL_PACK_END()
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief The function is used for Application logic.
 * It is called infinitely.
 *****************************************************************************/
void app_process_action(void);

#endif // APP_PROCESS_H
