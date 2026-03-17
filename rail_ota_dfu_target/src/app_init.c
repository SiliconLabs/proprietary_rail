/***************************************************************************//**
 * @file
 * @brief app_init.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_rail.h"
#include "sl_rail_util_init.h"

#include "app_log.h"
#include "app_init.h"
#include "ota_dfu_target.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define SEGMENT_PAYLOAD_LENGTH  (SL_OTA_DFU_TARGET_SEGMENT_LENGTH + 2)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

SL_RAIL_DECLARE_FIFO_BUFFER(tx_fifo, SL_OTA_DFU_TARGET_TX_FIFO_LENGTH);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
void app_init(void)
{
  sl_rail_handle_t rail_handle;
  sl_rail_status_t status;

  // Print the welcome message.
  app_log("RAIL Proprietary - OTA DFU Target application\n");

  // Print storage information.
  sl_rail_ota_dfu_print_storage_info();

  // Get the RAIL handle used by the application.
  rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Initialize the Tx FIFO.
  status = sl_rail_set_tx_fifo(rail_handle,
                               tx_fifo,
                               SL_OTA_DFU_TARGET_TX_FIFO_LENGTH,
                               0,
                               0);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    app_log_error("sl_rail_set_tx_fifo() failed: %lu\n", status);
  }

  // Configure auto-ack.
  sl_rail_auto_ack_config_t auto_ack_config = {
    .enable = true,
    .ack_timeout_us = 1000, // Unused by the target path.
    .rx_transitions = {
      .success = SL_RAIL_RF_STATE_RX, // Stay in RX.
      .error = SL_RAIL_RF_STATE_RX, // Ignored.
    },
    .tx_transitions = {
      .success = SL_RAIL_RF_STATE_RX, // Return to RX after ACK transmission.
      .error = SL_RAIL_RF_STATE_RX, // Ignored.
    }
  };
  status = sl_rail_config_auto_ack(rail_handle, &auto_ack_config);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    app_log_error("sl_rail_config_auto_ack() failed!\n");
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
