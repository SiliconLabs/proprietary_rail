/***************************************************************************//**
 * @file
 * @brief app_init.c
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_timer.h"
#include "em_gpio.h"

#include "app_log.h"

#include "sl_rail_util_init.h"

#include "sl_radioprs_config_async_dout.h"
#include "sl_radioprs_config_sync_dout.h"

#include "sl_rail_direct_mode_detector_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void initTimers(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// RF symbol duration in timer ticks
uint32_t symbol_duration;

// Payload buffer for transmitted packet
__ALIGNED(
  RAIL_FIFO_ALIGNMENT) uint8_t tx_payload[SL_DIRECT_MODE_DETECTOR_TX_FIFO_SIZE];
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
  // Get RAIL handle, used later by the application
  RAIL_Handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  initTimers();

  // Fills up TX payload with a user-defined pattern
  memset(tx_payload, SL_DIRECT_MODE_DETECTOR_PAYLOAD_PATTERN,
         sizeof(tx_payload));

  // Starts RX: after this point the radio will be always in RX mode except
  // when transmitting a packet or packet printing. Normal packet detection is
  // disabled by the radio configuration.
  RAIL_StartRx(rail_handle, SL_DIRECT_MODE_DETECTOR_DEFAULT_CHANNEL, NULL);

  // At this point the active channel is set, RAIL_GetSymbolRate()
  // gets the correct symbol rate
  symbol_duration = CMU_ClockFreqGet(cmuClock_TIMER0)
                    / RAIL_GetSymbolRate(rail_handle);

  // TX fifo filled only once, TX_OPTION_RESEND is used to keep the payload
  // in the FIFO
  uint16_t fifo_size = RAIL_SetTxFifo(rail_handle,
                                      tx_payload,
                                      SL_DIRECT_MODE_DETECTOR_PACKET_LENGTH,
                                      SL_DIRECT_MODE_DETECTOR_TX_FIFO_SIZE);
  if (fifo_size == 0) {
    app_log_error("RAIL_SetTxFifo() failed!\n");
  } else if (fifo_size != SL_DIRECT_MODE_DETECTOR_TX_FIFO_SIZE) {
    app_log_warning("TX FIFO is set to %d\n", fifo_size);
  } else {
  }

  return rail_handle;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void initTimers()
{
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Initialize the TIMER0 for high pulse measurement

  TIMER_Init_TypeDef timer0Init = TIMER_INIT_DEFAULT;
  timer0Init.enable = false;
  timer0Init.riseAction = timerInputActionReloadStart;

  TIMER_Init(TIMER0, &timer0Init);

  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  timerCCInit.eventCtrl = timerEventFalling;
  timerCCInit.edge = timerEdgeFalling;
  timerCCInit.mode = timerCCModeCapture;
  timerCCInit.prsInput = true;
  timerCCInit.prsInputType = timerPrsInputAsyncLevel;
  timerCCInit.prsSel = RADIOPRS_ASYNC_DOUT_CHANNEL;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Reduce TIMER0 to a 16-bit counter to make it similar to TIMER1
  TIMER_TopSet(TIMER0, 0x0000FFFFUL);

  // Enable TIMER0 interrupts - only CC0 is used
  TIMER_IntClear(TIMER0, _TIMER_IEN_CC0_MASK);
  TIMER_IntEnable(TIMER0, _TIMER_IEN_CC0_MASK);

  NVIC_EnableIRQ(TIMER0_IRQn);

  TIMER_Enable(TIMER0, true);

  CMU_ClockEnable(cmuClock_TIMER1, true);

  // Initialize the TIMER1 for low pulse measurement

  TIMER_Init_TypeDef timer1Init = TIMER_INIT_DEFAULT;
  timer1Init.enable = false;
  timer1Init.fallAction = timerInputActionReloadStart;

  TIMER_Init(TIMER1, &timer1Init);

  timerCCInit.eventCtrl = timerEventRising;
  timerCCInit.edge = timerEdgeRising;
  timerCCInit.mode = timerCCModeCapture;
  timerCCInit.prsInput = true;
  timerCCInit.prsInputType = timerPrsInputAsyncLevel;
  timerCCInit.prsSel = RADIOPRS_ASYNC_DOUT_CHANNEL;
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Set the highest possible TOP value
  TIMER_TopSet(TIMER1, 0x0000FFFFUL);

  // Enable TIMER1 interrupts - only CC0 is used
  TIMER_IntClear(TIMER1, _TIMER_IEN_CC0_MASK);
  TIMER_IntEnable(TIMER1, _TIMER_IEN_CC0_MASK);

  NVIC_EnableIRQ(TIMER1_IRQn);

  TIMER_Enable(TIMER1, true);
}
