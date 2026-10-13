/***************************************************************************//**
 * @file
 * @brief range_test_sd_log.h
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

#ifndef RANGE_TEST_SD_LOG_H_
#define RANGE_TEST_SD_LOG_H_

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>

#include "ff.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
typedef struct {
  double latitude;
  double longitude;
  float altitude;
  float time;
} gps_data_t;

// Set the default log file name format
#define SL_RANGE_TEST_DEFAULT_LOG_FILE_NAME_FORMAT \
  "000000.csv"

// Set the Rx log header string with GPS present
#define SL_RANGE_TEST_SD_LOG_HEADER_RX_GPS \
  "Packet index,RSSI [dBm],MA [%],PER [%],Latitude [°],Longitude [°],Altitude [m],Time [HHMMSS]\n"

// Set the Tx log header string with GPS present
#define SL_RANGE_TEST_SD_LOG_HEADER_TX_GPS \
  "Packets sent,Latitude [°],Longitude [°],Altitude [m],Time [HHMMSS]\n"

// Set the Rx log header string with no GPS present
#define SL_RANGE_TEST_SD_LOG_HEADER_RX_NO_GPS \
  "Packet index,RSSI [dBm],MA [%],PER [%]\n"

// Set the Tx log header string with no GPS present
#define SL_RANGE_TEST_SD_LOG_HEADER_TX_NO_GPS \
  "Packets sent\n"

// Set the Rx log settings header string
#define SL_RANGE_TEST_SD_LOG_SETTINGS_HEADER_RX \
  "PHY,Channel number,Payload length,Tx power,MA Window size\n"

// Set the Tx log settings header string
#define SL_RANGE_TEST_SD_LOG_SETTINGS_HEADER_TX \
  "PHY,Channel number,Payload length,Tx power\n"

// Set the log separator string
#define SL_RANGE_TEST_SD_LOG_SEPARATOR \
  "Sep=,\n"

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * The function is used to retrieve the latest GPS data.
 * This component contains a weak, empty implementation of this function, which
 * can be overridden by the application for the actual GPS data retrieval.
 *
 * @param data Pointer to a gps_data_t structure where the GPS data will be
 * stored.
 *****************************************************************************/
void gps_handler_get_data(gps_data_t *data);

/**************************************************************************//**
 * The function is used to start a new log entry on the SD card.
 * This component contains a weak, empty implementation of this function, which
 * can be overridden by the application for the actual SD card logging
 * implementation.
 *
 * @param log_file_name The name of the log file where the log entry will be
 * stored.
 * @returns FRESULT Returns the result of the operation as an FRESULT enum.
 *****************************************************************************/
FRESULT sd_start_log_entry(const char *log_file_name);

/**************************************************************************//**
 * The function is used to stop the current log entry on the SD card.
 * This component contains a weak, empty implementation of this function, which
 * can be overridden by the application for the actual SD card logging
 * implementation.
 *
 * @returns FRESULT Returns the result of the operation as an FRESULT enum.
 *****************************************************************************/
FRESULT sd_stop_log_entry(void);

/**************************************************************************//**
 * The function is used to log a string to the SD card.
 * This component contains a weak, empty implementation of this function, which
 * can be overridden by the application for the actual SD card logging
 * implementation.
 *
 * @param str The string to be logged.
 * @param len The length of the string to be logged.
 * @returns FRESULT Returns the result of the operation as an FRESULT enum.
 *****************************************************************************/
FRESULT sd_log(const char *str, unsigned int len);

/**************************************************************************//**
 * The function is used to check if SD card is present or not.
 * This component contains a weak, empty implementation of this function, which
 * can be overridden by the application for the actual SD card logging
 * implementation.
 *
 * @returns Returns the flag indicating if an SD card is present or not.
 *****************************************************************************/
bool sd_is_present(void);

/**************************************************************************//**
 * The function is used for SD and GPS extension process handling.
 *****************************************************************************/
void app_process_action_range_test_log(void);

#endif // RANGE_TEST_SD_LOG_H_
