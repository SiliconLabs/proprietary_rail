/***************************************************************************//**
 * @file
 * @brief app_process.h
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

#ifndef APP_PROCESS_H
#define APP_PROCESS_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_rail.h"
#include "sl_rail_ir_calibration_config.h"

#include "rail_config.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
// One persisted IR calibration record per `channelConfigs[]` entry.
typedef struct {
  sl_rail_ir_cal_values_t values; // Last computed or loaded blob.
  uint32_t last_cal_us; // Most recent sl_rail_calibrate_ir() duration.
  uint32_t last_apply_us; // Most recent sl_rail_apply_ir_calibration()
                          // duration.
  uint32_t last_load_us; // Most recent NVM-load + apply duration.
  bool calibrated; // Hardware currently runs with these values.
  bool loaded_from_nvm; // Source of the active values.
} ir_cal_slot_t;

// The application state, including the necessary information to perform
// calibration and store the results.
typedef struct {
  ir_cal_slot_t slots[SL_IR_CALIBRATION_NUM_PROTOCOLS];
  uint8_t config_index; // Active `channelConfigs[]` index.
  uint16_t channel; // Active channel within active config.
  bool ir_cal_pending; // RAIL has an unhandled IR calibration request.
} app_state_t;
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
extern app_state_t app_state;
extern bool auto_ir_cal;
extern bool iterate_all_phys;
// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * The function is used for Application logic.
 * It is called infinitely.
 *****************************************************************************/
void app_process_action(void);

/**************************************************************************//**
 * Run sl_rail_calibrate_ir() against the *current* channel/protocol only,
 * or every PHY when `iterate_all_phys` is true.
 *****************************************************************************/
void perform_ir_cal(void);

/**************************************************************************//**
 * Wrapper around sl_rail_apply_ir_calibration() for the current protocol.
 *****************************************************************************/
void apply_ir_cal_values(void);

/**************************************************************************//**
 * Persist the active slot (active configuration) to NVM3 (key = base +
 * config_index).
 *
 * @return true if the save was successful, false otherwise.
 *****************************************************************************/
bool save_ir_cal_values(void);

/**************************************************************************//**
 * Load the active slot from NVM3 and immediately apply it. Timed end-to-end.
 *****************************************************************************/
void load_ir_cal_values(void);

/**************************************************************************//**
 * Helper to print a tag for a slot's calibration state in the CLI status.
 * "UNCAL" if never calibrated, "CAL(NVM)" if loaded from NVM, "CAL" if
 * freshly calibrated.
 *
 * @param slot The slot to query.
 * @return A string tag for that slot's state.
 *****************************************************************************/
const char *cal_tag(const ir_cal_slot_t *slot);

/**************************************************************************//**
 * Helper to get a pointer to the active slot based on app_state.config_index.
 *
 * @return Pointer to the active slot.
 *****************************************************************************/
ir_cal_slot_t *active_slot(void);

#endif // APP_PROCESS_H
