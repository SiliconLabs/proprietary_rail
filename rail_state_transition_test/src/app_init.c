/***************************************************************************//**
 * @file
 * @brief app_init.c
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

#include "em_cmu.h"
#include "em_prs.h"
#include "em_gpio.h"
#include "em_timer.h"

#include "app_init.h"
#include "app_log.h"

#include "sl_rail_state_transition_test_config.h"
#include "sl_rail_util_init.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Initialize PRS peripherals.
 * Configures 9 radio PRS signals from which 4 are traced to GPIO pins and 3 are
 * consumed by TIMER peripherals.
 */
static void init_PRS(void);

/**
 * @brief Initialize TIMER peripherals. Measures pulse widths of radio PRS
 * signals to calculate state transition times.
 */
static void init_TIMERs(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/**
 * @brief TIMER peripherals frequency.
 */
float timer_freq;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The function is used for some basic initialization related to the app.
 *****************************************************************************/
RAIL_Handle_t app_init(void)
{
  RAIL_Status_t rail_status;

  // Initialize PRS and TIMER peripherals
  init_PRS();
  init_TIMERs();

  timer_freq = CMU_ClockFreqGet(cmuClock_TIMER0);

  app_log_debug("TIMER peripherals frequency: %.2f us\n", timer_freq);

  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  /* Configure options in RAIL that may impact the state transition times */

#if SL_RAIL_STATE_TRANSITION_TEST_BEST_EFFORT_TIMING
  RAIL_StateTiming_t state_timing = {
    .idleToRx = 0,
    .idleToTx = 0,
    .rxToTx = 0,
    .txToRx = 0,
    .rxSearchTimeout = 0,
    .txToRxSearchTimeout = 0,
    .txToTx = 0,
  };

  rail_status = RAIL_SetStateTiming(rail_handle, &state_timing);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_SetStateTiming failed with code %ld\n", rail_status);
  }
#endif

#if SL_RAIL_STATE_TRANSITION_TEST_ENABLE_SYNTH_CAL_CACHING
  rail_status = RAIL_EnableCacheSynthCal(rail_handle, true);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_EnableCacheSynthCal failed with code %ld\n",
                  rail_status);
  }
#endif

  // Start Rx on both channels to cache the calibration values
  RAIL_StartRx(rail_handle, SL_RAIL_STATE_TRANSITION_TEST_PRIMARY_CHANNEL,
               NULL);
  while (RAIL_GetRadioStateAlt(rail_handle) != RAIL_RAC_STATE_RXSEARCH) {}
  RAIL_Idle(rail_handle, RAIL_IDLE, true);

  RAIL_StartRx(rail_handle,
               SL_RAIL_STATE_TRANSITION_TEST_SECONDARY_CHANNEL,
               NULL);
  while (RAIL_GetRadioStateAlt(rail_handle) != RAIL_RAC_STATE_RXSEARCH) {}
  RAIL_Idle(rail_handle, RAIL_IDLE, true);

#if SL_RAIL_STATE_TRANSITION_TEST_ENABLE_FAST_RX_2_RX
  rail_status = RAIL_ConfigRxOptions(rail_handle,
                                     RAIL_RX_OPTION_FAST_RX2RX,
                                     RAIL_RX_OPTION_FAST_RX2RX);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_ConfigRxOptions failed with code %ld\n", rail_status);
  }
#endif

#if SL_RAIL_STATE_TRANSITION_TEST_BYPASS_CALIBRATIONS
  rail_status = RAIL_ConfigRxOptions(rail_handle,
                                     RAIL_RX_OPTION_SKIP_SYNTH_CAL
                                     | RAIL_RX_OPTION_SKIP_DC_CAL,
                                     RAIL_RX_OPTION_SKIP_SYNTH_CAL
                                     | RAIL_RX_OPTION_SKIP_DC_CAL);

  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_ConfigRxOptions failed with code %ld\n", rail_status);
  }
#endif

#if SL_RAIL_STATE_TRANSITION_TEST_PTI_DISABLE
  rail_status = RAIL_EnablePti(rail_handle, false);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_EnablePti failed with code %ld\n", rail_status);
  }
#endif

  // Hardcode init measurement scenario printing
  app_log_info("Current mode: MANUAL_TRANSITIONS\n");

  return rail_handle;
}

static void init_PRS(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Configure PRS channels */
  PRS_ConnectSignal(PRS_CHANNEL_RX, prsTypeAsync, PRS_RACL_RX);
  PRS_ConnectSignal(PRS_CHANNEL_ACTIVE, prsTypeAsync, PRS_RACL_TX);
  PRS_ConnectSignal(PRS_CHANNEL_LNA, prsTypeAsync, PRS_RACL_LNAEN);
  PRS_ConnectSignal(PRS_CHANNEL_READY_OR_ACTIVE, prsTypeAsync, PRS_RACL_PAEN);
  PRS_ConnectSignal(PRS_CHANNEL_BUSY_OR_READY_OR_ACTIVE,
                    prsTypeAsync,
                    PRS_RACL_ACTIVE);
  PRS_ConnectSignal(PRS_CHANNEL_BUSY, prsTypeAsync, PRS_RACL_ACTIVE);
  PRS_ConnectSignal(PRS_CHANNEL_BUSY_OR_READY, prsTypeAsync, PRS_RACL_ACTIVE);
  PRS_ConnectSignal(PRS_CHANNEL_READY_AUX, prsTypeAsync, PRS_RACL_LNAEN);
  PRS_ConnectSignal(PRS_CHANNEL_READY, prsTypeAsync, PRS_RACL_PAEN);

  /* Construct complex PRS signals */
  PRS_Combine(PRS_CHANNEL_ACTIVE, PRS_CHANNEL_RX, prsLogic_A_OR_B);
  PRS_Combine(PRS_CHANNEL_READY_OR_ACTIVE, PRS_CHANNEL_LNA, prsLogic_A_OR_B);
  PRS_Combine(PRS_CHANNEL_BUSY, PRS_CHANNEL_READY_OR_ACTIVE, prsLogic_A_XOR_B);
  PRS_Combine(PRS_CHANNEL_BUSY_OR_READY,
              PRS_CHANNEL_ACTIVE,
              prsLogic_A_XOR_B);
  PRS_Combine(PRS_CHANNEL_READY_AUX, PRS_CHANNEL_ACTIVE, prsLogic_A_XOR_B);
  PRS_Combine(PRS_CHANNEL_READY, PRS_CHANNEL_READY_AUX, prsLogic_A_XOR_B);

  /* Initialize GPIO pins */
  GPIO_PinModeSet(PRS_PORT_ACTIVE, PRS_PIN_ACTIVE, gpioModePushPull, 0);
  GPIO_PinModeSet(PRS_PORT_BUSY,
                  PRS_PIN_BUSY,
                  gpioModePushPull,
                  0);
  GPIO_PinModeSet(PRS_PORT_READY, PRS_PIN_READY, gpioModePushPull, 0);
  GPIO_PinModeSet(PRS_PORT_BUSY_OR_READY,
                  PRS_PIN_BUSY_OR_READY,
                  gpioModePushPull,
                  0);

  /* Map PRS channels to GPIO pins */
  PRS_PinOutput(PRS_CHANNEL_ACTIVE,
                prsTypeAsync,
                PRS_PORT_ACTIVE,
                PRS_PIN_ACTIVE);
  PRS_PinOutput(PRS_CHANNEL_READY,
                prsTypeAsync,
                PRS_PORT_READY,
                PRS_PIN_READY);
  PRS_PinOutput(PRS_CHANNEL_BUSY, prsTypeAsync, PRS_PORT_BUSY, PRS_PIN_BUSY);
  PRS_PinOutput(PRS_CHANNEL_BUSY_OR_READY,
                prsTypeAsync,
                PRS_PORT_BUSY_OR_READY,
                PRS_PIN_BUSY_OR_READY);
}

static void init_TIMERs(void)
{
  /* TIMER0: Measures RX->TX and TX->RX transitions (ACTIVE low pulse) */
  CMU_ClockEnable(cmuClock_TIMER0, true);

  TIMER_Init_TypeDef timer_0_init = TIMER_INIT_DEFAULT;
  timer_0_init.enable = false;
  timer_0_init.fallAction = timerInputActionReloadStart;

  TIMER_Init(TIMER0, &timer_0_init);

  TIMER_InitCC_TypeDef timer_cc_init = TIMER_INITCC_DEFAULT;

  timer_cc_init.eventCtrl = timerEventRising;
  timer_cc_init.edge = timerEdgeRising;
  timer_cc_init.mode = timerCCModeCapture;
  timer_cc_init.prsInput = true;
  timer_cc_init.prsInputType = timerPrsInputAsyncLevel;
  timer_cc_init.prsSel = PRS_CHANNEL_ACTIVE;
  TIMER_InitCC(TIMER0, 0, &timer_cc_init);

  TIMER_TopSet(TIMER0, (uint32_t) 0xFFFFFFFFFUL);

  // Enable TIMER0 interrupts - only CC0 is used
  TIMER_IntClear(TIMER0, _TIMER_IEN_CC0_MASK);
  TIMER_IntEnable(TIMER0, _TIMER_IEN_CC0_MASK);

  TIMER_Enable(TIMER0, true);

  // TIMER1: Measures request to de/modulation start time (BUSY + READY high
  //   pulse)
  CMU_ClockEnable(cmuClock_TIMER1, true);

  TIMER_Init_TypeDef timer_1_init = TIMER_INIT_DEFAULT;
  timer_1_init.enable = false;
  timer_1_init.riseAction = timerInputActionReloadStart;

  TIMER_Init(TIMER1, &timer_1_init);

  timer_cc_init.eventCtrl = timerEventFalling;
  timer_cc_init.edge = timerEdgeFalling;
  timer_cc_init.mode = timerCCModeCapture;
  timer_cc_init.prsInput = true;
  timer_cc_init.prsInputType = timerPrsInputAsyncLevel;
  timer_cc_init.prsSel = PRS_CHANNEL_BUSY_OR_READY;
  TIMER_InitCC(TIMER1, 0, &timer_cc_init);

  // Set the highest possible TOP value
  TIMER_TopSet(TIMER1, (uint32_t) 0x0000FFFFUL);

  // Enable TIMER1 interrupts - only CC0 is used
  TIMER_IntClear(TIMER1, _TIMER_IEN_CC0_MASK);
  TIMER_IntEnable(TIMER1, _TIMER_IEN_CC0_MASK);

  TIMER_Enable(TIMER1, true);

  CMU_ClockEnable(cmuClock_TIMER2, true);

  // TIMER2: Measures request to PA/LNA active time (BUSY high pulse)
  TIMER_Init_TypeDef timer_2_init = TIMER_INIT_DEFAULT;
  timer_2_init.enable = false;
  timer_2_init.riseAction = timerInputActionReloadStart;

  TIMER_Init(TIMER2, &timer_2_init);

  timer_cc_init.eventCtrl = timerEventFalling;
  timer_cc_init.edge = timerEdgeFalling;
  timer_cc_init.mode = timerCCModeCapture;
  timer_cc_init.prsInput = true;
  timer_cc_init.prsInputType = timerPrsInputAsyncLevel;
  timer_cc_init.prsSel = PRS_CHANNEL_BUSY;
  TIMER_InitCC(TIMER2, 0, &timer_cc_init);

  // Set the highest possible TOP value
  TIMER_TopSet(TIMER2, (uint32_t) 0x0000FFFFUL);

  // Enable TIMER2 interrupts - only CC0 is used
  TIMER_IntClear(TIMER2, _TIMER_IEN_CC0_MASK);
  TIMER_IntEnable(TIMER2, _TIMER_IEN_CC0_MASK);

  TIMER_Enable(TIMER2, true);

  // TIMER3: Placeholder for API call to ... (manual start)
  CMU_ClockEnable(cmuClock_TIMER3, true);

  // request to PA/LNA active

  TIMER_Init_TypeDef timer3Init = TIMER_INIT_DEFAULT;
  timer3Init.enable = false;

  TIMER_Init(TIMER3, &timer3Init);

  timer_cc_init.eventCtrl = timerEventRising;
  timer_cc_init.edge = timerEdgeRising;
  timer_cc_init.mode = timerCCModeCapture;
  timer_cc_init.prsInput = true;
  timer_cc_init.prsInputType = timerPrsInputAsyncLevel;
  TIMER_InitCC(TIMER3, 0, &timer_cc_init);

  // Set the highest possible TOP value
  TIMER_TopSet(TIMER3, (uint32_t) 0x0000FFFFUL);

  // Enable TIMER3 interrupts - only CC0 is used
  TIMER_IntClear(TIMER3, _TIMER_IEN_CC0_MASK);
  TIMER_IntEnable(TIMER3, _TIMER_IEN_CC0_MASK);

  TIMER_Enable(TIMER3, true);
}
