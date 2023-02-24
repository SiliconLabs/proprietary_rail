#include "app_process.h"
#include "sl_cli_instances.h"
#include "sl_rail_util_init.h"
#include "response_print.h"
#include "app_init.h"
#include "app_log.h"

extern bool tx_log_enable;

extern packet_t packets[NUMBER_OF_PACKETS];
extern packetSequenceElement_t sequence[SEQUENCE_LENGTH];

extern uint8_t sequence_length;
extern uint32_t repeats;
extern state_t state;

void print_packet(uint8_t packet_id)
{
  printf("{payload:");
  for (int i = 0; i < packets[packet_id].length; i++) {
    printf(" 0x%.2x", packets[packet_id].payload[i]);
  }
  printf("}}\n");
}

bool validate_packet(uint8_t packet_id)
{
  if (packet_id < NUMBER_OF_PACKETS) {
    return true;
  }
  app_log_error("Invalid packet id\n");
  return false;
}

void cli_tx(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  tx_toggle();
}

void cli_tx_log(sl_cli_command_arg_t *arguments)
{
  if (sl_cli_get_argument_count(arguments) == 1) {
    tx_log_enable = sl_cli_get_argument_uint8(arguments, 0);
  } else {
    tx_log_enable = true;
  }
  responsePrint(sl_cli_get_command_string(arguments, 0),
                "TX log:%s",
                tx_log_enable ? "Enabled" : "Disabled");
}

void cli_get_payload(sl_cli_command_arg_t *arguments)
{
  uint8_t packet_id = sl_cli_get_argument_uint8(arguments, 0);
  if (validate_packet(packet_id)) {
    responsePrintStart(sl_cli_get_command_string(arguments, 0));
    responsePrintContinue("len:%lu", packets[packet_id].length);
    print_packet(packet_id);
  }
}

void cli_set_payload(sl_cli_command_arg_t *arguments)
{
  if (state == S_IDLE) {
    uint8_t packet_id = sl_cli_get_argument_uint8(arguments, 0);
    uint16_t offset = sl_cli_get_argument_uint16(arguments, 1);
    if (validate_packet(packet_id)) {
      for (uint8_t i = 0; i < sl_cli_get_argument_count(arguments) - 2; ++i) {
        packets[packet_id].payload[offset + i]
          = sl_cli_get_argument_uint8(arguments, i + 2);
      }
      cli_get_payload(arguments);
    }
  } else {
    app_log_error("Cannot set payload during TX\n");
  }
}

void cli_set_length(sl_cli_command_arg_t *arguments)
{
  if (state == S_IDLE) {
    uint8_t packet_id = sl_cli_get_argument_uint8(arguments, 0);
    uint16_t length = sl_cli_get_argument_uint16(arguments, 1);
    packets[packet_id].length = length;
    responsePrint(sl_cli_get_command_string(arguments, 0),
                  "packet id:%u,len:%lu",
                  packet_id,
                  packets[packet_id].length);
  } else {
    app_log_error("Cannot set length during TX\n");
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
                      "packet id:%u,channel:%u",
                      packet_id,
                      packets[packet_id].channel);
      } else {
        app_log_error("Invalid channel\n");
      }
    }
  } else {
    app_log_error("Cannot set channel during TX\n");
  }
}

void cli_get_packet_info(sl_cli_command_arg_t *arguments)
{
  uint8_t packet_id = sl_cli_get_argument_uint8(arguments, 0);
  if (validate_packet(packet_id)) {
    responsePrintStart(sl_cli_get_command_string(arguments, 0));
    responsePrintContinue("packet id:%u, channel:%u, len:%lu",
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
    app_log_error("Cannot set repeat during TX\n");
  }
}

void cli_get_seq(sl_cli_command_arg_t *arguments)
{
  responsePrintStart(sl_cli_get_command_string(arguments, 0));
  printf("{sequence:");
  for (uint8_t i = 0; i < sequence_length; ++i) {
    printf(" id:%u", sequence[i].packet->id);
    printf(" delay:%lu Us", sequence[i].delay);
  }
  printf("}}\n");
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
      sequence_length = number_of_parameters / 2;
      for (uint8_t i = 0, j = 0; i < number_of_parameters - 1; i += 2, ++j) {
        sequence[j].packet = &packets[sl_cli_get_argument_uint32(arguments, i)];
        sequence[j].delay = sl_cli_get_argument_uint32(arguments, i + 1);
      }
      cli_get_seq(arguments);
    }
    app_log_error("Cannot set sequence during TX\n");
  } else {
    printf("Incorrect number of arguments\n");
  }
}
