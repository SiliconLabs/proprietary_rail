/***************************************************************************//**
 * @file
 * @brief ota_dfu_target.h
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

#ifndef OTA_DFU_TARGET_H_
#define OTA_DFU_TARGET_H_

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

#include "sl_rail_ota_dfu_target_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define SL_RAIL_OTA_DFU_INVALID_IMAGE_SIZE        0xFFFFFFFFUL
#define SL_RAIL_OTA_DFU_NOT_DETERMINED_IMAGE_SIZE 0x0UL

#define SL_OTA_DFU_TARGET_ACK_OK                  0x01U
#define SL_OTA_DFU_TARGET_ACK_FAIL                0x00U

#define SL_OTA_DFU_TARGET_LAST_SEGMENT_MASK       0x8000U
#define SL_OTA_DFU_TARGET_SEGMENT_ID_MASK         0x7FFFU
#define SL_OTA_DFU_TARGET_INVALID_SEGMENT_ID      0xFFFFU

#define SL_OTA_DFU_TARGET_SEGMENT_PACKET_LENGTH \
        (SL_OTA_DFU_TARGET_SEGMENT_LENGTH + 2U)

typedef enum sl_rail_ota_dfu_target_state {
  // Valid image is present; wait for install or erase.
  sl_rail_ota_dfu_target_state_idle,
  // Wait for segments when no valid image is present or a transfer is in
  //   progress.
  sl_rail_ota_dfu_target_state_wait_segment,
  // Validate the segment ID and write the payload to storage.
  sl_rail_ota_dfu_target_state_process_segment,
  // Verify the image during boot and after the last segment is received.
  sl_rail_ota_dfu_target_state_validate_image,
  // Install the validated image after a user request.
  sl_rail_ota_dfu_target_state_install_image,
  // Erase the storage slot after an error or user request.
  sl_rail_ota_dfu_target_state_erase_slot,
} sl_rail_ota_dfu_target_state_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Prints slot information via app_log.
 ******************************************************************************/
void sl_rail_ota_dfu_print_storage_info(void);

/***************************************************************************//**
 * Scans the slot for the end tag of the received GBL and returns the exact
 * image size.
 *
 * @return Size in bytes on success, otherwise
 *         @ref SL_RAIL_OTA_DFU_INVALID_IMAGE_SIZE.
 ******************************************************************************/
uint32_t sl_rail_ota_dfu_get_image_size(void);

/***************************************************************************//**
 * Writes one fixed-size OTA segment into the configured bootloader slot.
 *
 * @param[in] segment_id Segment number without the last-segment flag.
 * @param[in] payload Segment payload bytes.
 * @param[in] payload_length Number of bytes to write.
 * @return bootloader status code.
 ******************************************************************************/
int32_t sl_rail_ota_dfu_target_write_segment(uint16_t segment_id,
                                             uint8_t *payload,
                                             uint16_t payload_length);

#endif /* OTA_DFU_TARGET_H_ */
