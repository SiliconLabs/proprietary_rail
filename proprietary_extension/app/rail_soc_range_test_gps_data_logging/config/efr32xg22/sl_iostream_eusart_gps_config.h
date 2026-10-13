/***************************************************************************//**
 * @file
 * @brief IOSTREAM_EUSART Config.
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

#ifndef SL_IOSTREAM_EUSART_GPS_CONFIG_H
#define SL_IOSTREAM_EUSART_GPS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> EUART settings

// <q SL_IOSTREAM_EUSART_GPS_ENABLE_HIGH_FREQUENCY> Enable High frequency
// mode
// <i> Default: 1
#define SL_IOSTREAM_EUSART_GPS_ENABLE_HIGH_FREQUENCY        1

// <o SL_IOSTREAM_EUSART_GPS_BAUDRATE> Baud rate
// <i> Default: 9600
#define SL_IOSTREAM_EUSART_GPS_BAUDRATE                     9600

// <o SL_IOSTREAM_EUSART_GPS_PARITY> Parity mode to use
// <SL_IOSTREAM_EUSART_UART_NO_PARITY=> No Parity
// <SL_IOSTREAM_EUSART_UART_EVEN_PARITY=> Even parity
// <SL_IOSTREAM_EUSART_UART_ODD_PARITY=> Odd parity
// <i> Default: SL_IOSTREAM_EUSART_UART_NO_PARITY
#define SL_IOSTREAM_EUSART_GPS_PARITY SL_IOSTREAM_EUSART_UART_NO_PARITY

// <o SL_IOSTREAM_EUSART_GPS_STOP_BITS> Number of stop bits to use.
// <SL_IOSTREAM_EUSART_UART_STOP_BITS_0P5=> 0.5 stop bits
// <SL_IOSTREAM_EUSART_UART_STOP_BITS_1=> 1 stop bits
// <SL_IOSTREAM_EUSART_UART_STOP_BITS_1P5=> 1.5 stop bits
// <SL_IOSTREAM_EUSART_UART_STOP_BITS_2=> 2 stop bits
// <i> Default: SL_IOSTREAM_EUSART_UART_STOP_BITS_1
#define SL_IOSTREAM_EUSART_GPS_STOP_BITS SL_IOSTREAM_EUSART_UART_STOP_BITS_1

// <o SL_IOSTREAM_EUSART_GPS_FLOW_CONTROL_TYPE> Flow control
// <SL_IOSTREAM_EUSART_UART_FLOW_CTRL_NONE=> None
// <SL_IOSTREAM_EUSART_UART_FLOW_CTRL_CTS=> CTS
// <SL_IOSTREAM_EUSART_UART_FLOW_CTRL_RTS=> RTS
// <SL_IOSTREAM_EUSART_UART_FLOW_CTRL_CTS_RTS=> CTS/RTS
// <SL_IOSTREAM_EUSART_UART_FLOW_CTRL_SOFT=> Software Flow control (XON/XOFF)
// <i> Default: SL_IOSTREAM_EUSART_UART_FLOW_CTRL_NONE
#define SL_IOSTREAM_EUSART_GPS_FLOW_CONTROL_TYPE \
  SL_IOSTREAM_EUSART_UART_FLOW_CTRL_NONE

// <o SL_IOSTREAM_EUSART_GPS_RX_BUFFER_SIZE> Receive buffer size in bytes
// <i> Default: 500
#define SL_IOSTREAM_EUSART_GPS_RX_BUFFER_SIZE                             500

// <q SL_IOSTREAM_EUSART_GPS_CONVERT_BY_DEFAULT_LF_TO_CRLF> Convert \n to
// \r\n
// <i> It can be changed at runtime using the C API.
// <i> 0 means the conversion is disabled, 1 means it is enabled.
// <i> Default: 0
#define SL_IOSTREAM_EUSART_GPS_CONVERT_BY_DEFAULT_LF_TO_CRLF              0

// <q SL_IOSTREAM_EUSART_GPS_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION>
// Restrict the energy mode to allow the reception.
// <i> Default: 1
// <i> Limits the lowest energy mode the system can sleep to in order to keep
// the reception on. May cause higher power consumption.
#define SL_IOSTREAM_EUSART_GPS_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION    1

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <eusart signal=TX,RX,(CTS),(RTS)> SL_IOSTREAM_EUSART_GPS
// $[EUSART_SL_IOSTREAM_EUSART_GPS]
#ifndef SL_IOSTREAM_EUSART_GPS_PERIPHERAL
#define SL_IOSTREAM_EUSART_GPS_PERIPHERAL     EUART0
#endif
#ifndef SL_IOSTREAM_EUSART_GPS_PERIPHERAL_NO
#define SL_IOSTREAM_EUSART_GPS_PERIPHERAL_NO  0
#endif

// EUART0 TX on PD03
#ifndef SL_IOSTREAM_EUSART_GPS_TX_PORT
#define SL_IOSTREAM_EUSART_GPS_TX_PORT        SL_GPIO_PORT_D
#endif
#ifndef SL_IOSTREAM_EUSART_GPS_TX_PIN
#define SL_IOSTREAM_EUSART_GPS_TX_PIN         3
#endif

// EUART0 RX on PD02
#ifndef SL_IOSTREAM_EUSART_GPS_RX_PORT
#define SL_IOSTREAM_EUSART_GPS_RX_PORT        SL_GPIO_PORT_D
#endif
#ifndef SL_IOSTREAM_EUSART_GPS_RX_PIN
#define SL_IOSTREAM_EUSART_GPS_RX_PIN         2
#endif

// [EUSART_SL_IOSTREAM_EUSART_GPS]$
// <<< sl:end pin_tool >>>

#endif
