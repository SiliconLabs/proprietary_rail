/***************************************************************************//**
 * @file app_init.c
 * @brief app_init.c
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
#include "rail.h"
#include "em_device.h"
#include "rail_types.h"
#include "rail_config.h"
#include "sl_rail_util_init.h"
#include "app_init.h"
#include "app_process.h"
//#include "sl_simple_led_instances.h" //This line should be commented out
#include "app_log.h"
#include "sl_duty_cycle_config.h"
#include "sl_duty_cycle_utility.h"
#include "sl_power_manager.h"

#if DUTY_CYCLE_USE_LCD_BUTTON == 1
  #include "app_graphics.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define DUTY_CYCLY_SYNC_WORDS_TIME  1670
#define DUTY_CYCLY_MARGIN_TIME      1000
#define DUTY_CYCLY_SYNC_TIMEOUT     (DUTY_CYCLY_SYNC_WORDS_TIME\
                                    + DUTY_CYCLE_PERIOD\
                                    + DUTY_CYCLE_ON_TIME\
                                    + DUTY_CYCLY_MARGIN_TIME)
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

volatile RAIL_RxChannelHoppingConfigMultiMode_t duty_cycle_multi_mode_config = {
    .timingSense = 1600,
    .preambleSense = 2200,
    //preamble+sync = 4167+1500000+1670 = 1505834
    .syncDetect = DUTY_CYCLY_SYNC_TIMEOUT,
    .timingReSense = 1600,
    .status = 0,
};

/// Config for the correct timing of the dutycycle API
RAIL_RxDutyCycleConfig_t duty_cycle_config = {
  .delay = ((uint32_t) DUTY_CYCLE_OFF_TIME),
  .delayMode = RAIL_RX_CHANNEL_HOPPING_DELAY_MODE_STATIC,
  .mode = RAIL_RX_CHANNEL_HOPPING_MODE_MULTI_SENSE, // multi sense
  .parameter =  (uint32_t) (void *) &duty_cycle_multi_mode_config
};

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Print sample app name
 *****************************************************************************/
SL_WEAK void print_sample_app_name(const char* app_name)
{
  app_log_info("%s\n", app_name);
}

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  // For handling error codes
  RAIL_Status_t rail_status = RAIL_STATUS_NO_ERROR;

  // To calculate proper preamble
  uint32_t bit_rate = 0;

  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Get current bitrate
  bit_rate = RAIL_GetBitRate(rail_handle);

  // EFR32xG23 chips support the new RadioConfigurator Fast Preamble Settings
#if (_SILICON_LABS_32B_SERIES_2_CONFIG != 3)
  // preamble length in bits
  uint16_t preamble_bit_length = 0;

  // Calculate preamble length from bitrate and time
  preamble_bit_length = calculate_preamble_bit_length_from_time(bit_rate, 
                                                            &duty_cycle_config);

  rail_status = RAIL_SetTxAltPreambleLength(rail_handle, preamble_bit_length);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_SetTxAltPreambleLength failed with %d \n", rail_status);
  }
#else
  rail_status = RAIL_GetDefaultRxDutyCycleConfig(rail_handle,
                                                 &duty_cycle_config);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_GetDefaultRxDutyCycleConfig failed with %d \n",
                  rail_status);
  }
#endif
  // These lines should be commented out
  // Turn OFF LEDs
  //sl_led_turn_off(&sl_led_led0);
  //sl_led_turn_off(&sl_led_led1);

#if DUTY_CYCLE_USE_LCD_BUTTON == 1
  // LCD start
  graphics_init();
#endif

  // CLI info message
  print_sample_app_name("Long Preamble Duty Cycle");
#if (_SILICON_LABS_32B_SERIES_2_CONFIG != 3)
  app_log_info("Preamble length %d for bitrate %d b/s with %d us off time\n",
               preamble_bit_length,
               bit_rate, duty_cycle_config.delay);
#else
  app_log_info("Bitrate %d b/s with %d us off time\n", bit_rate,
                                                       duty_cycle_config.delay);
#endif

  // Allow state machine to run without interrupt
  set_first_run(true);

  // Config DutyCycle API
  RAIL_ConfigRxDutyCycle(rail_handle, &duty_cycle_config);

  // Enable duty cycle mode
  RAIL_EnableRxDutyCycle(rail_handle, true);

  // Start RX
  RAIL_StartRx(rail_handle, CHANNEL, NULL);

#if DUTY_CYCLE_ALLOW_EM2 == 1
  // EM2 sleep level
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
#endif

  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
