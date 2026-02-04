/***************************************************************************//**
 * @file script_ci.c
 * @brief CLI implementations for the scripting features of the multiprotocol
 *   app. These features allow a user to upload a series of commands to the
 *   chip to be executed consecutively without waiting for further CLI input
 *   until the uploaded script is completed. This is useful in testing
 *   scenarios where they delay related to entering commands one-by-one
 *   is too great.
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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "sl_cli_instances.h"

#include "sl_rail.h"
#include "sl_rail_util_init.h"
#include "response_print.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
#include "sl_cli_storage_ram_instances.h"
#endif
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
#include "sl_cli_storage_nvm3_instances.h"
#endif
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
#include "sl_cli_delay.h"
#endif

static bool flash_command_script_exists;
static bool ram_command_script_exists;

bool parse_time_mode_from_string(char *str, sl_rail_time_mode_t *mode)
{
  if (strcasecmp("dis", str) == 0) {
    *mode = SL_RAIL_TIME_DISABLED;
  } else if (strcasecmp("rel", str) == 0) {
    *mode = SL_RAIL_TIME_DELAY;
  } else if (strcasecmp("abs", str) == 0) {
    *mode = SL_RAIL_TIME_ABSOLUTE;
  } else {
    *mode = SL_RAIL_TIME_DISABLED;
    return false;
  }
  return true;
}

// non-blocking wait
void wait(sl_cli_command_arg_t *args)
{
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
  // Relative time by default
  sl_rail_time_mode_t time_mode = SL_RAIL_TIME_DELAY;

  if ((sl_cli_get_argument_count(args) >= 2)
      && !parse_time_mode_from_string(sl_cli_get_argument_string(args, 1),
                                      &time_mode)) {
    responsePrintError(sl_cli_get_command_string(args, 0),
                       0x3,
                       "Invalid time mode");
    return;
  }
  // Get RAIL handle, used later by the application
  sl_rail_handle_t rail_handle
    = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  sl_rail_time_t time_start = sl_rail_get_time(rail_handle);
  sl_rail_time_t time_duration_us = sl_cli_get_argument_uint32(args, 0);

  // In the code, everything is handled as relative time, so convert this
  // to a relative value
  if (time_mode == SL_RAIL_TIME_ABSOLUTE) {
    time_duration_us -= time_start;
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "current_time:%u,delay:%u,resume_time:%u",
                time_start,
                time_duration_us,
                time_duration_us + time_start);

  // Take into account the time it took to printf above.
  uint32_t extra_offset = sl_rail_get_time(rail_handle) - time_start;
  if (extra_offset < time_duration_us) {
    time_duration_us -= extra_offset;
  } else {
    time_duration_us = 0;
  }

  // Remove the optional 2nd CLI argument so sl_cli_delay_command works.
  // Also, convert microsecond time delay into milliseconds.
  uint32_t time_duration_ms = time_duration_us / 1000; // convert us to ms
  args->argc = 2; // pass only cmd and delay (and not optional argument)
  args->argv[1] = (void *)&time_duration_ms;

  // Call the non-blocking wait function (with a relative delay in ms).
  sl_cli_delay_command(args);
#else
  (void)args;
  responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                     "CLI Delay support not enabled");
#endif // SL_CATALOG_CLI_DELAY_PRESENT
}

void enter_script(sl_cli_command_arg_t *args)
{
  bool use_flash = (sl_cli_get_argument_count(args) >= 1)
                   && !!sl_cli_get_argument_uint8(args, 0);
  bool success = true;

  // Don't pass along the optional input argument.
  args->argc = 1; // pass only cmd (and not optional argument)

  if (use_flash) {
    // Use flash.
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
    if (flash_command_script_exists) {
      sl_cli_storage_nvm3_clear(args);
    }
    flash_command_script_exists = true;
    sl_cli_storage_nvm3_define(args);
#else
    (void)flash_command_script_exists;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Flash support not enabled");
    return;
#endif // SL_CATALOG_CLI_STORAGE_NVM3_PRESENT
  } else {
    // Use RAM.
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
    if (ram_command_script_exists) {
      sl_cli_storage_ram_clear(args);
    }
    ram_command_script_exists = true;
    sl_cli_storage_ram_define(args);
#else
    (void)ram_command_script_exists;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "RAM support not enabled");
    return;
#endif // SL_CATALOG_CLI_STORAGE_RAM_PRESENT
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "location:%s,status:%s",
                use_flash ? "flash" : "RAM",
                success ? "Success" : "Failure");
}

void clear_script(sl_cli_command_arg_t *args)
{
  bool use_flash = (sl_cli_get_argument_count(args) >= 1)
                   && !!sl_cli_get_argument_uint8(args, 0);
  bool success = true;

  // Don't pass along the optional input argument.
  args->argc = 1; // pass only cmd (and not optional argument)

  if (use_flash) {
    // Use flash.
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
    flash_command_script_exists = false;
    sl_cli_storage_nvm3_clear(args);
#else
    (void)flash_command_script_exists;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Flash support not enabled");
    return;
#endif // SL_CATALOG_CLI_STORAGE_NVM3_PRESENT
  } else {
    // Use RAM.
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
    ram_command_script_exists = false;
    sl_cli_storage_ram_clear(args);
#else
    (void)ram_command_script_exists;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "RAM support not enabled");
    return;
#endif // SL_CATALOG_CLI_STORAGE_RAM_PRESENT
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "location:%s,status:%s",
                use_flash ? "flash" : "RAM",
                success ? "Success" : "Failure");
}

void print_script(sl_cli_command_arg_t *args)
{
  bool use_flash = (sl_cli_get_argument_count(args) >= 1)
                   && !!sl_cli_get_argument_uint8(args, 0);
  bool success = false;
  uint32_t script_count = 0U;

  // Don't pass along the optional input argument.
  args->argc = 1; // pass only cmd (and not optional argument)

  if (use_flash) {
    // Use flash.
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
    script_count = sl_cli_storage_nvm3_count(args->handle);
    if (script_count > 0U) {
      sl_cli_storage_nvm3_list(args);
      success = true;
    }
#else
    (void)script_count;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Flash support not enabled");
    return;
#endif // SL_CATALOG_CLI_STORAGE_NVM3_PRESENT
  } else {
    // Use RAM.
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
    script_count = sl_cli_storage_ram_count(args->handle);
    if (script_count > 0U) {
      sl_cli_storage_ram_list(args);
      success = true;
    }
#else
    (void)script_count;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "RAM support not enabled");
    return;
#endif // SL_CATALOG_CLI_STORAGE_RAM_PRESENT
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "location:%s,status:%s,scriptCount:%u",
                use_flash ? "flash" : "RAM",
                success ? "Success" : "Failure",
                script_count);
}

void run_script(sl_cli_command_arg_t *args)
{
  bool use_flash = (sl_cli_get_argument_count(args) >= 1)
                   && !!sl_cli_get_argument_uint8(args, 0);
  bool success = false;
  uint32_t script_count = 0U;

  // Don't pass along the optional input argument.
  args->argc = 1; // pass only cmd (and not optional argument)

  if (use_flash) {
    // Use flash.
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
    script_count = sl_cli_storage_nvm3_count(args->handle);
    if (script_count > 0U) {
      sl_cli_storage_nvm3_execute(args);
      success = true;
    }
#else
    (void)script_count;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "Flash support not enabled");
    return;
#endif // SL_CATALOG_CLI_STORAGE_NVM3_PRESENT
  } else {
    // Use RAM.
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
    script_count = sl_cli_storage_ram_count(args->handle);
    if (script_count > 0U) {
      sl_cli_storage_ram_execute(args);
      success = true;
    }
#else
    (void)script_count;
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12,
                       "RAM support not enabled");
    return;
#endif // SL_CATALOG_CLI_STORAGE_RAM_PRESENT
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "location:%s,status:%s",
                use_flash ? "flash" : "RAM",
                success ? "Success" : "Failure");
}
