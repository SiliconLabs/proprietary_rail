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

#include "sl_rail_ota_dfu_host_config.h"

#include "app_log.h"
#include "ota_dfu_host.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

SL_RAIL_DECLARE_FIFO_BUFFER(tx_fifo, SL_OTA_DFU_HOST_TX_FIFO_LENGTH);

uint32_t last_segment_id = 0;

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
  app_log("RAIL Proprietary OTA DFU host application\n");

  // Get the RAIL handle used by the application.
  rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Print bootloader storage information.
  sl_rail_ota_dfu_print_storage_info();

  // Retrieve image information.
  uint32_t image_size = SL_RAIL_OTA_DFU_NOT_DETERMINED_IMAGE_SIZE;

  image_size = sl_rail_ota_dfu_get_image_size();
  if (image_size == SL_RAIL_OTA_DFU_INVALID_IMAGE_SIZE) {
    app_log_error("No image found. Aborting.\n");
  } else {
    app_log("Image size: %lu bytes\n", image_size);
  }

  // Use ceiling division to determine the last segment ID.
  last_segment_id = (image_size + SL_OTA_DFU_HOST_SEGMENT_LENGTH - 1)
                    / SL_OTA_DFU_HOST_SEGMENT_LENGTH;
  if (last_segment_id & SL_OTA_DFU_HOST_LAST_SEGMENT_MASK) {
    app_log_error(
      "Image size exceeds the maximum supported size: %d bytes. Increase the segment size from %d bytes.\n",
      SL_OTA_DFU_HOST_SEGMENT_LENGTH * SL_OTA_DFU_HOST_SEGMENT_ID_MASK,
      SL_OTA_DFU_HOST_SEGMENT_LENGTH);
  }

  // Initialize the Tx FIFO.
  status = sl_rail_set_tx_fifo(rail_handle,
                               tx_fifo,
                               SL_OTA_DFU_HOST_TX_FIFO_LENGTH,
                               0,
                               0);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    app_log_error("Failed to configure the Tx FIFO.\n");
  }

  // Configure auto-ack.
  sl_rail_auto_ack_config_t auto_ack_config = {
    .enable = true,
    .ack_timeout_us = SL_OTA_DFU_HOST_ACK_TIMEOUT_US,
    .rx_transitions = {
      .success = SL_RAIL_RF_STATE_IDLE, // The app does not use standard RX.
      .error = SL_RAIL_RF_STATE_IDLE, // Ignored.
    },
    .tx_transitions = {
      .success = SL_RAIL_RF_STATE_IDLE,
      .error = SL_RAIL_RF_STATE_IDLE, // Ignored.
    }
  };

  status = sl_rail_config_auto_ack(rail_handle, &auto_ack_config);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    app_log_error("sl_rail_config_auto_ack() failed!\n");
  }

  app_log("Press PB0 to start the DFU transfer.\n");
  app_log("Transfer will complete in %lu blocks.\n", last_segment_id + 1);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
