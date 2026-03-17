/***************************************************************************//**
 * @file
 * @brief ota_dfu_host.c
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
#include "btl_errorcode.h"
#include "btl_interface.h"
#include "btl_interface_storage.h"
#include "gbl/btl_gbl_format.h"

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

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

static BootloaderStorageSlot_t slot_info;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_rail_ota_dfu_print_storage_info(void)
{
  int32_t status;
  BootloaderStorageInformation_t bootloader_storage_info;

  bootloader_getStorageInfo(&bootloader_storage_info);

  status = bootloader_getStorageSlotInfo(SL_OTA_DFU_HOST_DEFAULT_SLOT_ID,
                                         &slot_info);
  if (status == BOOTLOADER_OK) {
    app_log("Storage slot start address: 0x%08lx\n", slot_info.address);
    app_log("Storage slot length: %lu bytes\n", slot_info.length);
  } else {
    app_log_error("Failed to read storage slot information: %ld\n",
                  status);
  }
}

uint32_t sl_rail_ota_dfu_get_image_size(void)
{
  int32_t status; // Store return values from bootloader APIs.
  uint32_t offset = 0;
  bool last_tag_header_found = false;
  GblTagHeader_t tag_header; // Tag ID and length used while parsing the image.

  // Store the image size in bytes.
  uint32_t image_size;

  // Read the first tag.
  status = bootloader_readStorage(SL_OTA_DFU_HOST_DEFAULT_SLOT_ID, offset,
                                  (uint8_t *)&tag_header, sizeof tag_header);
  // Detect an invalid image early.
  if ((tag_header.tagId != GBL_TAG_ID_HEADER_V3) || (status != BOOTLOADER_OK)) {
    image_size = SL_RAIL_OTA_DFU_INVALID_IMAGE_SIZE;
    return image_size;
  }

  // Walk the slot until the end tag is found.
  for (offset =
         sizeof tag_header + tag_header.length; offset < slot_info.length;)
  {
    // Read the next tag.
    status = bootloader_readStorage(SL_OTA_DFU_HOST_DEFAULT_SLOT_ID, offset,
                                    (uint8_t *)&tag_header, sizeof tag_header);
    if (status != BOOTLOADER_OK) {
      image_size = SL_RAIL_OTA_DFU_INVALID_IMAGE_SIZE;
      break;
    }

    // GBL_TAG_ID_END marks the end of the image.
    if (tag_header.tagId == GBL_TAG_ID_END) {
      image_size = offset + tag_header.length + sizeof tag_header;
      last_tag_header_found = true;
      break;
    }

    // Advance to the next tag.
    offset += (sizeof tag_header) + tag_header.length;
  }

  // Report failure if the end tag was not found before the slot boundary.
  if (!last_tag_header_found) {
    image_size = SL_RAIL_OTA_DFU_INVALID_IMAGE_SIZE;
  }

  return image_size;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
