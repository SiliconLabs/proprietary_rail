/***************************************************************************//**
 * @file
 * @brief RAIL Improved RF Sense Transmitter Example Config
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

#ifndef SL_RAIL_IMPROVED_RFSENSE_TRANSMITTER_CONFIG_H
#define SL_RAIL_IMPROVED_RFSENSE_TRANSMITTER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu

// <h>Packet Settings

// <o SL_IMPROVED_RFSENSE_TRANSMITTER_PAYLOAD_LENGTH> Payload length
// <i> Set Payload length in bytes
// <i> Default: 3
#define SL_IMPROVED_RFSENSE_TRANSMITTER_PAYLOAD_LENGTH 3

// <o SL_IMPROVED_RFSENSE_TRANSMITTER_FIFO_LENGTH> Tx FIFO length
// <i> Set Tx FIFO length in bytes
// <i> Default: 256
#define SL_IMPROVED_RFSENSE_TRANSMITTER_FIFO_LENGTH    256

// <o SL_IMPROVED_RFSENSE_TRANSMITTER_TX_DELAY_US> Packet transmit delay
// <i> Set the delay between the transmittions in us
// <i> Default: 150000
#define SL_IMPROVED_RFSENSE_TRANSMITTER_TX_DELAY_US    150000

// </h> end Static Variables

// <<< end of configuration section >>>

// Array containing the channel indexes used to transmit the packets
#define SL_IMPROVED_RFSENSE_TRANSMITTER_ARRAY_OF_CHANNELS \
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 }

#endif // SL_RAIL_IMPROVED_RFSENSE_TRANSMITTER_CONFIG_H
