/***************************************************************************//**
 * @file duty_cycle_utility.c
 * @brief calculate the required premable length for LDC
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
#include "sl_duty_cycle_utility.h"
#include "sl_duty_cycle_config.h"
#include "app_assert.h"
#include "app_log.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// defines for calculating the minimum preamble to for the on time comaprison
#define PREAMBLE_PATTERN_LENGTH (2u)
#define PREAMBLE_PATTERN (2u)
#define PREAMBLE_OVERSAMPLING (10u)
#define U_SEC  ((uint32_t) 1000000)
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
/*******************************************************************************
 * Calculate the proper length of bit for preamble
 *
 * @param bit_rate: what bitrate is used for sending
 *
 * @param duty_cycle_config: address of a preconfigured structure
 *
 * @return uint16_t: preamle_bit_length
 ******************************************************************************/
uint16_t calculate_preamble_bit_length_from_time(const uint32_t bit_rate,
                                    RAIL_RxDutyCycleConfig_t *duty_cycle_config)
{
  float on_time = 0;
  float off_time = 0;
  float preamble_time = 0;
  float preamle_bit_length = 0;

  on_time = DUTY_CYCLE_ON_TIME;
  off_time = duty_cycle_config->delay;

  preamble_time = ((float)(PREAMBLE_PATTERN_LENGTH
                          * PREAMBLE_PATTERN
                          * PREAMBLE_OVERSAMPLING)
                          * U_SEC) / bit_rate;

  app_assert(preamble_time < on_time,
             "Please modify the on time according to the bitrate!\n");

  while (1) {
    preamble_time = (off_time + (2 * on_time)) / 1000000;
    preamle_bit_length = (preamble_time * bit_rate);
    if (preamle_bit_length <= 50000) {
      break;
    }
    off_time = off_time - on_time;
  }

  if (((uint32_t)off_time) != DUTY_CYCLE_OFF_TIME) {
    app_log_warning("Duty Cycle Off time was changed to ensure stable"
                    "working\n");
  }

  return preamle_bit_length;
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
