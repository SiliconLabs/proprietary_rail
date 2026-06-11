/**
 * @file
 * @brief app_cli.c
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
#include "app_process.h"
#include "app_assert.h"

#include "sl_cli_instances.h"
#include "sl_rail_util_init.h"

#include "rail_config.h"

#include "response_print.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void cli_perform_ir_cal(sl_cli_command_arg_t *args)
{
  (void) args;
  perform_ir_cal();
}

void cli_get_pending_ir_cal(sl_cli_command_arg_t *args)
{
  sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
    SL_RAIL_UTIL_HANDLE_INST0);
  const sl_rail_cal_mask_t pending = sl_rail_get_pending_cal(rail_handle);
#if SL_RAIL_SUPPORTS_OFDM_PA
  if ((pending & SL_RAIL_CAL_OFDM_TX_IR_CAL)
      || (pending & SL_RAIL_CAL_RX_IR_CAL)) {
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "pending:%s%s",
      (pending & SL_RAIL_CAL_OFDM_TX_IR_CAL) ? "OFDM_TX_IR_CAL " : "",
      (pending & SL_RAIL_CAL_RX_IR_CAL) ? "RX_IR_CAL" : "");
  } else {
    responsePrint(
      sl_cli_get_command_string(args, 0), "pending:NONE");
  }
#else
  responsePrint(
    sl_cli_get_command_string(args, 0),
    "pending:%s",
    (pending & SL_RAIL_CAL_RX_IR_CAL) ? "RX_IR_CAL" : "NONE");
#endif // SL_RAIL_SUPPORTS_OFDM_PA
}

void cli_apply_ir_cal_values(sl_cli_command_arg_t *args)
{
  if ((sl_cli_get_argument_count(args) != 2)
      && (sl_cli_get_argument_count(args) != 4)) {
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "Error:Invalid number of arguments for calibration");
    return;
  }

  if (!sl_rail_supports_ofdm_pa(sl_rail_util_get_handle(
                                  SL_RAIL_UTIL_HANDLE_INST0))) {
    if (sl_cli_get_argument_count(args) > 2) {
      responsePrint(
        sl_cli_get_command_string(args, 0),
        "Error:Too many arguments for this part (no OFDM-PA support)");
      return;
    }
  }

  ir_cal_slot_t *slot = active_slot();
  SL_RAIL_IR_CAL_VAL(slot->values, 0) = sl_cli_get_argument_uint32(args, 0);
  SL_RAIL_IR_CAL_VAL(slot->values, 1) = sl_cli_get_argument_uint32(args, 1);
#if SL_RAIL_SUPPORTS_OFDM_PA
  if (sl_cli_get_argument_count(args) == 4) {
    slot->values.tx_ir_cal_values.dc_offset_iq =
      sl_cli_get_argument_uint32(args, 2);
    slot->values.tx_ir_cal_values.phi_epsilon =
      sl_cli_get_argument_uint32(args, 3);
  }
#endif // SL_RAIL_SUPPORTS_OFDM_PA

  apply_ir_cal_values();
}

void cli_get_ir_cal_values(sl_cli_command_arg_t *args)
{
  responsePrintStart(sl_cli_get_command_string(args, 0));
#if SL_RAIL_SUPPORTS_OFDM_PA
  responsePrintContinue(
    "rx_ir:{0x%08lX;0x%08lX},tx_ir:{dc=0x%08lX;phi=0x%08lX}",
    (sl_rail_channel_config_t *)channelConfigs[app_state.config_index]->configs->attr->calValues.rxIrCalValues[
      0],
    (sl_rail_channel_config_t *)channelConfigs[app_state.config_index]->configs->attr->calValues.rxIrCalValues[
      1],
    (sl_rail_channel_config_t *)channelConfigs[app_state.config_index]->configs->attr->calValues.txIrCalValues.dcOffsetIQ,
    (sl_rail_channel_config_t *)channelConfigs[app_state.config_index]->configs->attr->calValues.txIrCalValues.phiEpsilon);
#else
  responsePrintContinue(
    "rx_ir:{0x%08lX;0x%08lX}",
    (sl_rail_channel_config_t *)channelConfigs[app_state.config_index]->configs->attr->calValues[
      0],
    (sl_rail_channel_config_t *)channelConfigs[app_state.config_index]->configs->attr->calValues[
      1]);
#endif // SL_RAIL_SUPPORTS_OFDM_PA
  responsePrintEnd("}");
}

void cli_save_ir_cal_values(sl_cli_command_arg_t *args)
{
  if (save_ir_cal_values()) {
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "saved cfg to NVM3:%u",
      app_state.config_index);
  } else {
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "failed to save cfg to NVM3:%u",
      app_state.config_index);
  }
}

void cli_load_ir_cal_values(sl_cli_command_arg_t *args)
{
  (void) args;
  load_ir_cal_values();
}

void cli_set_channel(sl_cli_command_arg_t *args)
{
  sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
    SL_RAIL_UTIL_HANDLE_INST0);
  uint16_t channel_tmp = sl_cli_get_argument_uint16(args, 0);
  sl_rail_status_t status = sl_rail_is_valid_channel(rail_handle,
                                                     channel_tmp);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "invalid channel:%u",
      channel_tmp);
  } else {
    app_state.channel = channel_tmp;
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "channel:%u",
      app_state.channel);
  }

  status = sl_rail_start_rx(rail_handle, app_state.channel, NULL);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_start_rx failed: 0x%X\n",
             (unsigned)status);
}

void cli_get_channel(sl_cli_command_arg_t *args)
{
  responsePrint(
    sl_cli_get_command_string(args, 0),
    "channel:%u",
    app_state.channel);
}

void cli_set_configindex(sl_cli_command_arg_t *args)
{
  uint8_t index = sl_cli_get_argument_uint8(args, 0);

  // Be sure that the proposed index is valid. Scan through all possible
  // indexes and check for the last NULL parameter since you can't
  // use sizeof on an extern-ed array without an explicit index.
  for (uint8_t i = 0; i <= index; i++) {
    if (channelConfigs[i] == NULL) {
      responsePrintError(sl_cli_get_command_string(args, 0),
                         0x11,
                         "Invalid radio config index:%u",
                         index);
      return;
    }
  }

  sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
    SL_RAIL_UTIL_HANDLE_INST0);

  sl_rail_status_t status =
    sl_rail_config_channels(rail_handle,
                            (sl_rail_channel_config_t *)channelConfigs[index],
                            NULL);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    responsePrintError(
      sl_cli_get_command_string(args, 0),
      0x11,
      "Could not set radio config index:%u",
      index);
    return;
  }
  app_state.config_index = index;
  responsePrint(sl_cli_get_command_string(args, 0), "active config:%u", index);

  // If the current channel is not present in the new config we need to set it
  // to a valid value
  status = sl_rail_is_valid_channel(rail_handle, app_state.channel);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    app_state.channel =
      channelConfigs[index]->configs->channelNumberStart;
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "the active channel is changed to:%u",
      app_state.channel);
  }

  // After switching, the new PHY will need its own IR calibration the next time
  // we go on-air. Whether that runs automatically or stays deferred is governed
  // by auto_ir_cal.
  status = sl_rail_start_rx(rail_handle, app_state.channel, NULL);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_start_rx failed: 0x%X\n",
             (unsigned)status);
}

void cli_get_configindex(sl_cli_command_arg_t *args)
{
  responsePrint(
    sl_cli_get_command_string(args, 0),
    "config index:%u\n",
    app_state.config_index);
}

void cli_set_auto_ir_cal(sl_cli_command_arg_t *args)
{
  auto_ir_cal = sl_cli_get_argument_uint8(args, 0) != 0;
  responsePrint(
    sl_cli_get_command_string(args, 0),
    "auto_ir_cal:%s",
    auto_ir_cal ? "on" : "off");
  if (auto_ir_cal && app_state.ir_cal_pending) {
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "servicing pending IR calibration request now:");
    perform_ir_cal();
  }
}

void cli_get_auto_ir_cal(sl_cli_command_arg_t *args)
{
  responsePrint(
    sl_cli_get_command_string(args, 0),
    "auto_ir_cal:%s",
    auto_ir_cal ? "on" : "off");
}

void cli_set_iterate_all_phys(sl_cli_command_arg_t *args)
{
  if (sl_cli_get_argument_uint8(args, 0) != 0) {
    iterate_all_phys = true;
  } else {
    iterate_all_phys = false;
  }
  responsePrint(
    sl_cli_get_command_string(args, 0),
    "iterate_all_phys:%s",
    iterate_all_phys ? "on" : "off");
}

void cli_get_iterate_all_phys(sl_cli_command_arg_t *args)
{
  responsePrint(
    sl_cli_get_command_string(args, 0),
    "iterate_all_phys:%s",
    iterate_all_phys ? "on" : "off");
}

void cli_status(sl_cli_command_arg_t *args)
{
  responsePrintStart(sl_cli_get_command_string(args, 0));
  responsePrintContinue(
    "current_cfg:%u,current_ch:%u,auto_ir_cal:%s,iterate_all_phys:%s,ir_cal_pending:%s",
    app_state.config_index,
    app_state.channel,
    auto_ir_cal ? "on" : "off",
    iterate_all_phys ? "on" : "off",
    app_state.ir_cal_pending ? "yes" : "no");
  app_log("\n");
  for (uint8_t i = 0; i < SL_IR_CALIBRATION_NUM_PROTOCOLS; i++) {
    const ir_cal_slot_t *s = &app_state.slots[i];
    responsePrintContinue(
      "cfg:%u %s,cal:%lu us,apply:%lu us,load:%lu us",
      i, cal_tag(s),
      s->last_cal_us,
      s->last_apply_us,
      s->last_load_us);
    app_log("\n");
#if SL_RAIL_SUPPORTS_OFDM_PA
    responsePrintContinue(
      "rx_ir:{0x%08lX;0x%08lX},tx_ir:{dc=0x%08lX;phi=0x%08lX}",
      SL_RAIL_IR_CAL_VAL(s->values, 0),
      SL_RAIL_IR_CAL_VAL(s->values, 1),
      s->values.tx_ir_cal_values.dc_offset_iq,
      s->values.tx_ir_cal_values.phi_epsilon);
#else
    responsePrintContinue(
      "rx_ir:{0x%08lX;0x%08lX}",
      SL_RAIL_IR_CAL_VAL(s->values, 0),
      SL_RAIL_IR_CAL_VAL(s->values, 1));
#endif // SL_RAIL_SUPPORTS_OFDM_PA
    app_log("\n");
  }
  responsePrintEnd("}");
}

void cli_get_rssi(sl_cli_command_arg_t *args)
{
  sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
    SL_RAIL_UTIL_HANDLE_INST0);
  int16_t rssi_q =
    sl_rail_get_rssi(rail_handle, SL_RAIL_GET_RSSI_NO_WAIT);

  // The lowest negative value is used to indicate an error reading the
  // RSSI
  if (rssi_q == SL_RAIL_RSSI_INVALID) {
    responsePrintError(
      sl_cli_get_command_string(args, 0),
      0x08,
      "Could not read RSSI:");
    return;
  }

  responsePrint(
    sl_cli_get_command_string(args, 0),
    "rssi:%d",
    rssi_q / 4);
}

void cli_get_rf_path(sl_cli_command_arg_t *args)
{
  sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
    SL_RAIL_UTIL_HANDLE_INST0);
  sl_rail_antenna_sel_t rf_path = SL_RAIL_ANTENNA_AUTO;
  sl_rail_status_t status = sl_rail_get_rf_path(rail_handle, &rf_path);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_get_rf_path failed: 0x%X\n",
             (unsigned)status);
  responsePrint(sl_cli_get_command_string(args, 0), "rf_path:%u", rf_path);
}

void cli_set_rf_path(sl_cli_command_arg_t *args)
{
  sl_rail_handle_t rail_handle = sl_rail_util_get_handle(
    SL_RAIL_UTIL_HANDLE_INST0);
  sl_rail_antenna_sel_t rf_path =
    (sl_rail_antenna_sel_t)sl_cli_get_argument_uint8(args, 0);

  sl_rail_antenna_config_t antenna_config;

  sl_rail_status_t status = sl_rail_idle(rail_handle, SL_RAIL_IDLE, true);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_idle failed: 0x%X\n",
             (unsigned)status);

  antenna_config.default_rf_path = rf_path;
  status = sl_rail_config_antenna(rail_handle, &antenna_config);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_config_antenna failed: 0x%X\n",
             (unsigned)status);

  sl_rail_antenna_sel_t rf_path_configured;
  status = sl_rail_get_rf_path(rail_handle, &rf_path_configured);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_get_rf_path failed: 0x%X\n",
             (unsigned)status);
  if (rf_path_configured != rf_path) {
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "Failed to set rf_path:%u,active rf_path:%u",
      rf_path, rf_path_configured);
  } else {
    responsePrint(
      sl_cli_get_command_string(args, 0),
      "rf_path:%u",
      rf_path);
  }
  status = sl_rail_start_rx(rail_handle, app_state.channel, NULL);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_start_rx failed: 0x%X\n",
             (unsigned)status);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
