/***************************************************************************//**
 * @file
 * @brief IOSTREAM_USART Config.
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

#ifndef SL_IOSTREAM_USART_GPS_CONFIG_H
#define SL_IOSTREAM_USART_GPS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>USART settings

// <o SL_IOSTREAM_USART_GPS_BAUDRATE> Baud rate
// <i> Default: 115200
#define SL_IOSTREAM_USART_GPS_BAUDRATE              9600

// <o SL_IOSTREAM_USART_GPS_PARITY> Parity mode to use
// <usartNoParity=> No Parity
// <usartEvenParity=> Even parity
// <usartOddParity=> Odd parity
// <i> Default: usartNoParity
#define SL_IOSTREAM_USART_GPS_PARITY usartNoParity

// <o SL_IOSTREAM_USART_GPS_STOP_BITS> Number of stop bits to use.
// <usartStopbits0p5=> 0.5 stop bits
// <usartStopbits1=> 1 stop bits
// <usartStopbits1p5=> 1.5 stop bits
// <usartStopbits2=> 2 stop bits
// <i> Default: usartStopbits1
#define SL_IOSTREAM_USART_GPS_STOP_BITS usartStopbits1

// <o SL_IOSTREAM_USART_GPS_FLOW_CONTROL_TYPE> Flow control
// <usartHwFlowControlNone=> None
// <usartHwFlowControlCts=> CTS
// <usartHwFlowControlRts=> RTS
// <usartHwFlowControlCtsAndRts=> CTS/RTS
// <uartFlowControlSoftware=> Software Flow control (XON/XOFF)
// <i> Default: usartHwFlowControlNone
#define SL_IOSTREAM_USART_GPS_FLOW_CONTROL_TYPE \
  usartHwFlowControlNone

// <o SL_IOSTREAM_USART_GPS_RX_BUFFER_SIZE> Receive buffer size
// <i> Default: 32
#define SL_IOSTREAM_USART_GPS_RX_BUFFER_SIZE                             500

// <q SL_IOSTREAM_USART_GPS_CONVERT_BY_DEFAULT_LF_TO_CRLF> Convert \n to \r\n
// <i> It can be changed at runtime using the C API.
// <i> 0 means the conversion is disabled, 1 means it is enabled.
// <i> Default: 0
#define SL_IOSTREAM_USART_GPS_CONVERT_BY_DEFAULT_LF_TO_CRLF              0

// <q SL_IOSTREAM_USART_GPS_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION> Restrict
// the energy mode to allow the reception.
// <i> Default: 1
// <i> Limits the lowest energy mode the system can sleep to in order to keep
// the reception on. May cause higher power consumption.
#define SL_IOSTREAM_USART_GPS_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION    1

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <usart signal=TX,RX,(CTS),(RTS)> SL_IOSTREAM_USART_GPS
// $[USART_SL_IOSTREAM_USART_GPS]
#ifndef SL_IOSTREAM_USART_GPS_PERIPHERAL
#define SL_IOSTREAM_USART_GPS_PERIPHERAL      USART1
#endif
#ifndef SL_IOSTREAM_USART_GPS_PERIPHERAL_NO
#define SL_IOSTREAM_USART_GPS_PERIPHERAL_NO   1
#endif

// USART1 TX on PB03
#ifndef SL_IOSTREAM_USART_GPS_TX_PORT
#define SL_IOSTREAM_USART_GPS_TX_PORT         SL_GPIO_PORT_B
#endif
#ifndef SL_IOSTREAM_USART_GPS_TX_PIN
#define SL_IOSTREAM_USART_GPS_TX_PIN          3
#endif

// USART1 RX on PB02
#ifndef SL_IOSTREAM_USART_GPS_RX_PORT
#define SL_IOSTREAM_USART_GPS_RX_PORT         SL_GPIO_PORT_B
#endif
#ifndef SL_IOSTREAM_USART_GPS_RX_PIN
#define SL_IOSTREAM_USART_GPS_RX_PIN          2
#endif

// [USART_SL_IOSTREAM_USART_GPS]$
// <<< sl:end pin_tool >>>

#endif
