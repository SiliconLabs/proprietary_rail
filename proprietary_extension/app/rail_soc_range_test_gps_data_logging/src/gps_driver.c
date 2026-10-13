/***************************************************************************//**
 * @file
 * @brief gps_driver.c
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
#include "gps_driver.h"
#include "range_test_sd_log.h"

#include "app_queue.h"
#include "app_log.h"
#include "app_assert.h"

#include "sl_sleeptimer.h"
#include "sl_gps_driver_config.h"

#include "mikroe_lea6s.h"

#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void parser_application (void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// app queue to store the received GPS data
APP_QUEUE(uart_rx_queue, uint8_t, 2 * SL_GPS_DRIVER_PARSER_BUFFER_SIZE);
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// buffer to store the received GPS data
static uint8_t uart_rx_buffer[SL_GPS_DRIVER_UART_RX_BUFFER_SIZE];
// buffer used to parse GPS data
static uint8_t parser_buffer[SL_GPS_DRIVER_PARSER_BUFFER_SIZE];
// This variable stores the GPS data
static gps_data_t current_gps_data = { 0, 0, 0, 0 };

static sl_status_t status;

// GPS data variables used by the parser_application
static uint8_t latitude_degree_part[SL_GPS_DRIVER_PARSER_DATA_SIZE];
static uint8_t latitude_minute_part[SL_GPS_DRIVER_PARSER_DATA_SIZE];
static uint8_t longitude_degree_part[SL_GPS_DRIVER_PARSER_DATA_SIZE];
static uint8_t longitude_minute_part[SL_GPS_DRIVER_PARSER_DATA_SIZE];
// Buffer used by the parser_application
static uint8_t parsed_field_buffer[SL_GPS_DRIVER_PARSER_RESULT_BUFFER_SIZE];
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for GPS module initialization.
 *****************************************************************************/
void gps_handler_init(void)
{
  status = mikroe_lea6s_init(sl_iostream_uart_gps_handle, false);
  app_assert_status(status);
  status = APP_QUEUE_INIT(&uart_rx_queue,
                          uint8_t,
                          2 * SL_GPS_DRIVER_PARSER_BUFFER_SIZE);
  app_assert_status(status);
  mikroe_lea6s_wakeup();
  // Wait for the GPS module to wake up
  sl_sleeptimer_delay_millisecond(SL_GPS_DRIVER_WAKEUP_DELAY_MS);
}

/**************************************************************************//**
 * The function is used to retrieve the latest GPS data.
 *****************************************************************************/
void gps_handler_get_data(gps_data_t *data)
{
  data->latitude = current_gps_data.latitude;
  data->longitude = current_gps_data.longitude;
  data->altitude = current_gps_data.altitude;
  data->time = current_gps_data.time;
}

/**************************************************************************//**
 * The function is used for GPS handling.
 *****************************************************************************/
void gps_handler_process(void)
{
  // actual data size stored in uart_rx_buffer
  static uint16_t buffer_size = 0;
  // this variable stores the size read from the GPS
  uint16_t actual_size;

  status = mikroe_lea6s_generic_read(
    uart_rx_buffer + buffer_size,
    SL_GPS_DRIVER_UART_RX_BUFFER_SIZE - buffer_size,
    &actual_size);
  if (status == SL_STATUS_OK) {
    buffer_size += actual_size;
  }

  if (buffer_size >= SL_GPS_DRIVER_UART_RX_BUFFER_SIZE) {
    buffer_size = 0;
    // Validation of the received data and push data to the queue
    for (uint16_t i = 0; i < SL_GPS_DRIVER_UART_RX_BUFFER_SIZE; i++)
    {
      // Change '\0' character to '\r' for the parser
      if (uart_rx_buffer[i] == 0) {
        uart_rx_buffer[i] = '\r';
      }

      if (!app_queue_is_full(&uart_rx_queue)) {
        app_queue_add(&uart_rx_queue, (uart_rx_buffer + i));
      }
    }

    // Clear RX buffer
    memset(uart_rx_buffer, 0, SL_GPS_DRIVER_UART_RX_BUFFER_SIZE);
    parser_application();
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for GPS data parsing.
 *****************************************************************************/
static void parser_application(void)
{
  static uint16_t index = 0;
  mikroe_leas6_parser_result_t parse_status;

  double latitude = 0;
  double longitude = 0;
  float altitude = 0;

  while (!app_queue_is_empty(&uart_rx_queue)) {
    app_queue_remove(&uart_rx_queue, parser_buffer + index);

    if (*(parser_buffer + index) == '\n') {
      /*
       * Parse latitude.
       *
       * NMEA format:
       *   DDMM.MMMM
       *
       * Example:
       *   2451.7100,N
       *
       * Decimal degrees:
       *   24 + (51.7100 / 60) = 24.861833
       */
      parse_status = mikroe_lea6s_generic_parser(parser_buffer,
                                                 gps_command_nema_gpgga_e,
                                                 gpgga_element_latitude_e,
                                                 parsed_field_buffer);

      if ((strlen((const char *) parsed_field_buffer) > 2)
          && (gps_parser_no_error_e == parse_status)) {
        memcpy((void *)latitude_degree_part,
               (const void *)parsed_field_buffer,
               2);
        memcpy((void *)latitude_minute_part,
               (const void *)(parsed_field_buffer + 2),
               strlen((const char *)parsed_field_buffer) - 2);
        latitude = (atof((const char *)latitude_degree_part)
                    + (atof((const char *)(latitude_minute_part)) / 60.0));
        memset(parsed_field_buffer, 0, sizeof(parsed_field_buffer));

        /*
         * Parse latitude side.
         *
         * N = positive latitude
         * S = negative latitude
         */
        parse_status = mikroe_lea6s_generic_parser(parser_buffer,
                                                   gps_command_nema_gpgga_e,
                                                   gpgga_element_latitude_side_e,
                                                   parsed_field_buffer);
        if ((gps_parser_no_error_e == parse_status)
            && (strlen((const char *) parsed_field_buffer) == 1)
            && ((parsed_field_buffer[0] == 'N')
                || (parsed_field_buffer[0] == 'S'))) {
          if (parsed_field_buffer[0] == 'S') {
            latitude = -latitude;
          }
          memset(parsed_field_buffer, 0, sizeof(parsed_field_buffer));

          /*
           * Parse longitude.
           *
           * NMEA format:
           *   DDDMM.MMMM
           *
           * Example:
           *   12100.9900,E
           *
           * Decimal degrees:
           *   121 + (0.9900 / 60) = 121.016500
           */
          parse_status = mikroe_lea6s_generic_parser(parser_buffer,
                                                     gps_command_nema_gpgga_e,
                                                     gpgga_element_longitude_e,
                                                     parsed_field_buffer);
          if ((strlen((const char *) parsed_field_buffer) > 3)
              && (gps_parser_no_error_e == parse_status)) {
            memcpy((void *)longitude_degree_part,
                   (const void *)parsed_field_buffer,
                   3);
            memcpy((void *)longitude_minute_part,
                   (const void *) (parsed_field_buffer + 3),
                   strlen((const char *)parsed_field_buffer) - 3);
            longitude = (atof((const char *)longitude_degree_part)
                         + (atof((const char *)(longitude_minute_part))
                            / 60.0));
            memset(parsed_field_buffer, 0, sizeof(parsed_field_buffer));

            /*
             * Parse longitude side.
             *
             * E = positive longitude
             * W = negative longitude
             */
            parse_status = mikroe_lea6s_generic_parser(parser_buffer,
                                                       gps_command_nema_gpgga_e,
                                                       gpgga_element_longitude_side_e,
                                                       parsed_field_buffer);
            if ((gps_parser_no_error_e == parse_status)
                && (strlen((const char *)parsed_field_buffer) == 1)
                && ((parsed_field_buffer[0] == 'E')
                    || (parsed_field_buffer[0] == 'W'))) {
              if (parsed_field_buffer[0] == 'W') {
                longitude = -longitude;
              }
              memset(parsed_field_buffer, 0, sizeof(parsed_field_buffer));

              /*
               * Parse altitude.
               */
              parse_status = mikroe_lea6s_generic_parser(parser_buffer,
                                                         gps_command_nema_gpgga_e,
                                                         gpgga_element_altitude_e,
                                                         parsed_field_buffer);
              if ((strlen((const char *) parsed_field_buffer) > 0)
                  && (gps_parser_no_error_e == parse_status)) {
                altitude = atof((const char *)parsed_field_buffer);
                memset(parsed_field_buffer, 0, sizeof(parsed_field_buffer));

                /*
                 * Parse UTC time.
                 */
                parse_status = mikroe_lea6s_generic_parser(parser_buffer,
                                                           gps_command_nema_gpgga_e,
                                                           gpgga_element_time_e,
                                                           parsed_field_buffer);
                if ((strlen((const char *) parsed_field_buffer) > 0)
                    && (gps_parser_no_error_e == parse_status)) {
                  /*
                   * Only update current_gps_data after all
                   * required fields have been parsed successfully.
                   */
                  current_gps_data.time =
                    atof((const char *)parsed_field_buffer);
                  current_gps_data.altitude = altitude;
                  current_gps_data.longitude = longitude;
                  current_gps_data.latitude = latitude;
                }
              }
            }
          }
        }
      }
      index = 0;
      memset(parser_buffer, 0, SL_GPS_DRIVER_PARSER_BUFFER_SIZE);
    } else {
      if (index < (SL_GPS_DRIVER_PARSER_BUFFER_SIZE - 1)) {
        index++;
      }
    }
  }
  memset(latitude_degree_part, 0, SL_GPS_DRIVER_PARSER_DATA_SIZE);
  memset(latitude_minute_part, 0, SL_GPS_DRIVER_PARSER_DATA_SIZE);
  memset(longitude_degree_part, 0, SL_GPS_DRIVER_PARSER_DATA_SIZE);
  memset(longitude_minute_part, 0, SL_GPS_DRIVER_PARSER_DATA_SIZE);
  memset(parsed_field_buffer, 0, SL_GPS_DRIVER_PARSER_RESULT_BUFFER_SIZE);
}
