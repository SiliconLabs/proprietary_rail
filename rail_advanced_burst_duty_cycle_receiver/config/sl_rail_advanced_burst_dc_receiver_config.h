/***************************************************************************//**
 * @file
 * @brief RAIL Advanced Burst Duty Cycle Receiver Example Config
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

#ifndef SL_RAIL_ADVANCED_BURST_DC_RECEIVER_CONFIG_H
#define SL_RAIL_ADVANCED_BURST_DC_RECEIVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu

// <o SL_ADVANCED_BURST_DC_RECEIVER_BUFFER_LENGTH> Rx Buffer length
// <i> Set Rx Buffer length in bytes. This should be enough to hold the data
// <i> packet, so at least SL_ADVANCED_BURST_DC_RECEIVER_MIN_DATA_LEN.
// <i> Default: 512
#define SL_ADVANCED_BURST_DC_RECEIVER_BUFFER_LENGTH                 512

// <o SL_ADVANCED_BURST_DC_RECEIVER_DEFAULT_CHANNEL> Default channel
// <i> Set the index of the default channel used to receive
// <i> Default: 0
#define SL_ADVANCED_BURST_DC_RECEIVER_DEFAULT_CHANNEL               0

// <o SL_ADVANCED_BURST_DC_RECEIVER_MIN_DATA_LEN> Minimum data length
// <i> Minimum acceptable frame length with data frame
// <i> Default: 17
#define SL_ADVANCED_BURST_DC_RECEIVER_MIN_DATA_LEN                  17

// <o SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_PRIORITY> Rx window priority
// <i> Priority of the Rx windows. If other higher priority scheduled Rx or Tx
// <i> is happening at the same time, the window will be delayed, which may
// <i> cause missing packets.
// <i> Default: 0
#define SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_PRIORITY               0

// <o SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_SLIPTIME_US> Rx window sliptime
// <i> Sliptime of the Rx window. It could theoretically add PER (Packet Error
// <i> Rate), but since the probability is low, it is recommended to allow the
// <i> other protocol to delay the window. In microseconds.
// <i> Default: 5000
#define SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_SLIPTIME_US            5000

// <o SL_ADVANCED_BURST_DC_RECEIVER_RX_PRIORITY> Scheduled Rx priority
// <i> Priority of the the scheduled Rx for burst. Prio 0 is most important
// <i> Default: 0
#define SL_ADVANCED_BURST_DC_RECEIVER_RX_PRIORITY                   0

// <o SL_ADVANCED_BURST_DC_RECEIVER_MAX_PACKET_LEN> Max packet length
// <i> Maximum packet length to process (valid for both burst and data packet).
// <i> Above it will generate \ref RAIL_SLEEPDC_STATUS_PACKET_TOO_LONG.
// <i> Default: 256
#define SL_ADVANCED_BURST_DC_RECEIVER_MAX_PACKET_LEN                256

// <o SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_ON_NO_TIMESTAMP_US> Wake early
// on no timestamp
// <i> If timestamp cannot be recovered, we use the current timestamp when
// processing the packet for anchor, which will be delayed from the actual
// timestamp. This will be compensated by the below value for scheduled Rx.
// <i> Default: 1000
#define SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_ON_NO_TIMESTAMP_US 1000

// <o SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_FOR_DATA_MSG_US> Wake early
// for data message
// <i> In burst mode, the data message is tightly scheduled to be transmitted at
// anchor time. However we need to start Rx a bit earlier to make sure state
// transition time is done when the preamble starts. This doesn't need to be
// much though. In microseconds.
// <i> Default: 80
#define SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_FOR_DATA_MSG_US    80

// <o SL_ADVANCED_BURST_DC_RECEIVER_BURST_CYCLE_TIME_US> Burst cycle time
// <i> Period time in microseconds in burst mode. Equals transmission time or on
// time plus off time.
// <i> Default: 100e3
#define SL_ADVANCED_BURST_DC_RECEIVER_BURST_CYCLE_TIME_US           100e3

// <o SL_ADVANCED_BURST_DC_RECEIVER_BURST_ON_TIME_US> Burst on time
// <i> On time in burst mode. Should be burst packet length plus some margin
// (preamble + syncword).
// <i> Default: 320
#define SL_ADVANCED_BURST_DC_RECEIVER_BURST_ON_TIME_US              320

// </h> end Static Variables

// <<< end of configuration section >>>

#endif  // SL_RAIL_ADVANCED_BURST_DC_RECEIVER_CONFIG_H
