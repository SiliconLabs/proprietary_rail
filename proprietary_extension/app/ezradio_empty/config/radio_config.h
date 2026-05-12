/*! @file radio_config.h
 * @brief This file contains the automatically generated
 * configurations.
 *
 * @n WDS GUI Version: 3.2.3.0
 * @n Device: Si4455 Rev.: B1                                 
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2013 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef RADIO_CONFIG_H_
#define RADIO_CONFIG_H_

// USER DEFINED PARAMETERS
// Define your own parameters here

// INPUT DATA
/*
// %%	Crys_freq(Hz)	Crys_tol(ppm)	IF_mode	High_perf_Ch_Fil	OSRtune	Ch_Fil_Bw_AFC	ANT_DIV	PM_pattern
// 	30000000	30	2	1	0	0	0	0
//%%	MOD_type	Rsymb(sps)	Fdev(Hz)	RXBW(Hz)	Mancheste	AFC_en	Rsymb_error	Chip-Version
// 	2	10000	30000	200000	0	1	0.0	2
//%%	RF Freq.(MHz)	API_TC	fhst	inputBW	BERT	RAW_dout	D_source	Hi_pfm_div
// 	915	28	250000	0	0	0	0	0
//
// # WB filter 3 (BW = 185.22 kHz);  NB-filter 3 (BW = 185.22 kHz) 

// 
// Modulation index: 6
*/


// CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ                     {30000000L}
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER                    {0x00}
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH               {0x07}
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP        {0x03}
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET       {0xF000}
#define RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD					   {0x42, 0x55, 0x54, 0x54, 0x4F, 0x4E, 0x32}


// CONFIGURATION COMMANDS

/*
// Command:                  RF_POWER_UP
// Description:              Power-up the device with the specified function. Power-up is complete when the CTS bit is set. This command may take longer to set the CTS bit than other commands.
*/
#define RF_POWER_UP 0x02, 0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80

/*
// Set properties:           RF_INT_CTL_ENABLE_2
// Number of properties:     2
// Group ID:                 0x01
// Start ID:                 0x00
// Default values:           0x04, 0x00, 
// Descriptions:
//   INT_CTL_ENABLE - Enables top-level interrupt sources to generate HW interrupts at the NIRQ pin. The three interrupt groups are Chip, Modem and Packet Handler. Each of them contains multiple possible interrupt sources that must be individually enabled via the INT_CTL_PH_ENABLE, INT_CTL_MODEM_ENABLE, INT_CTL_CHIP_ENABLE properties. Note that this property only provides for global enabling/disabling of the HW interrupt indication on the NIRQ output pin. An internal interrupt event may still be generated even if the indication on the NIRQ output pin is disabled. The interrupt event may always be monitored by polling a GPIO pin, or via GET_INT_STATUS, GET_CHIP_STATUS, GET_PH_STATUS, or GET_MODEM_STATUS commands.
//   INT_CTL_PH_ENABLE - Enable individual interrupt sources within the Packet Handler Interrupt Group in order to generate a HW interrupt at the NIRQ output pin. In order to fully enable a HW interrupt, it is necessary to enable both the individual interrupt source (within this property) as well as the corresponding interrupt group (e.g., set INT_CTL_ENABLE:PH_INT_STATUS_EN). Note that even if an interrupt source is not enabled to generate a HW NIRQ interrupt, the given interrupt event still may occur inside the chip and may be monitored by polling a GPIO pin, or via the GET_INT_STATUS or GET_PH_STATUS commands.
*/
#define RF_INT_CTL_ENABLE_2 0x11, 0x01, 0x02, 0x00, 0x01, 0x20

/*
// Set properties:           RF_FRR_CTL_A_MODE_4
// Number of properties:     4
// Group ID:                 0x02
// Start ID:                 0x00
// Default values:           0x01, 0x02, 0x09, 0x00, 
// Descriptions:
//   FRR_CTL_A_MODE - Set the data that is present in fast response register A.
//   FRR_CTL_B_MODE - Set the data that is present in fast response register B.
//   FRR_CTL_C_MODE - Set the data that is present in fast response register C.
//   FRR_CTL_D_MODE - Set the data that is present in fast response register D.
*/
#define RF_FRR_CTL_A_MODE_4 0x11, 0x02, 0x04, 0x00, 0x01, 0x02, 0x09, 0x00

/*
// Set properties:           RF_EZCONFIG_XO_TUNE_1
// Number of properties:     1
// Group ID:                 0x24
// Start ID:                 0x03
// Default values:           0x40, 
// Descriptions:
//   EZCONFIG_XO_TUNE - Crystal oscillator frequency tuning value. 0x00 is maximum frequency value and 0x7F is lowest frequency value. Each LSB code corresponds to a 70 fF capacitance change. The total adjustment range assuming a 30 MHz XTAL is +/-100ppm.
*/
#define RF_EZCONFIG_XO_TUNE_1 0x11, 0x24, 0x01, 0x03, 0x52

/*
// Command:                  RF_WRITE_TX_FIFO
// Description:              This command does not cause CTS to go low, and can be sent and the reply read while CTS is low. This command has no response to be read. If you write more data than the TX FIFO can hold it will trigger a FIFO Overflow interrupt.
*/
#define RF_WRITE_TX_FIFO 0x66, 0x92, 0x6B, 0xE1, 0xAA, 0x48, 0x70, 0x96, 0x57, 0x74, 0xC7, 0x8A, 0xB6, 0x54, 0xFC, 0x27, 0x7E, 0x2E, 0xE6, 0x94, \
0xB0, 0x33, 0xD9, 0x13, 0xD0, 0x56, 0x87, 0x43, 0x31, 0xD8, 0x59, 0x2A, 0x05, 0x1D, 0x52, 0x50, 0xA5, 0x93, 0x96, 0x56, \
0xEC, 0x6F, 0xE5, 0x12, 0x54, 0xA9, 0xE4, 0x57, 0x0E, 0x12, 0x4F, 0x64, 0x63, 0x74, 0xC0, 0xBD, 0x23, 0xAE, 0x79, 0x50, \
0xB2, 0xB1, 0x41, 0x16, 0xFC, 0x4F, 0x75, 0x6D, 0x03, 0x60, 0x45, 0xD6, 0xCF, 0xF5, 0xB4, 0xA0, 0x54, 0x32, 0x84, 0x98, \
0xBD, 0xDC, 0x48, 0x5C, 0xF9, 0x6F, 0x7F, 0xC1, 0x27, 0xA9, 0x69, 0x6E, 0x99, 0x51, 0xDE, 0x8E, 0x57, 0x52, 0x36, 0x03, \
0xA4, 0x47, 0x41, 0x56, 0xBF, 0xA2, 0xA4

/*
// Command:                  RF_NOP
// Description:              Can be used to ensure communication with the device.
*/
#define RF_NOP 0x00

/*
// Command:                  RF_WRITE_TX_FIFO_1
// Description:              This command does not cause CTS to go low, and can be sent and the reply read while CTS is low. This command has no response to be read. If you write more data than the TX FIFO can hold it will trigger a FIFO Overflow interrupt.
*/
#define RF_WRITE_TX_FIFO_1 0x66, 0x41, 0xFA, 0xA1, 0x7A, 0x6F, 0xBE, 0x9A, 0xCE, 0xA6, 0xFF, 0x54, 0x4E, 0x15, 0x7F, 0xE8, 0x1D, 0x3F, 0x81, 0x04, \
0x1D, 0x6A, 0xD8, 0xC7, 0xF4, 0x04, 0xE8, 0x9A, 0xC7, 0xE4, 0x89, 0xB3, 0xA4, 0x31, 0xFA, 0x61, 0x45, 0xE1, 0x39, 0x06, \
0x9D, 0x69, 0xD8, 0xCD, 0x3B, 0x15, 0x51, 0x96, 0x5A, 0x8D, 0x56, 0x2D, 0x1D, 0x8D, 0xC6, 0x77, 0x9B, 0xE9, 0x22, 0x48, \
0x63, 0xDD, 0x22, 0xFF, 0xC9, 0x93, 0x7A, 0x87, 0x0A, 0x0A, 0xA5, 0x1C, 0x9E, 0x27, 0x33, 0x86, 0xBC, 0xB7, 0x95, 0x38, \
0xEC, 0xD3, 0x21, 0xDB, 0x11, 0x9B, 0x96, 0xD0, 0x9C, 0x8F, 0xC0, 0x56, 0x7A, 0xC0, 0x30, 0xD7, 0x55, 0x2B, 0xB0, 0x8A, \
0xCE, 0x75, 0x32, 0x0C, 0x86, 0x5E

/*
// Command:                  RF_EZCONFIG_CHECK
// Description:              
*/
#define RF_EZCONFIG_CHECK 0x19, 0xC0, 0xF2

/*
// Command:                  RF_GPIO_PIN_CFG
// Description:              Configures the gpio pins.
*/
#define RF_GPIO_PIN_CFG 0x13, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00


// AUTOMATICALLY GENERATED CODE! 
// DO NOT EDIT/MODIFY BELOW THIS LINE!
// --------------------------------------------

#ifndef FIRMWARE_LOAD_COMPILE
#define RADIO_CONFIGURATION_DATA_ARRAY { \
        0x07, RF_POWER_UP, \
        0x06, RF_INT_CTL_ENABLE_2, \
        0x08, RF_FRR_CTL_A_MODE_4, \
        0x05, RF_EZCONFIG_XO_TUNE_1, \
        0x6B, RF_WRITE_TX_FIFO, \
        0x01, RF_NOP, \
        0x6A, RF_WRITE_TX_FIFO_1, \
        0x03, RF_EZCONFIG_CHECK, \
        0x08, RF_GPIO_PIN_CFG, \
        0x00 \
 }
#else
#define RADIO_CONFIGURATION_DATA_ARRAY { 0 }
#endif

// DEFAULT VALUES FOR CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ_DEFAULT                     30000000L
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER_DEFAULT                    0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH_DEFAULT               0x10
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP_DEFAULT        0x01
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET_DEFAULT       0x1000
#define RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD_DEFAULT  		   0x42, 0x55, 0x54, 0x54, 0x4F, 0x4E, 0x31 // BUTTON1 

#define RADIO_CONFIGURATION_DATA_RADIO_PATCH_INCLUDED                      0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PATCH_SIZE                          0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PATCH                               {  }

#ifndef RADIO_CONFIGURATION_DATA_ARRAY
#error "This property must be defined!"
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ         { RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ_DEFAULT }
#endif

#ifndef RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER        { RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER_DEFAULT }
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH   { RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH_DEFAULT }
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP  { RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP_DEFAULT }
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET { RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET_DEFAULT }
#endif

#ifndef RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD
#define RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD        { RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD_DEFAULT }
#endif

#define RADIO_CONFIGURATION_DATA { \
                            Radio_Configuration_Data_Array,                            \
                            RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,                   \
                            RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH,              \
                            RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP,       \
                            RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET,      \
                            RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD                    \
                            }

#endif /* RADIO_CONFIG_H_ */
