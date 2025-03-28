/***************************************************************************//**
 * @file
 * @brief RAIL Direct Mode Packet Detector Example Config
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

#ifndef SL_RAIL_DIRECT_MODE_DETECTOR_CONFIG_H
#define SL_RAIL_DIRECT_MODE_DETECTOR_CONFIG_H

#define SL_DETECT_ONLY                    0
#define SL_PACKET_RECEIVE                 1

// <<< Use Configuration Wizard in Context Menu

// <h>Packet Settings

// <o SL_DIRECT_MODE_DETECTOR_TX_FIFO_SIZE> Tx FIFO size
// <i> Set Tx FIFO size in bytes
// <i> Default: 64
#define SL_DIRECT_MODE_DETECTOR_TX_FIFO_SIZE                  64

// <o SL_DIRECT_MODE_DETECTOR_PACKET_LENGTH> Packet length
// <i> Set Packet length in bytes
// <i> Default: 16
#define SL_DIRECT_MODE_DETECTOR_PACKET_LENGTH                 16

// <o SL_DIRECT_MODE_DETECTOR_PAYLOAD_PATTERN> Payload pattern
// <i> Set Payload pattern
// <i> Default: 0x96
#define SL_DIRECT_MODE_DETECTOR_PAYLOAD_PATTERN               0x96

// </h> end Packet Settings

// <h>Debug Settings

// <o SL_DIRECT_MODE_DETECTOR_APP_MODE> Application mode
// <> Set application mode
// <i> Default: 0
// <i> 0: Debug GPIOs are unused
// <i> 1: Debug GPIOs are used
#define SL_DIRECT_MODE_DEBUG_GPIO                             0

// </h> end Debug Settings

// <h>Detection Settings

// <o SL_DIRECT_MODE_DETECTOR_SYNCWORD_PATTERN> Expected syncword pattern
// <i> Set the 32-bit syncword pattern
// <i> Default: 0xF68D
#define SL_DIRECT_MODE_DETECTOR_SYNCWORD_PATTERN                0xF68DUL

// <o SL_DIRECT_MODE_DETECTOR_SYNCWORD_MASK> Expected syncword mask
// <i> Set the 32-bit syncword mask
// <i> Default: 16
#define SL_DIRECT_MODE_DETECTOR_SYNCWORD_MASK                   0xFFFFUL

// <o SL_DIRECT_MODE_DETECTOR_APP_MODE> Application mode
// <SL_DETECT_ONLY=> Detection only
// <SL_PACKET_RECEIVE=> Packet receive
// <i> Set application mode
// <i> Default: Detection only
#define SL_DIRECT_MODE_DETECTOR_APP_MODE              SL_DETECT_ONLY

// </h> end Detection Settings

// <h>Static Variables

// <o SL_DIRECT_MODE_DETECTOR_DEFAULT_CHANNEL> Default channel
// <i> Set default channel
// <i> Default: 0
#define SL_DIRECT_MODE_DETECTOR_DEFAULT_CHANNEL  0

// </h> end Static Variables

// <<< end of configuration section >>>

#endif // SL_RAIL_DIRECT_MODE_DETECTOR_CONFIG_H
