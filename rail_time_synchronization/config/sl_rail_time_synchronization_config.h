/***************************************************************************//**
 * @file
 * @brief RAIL Time Synchronization Example Config
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 *******************************************************************************
 * # Experimental Quality
 * This code has not been formally tested and is provided as-is. It is not
 * suitable for production environments. In addition, this code will not be
 * maintained and there may be no bug maintenance planned for these resources.
 * Silicon Labs may update projects from time to time.
 ******************************************************************************/

#ifndef SL_RAIL_TIME_SYNCHRONIZATION_CONFIG_H
#define SL_RAIL_TIME_SYNCHRONIZATION_CONFIG_H

// <<< Use Configuration Wizard in Context Menu

// <h>Packet Settings

// <o SL_TIME_SYNCHRONIZATION_TX_FIFO_SIZE> TX FIFO size
// <i> Set TX FIFO size in bytes
// <i> Default: 64
#define SL_TIME_SYNCHRONIZATION_TX_FIFO_SIZE                64

// <o SL_TIME_SYNCHRONIZATION_PACKET_LENGTH> Packet length
// <i> Set Packet length in bytes
// <i> Default: 16
#define SL_TIME_SYNCHRONIZATION_PACKET_LENGTH               16

// <o SL_TIME_SYNCHRONIZATION_PACKET_OFFSET> Packet offset
// <i> Set offset of timestamp position within the packet
// <i> Default: 0

// WARNING: Currently, the only supported value is
// SL_TIME_SYNCHRONIZATION_PACKET_OFFSET = SL_TIME_SYNCHRONIZATION_PACKET_LENGTH - 4
#define SL_TIME_SYNCHRONIZATION_PACKET_OFFSET               12

// <o SL_TIME_SYNCHRONIZATION_PREABMLE_LENGTH_TOTAL> Preamble length
// <i> Set Preamble length in bits
// <i> Default: 40
#define SL_TIME_SYNCHRONIZATION_PREABMLE_LENGTH_TOTAL       40

// <o SL_TIME_SYNCHRONIZATION_SYNCWORD_LENGTH_TOTAL> Syncword length
// <i> Set Syncword length in bits
// <i> Default: 16
#define SL_TIME_SYNCHRONIZATION_SYNCWORD_LENGTH_TOTAL       16

// </h> end Packet Settings

// <h>Time Settings

// <o SL_TIME_SYNCHRONIZATION_PROPAGATION_DELAY_US> Propagation delay
// <i> Set propagation delay in microseconds
// <i> Default: 22
#define SL_TIME_SYNCHRONIZATION_PROPAGATION_DELAY_US        22

// </h> end Time Settings

// <h>Static Variables

// <o SL_TIME_SYNCHRONIZATION_DEFAULT_CHANNEL> Default channel
// <i> Set default channel
// <i> Default: 0
#define SL_TIME_SYNCHRONIZATION_DEFAULT_CHANNEL  0

// </h> end Static Variables

// <<< end of configuration section >>>

#endif // SL_RAIL_TIME_SYNCHRONIZATION_CONFIG_H
