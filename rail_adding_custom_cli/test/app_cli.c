/***************************************************************************//**
* @file app_cli.c
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* SPDX-License-Identifier: Zlib
*
* The licensor of this software is Silicon Laboratories Inc.
*
* This software is provided \'as-is\', without any express or implied
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
#include <stdint.h>
#include "sl_component_catalog.h"
#include "em_chip.h"
#include "app_log.h"
#include "sl_cli.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif


#include "sl_cli_handles.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Used for indicates the current status of forwarding rx packets on UART
#define ON   "ON"
/// Used for indicates the current status of forwarding rx packets on UART
#define OFF  "OFF"

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Flag, indicating transmit request (button has pressed / CLI transmit request has occured)
extern volatile bool tx_requested;
/// Flag, indicating received packet is forwarded on CLI or not
extern volatile bool rx_requested;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * CLI - info message: Unique ID of the board
 *****************************************************************************/
void cli_info(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  app_log_info("Info:\n");
  app_log_info("  MCU Id:       0x%llx\n", SYSTEM_GetUnique());
  app_log_info("  Fw RX Packet: %s\n", (rx_requested == true) ? ON : OFF);
}

/******************************************************************************
 * CLI - send: Sets a flag indicating that a packet has to be sent
 *****************************************************************************/
void cli_send_packet(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  tx_requested = true;
  app_log_info("Send packet request\n");
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - receive: Turn on/off received message
 *****************************************************************************/
void cli_receive_packet(sl_cli_command_arg_t *arguments)
{
  uint8_t rxForward = sl_cli_get_argument_uint8(arguments, 0);
  const char* str_rx_fw;
  if (rxForward == 0) {
    rx_requested = false;
    str_rx_fw = OFF;
  } else {
    rx_requested = true;
    str_rx_fw = ON;
  }

  app_log_info("Received packets: %s\n", str_rx_fw);
}

/******************************************************************************
 * CLI - run: Handler of the `custom run` command
 *****************************************************************************/
void cmd_run_handler(sl_cli_command_arg_t *arguments)
{
  int argc = sl_cli_get_argument_count(arguments);

  if (argc < 1) {
    printf("Not enough arguments. Expected 1! \n");
    return;
  }

  uint8_t enable = sl_cli_get_argument_uint8(arguments, 0);
  if (enable) {
    printf("To run \n");
  } else {
    printf("To stop \n");
  }
}

/******************************************************************************
 * CLI - test: Handler of the `custom test` command
 *****************************************************************************/
void cmd_custom_test_handler(sl_cli_command_arg_t *arguments)
{
  int argc = sl_cli_get_argument_count(arguments);

  if (argc < 1) {
    printf("Not enough arguments. Expected 1! \n");
    return;
  }

  printf("user input=[%s] \n", sl_cli_get_argument_string(arguments, 0));
}

static const sl_cli_command_info_t custom_cmd_test = \
  SL_CLI_COMMAND(cmd_custom_test_handler,
                 "Custom test command",
                 "Input any string as the parameter",
                 { SL_CLI_ARG_STRING, SL_CLI_ARG_END, });

static sl_cli_command_entry_t custom_menu_entry[] = {
  { "test", &custom_cmd_test, false },
  { NULL, NULL, false },
};

static const sl_cli_command_info_t cmd_run = \
  SL_CLI_COMMAND(cmd_run_handler,
                 "Run command",
                 "Argument 0:stop, 1:run",
                 { SL_CLI_ARG_UINT8, SL_CLI_ARG_END, });

static const sl_cli_command_info_t custom_cmd_menu = \
  SL_CLI_COMMAND_GROUP(custom_menu_entry,
                       "The custom command group");

static sl_cli_command_entry_t custom_cmd_table[] = {
  { "run", &cmd_run, false },
  { "custom_menu", &custom_cmd_menu, false },
  { NULL, NULL, false },
};

static sl_cli_command_group_t custom_cmd_group = {
  { NULL },
  false,
  custom_cmd_table
};

void cli_custom_cmd_init()
{
  sl_cli_command_add_command_group(sl_cli_example_handle, &custom_cmd_group);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

