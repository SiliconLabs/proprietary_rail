/***************************************************************************//**
 * @file
 * @brief app_process.c
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
#include "sl_rail_util_init.h"

#include "app_log.h"
#include "app_assert.h"
#include "app_process.h"

#include "nvm3_default.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * Helper to compute the NVM3 key for a given config index.
 *
 * @param cfg_index Index into channelConfigs[], i.e. the config slot to
 * save/load.
 *
 * @return The NVM3 key to use for that slot's persisted calibration data.
 *****************************************************************************/
static uint32_t nvm3_key_for(uint8_t cfg_index);

#if SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL

/******************************************************************************
 * Calibrate one channelConfigs[] slot.
 *
 * @param rail_handle RAIL handle for the active radio instance.
 * @param cfg_index Index into channelConfigs[] to calibrate.
 *****************************************************************************/
static void calibrate_config(sl_rail_handle_t rail_handle, uint8_t cfg_index);

#endif // SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// Application state, including the necessary information to perform calibration
// and store the results.
app_state_t app_state = {
  .config_index = SL_IR_CALIBRATION_DEFAULT_CONFIG_INDEX,
  .channel = SL_IR_CALIBRATION_DEFAULT_CHANNEL,
  .ir_cal_pending = false,
};

// Global flag to enable automatic calibration, mutable by the CLI.
bool auto_ir_cal = SL_IR_CALIBRATION_AUTO_IR_CAL;

// Global flag to iterate all PHYs, mutable by the CLI.
bool iterate_all_phys = SL_IR_CALIBRATION_ITERATE_ALL_PHYS;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// Flag to signal a new calibration for the main loop when a IR calibration
// request event occurs.
static volatile bool cal_needed_notify = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(void)
{
  if (cal_needed_notify) {
    cal_needed_notify = false;
    sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
      SL_RAIL_UTIL_HANDLE_INST0);
    const sl_rail_cal_mask_t pending = sl_rail_get_pending_cal(rail_handle);

    if (pending & SL_RAIL_CAL_ONETIME_IR_CAL) {
#if SL_RAIL_SUPPORTS_OFDM_PA
      app_log("{{(ir_cal)}{CAL_NEEDED: %s%s}",
              (pending & SL_RAIL_CAL_OFDM_TX_IR_CAL) ? "OFDM_TX_IR_CAL " : "",
              (pending & SL_RAIL_CAL_RX_IR_CAL) ? "RX_IR_CAL" : "");
#else
      app_log("{{(ir_cal)}{CAL_NEEDED: %s}",
              (pending & SL_RAIL_CAL_RX_IR_CAL) ? "RX_IR_CAL" : "NONE");
#endif // SL_RAIL_SUPPORTS_OFDM_PA
      if (auto_ir_cal) {
        app_log(" -> auto-running sl_rail_calibrate_ir}\n");
        perform_ir_cal();
      } else {
        app_log(
          " on {cfg:%u}{ch:%u} (deferred - run `perform_ir_cal`)}\n",
          app_state.config_index,
          app_state.channel);
      }
    } else {
      // This means that the notification happened after the calibration
      // This is the case when set_iterate_all_phys is enabled
    }
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
SL_CODE_RAM void sl_rail_util_on_event(sl_rail_handle_t rail_handle,
                                       sl_rail_events_t events)
{
  if (events & SL_RAIL_EVENT_CAL_NEEDED) {
    const sl_rail_cal_mask_t pending = sl_rail_get_pending_cal(rail_handle);
    if (pending & SL_RAIL_CAL_TEMP) {
      sl_rail_status_t status = sl_rail_calibrate_temp(rail_handle);
      app_assert(status == SL_RAIL_STATUS_NO_ERROR,
                 "sl_rail_calibrate_temp failed with %X",
                 (unsigned) status);
    }
    if (pending & SL_RAIL_CAL_ONETIME_IR_CAL) {
      cal_needed_notify = true;
      app_state.ir_cal_pending = true;
    }
  }
}

void perform_ir_cal(void)
{
#if SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL
  sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
    SL_RAIL_UTIL_HANDLE_INST0);

  sl_rail_status_t status = sl_rail_idle(rail_handle, SL_RAIL_IDLE, true);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_idle failed: 0x%X\n",
             (unsigned)status);

  if (iterate_all_phys) {
    app_log("{{(ir_cal)}{iterating over all %u compiled-in PHYs}}\n",
            SL_IR_CALIBRATION_NUM_PROTOCOLS);
    const uint8_t saved_cfg_index = app_state.config_index;

    for (uint8_t i = 0; i < SL_IR_CALIBRATION_NUM_PROTOCOLS; i++) {
      status =
        sl_rail_config_channels(rail_handle,
                                (sl_rail_channel_config_t *)channelConfigs[i],
                                NULL);
      if (status != SL_RAIL_STATUS_NO_ERROR) {
        app_log(
          "{{(ir_cal)}{cfg %u: sl_rail_config_channels failed (0x%X), skip}}\n",
          i, (unsigned)status);
        continue;
      }
      status =
        sl_rail_prepare_channel(rail_handle,
                                ((sl_rail_channel_config_t *)channelConfigs[i])->p_entries->channel_number_start);
      if (status != SL_RAIL_STATUS_NO_ERROR) {
        app_log(
          "{{(ir_cal)}{cfg %u: sl_rail_prepare_channel failed (0x%X), skip}}\n",
          i, (unsigned)status);
        continue;
      }
      calibrate_config(rail_handle, i);
    }

    // Restore the user's selected config so Rx resumes where they expect.
    status =
      sl_rail_config_channels(
        rail_handle,
        (sl_rail_channel_config_t *)channelConfigs[saved_cfg_index],
        NULL);
    app_assert(status == SL_RAIL_STATUS_NO_ERROR,
               "sl_rail_config_channels failed: 0x%X\n",
               (unsigned)status);
  } else {
    calibrate_config(rail_handle, app_state.config_index);
  }

  app_state.ir_cal_pending = false;

  status = sl_rail_start_rx(rail_handle, app_state.channel, NULL);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_start_rx failed: 0x%X\n",
             (unsigned)status);
#else
  app_log("{{(ir_cal)}{perform_ir_cal disabled at compile time "
          "(SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL=0)}}\n");
#endif // SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL
}

void apply_ir_cal_values(void)
{
  sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
    SL_RAIL_UTIL_HANDLE_INST0);
  ir_cal_slot_t *slot = active_slot();

  sl_rail_status_t status =
    sl_rail_idle(rail_handle, SL_RAIL_IDLE, true);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_idle failed: 0x%X\n",
             (unsigned)status);

  sl_rail_antenna_sel_t rf_path = SL_RAIL_ANTENNA_AUTO;
  status = sl_rail_get_rf_path(rail_handle, &rf_path);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_get_rf_path failed: 0x%X\n",
             (unsigned)status);

  const sl_rail_time_t t0 = sl_rail_get_time(rail_handle);

  status = sl_rail_apply_ir_calibration(rail_handle,
                                        &slot->values,
                                        rf_path);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_apply_ir_calibration failed: 0x%X\n",
             (unsigned)status);

  const sl_rail_time_t t1 = sl_rail_get_time(rail_handle);

  slot->last_apply_us = (uint32_t)(t1 - t0);
  slot->calibrated = true;
  app_state.ir_cal_pending = false;

  app_log("{{(apply)}{cfg:%u}{rf_path:%u}{duration:%lu us}}\n",
          app_state.config_index,
          rf_path,
          slot->last_apply_us);

  status = sl_rail_start_rx(rail_handle, app_state.channel, NULL);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_start_rx failed: 0x%X\n",
             (unsigned)status);
}

bool save_ir_cal_values(void)
{
  ir_cal_slot_t *slot = active_slot();
  const uint32_t key = nvm3_key_for(app_state.config_index);

  sl_status_t status = nvm3_writeData(nvm3_defaultHandle, key,
                                      &slot->values, sizeof(slot->values));

  return (status == SL_STATUS_OK);
}

void load_ir_cal_values(void)
{
  sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
    SL_RAIL_UTIL_HANDLE_INST0);
  ir_cal_slot_t *slot = active_slot();
  const uint32_t key = nvm3_key_for(app_state.config_index);

  sl_rail_status_t status =
    sl_rail_idle(rail_handle, SL_RAIL_IDLE, true);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_idle failed: 0x%X\n",
             (unsigned)status);

  // Time the full "production" path: NVM lookup + read +
  // sl_rail_apply_ir_calibration.
  const sl_rail_time_t t0 = sl_rail_get_time(rail_handle);

  uint32_t type = 0;
  size_t length = 0;
  status = nvm3_getObjectInfo(nvm3_defaultHandle,
                              key,
                              &type,
                              &length);
  if ((status != SL_STATUS_OK)
      || (type != NVM3_OBJECTTYPE_DATA)
      || (length != sizeof(slot->values))) {
    app_log("{{(nvm3)}{no_record_at_key:0x%08lX}}\n", key);

    status = sl_rail_start_rx(rail_handle, app_state.channel, NULL);
    app_assert(status == SL_RAIL_STATUS_NO_ERROR,
               "sl_rail_start_rx failed: 0x%X\n",
               (unsigned)status);

    return;
  }
  status =
    nvm3_readData(nvm3_defaultHandle, key, &slot->values,
                  sizeof(slot->values));
  if (status != SL_STATUS_OK) {
    app_log("{{(nvm3)}{read_failed:0x%08lX}}\n", status);

    status = sl_rail_start_rx(rail_handle, app_state.channel, NULL);
    app_assert(status == SL_RAIL_STATUS_NO_ERROR,
               "sl_rail_start_rx failed: 0x%X\n",
               (unsigned)status);

    return;
  }

  status = sl_rail_apply_ir_calibration(rail_handle,
                                        &slot->values,
                                        SL_RAIL_ANTENNA_AUTO);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_apply_ir_calibration failed: 0x%X\n",
             (unsigned)status);

  const sl_rail_time_t t1 = sl_rail_get_time(rail_handle);
  slot->last_load_us = (uint32_t)(t1 - t0);
  slot->last_apply_us = slot->last_load_us;
  slot->calibrated = true;
  slot->loaded_from_nvm = true;
  app_state.ir_cal_pending = false;

  app_log(
    "{{(load)}{cfg:%u}{key:0x%08lX}{duration:%lu us (incl. NVM3 read)}}\n",
    app_state.config_index,
    key,
    slot->last_load_us);

  status = sl_rail_start_rx(rail_handle, app_state.channel, NULL);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_start_rx failed: 0x%X\n",
             (unsigned)status);

  return;
}

const char *cal_tag(const ir_cal_slot_t *slot)
{
  if (!slot->calibrated) {
    return "UNCAL";
  }
  return slot->loaded_from_nvm ? "CAL(NVM)" : "CAL";
}

ir_cal_slot_t *active_slot(void)
{
  uint8_t i = app_state.config_index;
  if (i >= SL_IR_CALIBRATION_NUM_PROTOCOLS) {
    i = 0;
  }
  return &app_state.slots[i];
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static uint32_t nvm3_key_for(uint8_t cfg_index)
{
  return SL_IR_CALIBRATION_NVM3_KEY_BASE + (uint32_t)cfg_index;
}

#if SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL
static void calibrate_config(sl_rail_handle_t rail_handle, uint8_t cfg_index)
{
  ir_cal_slot_t *slot = &app_state.slots[cfg_index];
  sl_rail_antenna_sel_t rf_path = SL_RAIL_ANTENNA_AUTO;
  sl_rail_status_t status = sl_rail_get_rf_path(rail_handle, &rf_path);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_get_rf_path failed: 0x%X\n",
             (unsigned)status);

  const sl_rail_time_t t0 = sl_rail_get_time(rail_handle);
  status = sl_rail_calibrate_ir(rail_handle,
                                &slot->values,
                                rf_path);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_calibrate_ir failed: 0x%X\n",
             (unsigned)status);
  const sl_rail_time_t t1 = sl_rail_get_time(rail_handle);
  const uint32_t total = (uint32_t)(t1 - t0);

  app_log("{{(ir_cal)}{cfg:%u}{rf_path:%u}{duration:%lu us}}}\n",
          cfg_index, rf_path, total);

  slot->last_cal_us = total;
  slot->calibrated = true;
  slot->loaded_from_nvm = false;

#if SL_RAIL_SUPPORTS_OFDM_PA
  app_log(
    "{{(ir_cal)}{cfg:%u}{rx_ir:(0x%08lX;0x%08lX)}{tx_ir:(dc=0x%08lX;phi=0x%08lX)}}\n",
    cfg_index,
    SL_RAIL_IR_CAL_VAL(slot->values, 0),
    SL_RAIL_IR_CAL_VAL(slot->values, 1),
    slot->values.tx_ir_cal_values.dc_offset_iq,
    slot->values.tx_ir_cal_values.phi_epsilon);
#else
  app_log(
    "{{(ir_cal)}{cfg:%u}{rx_ir:(0x%08lX;0x%08lX)}}\n",
    cfg_index,
    SL_RAIL_IR_CAL_VAL(slot->values, 0),
    SL_RAIL_IR_CAL_VAL(slot->values, 1));
#endif // SL_RAIL_SUPPORTS_OFDM_PA
}

#endif // SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL
