/***************************************************************************//**
 * @file
 * @brief range_test_sd_log.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdio.h>

#include "string.h"

#include "em_common.h"

#include "diskio.h"
#include "ff.h"

#include "sl_component_catalog.h"
#include "sl_range_test_sd_log_config.h"

#include "range_test_sd_log.h"

#include "app_log.h"
#include "app_process.h"
#include "app_measurement.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#if SL_GPS_DRIVER_PRESENT
#define DATA_HEADER_RX SL_RANGE_TEST_SD_LOG_HEADER_RX_GPS
#define DATA_HEADER_TX SL_RANGE_TEST_SD_LOG_HEADER_TX_GPS
#else
#define DATA_HEADER_RX SL_RANGE_TEST_SD_LOG_HEADER_RX_NO_GPS
#define DATA_HEADER_TX SL_RANGE_TEST_SD_LOG_HEADER_TX_NO_GPS
#endif

// States of the SD logging state machine
typedef enum {
  SD_LOG_STATE_WAIT_SD = 0,  // No SD card present, wait until one is inserted
  SD_LOG_STATE_IDLE,         // SD present, wait for a measurement to start
  SD_LOG_STATE_START,        // Open the log file and write the headers
  SD_LOG_STATE_LOGGING,      // Append a data line whenever a field changes
  SD_LOG_STATE_STOP          // Close the current log file
} sd_log_state_t;
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static bool sd_check_and_save_logging_fields_changed(void);
static int sd_prepare_log_header(void);
static int sd_prepare_log_data(void);
static sd_log_state_t sd_log_handle_idle(void);
static sd_log_state_t sd_log_handle_start(void);
static sd_log_state_t sd_log_handle_logging(void);
static sd_log_state_t sd_log_handle_stop(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// State machine variable to hold the current state of the Range Test
// application
extern state_t state;

// Base Range Test application settings
extern volatile range_test_settings_t range_test_settings;

// All data regarding the Range Test
extern volatile range_test_measurement_t range_test_measurement;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
#if SL_GPS_DRIVER_PRESENT
static gps_data_t gps_data;
#endif
// header string of the log file
static const char data_header_rx[] = DATA_HEADER_RX;
static const char data_header_tx[] = DATA_HEADER_TX;

// Top line of the Rx log file contains the settings
static const char settings_header_rx[] =
  SL_RANGE_TEST_SD_LOG_SETTINGS_HEADER_RX;
// Top line of the Tx log file contains the settings
static const char settings_header_tx[] =
  SL_RANGE_TEST_SD_LOG_SETTINGS_HEADER_TX;

// defines .csv file separator character
static const char separator[] = SL_RANGE_TEST_SD_LOG_SEPARATOR;

// current state of the SD logging state machine
static sd_log_state_t sd_log_state = SD_LOG_STATE_WAIT_SD;

// SD buffer used for logging
static char sd_log_buffer[SL_RANGE_TEST_SD_LOG_BUFFER_SIZE] = { 0 };

// Default log file name, will be used if no GPS time is available
static char log_file_name[] = SL_RANGE_TEST_DEFAULT_LOG_FILE_NAME_FORMAT;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for SD and GPS extension process handling.
 *****************************************************************************/
void app_process_action_range_test_log(void)
{
  // Losing the SD card at any point aborts logging and returns to WAIT_SD.
  if (!sd_is_present()) {
    if (sd_log_state != SD_LOG_STATE_WAIT_SD) {
      if (sd_stop_log_entry() != FR_OK) {
        app_log_warning("Stopping log failed!\n");
      }
      sd_log_state = SD_LOG_STATE_WAIT_SD;
    }
    return;
  }

  switch (sd_log_state) {
    case SD_LOG_STATE_WAIT_SD:
      // An SD card is now present, start waiting for a measurement.
      sd_log_state = SD_LOG_STATE_IDLE;
    case SD_LOG_STATE_IDLE:
      sd_log_state = sd_log_handle_idle();
      break;
    case SD_LOG_STATE_START:
      sd_log_state = sd_log_handle_start();
      break;
    case SD_LOG_STATE_LOGGING:
      sd_log_state = sd_log_handle_logging();
      break;
    case SD_LOG_STATE_STOP:
      sd_log_state = sd_log_handle_stop();
      break;
    default:
      sd_log_state = SD_LOG_STATE_WAIT_SD;
      break;
  }
}

SL_WEAK FRESULT sd_start_log_entry(const char *log_file_name)
{
  (void)log_file_name;
  return FR_DISK_ERR;
}

SL_WEAK FRESULT sd_stop_log_entry(void)
{
  return FR_DISK_ERR;
}

SL_WEAK FRESULT sd_log(const char *str, unsigned int len)
{
  (void)str;
  (void)len;
  return FR_DISK_ERR;
}

SL_WEAK bool sd_is_present(void)
{
  return false;
}

SL_WEAK void gps_handler_get_data(gps_data_t *data)
{
  (void)data;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Idle state handler: an SD card is present, wait until a measurement starts.
 * @returns The next state of the SD logging state machine.
 *****************************************************************************/
static sd_log_state_t sd_log_handle_idle(void)
{
  if ((range_test_settings.radio_mode == RADIO_MODE_RX)
      && (state == START_MEASUREMENT)) {
    return SD_LOG_STATE_START;
  } else if ((range_test_settings.radio_mode == RADIO_MODE_TX)
             && (state == SEND_MEASUREMENT)) {
    return SD_LOG_STATE_START;
  }
  return SD_LOG_STATE_IDLE;
}

/******************************************************************************
 * Start state handler: open a new log file and write the settings and data
 * headers.
 * @returns The next state of the SD logging state machine.
 *****************************************************************************/
static sd_log_state_t sd_log_handle_start(void)
{
#if SL_GPS_DRIVER_PRESENT
  gps_handler_get_data(&gps_data);
  snprintf(log_file_name, sizeof(log_file_name), "%6.0f.csv", gps_data.time);
#else
  snprintf(log_file_name, sizeof(log_file_name), "%.2s%.2s%.2s.csv",
         __TIME__, __TIME__ + 3, __TIME__ + 6);
#endif
  int len = sd_prepare_log_header();
  if ((len < 0) || ((size_t)len >= sizeof(sd_log_buffer))) {
    app_log_warning("Formatting log header failed!\n");
    return SD_LOG_STATE_IDLE;
  }
  if (sd_start_log_entry(log_file_name) != FR_OK) {
    return SD_LOG_STATE_IDLE;
  }

  bool successful_log = true;
  if (range_test_settings.radio_mode == RADIO_MODE_RX) {
    if (sd_log(separator, sizeof(separator) - 1) != FR_OK) {
      successful_log = false;
    } else if (sd_log(settings_header_rx,
                      sizeof(settings_header_rx) - 1) != FR_OK) {
      successful_log = false;
    } else if (sd_log(sd_log_buffer, (unsigned int)len) != FR_OK) {
      successful_log = false;
    } else if (sd_log(data_header_rx, sizeof(data_header_rx) - 1) != FR_OK) {
      successful_log = false;
    }
  } else {
    if (sd_log(separator, sizeof(separator) - 1) != FR_OK) {
      successful_log = false;
    } else if (sd_log(settings_header_tx,
                      sizeof(settings_header_tx) - 1) != FR_OK) {
      successful_log = false;
    } else if (sd_log(sd_log_buffer, (unsigned int)len) != FR_OK) {
      successful_log = false;
    } else if (sd_log(data_header_tx, sizeof(data_header_tx) - 1) != FR_OK) {
      successful_log = false;
    }
  }
  if (!successful_log) {
    app_log_warning("Starting log failed!\n");
    if (sd_stop_log_entry() != FR_OK) {
      app_log_warning("Stopping log failed!\n");
    }
    return SD_LOG_STATE_IDLE;
  }

  return SD_LOG_STATE_LOGGING;
}

/******************************************************************************
 * Logging state handler: append a data line whenever a logged field changes,
 * or hand over to the stop state when the menu is shown.
 * @returns The next state of the SD logging state machine.
 *****************************************************************************/
static sd_log_state_t sd_log_handle_logging(void)
{
  if (state == MENU_SCREEN) {
    return SD_LOG_STATE_STOP;
  }

  if (sd_check_and_save_logging_fields_changed()) {
#if SL_GPS_DRIVER_PRESENT
    gps_handler_get_data(&gps_data);
#endif
    int len = sd_prepare_log_data();
    if ((len < 0) || ((size_t)len >= sizeof(sd_log_buffer))) {
      app_log_warning("Formatting log data failed!\n");
      return SD_LOG_STATE_STOP;
    }
    if (sd_log(sd_log_buffer, (unsigned int)len) != FR_OK) {
      app_log_warning("Logging failed!\n");
      return SD_LOG_STATE_STOP;
    }
  }

  return SD_LOG_STATE_LOGGING;
}

/******************************************************************************
 * Stop state handler: close the current log file and return to idle.
 * @returns The next state of the SD logging state machine.
 *****************************************************************************/
static sd_log_state_t sd_log_handle_stop(void)
{
  if (sd_stop_log_entry() != FR_OK) {
    app_log_warning("Stopping log failed!\n");
  }
  return SD_LOG_STATE_IDLE;
}

/******************************************************************************
 * The function is used to check if any field of data logging has been changed,
 * so new log can be added. It also saves the new values to static variables for
 * next comparison.
 * @returns true if any field has been changed, false otherwise
 *****************************************************************************/
static bool sd_check_and_save_logging_fields_changed(void)
{
  static range_test_measurement_t last_measurement = { 0 };
  bool changed = false;

  // We don't want this field to trigger a new log entry, so we don't compare
  // it.
  last_measurement.tx_is_running = range_test_measurement.tx_is_running;

  if (memcmp(&last_measurement, &range_test_measurement,
             sizeof(range_test_measurement)) != 0) {
    changed = true;
    last_measurement = range_test_measurement;
  }

  return changed;
}

/******************************************************************************
 * The function is used to set the SD logging header format and data.
 * @returns The number of characters written to the log buffer.
 *****************************************************************************/
static int sd_prepare_log_header(void)
{
  if (range_test_settings.radio_mode == RADIO_MODE_RX) {
    return snprintf(sd_log_buffer, sizeof(sd_log_buffer),
                    "%u,%u,%u,%d,%u\n",
                    range_test_settings.current_phy,
                    range_test_settings.channel,
                    range_test_settings.payload_length,
                    range_test_settings.tx_power,
                    range_test_settings.moving_average_window_size);
  } else if (range_test_settings.radio_mode == RADIO_MODE_TX) {
    return snprintf(sd_log_buffer, sizeof(sd_log_buffer),
                    "%u,%u,%u,%d\n",
                    range_test_settings.current_phy,
                    range_test_settings.channel,
                    range_test_settings.payload_length,
                    range_test_settings.tx_power);
  } else {
    return -1;
  }
}

/******************************************************************************
 * The function is used to set the SD logging format and data based on GPS
 * presence.
 * @returns The number of characters written to the log buffer.
 *****************************************************************************/
static int sd_prepare_log_data(void)
{
#if SL_GPS_DRIVER_PRESENT
  if (range_test_settings.radio_mode == RADIO_MODE_RX) {
    return snprintf(sd_log_buffer, sizeof(sd_log_buffer),
                    "%5u/%5u,%d,%.1f,%.1f,%.6f,%.6f,%.0f,%.0f\n",
                    range_test_measurement.packets_received_correctly,
                    range_test_measurement.packets_received_counter,
                    range_test_measurement.rssi_latch_value,
                    range_test_measurement.moving_average,
                    range_test_measurement.PER,
                    gps_data.latitude,
                    gps_data.longitude,
                    gps_data.altitude,
                    gps_data.time);
  } else {
    return snprintf(sd_log_buffer, sizeof(sd_log_buffer),
                    "%5u,%.6f,%.6f,%.0f,%.0f\n",
                    range_test_measurement.packets_sent,
                    gps_data.latitude,
                    gps_data.longitude,
                    gps_data.altitude,
                    gps_data.time);
  }
#else
  if (range_test_settings.radio_mode == RADIO_MODE_RX) {
    return snprintf(sd_log_buffer, sizeof(sd_log_buffer),
                    "%5u/%5u,%d,%.1f,%.1f\n",
                    range_test_measurement.packets_received_correctly,
                    range_test_measurement.packets_received_counter,
                    range_test_measurement.rssi_latch_value,
                    range_test_measurement.moving_average,
                    range_test_measurement.PER);
  } else {
    return snprintf(sd_log_buffer, sizeof(sd_log_buffer),
                    "%5u\n",
                    range_test_measurement.packets_sent);
  }
#endif
}
