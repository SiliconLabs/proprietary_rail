/***************************************************************************//**
 *  @file rail_config.h
 * @brief RAIL Configuration
 * @details
 *   WARNING: Auto-Generated Radio Config Header  -  DO NOT EDIT
 *   Radio Configurator Version: x.y.z
 *   RAIL Adapter Version: x.y.z
 *   RAIL Compatibility: x.y
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __RAIL_CONFIG_H__
#define __RAIL_CONFIG_H__

#include <stdint.h>
#include "rail_types.h"

#define PHY_ACCELERATION_BUFFER phyAccelerationBuffer
extern uint32_t phyAccelerationBuffer[];

#define RADIO_CONFIG_XTAL_FREQUENCY 38400000UL

#define RAIL0_PHY_LONGRANGE_490M_OQPSK_DSSS8_9P6KBPS_PHY_LONGRANGE_490M_OQPSK_DSSS8_9P6KBPS
#define RAIL0_PHY_LONGRANGE_490M_OQPSK_DSSS8_9P6KBPS_PROFILE_LONG_RANGE
extern const RAIL_ChannelConfig_t *channelConfigs[];

#endif // __RAIL_CONFIG_H__
