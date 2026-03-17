/***************************************************************************//**
 * @file
 * @brief RAIL OTA DFU Target Config
 *****************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
 *****************************************************************************
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

#ifndef SL_RAIL_OTA_DFU_TARGET_CONFIG_H_
#define SL_RAIL_OTA_DFU_TARGET_CONFIG_H_

// <<< Use Configuration Wizard in Context Menu

// <h>Packet Settings

// <o SL_OTA_DFU_TARGET_SEGMENT_LENGTH> Image segment length
// <i> Set image segment length in bytes.
// <i> Default: 16
#define SL_OTA_DFU_TARGET_SEGMENT_LENGTH         16

// <o SL_OTA_DFU_TARGET_TX_FIFO_LENGTH> Tx buffer length
// <i> Set Tx buffer length in bytes.
// <i> Default: 256
#define SL_OTA_DFU_TARGET_TX_FIFO_LENGTH         256

// </h> end Packet Settings

// <h>Default Settings

// <o SL_OTA_DFU_TARGET_DEFAULT_CHANNEL> Default channel
// <i> Set the default channel used to receive segments and transmit ACKs.
// <i> Default: 0
#define SL_OTA_DFU_TARGET_DEFAULT_CHANNEL       0

// <o SL_OTA_DFU_TARGET_DEFAULT_SLOT_ID> Default slot ID
// <i> Default: 0
#define SL_OTA_DFU_TARGET_DEFAULT_SLOT_ID       0

// </h> end Default Settings

// <<< end of configuration section >>>

#endif // SL_RAIL_OTA_DFU_TARGET_CONFIG_H_
