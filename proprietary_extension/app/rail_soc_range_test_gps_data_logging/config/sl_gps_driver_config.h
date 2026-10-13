/***************************************************************************//**
 * @file sl_gps_driver_config.h
 * @brief sl_gps_driver_config.h
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
 ******************************************************************************
 * # Experimental Quality
 * This code has not been formally tested and is provided as-is. It is not
 * suitable for production environments. In addition, this code will not be
 * maintained and there may be no bug maintenance planned for these resources.
 * Silicon Labs may update projects from time to time.
 ******************************************************************************/

#ifndef SL_GPS_DRIVER_CONFIG_H_
#define SL_GPS_DRIVER_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

// <<< Use Configuration Wizard in Context Menu

// <h>Driver settings

// <o SL_GPS_DRIVER_UART_RX_BUFFER_SIZE> Rx buffer size
// <i> Set the Rx buffer size in bytes
// <i> Default: 500
#define SL_GPS_DRIVER_UART_RX_BUFFER_SIZE                              500

// <o SL_GPS_DRIVER_PARSER_BUFFER_SIZE> Parser buffer size
// <i> Set the parser buffer size in bytes.
// <i> Should be the same as Rx buffer size.
// <i> Default: 500
#define SL_GPS_DRIVER_PARSER_BUFFER_SIZE                               500

// <o SL_GPS_DRIVER_PARSER_RESULT_BUFFER_SIZE> Parser result buffer size
// <i> Set the parser result buffer size in bytes.
// <i> Default: 200
#define SL_GPS_DRIVER_PARSER_RESULT_BUFFER_SIZE                        200

// <o SL_GPS_DRIVER_PARSER_DATA_SIZE> Parser data size
// <i> Set the parser data size in bytes.
// <i> Default: 10
#define SL_GPS_DRIVER_PARSER_DATA_SIZE                                 10

// <o SL_GPS_DRIVER_WAKEUP_DELAY_MS> GPS Wakeup time
// <i> Set the GPS wakeup time in milliseconds.
// <i> Default: 5000
#define SL_GPS_DRIVER_WAKEUP_DELAY_MS                                  5000

// </h> end Static Variables

// <<< end of configuration section >>>

#ifdef __cplusplus
}
#endif

#endif // SL_GPS_DRIVER_CONFIG_H_
