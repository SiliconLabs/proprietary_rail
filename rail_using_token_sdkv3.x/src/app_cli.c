/***************************************************************************//**
* @file app_cli.c
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "string.h"
#include "sl_cli_handles.h"
#include "sl_token_manager.h"

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
/// Flag, indicating transmit request (button has pressed / CLI transmit 
/// request has occured)
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
 * CLI - token: Get basic/counter/index token
 *****************************************************************************/
void cmd_get_token_handler(sl_cli_command_arg_t *arguments)
{
  if (0 == strcmp("basic", sl_cli_get_command_string(arguments, 1))) {
    uint16_t value;
    halCommonGetToken(&value, TOKEN_BASIC_DEMO);
    printf("The value of TOKEN_BASIC_DEMO is: 0x%04X \n", value);
  } else if (0 == strcmp("counter", sl_cli_get_command_string(arguments, 1))) {
    uint32_t value;
    halCommonGetToken(&value, TOKEN_COUNTER_DEMO);
    printf("The value of TOKEN_COUNTER_DEMO is: 0x%08lX \n", value);
  } else if (0 == strcmp("index", sl_cli_get_command_string(arguments, 1))) {
    uint8_t index;
    uint32_t value;

    if (sl_cli_get_argument_count(arguments) < 1) {
      printf("please input the index. \n");
      return;
    }

    index = sl_cli_get_argument_uint8(arguments, 0);
    halCommonGetIndexedToken(&value, TOKEN_INDEX_DEMO, index);
    printf("The value of TOKEN_INDEX_DEMO[%d] is: 0x%08lX \n", index, value);
  } else {
    //impossible branch
  }
}

/******************************************************************************
 * CLI - token: Set basic/counter/index token
 *****************************************************************************/
void cmd_set_token_handler(sl_cli_command_arg_t *arguments)
{
  if (0 == strcmp("basic", sl_cli_get_command_string(arguments, 1))) {
    uint16_t value;

    if (sl_cli_get_argument_count(arguments) < 1) {
      printf("please input the value. \n");
      return;
    }

    value = sl_cli_get_argument_uint16(arguments, 0);
    halCommonSetToken(TOKEN_BASIC_DEMO, &value);
    printf("Set the value of TOKEN_BASIC_DEMO to: 0x%04X \n", value);
  } else if (0 == strcmp("counter", sl_cli_get_command_string(arguments, 1))) {
    halCommonIncrementCounterToken(TOKEN_COUNTER_DEMO);
    printf("Increase the counter TOKEN_COUNTER_DEMO \n");
  } else if (0 == strcmp("index", sl_cli_get_command_string(arguments, 1))) {
    uint8_t index;
    uint32_t value;

    if (sl_cli_get_argument_count(arguments) < 2) {
        printf("please input the index and value. \n");
        return;
    }

    index = (uint8_t)sl_cli_get_argument_uint32(arguments, 0);
    value = sl_cli_get_argument_uint32(arguments, 1);
    halCommonSetIndexedToken(TOKEN_INDEX_DEMO, index, &value);
    printf("Set the value of TOKEN_INDEX_DEMO[%d] to: 0x%08lX \n", index, 
                                                                   value);
  } else {
    //impossible branch
  }
}

/******************************************************************************
 * CLI - eui64: get the mfg token eui64
 *****************************************************************************/
void cmd_get_eui64_handler(sl_cli_command_arg_t *arguments)
{
  uint8_t eui64[8] = {0};

  (void)arguments;

  halCommonGetMfgToken(eui64, TOKEN_MFG_CUSTOM_EUI_64);
  printf("TOKEN_MFG_CUSTOM_EUI_64:%02X%02X%02X%02X%02X%02X%02X%02X \n", 
         eui64[7], 
         eui64[6], 
         eui64[5], 
         eui64[4],
         eui64[3], 
         eui64[2], 
         eui64[1], 
         eui64[0]);
}

/******************************************************************************
 * CLI - eui64: set the mfg token eui64
 *****************************************************************************/
void cmd_set_eui64_handler(sl_cli_command_arg_t *arguments)
{
  uint8_t eui64[8] = {0};
  size_t len = 0;
  uint8_t *pdata;

  if (sl_cli_get_argument_count(arguments) < 1) {
    printf("please input the eui64. \n");
    return;
  }

  pdata = sl_cli_get_argument_hex(arguments, 0, &len);
  if (NULL != pdata && sizeof(eui64) == len) {
    eui64[7] = pdata[0];
    eui64[6] = pdata[1];
    eui64[5] = pdata[2];
    eui64[4] = pdata[3];
    eui64[3] = pdata[4];
    eui64[2] = pdata[5];
    eui64[1] = pdata[6];
    eui64[0] = pdata[7];

    //mfg token can be set only once
    int ret = halCommonSetMfgToken(TOKEN_MFG_CUSTOM_EUI_64, eui64);
    printf("set TOKEN_MFG_CUSTOM_EUI_64 ret=%d \n", ret);
  } else {
    printf("Input invalid \n");
  }
}

static const sl_cli_command_info_t cmd_token_get = \
  SL_CLI_COMMAND(cmd_get_token_handler,
                 "Get token value",
                 "For index token, please input the index as well",
                 { SL_CLI_ARG_UINT8OPT, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cmd_token_set = \
  SL_CLI_COMMAND(cmd_set_token_handler,
                 "Set token value",
                 "for basic token, please input the value. \
                 No need for counter token",
                 { SL_CLI_ARG_UINT32OPT, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cmd_indextoken_set = \
  SL_CLI_COMMAND(cmd_set_token_handler,
                 "Set token value",
                 "For index token, the arg order is: <index> <value>",
                 { SL_CLI_ARG_UINT32, SL_CLI_ARG_UINT32, SL_CLI_ARG_END, });

static sl_cli_command_entry_t cmd_token_oper_entry[] = {
  { "get", &cmd_token_get, false },
  { "set", &cmd_token_set, false },
};

static sl_cli_command_entry_t cmd_indextoken_oper_entry[] = {
  { "get", &cmd_token_get, false },
  { "set", &cmd_indextoken_set, false },
};

static const sl_cli_command_info_t cmd_indextoken_oper_menu = \
  SL_CLI_COMMAND_GROUP(cmd_indextoken_oper_entry,
                       "The token operation(get/set) group");

static const sl_cli_command_info_t cmd_token_oper_menu = \
  SL_CLI_COMMAND_GROUP(cmd_token_oper_entry,
                       "The token operation(get/set) group");

static sl_cli_command_entry_t cmd_token_submenu[] = {
  { "basic", &cmd_token_oper_menu, false },
  { "counter", &cmd_token_oper_menu, false },
  { "index", &cmd_indextoken_oper_menu, false },
  { NULL, NULL, false },
};

static const sl_cli_command_info_t cmd_eui64_get = \
  SL_CLI_COMMAND(cmd_get_eui64_handler,
                 "Get eui64",
                 "No argument",
                 { SL_CLI_ARG_END, });

static const sl_cli_command_info_t cmd_eui64_set = \
  SL_CLI_COMMAND(cmd_set_eui64_handler,
                 "Set eui64",
                 "The Eui64 value. e.g. {0011223344556677}",
                 { SL_CLI_ARG_HEX, SL_CLI_ARG_END, });

static sl_cli_command_entry_t cmd_eui64_table[] = {
  { "get", &cmd_eui64_get, false },
  { "set", &cmd_eui64_set, false },
  { NULL, NULL, false },
};

static const sl_cli_command_info_t cmd_eui64_group = \
  SL_CLI_COMMAND_GROUP(cmd_eui64_table,
                       "The eui64 command group");

static const sl_cli_command_info_t cmd_token_menu = \
  SL_CLI_COMMAND_GROUP(cmd_token_submenu,
                       "The token command group");

static sl_cli_command_entry_t custom_cmd_table[] = {
  { "token", &cmd_token_menu, false },
  { "eui64", &cmd_eui64_group, false },
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
