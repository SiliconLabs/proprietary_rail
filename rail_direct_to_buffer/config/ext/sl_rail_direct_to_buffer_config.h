/***************************************************************************//**
 * @file
 * @brief RAIL Direct to Buffer Example Config
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_RAIL_DIRECT_TO_BUFFER_EXT_CONFIG_H
#define SL_RAIL_DIRECT_TO_BUFFER_EXT_CONFIG_H

// <<< Use Configuration Wizard in Context Menu

// <h>FIFO Settings

// <o SL_DIRECT_TO_BUFFER_TX_FIFO_SIZE> Rx FIFO size
// <i> Set Rx FIFO size in bytes
// <i> Default: 1024
#define SL_DIRECT_TO_BUFFER_RX_FIFO_SIZE                1024

// </h> end FIFO Settings


// <h>Mode Settings

// <o SL_DIRECT_TO_BUFFER_RX_SOURCE> Source of Rx Data
// <RX_DEMOD_DATA=> Get 8-bit output from demodulator
// <RX_IQDATA_FILTLSB=> Get lowest 16 bits of I/Q data provided to demodulator
// <RX_IQDATA_FILTMSB=> Get highest 16 bits of I/Q data provided to demodulator
// <RX_DIRECT_MODE_DATA=> Get the binary async direct mode data
// <RX_DIRECT_SYNCHRONOUS_MODE_DATA=> Get the binary sync direct synchronous mode data
// <i> Default: RX_PACKET_DATA
#define SL_DIRECT_TO_BUFFER_RX_SOURCE  RX_DEMOD_DATA
// </h> end Mode Settings

// <h>Static Variables

// <o SL_DIRECT_TO_BUFFER_DEFAULT_CHANNEL> Default channel
// <i> Set default channel
// <i> Default: 0
#define SL_DIRECT_TO_BUFFER_DEFAULT_CHANNEL  0

// </h> end Static Variables

// <<< end of configuration section >>>

#endif // SL_RAIL_DIRECT_TO_BUFFER_EXT_CONFIG_H
