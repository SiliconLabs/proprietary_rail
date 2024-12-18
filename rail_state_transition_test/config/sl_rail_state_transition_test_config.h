/***************************************************************************//**
 * @file
 * @brief RAIL Proprietary - State Transition Test Example Config
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

#ifndef SL_RAIL_STATE_TRANSITION_TEST_CONFIG_H
#define SL_RAIL_STATE_TRANSITION_TEST_CONFIG_H

// <<< Use Configuration Wizard in Context Menu

// PRS and GPIO configuration: auto generated based on the selected radio
// platform.

// PRS_RACL_RX
#define PRS_CHANNEL_RX                      6
// PRS_RACL_RX | PRS_RACL_TX
#define PRS_CHANNEL_ACTIVE                  7
// PRS_RACL_LNAEN
#define PRS_CHANNEL_LNA                     8
// PRS_RACL_LNAEN | PRS_RACL_PAEN
#define PRS_CHANNEL_READY_OR_ACTIVE         9
// PRS_RACL_ACTIVE
#define PRS_CHANNEL_BUSY_OR_READY_OR_ACTIVE 10
// PRS_RACL_ACTIVE ^ (PRS_RACL_LNAEN | PRS_RACL_PAEN)
#define PRS_CHANNEL_BUSY                    0
// PRS_RACL_ACTIVE ^ (PRS_RACL_RX | PRS_RACL_TX)
#define PRS_CHANNEL_BUSY_OR_READY           1
// ((PRS_RACL_RX | PRS_RACL_TX) ^ PRS_RACL_LNAEN)
#define PRS_CHANNEL_READY_AUX               2
// ((PRS_RACL_RX | PRS_RACL_TX) ^ PRS_RACL_LNAEN) ^ PRS_RACL_PAEN
#define PRS_CHANNEL_READY                   11

// primary 1
#define PRS_PORT_ACTIVE                     gpioPortC
#define PRS_PIN_ACTIVE                      5

// primary 2
#define PRS_PORT_READY                      gpioPortC
#define PRS_PIN_READY                       7

// secondary 1
#define PRS_PORT_BUSY                       gpioPortA
#define PRS_PIN_BUSY                        6

// secondary 2
#define PRS_PORT_BUSY_OR_READY              gpioPortA
#define PRS_PIN_BUSY_OR_READY               7

// The following options may impact the state transition times. The default
// values are set to minimize the state transition times.

// <o SL_RAIL_STATE_TRANSITION_TEST_BEST_EFFORT_TIMING> Best effort timing
// <i> Set best effort timing by setting all state transitions to 0
// <i> Default: 1
#define SL_RAIL_STATE_TRANSITION_TEST_BEST_EFFORT_TIMING 1

// <o SL_RAIL_STATE_TRANSITION_TEST_ENABLE_SYNTH_CAL_CACHING> Enable synth cal caching
// <i> Enable synthesizer calibration caching
// <i> Default: 1
#define SL_RAIL_STATE_TRANSITION_TEST_ENABLE_SYNTH_CAL_CACHING 1

// <o SL_RAIL_STATE_TRANSITION_TEST_BYPASS_CALIBRATIONS> Disable calibrations
// <i> Bypass synthesizer and DC calibrations
// <i> Default: 1
#define SL_RAIL_STATE_TRANSITION_TEST_BYPASS_CALIBRATIONS 1

// <o SL_RAIL_STATE_TRANSITION_TEST_ENABLE_FAST_RX_2_RX> Enable fast RX to RX
// <i> Enable fast RX to RX transitions
// <i> Default: 1
#define SL_RAIL_STATE_TRANSITION_TEST_ENABLE_FAST_RX_2_RX 1

// <o SL_RAIL_STATE_TRANSITION_TEST_PTI_DISABLE> Disable PTI
// <i> Disable PTI
// <i> Default: 1
#define SL_RAIL_STATE_TRANSITION_TEST_PTI_DISABLE 1

// The following options are controlling the state transition measurement's
// behavior.

// <o SL_RAIL_STATE_TRANSITION_TEST_REPEAT_CYCLES> Repeat cycles
// <i> Number of cycles to repeat the state transition tests
// <i> Default: 100
#define SL_RAIL_STATE_TRANSITION_TEST_REPEAT_CYCLES 100

// <o SL_RAIL_STATE_TRANSITION_TEST_TX_PERIOD> Tx period
// <i> Tx period used in "TX for RX_COMPLETE" mode in symbols 
// <i> Default: 1500
#define SL_RAIL_STATE_TRANSITION_TEST_TX_PERIOD 1500

// <o SL_RAIL_STATE_TRANSITION_TEST_PRIMARY_CHANNEL> Primary channel
// Primary channel to use for multi-channel tests, must be different from
// secondary channel
// <i> Default: 0
#define SL_RAIL_STATE_TRANSITION_TEST_PRIMARY_CHANNEL 0

// <o SL_RAIL_STATE_TRANSITION_TEST_SECONDARY_CHANNEL> Secondary channel
// Secondary channel to use for multi-channel tests, must be different from
// primary channel 
// <i> Default: 1
#define SL_RAIL_STATE_TRANSITION_TEST_SECONDARY_CHANNEL 1

// <o SL_RAIL_STATE_TRANSITION_TEST_IDLE_MODE> Idle mode
// Idle mode to use for the state transition tests
// <i> Default: RAIL_IDLE
#define SL_RAIL_STATE_TRANSITION_TEST_IDLE_MODE RAIL_IDLE

// <o SL_STATE_TRANSITION_TEST_TX_FIFO_SIZE> TX FIFO size
// <i> Set TX FIFO size in bytes
// <i> Default: 64
#define SL_RAIL_STATE_TRANSITION_TEST_TX_FIFO_SIZE                  64
// <<< end of configuration section >>>

#endif // SL_RAIL_STATE_TRANSITION_TEST_CONFIG_H
