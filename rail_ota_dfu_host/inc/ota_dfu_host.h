/***************************************************************************//**
 * @file
 * @brief ota_dfu_host.h
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

#ifndef OTA_DFU_HOST_H_
#define OTA_DFU_HOST_H_

#define PAYLOAD_LENGTH 20
#define HEADER_LENGTH  4
#define DATA_LENGTH    (PAYLOAD_LENGTH - HEADER_LENGTH)

typedef enum ota_dfu_opcode { // highest 4 bits in the header. The reset of the
  //   header is defined in comment
  ota_dfu_start, // 28-bit image size, host to target, to initiate the OTA DFU
  ota_dfu_go_ahead = 0x1, // target to host, ready to receive image
  ota_dfu_data = 0x2, // 28-bit offset, followed by a 16-byte image fragment,
                      //   host to target
  ota_dfu_data_received = 0x3, // 28-bit offset, target to host
  ota_dfu_no_space = 0x4, // image is larger than slot size, target to host
  ota_dfu_data_error = 0x5, // 28-bit address of the last received fragment,
                            //   target to host
  ota_dfu_bootloader_error = 0x6, // 28-bit bootloader error code, target to
                                  //   host
  ota_dfu_finished = 0xe, // the last packet is written to the storage, target
                          //   to host
  ota_dfu_unknown = 0xf // target to host, undefined error
} ota_dfu_opcode_t;

#define OTA_DFU_HEADER_OPCODE(header)                                     ((         \
                                                                             header) \
                                                                           >> 28)
#define OTA_DFU_HEADER_REST(header)                                       ((         \
                                                                             header) \
                                                                           &         \
                                                                           0x0fffffff)
#define OTA_DFU_SET_HEADER(buffer, opcode,                                             \
                           rest)                                          *((          \
                                                                              uint32_t \
                                                                              *)(      \
                                                                              buffer)) \
    = ((opcode) << 28) | (rest)

void ota_dfu_init(void);
uint32_t ota_dfu_image_size(void);
void ota_dfu_start_packet(uint8_t *payload);
ota_dfu_opcode_t ota_dfu_process_response(uint8_t *rx_buffer, uint8_t *payload);

#endif /* OTA_DFU_HOST_H_ */
