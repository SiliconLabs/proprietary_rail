/***************************************************************************//**
 * @file
 * @brief ota_dfu_host.c
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "btl_interface.h"
#include "btl_interface_storage.h"
#include "app_log.h"
#include "ota_dfu_host.h"

#define SLOT_ID        0
#define GBL_TAG_ID_END 0xFC0404FC

static BootloaderStorageInformation_t bootloader_storage_info;
static BootloaderStorageSlot_t slot_info;

void ota_dfu_init(void)
{
  bootloader_getStorageInfo(&bootloader_storage_info);
  app_log("numStorageSlots = %lu\r\n", bootloader_storage_info.numStorageSlots);

  bootloader_getStorageSlotInfo(SLOT_ID, &slot_info);
  app_log("address: 0x%08lx\r\n", slot_info.address);
  app_log("length:  0x%08lx\r\n", slot_info.length);
}

uint32_t ota_dfu_image_size(void)
{
  static uint32_t image_size;

  uint32_t gbl_tag[2]; // Tag ID & Tag length
  uint32_t offset = 0;

  if (image_size != 0) {
    return image_size;                   // Don't check again!
  }
  for (offset = 0; offset < slot_info.length; )
  {
    bootloader_readStorage(SLOT_ID, offset, (uint8_t *)gbl_tag, sizeof gbl_tag);
    if (gbl_tag[0] == GBL_TAG_ID_END) {
      return image_size = offset + gbl_tag[1] + sizeof gbl_tag;
    } else {
      if ((gbl_tag[1] & 0x3) != 0) {
        break;                         // Basic sanity check
      }
      offset += sizeof gbl_tag + gbl_tag[1];
    }
  }
  return 0;
}

void ota_dfu_start_packet(uint8_t *payload)
{
  OTA_DFU_SET_HEADER(payload, ota_dfu_start, ota_dfu_image_size());
}

uint32_t ota_dfu_process_response(uint8_t *rx_buffer, uint8_t *payload)
{
  static uint32_t offset, remaining;
  uint32_t received;
  enum ota_dfu_opcode action = ota_dfu_unknown, opcode;

  uint32_t rsp_header = *(uint32_t *) rx_buffer;

  opcode = OTA_DFU_HEADER_OPCODE(rsp_header);

  if (opcode == ota_dfu_go_ahead) { // first response
    offset = 0;
    remaining = ota_dfu_image_size();
    action = ota_dfu_data;
  } else if (opcode == ota_dfu_data_received) {
    received = OTA_DFU_HEADER_REST(rsp_header);
    if (received == offset) {
      offset += DATA_LENGTH;
      remaining -= DATA_LENGTH;
      action = ota_dfu_data;
    } else {
      action = ota_dfu_unknown;
    }
  } else if (opcode == ota_dfu_data_error) {
    received = OTA_DFU_HEADER_REST(rsp_header);
    if (received == offset) {
      action = ota_dfu_data;
    } else {
      action = ota_dfu_unknown;
    }
  } else if (opcode == ota_dfu_finished) {
    action = ota_dfu_finished;
  }

  if (action == ota_dfu_data) {
    uint32_t data_size = remaining > DATA_LENGTH ? DATA_LENGTH : remaining;
    bootloader_readStorage(SLOT_ID, offset, payload + HEADER_LENGTH, data_size);
  }
  OTA_DFU_SET_HEADER(payload, action, offset);
  return action;
}
