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
#include "sl_simple_led_instances.h"
#include "sl_rail_tutorial_downloading_messages_config.h"

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
SL_ALIGN(RAIL_FIFO_ALIGNMENT)
static uint8_t tx_fifo[SL_TUTORIAL_DOWNLOADING_MESSAGES_BUFFER_LENGTH]
SL_ATTRIBUTE_ALIGN(RAIL_FIFO_ALIGNMENT);
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization relates to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  uint16_t size = RAIL_SetTxFifo(rail_handle,
                                 tx_fifo,
                                 0,
                                 SL_TUTORIAL_DOWNLOADING_MESSAGES_BUFFER_LENGTH);
  if (size == 0) {
    // Turn led1 ON on SetTxFifo Error
    sl_led_toggle(&sl_led_led1);
  }
  RAIL_Status_t status = RAIL_ConfigEvents(rail_handle, RAIL_EVENTS_ALL,
                                           RAIL_EVENTS_TX_COMPLETION
                                           | RAIL_EVENTS_RX_COMPLETION
                                           | RAIL_EVENT_CAL_NEEDED
                                           );
  if (status != RAIL_STATUS_NO_ERROR) {
    // Turn led1 ON on ConfigEvents Error
    sl_led_toggle(&sl_led_led1);
  }
  status = RAIL_StartRx(rail_handle,
                        SL_TUTORIAL_DOWNLOADING_MESSAGES_DEFAULT_CHANNEL,
                        NULL);
  if (status != RAIL_STATUS_NO_ERROR) {
    // Turn led1 ON on StartRx Error
    sl_led_toggle(&sl_led_led1);
  }
  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
