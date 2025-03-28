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
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_rail_util_init.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_button_instances.h"
#include "app_log.h"
#include "ota_dfu_host.h"
#include "led_control.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define BUFFER_LENGTH 20
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
uint8_t rx_fifo[BUFFER_LENGTH];
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  uint32_t image_size;

  ota_dfu_init();

  image_size = ota_dfu_image_size();

  app_log("image size = %ld\r\n", ota_dfu_image_size());

  if (image_size == 0) {
    app_log("Image not present, aborting...\r\n");
    sl_button_disable(&sl_button_btn0);
    blinky_start(1, SLOW_PERIOD, SLOW_DUTY_CYCLE);
    return 0;
  }

  led_on(0); // Signal that we have an image

  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  RAIL_SetTxFifo(rail_handle, rx_fifo, 0, BUFFER_LENGTH);

  RAIL_ConfigEvents(rail_handle, RAIL_EVENTS_ALL,
                    RAIL_EVENTS_TX_COMPLETION | RAIL_EVENTS_RX_COMPLETION);

  app_log("Press button 0 to start sending DFU image.\r\n");

  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
