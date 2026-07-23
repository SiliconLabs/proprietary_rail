/***************************************************************************//**
 * @file
 * @brief RAIL Advanced Burst Duty Cycle Transmitter Example Config
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

#ifndef SL_RAIL_ADVANCED_BURST_DC_TRANSMITTER_CONFIG_H
#define SL_RAIL_ADVANCED_BURST_DC_TRANSMITTER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu

// <h>Packet Settings

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_BUFFER_LENGTH> Tx Buffer length
// <i> Set Tx Buffer length in bytes
// <i> Default: 512
#define SL_ADVANCED_BURST_DC_TRANSMITTER_BUFFER_LENGTH        512

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_DEFAULT_CHANNEL> Default channel
// <i> Set the index of the default channel used to transmit
// <i> Default: 0
#define SL_ADVANCED_BURST_DC_TRANSMITTER_DEFAULT_CHANNEL      0

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_POLYNOMIAL> CRC Polynomial
// <i> The used CRC polynomial value. To calculate this see GPCRC module.
// <i> Default: 0x8005 (CRC_16)
#define SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_POLYNOMIAL       0x8005;

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_SEED> CRC Seed
// <i> The initial value for the CRC calculation.
// <i> Default: 0x0
#define SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_SEED             0x0;

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_PRIORITY> DMP Priority
// <i> DMP priority to use for wakeup Tx. 0 is highest priority.
// <i> Default: 0
#define SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_PRIORITY         0

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_SLIPTIME_US> DMP Sliptime
// <i> Allowed delay to start a wakeup transmission, to help RAIL scheuler to
// schedule the protocols. In microseconds.
// <i> Default: 100000UL
#define SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_SLIPTIME_US      100000UL

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATTERN> Preamble pattern
// <i> Preamble pattern to use. This should match Preamble Base Pattern field
// <i> configured on the Radio Config GUI.
// <i> Default: 1
#define SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATTERN     1

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATT_LEN> Preamble pattern
// length
// <i> Length of the preamble pattern in bits. Note, that
// <i> (32-sync_word_bits)/SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATT_LEN
// <i> must have 0 remainder.
// <i> Default: 2
#define SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATT_LEN    2

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_ANCHOR_DELAY_US> Delay of the data
// message
// <i> Anchor delay after the burst packets. Minimum is the rail state
// transition time (minimum value is around 300).
// <i> Default: 800
#define SL_ADVANCED_BURST_DC_TRANSMITTER_ANCHOR_DELAY_US      800

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_LEN> Preamble pattern length
// <i> Length of the preamble pattern in bits that is sufficient on most PHYs.
// <i> Default: 32
#define SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_LEN         32

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_DATA_PAYLOAD_LEN> Data payload length
// <i> Payload length in bytes of the data message inside data_frame_t. The
// <i> minimum length is 16 as the app uses a fixed 16 lengh payload for the
// <i> data message.
// <i> Default: 16
#define SL_ADVANCED_BURST_DC_TRANSMITTER_DATA_PAYLOAD_LEN     16

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_SCHEDULE_DELAY_US> Schedule delay
// <i> Measured delay of scheduled transmissions. I.e., it should be scheduled
// this amount earlier
// <i> Default: 0
#define SL_ADVANCED_BURST_DC_TRANSMITTER_SCHEDULE_DELAY_US    0

// <o SL_ADVANCED_BURST_DC_TRANSMITTER_BURST_LEN_MS> Burst length in
// milliseconds
// <i> The length of the wakeup tx time
// <i> Default: 100
#define SL_ADVANCED_BURST_DC_TRANSMITTER_BURST_LEN_MS         100

// </h> end Static Variables

// <<< end of configuration section >>>

#endif // SL_RAIL_ADVANCED_BURST_DC_TRANSMITTER_CONFIG_H
