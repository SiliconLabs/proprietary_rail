/***************************************************************************//**
 * @file
 * @brief ota_dfu.c
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

#include "btl_interface.h"
#include "btl_interface_storage.h"
#include "app_log.h"
#include "ota_dfu_target.h"

#define SLOT_ID 0

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

ota_dfu_opcode_t ota_dfu_process_command(uint8_t *command, uint8_t *response)
{
  static enum receiving_state { idle, started, finished } state;
  static uint32_t image_size;
  static uint32_t  offset; // offset of the expected data
  enum ota_dfu_opcode cmd_opcode, rsp_opcode = ota_dfu_unknown;
  uint32_t rsp_rest = 0; // the rest of response header (following the opcode)

  uint32_t header = *(uint32_t *) command;
  cmd_opcode = OTA_DFU_HEADER_OPCODE(header);

  app_log("0x%08lx\r\n", header);

  switch (state) {
    case idle:
      if (cmd_opcode == ota_dfu_start) { // start command
        image_size = OTA_DFU_HEADER_REST(header);
        if (image_size > slot_info.length) {
          rsp_opcode = ota_dfu_no_space;
        } else {
          int32_t status;
          offset = 0;

          status = bootloader_eraseStorageSlot(SLOT_ID);

          if (status != BOOTLOADER_OK) {
            rsp_opcode = ota_dfu_bootloader_error | status;
          } else {
            state = started;
            rsp_opcode = ota_dfu_go_ahead;
          }
        }
      }
      break;
    case started:
      if (cmd_opcode == ota_dfu_start) {  // start command can still be
                                          //   re-transmitted
        rsp_opcode = ota_dfu_go_ahead;
      } else if (cmd_opcode == ota_dfu_data) {
        uint32_t address = OTA_DFU_HEADER_REST(header);

        if (address < offset) {
          rsp_opcode = ota_dfu_data_received;
          rsp_rest = offset - DATA_LENGTH;   // Send the previous
                                             //   acknowledgement
        } else if (address == offset) {
          int32_t status;
          uint32_t data_size = image_size - offset;
          if (data_size > DATA_LENGTH) {
            data_size = DATA_LENGTH;
          }
          status = bootloader_writeStorage(SLOT_ID,
                                           offset,
                                           command + HEADER_LENGTH,
                                           data_size);
          if (status != BOOTLOADER_OK) {
            rsp_opcode = ota_dfu_bootloader_error;
            rsp_rest = status;
          } else {
            if (offset + data_size == image_size) { // last packet received
              state = finished;
              rsp_opcode = ota_dfu_finished;
            } else {
              rsp_opcode = ota_dfu_data_received;
              rsp_rest = offset;
              offset += DATA_LENGTH;
            }
          }
        }
      }
      break;
    case finished:
      rsp_opcode = ota_dfu_finished;
      break;
    default:
      break;
  }
  OTA_DFU_SET_HEADER(response, rsp_opcode, rsp_rest);
  return rsp_opcode;
}

void ota_dfu_restart(void)
{
  bootloader_setImageToBootload(0);
  bootloader_rebootAndInstall();
}
