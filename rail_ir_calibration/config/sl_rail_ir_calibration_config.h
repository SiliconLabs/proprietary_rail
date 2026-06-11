/***************************************************************************//**
 * @file
 * @brief RAIL Image Rejection Calibration Example Config
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

#ifndef SL_RAIL_IR_CALIBRATION_CONFIG_H
#define SL_RAIL_IR_CALIBRATION_CONFIG_H

// <<< Use Configuration Wizard in Context Menu

// <h>Packet Settings

// <o SL_IR_CALIBRATION_DEFAULT_CONFIG_INDEX> Default config index
// <i> Set the index of the default config used to calibrate
// <i> Default: 0
#define SL_IR_CALIBRATION_DEFAULT_CONFIG_INDEX      0

// <o SL_IR_CALIBRATION_DEFAULT_CHANNEL> Default channel
// <i> Set the index of the default channel used to calibrate
// <i> Default: 0
#define SL_IR_CALIBRATION_DEFAULT_CHANNEL           0

// <o SL_IR_CALIBRATION_NUM_PROTOCOLS> Number of protocols
// <i> Set the number of protocols that is present in the radio configuration.
// <i> One per band you intend to use.
// <i> Default: 2
#define SL_IR_CALIBRATION_NUM_PROTOCOLS             2

// <o SL_IR_CALIBRATION_ITERATE_ALL_PHYS> Iterate all PHYs
// <i> Boot value of the global `iterate_all_phys` flag. When true,
// <i> `perform_ir_cal` walks every channelConfigs[] entry, switches into it,
// <i> and runs the cal. When false, only the currently active PHY is
// <i> calibrated. The flag is mutable at runtime (`set_iterate_all_phys 0|1`).
// <i> Default: false
#define SL_IR_CALIBRATION_ITERATE_ALL_PHYS          false

// <o SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL> Enable perform IR calibration <i>
// <i> This switch brackets the RAIL IR calibration entry point so the linker
// <i> can dead-strip the calibration engine when it is disabled. The point of
// <i> the demo is to contrast "compute fresh values on the device" with "load
// <i> them from NVM3 and apply", so only the calibrate-side calls are gated.
// <i> The sl_rail_apply_ir_calibration() / NVM3 paths are always linked.
// <i> Default: true
#define SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL      true

// <o SL_IR_CALIBRATION_AUTO_IR_CAL> Enable automatic IR calibration
// <i> Default value of the global `auto_ir_cal` flag.
// <i> When true, SL_RAIL_EVENT_CAL_NEEDED runs sl_rail_calibrate_ir() right
// away, the user is only notified.
// <i> When false, CAL_NEEDED is logged but deferred. The user runs
// <i> `perform_ir_cal` (or presses the trigger button) when they are ready.
// <i> `false` is what makes the before/after RSSI comparison meaningful.
// <i> Default: false
#define SL_IR_CALIBRATION_AUTO_IR_CAL                false

// <o SL_IR_CALIBRATION_NVM3_KEY_BASE> Base NVM3 key
// <i> Set the base NVM3 key for storing calibration data.
// <i> Each protocol slot uses `SL_IR_CALIBRATION_NVM3_KEY_BASE + index`
// <i> Default: 0x0000BEE0
#define SL_IR_CALIBRATION_NVM3_KEY_BASE              0x0000BEE0UL

// </h> end Static Variables

// <<< end of configuration section >>>

#endif // SL_RAIL_IR_CALIBRATION_CONFIG_H
