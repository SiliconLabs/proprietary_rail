/***************************************************************************//**
 * @file mikroe_gps_lea6s_config.h
 * @brief mikroe_gps_lea6s_config.h
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

#ifndef MIKROE_GPS_LEA6S_CONFIG_H_
#define MIKROE_GPS_LEA6S_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

// <<< sl:start pin_tool >>>

// <gpio optional=true> MIKROE_LEAS6_RESET
// $[GPIO_MIKROE_LEAS6_RESET]
#ifndef MIKROE_LEAS6_RESET_PORT
#define MIKROE_LEAS6_RESET_PORT                  SL_GPIO_PORT_D
#endif
#ifndef MIKROE_LEAS6_RESET_PIN
#define MIKROE_LEAS6_RESET_PIN                   4
#endif
// [GPIO_MIKROE_LEAS6_RESET]$

// <gpio optional=true> MIKROE_LEAS6_TIMEPULSE
// $[GPIO_MIKROE_LEAS6_TIMEPULSE]
#ifndef MIKROE_LEAS6_TIMEPULSE_PORT
#define MIKROE_LEAS6_TIMEPULSE_PORT              SL_GPIO_PORT_D
#endif
#ifndef MIKROE_LEAS6_TIMEPULSE_PIN
#define MIKROE_LEAS6_TIMEPULSE_PIN               5
#endif
// [GPIO_MIKROE_LEAS6_TIMEPULSE]$

// <<< sl:end pin_tool >>>

#ifdef __cplusplus
}
#endif

#endif // MIKROE_GPS_LEA6S_CONFIG_H_
