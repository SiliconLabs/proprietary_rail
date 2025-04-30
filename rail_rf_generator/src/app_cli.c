/**
 * @file
 * @brief app_cli.c
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_cli_instances.h"
#include "sl_rail_util_init.h"
#include "response_print.h"
#include "app_init.h"
#include "app_log.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void print_packet(uint8_t packet_id);

static bool validate_packet(uint8_t packet_id);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
extern bool tx_log_enable;

extern bool tx_toggle_flag;

extern packet_t packets[SL_RF_GENERATOR_NUMBER_OF_PACKETS];
extern packet_sequence_element_t sequence[SL_RF_GENERATOR_SEQUENCE_LENGTH];

extern uint8_t sequence_length;
extern uint32_t repeats;
extern state_t state;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void cli_tx(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  tx_toggle_flag = true;
}

void cli_tx_log(sl_cli_command_arg_t *arguments)
{
  if (sl_cli_get_argument_count(arguments) == 1) {
    tx_log_enable = sl_cli_get_argument_uint8(arguments, 0);
  } else {
    tx_log_enable = true;
  }
  responsePrint(sl_cli_get_command_string(arguments, 0),
                "Tx log: %s",
                tx_log_enable ? "Enabled" : "Disabled");
}

void cli_get_payload(sl_cli_command_arg_t *arguments)
{
  uint8_t packet_id = sl_cli_get_argument_uint8(arguments, 0);
  if (validate_packet(packet_id)) {
    responsePrintStart(sl_cli_get_command_string(arguments, 0));
    responsePrintContinue("len: %lu", packets[packet_id].length);
    print_packet(packet_id);
  }
}

void cli_set_payload(sl_cli_command_arg_t *arguments)
{
  if (state == S_IDLE) {
    uint8_t packet_id = sl_cli_get_argument_uint8(arguments, 0);
    uint16_t offset = sl_cli_get_argument_uint16(arguments, 1);
    if (validate_packet(packet_id)) {
      uint8_t count = sl_cli_get_argument_count(arguments) - 2;
      if ((offset + count) <= SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH) {
        for (uint8_t i = 0; i < count; ++i) {
          packets[packet_id].payload[offset + i]
            = sl_cli_get_argument_uint8(arguments, i + 2);
        }
        cli_get_payload(arguments);
      } else {
        app_log_error("Requested payload is too long!"
                      "The maximum is %u!\n",
                      SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH);
      }
    }
  } else {
    app_log_error("Cannot set payload during Tx\n");
  }
}

void cli_set_length(sl_cli_command_arg_t *arguments)
{
  if (state == S_IDLE) {
    uint8_t packet_id = sl_cli_get_argument_uint8(arguments, 0);
    uint16_t length = sl_cli_get_argument_uint16(arguments, 1);
    if (packet_id < SL_RF_GENERATOR_NUMBER_OF_PACKETS) {
      if (length <= SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH) {
        packets[packet_id].length = length;
        responsePrint(sl_cli_get_command_string(arguments, 0),
                      "packet id: %u, len: %lu",
                      packet_id,
                      packets[packet_id].length);
      } else {
        app_log_error("The requested length is bigger than the maximum (%u)\n",
                      SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH);
      }
    } else {
      app_log_error("Packet ID is out of range, there are only %u packets!\n",
                    SL_RF_GENERATOR_NUMBER_OF_PACKETS);
    }
  } else {
    app_log_error("Cannot set length during Tx\n");
  }
}

void cli_set_channel(sl_cli_command_arg_t *arguments)
{
  if (state == S_IDLE) {
    uint8_t packet_id = sl_cli_get_argument_uint8(arguments, 0);
    uint16_t channel = sl_cli_get_argument_uint16(arguments, 1);
    if (validate_packet(packet_id)) {
      RAIL_Handle_t rail_handle
        = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
      RAIL_Status_t status = RAIL_IsValidChannel(rail_handle, channel);
      if (status == RAIL_STATUS_NO_ERROR) {
        packets[packet_id].channel = channel;
        responsePrint(sl_cli_get_command_string(arguments, 0),
                      "packet id: %u, channel: %u",
                      packet_id,
                      packets[packet_id].channel);
      } else {
        app_log_error("Invalid channel\n");
      }
    }
  } else {
    app_log_error("Cannot set channel during Tx\n");
  }
}

void cli_get_packet_info(sl_cli_command_arg_t *arguments)
{
  uint8_t packet_id = sl_cli_get_argument_uint8(arguments, 0);
  if (validate_packet(packet_id)) {
    responsePrintStart(sl_cli_get_command_string(arguments, 0));
    responsePrintContinue("packet id: %u, channel: %u, len: %lu",
                          packet_id,
                          packets[packet_id].channel,
                          packets[packet_id].length);
    print_packet(packet_id);
  }
}

void cli_get_repeat(sl_cli_command_arg_t *arguments)
{
  responsePrintStart(sl_cli_get_command_string(arguments, 0));
  if (repeats) {
    responsePrintEnd("Repeat number: %lu", repeats);
  } else {
    responsePrintEnd("Repeat number: infinite");
  }
}

void cli_set_repeat(sl_cli_command_arg_t *arguments)
{
  if (state == S_IDLE) {
    repeats = sl_cli_get_argument_uint32(arguments, 0);
    cli_get_repeat(arguments);
  } else {
    app_log_error("Cannot set repeat during Tx\n");
  }
}

void cli_get_seq(sl_cli_command_arg_t *arguments)
{
  responsePrintStart(sl_cli_get_command_string(arguments, 0));
  app_log("{sequence:");
  for (uint8_t i = 0; i < sequence_length; ++i) {
    app_log(" id: %u", sequence[i].packet->id);
    app_log(" delay: %lu Us", sequence[i].delay);
  }
  app_log("}}\n");
}

void cli_set_seq(sl_cli_command_arg_t *arguments)
{
  uint8_t number_of_parameters = sl_cli_get_argument_count(arguments);
  if (!(number_of_parameters % 2)) {
    if (state == S_IDLE) {
      for (uint8_t i = 0; i < number_of_parameters - 1; i += 2) {
        if (!validate_packet(sl_cli_get_argument_uint8(arguments, i))) {
          return;
        }
      }
      if ((number_of_parameters / 2) <= SL_RF_GENERATOR_SEQUENCE_LENGTH) {
        sequence_length = number_of_parameters / 2;
        uint8_t arg_index;
        uint8_t packet_id;
        uint32_t delay;
        for (uint8_t seq_index = 0; seq_index < sequence_length; seq_index++) {
          arg_index = seq_index * 2;
          packet_id = sl_cli_get_argument_uint8(arguments, arg_index);
          delay = sl_cli_get_argument_uint32(arguments, arg_index + 1);

          sequence[seq_index].packet = &packets[packet_id];
          sequence[seq_index].delay = delay;
        }
        cli_get_seq(arguments);
      } else {
        app_log_error("Too long sequence! The maximum length is %u!\n",
                      SL_RF_GENERATOR_SEQUENCE_LENGTH);
      }
    } else {
      app_log_error("Cannot set sequence during Tx\n");
    }
  } else {
    app_log("Incorrect number of arguments\n");
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void print_packet(uint8_t packet_id)
{
  app_log("{payload:");
  for (int i = 0; i < packets[packet_id].length; i++) {
    app_log(" 0x%.2x", packets[packet_id].payload[i]);
  }
  app_log("}}\n");
}

static bool validate_packet(uint8_t packet_id)
{
  if (packet_id < SL_RF_GENERATOR_NUMBER_OF_PACKETS) {
    return true;
  }
  app_log_error("Invalid packet id\n");
  return false;
}
