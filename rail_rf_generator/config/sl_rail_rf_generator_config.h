/***************************************************************************//**
 * @file
 * @brief RAIL RF Generator Example Config
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

#ifndef SL_RAIL_RF_GENERATOR_CONFIG_H
#define SL_RAIL_RF_GENERATOR_CONFIG_H

// <<< Use Configuration Wizard in Context Menu

// <h>Packet Settings

// <o SL_RF_GENERATOR_PAYLOAD_LENGTH> Payload length
// <i> Set the initial payload length in bytes.
// <i> This is limited by SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH (max) and 16 (min).
// <i> Default: 16
#define SL_RF_GENERATOR_PAYLOAD_LENGTH                    16

// <o SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH> Maximum payload length
// <i> Set maximum payload length in bytes
// <i> Default: 256
#define SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH                256

// <o SL_RF_GENERATOR_FIFO_LENGTH> Tx Buffer length
// <i> Set Tx Buffer length in bytes.
// <i> This is limited by SL_RF_GENERATOR_FIFO_LENGTH.
// <i> Default: 256
#define SL_RF_GENERATOR_FIFO_LENGTH                       256

// <o SL_RF_GENERATOR_PACKET_DELAY_US> Delay between packet transmissions
// <i> Set the initial delay between the packets.
// <i> Minimum value is PHY dependent.
// <i> Default: 2000000
#define SL_RF_GENERATOR_PACKET_DELAY_US                   2000000

// <o SL_RF_GENERATOR_NUMBER_OF_PACKETS> Number of packets at startup
// <i> Set the number of packets stored in memory.
// <i> Default: 6
#define SL_RF_GENERATOR_NUMBER_OF_PACKETS                 6

// <o SL_RF_GENERATOR_SEQUENCE_LENGTH> Length of the sequence
// <i> Set the number of packets present in a sequence
// <i> Default: 6
#define SL_RF_GENERATOR_SEQUENCE_LENGTH                   6

// <o SL_RF_GENERATOR_DEFAULT_CHANNEL> Default channel
// <i> Set the index of the default channel used to transmit.
// <i> Default: 0
#define SL_RF_GENERATOR_DEFAULT_CHANNEL                   0

// </h> end Static Variables

// <<< end of configuration section >>>

#endif // SL_RAIL_RF_GENERATOR_CONFIG_H
