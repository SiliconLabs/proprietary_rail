/***************************************************************************//**
 * @file app_process.c
 * @brief Source file containing the main loop and the RAIL event handler
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 *
 * EXPERIMENTAL QUALITY
 * This code has been minimally tested to ensure that it builds with the
 * specified dependency versions and is suitable as a demonstration for
 * evaluation purposes only.
 * This code will be maintained at the sole discretion of Silicon Labs.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include "sl_component_catalog.h"
#include "rail.h"
#include "app_init.h"
#include "sl_simple_button_instances.h"
#include "sl_emlib_gpio_init_sampling_time_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// State machine
typedef enum {
  S_ONGOING_RECEPTION,
  S_END_OF_RECEPTION,
  S_PRINT_BUFFER,
  S_CALIBRATION_ERROR,
  S_FIFO_ERROR,
  S_IDLE,
} state_t;

typedef struct iq_data {
  int16_t I;
  int16_t Q;
} iq_data_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * The function printfs the received rx buffer.
 *
 * @returns None
 *****************************************************************************/
static void printf_rx_buffer();

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Flag, indicating received packet is forwarded on CLI or not
volatile bool rx_requested = false;

volatile bool fifo_full = false;

volatile uint16_t rxFifoIndex = 0;

int8_t rxAppFifo[RX_FIFO_SIZE];

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// The variable shows the actual state of the state machine
static volatile state_t state = S_IDLE;

/// Contains the last RAIL error events
static volatile uint64_t calibration_rail_err = 0;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;

  switch (state) {
    case S_IDLE:
      if (rx_requested) {
        rxFifoIndex = 0;
        rail_status = RAIL_StartRx(rail_handle, CHANNEL, NULL);
        app_assert(rail_status == RAIL_STATUS_NO_ERROR);
        rx_requested = false;
        state = S_ONGOING_RECEPTION;
      }
      break;
    case S_END_OF_RECEPTION:
      RAIL_Idle(rail_handle, RAIL_IDLE, true);
      RAIL_ResetFifo(rail_handle, false, true);
      state = S_PRINT_BUFFER;
      break;
    case S_ONGOING_RECEPTION:
      break;
    case S_CALIBRATION_ERROR:
      printf("Calibration error\n");
      state = S_IDLE;
      break;
    case S_FIFO_ERROR:
      RAIL_Idle(rail_handle, RAIL_IDLE, true);
      RAIL_ResetFifo(rail_handle, false, true);
      printf("RX FIFO error\n");
      state = S_IDLE;
      break;
    case S_PRINT_BUFFER:
      if (rxFifoIndex == RX_FIFO_SIZE) {
        printf_rx_buffer();
      }
      state = S_IDLE;
      break;
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs.
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
// Perform all calibrations when needed
  if (events & RAIL_EVENT_CAL_NEEDED) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL,
                                        RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      calibration_rail_err = (events & RAIL_EVENT_CAL_NEEDED);
      state = S_CALIBRATION_ERROR;
    }
  }

  if ((events & RAIL_EVENT_RX_FIFO_OVERFLOW)
      || (events & RAIL_EVENT_RX_FIFO_FULL)) {
    state = S_FIFO_ERROR;
  }

  if (events & RAIL_EVENT_RX_FIFO_ALMOST_FULL) {
    uint16_t availableBytes = 0;
    GPIO_PinOutToggle(SL_EMLIB_GPIO_INIT_SAMPLING_TIME_PORT,
                      SL_EMLIB_GPIO_INIT_SAMPLING_TIME_PIN);

    availableBytes = RAIL_GetRxFifoBytesAvailable(rail_handle);
    if (rxFifoIndex + availableBytes >= RX_FIFO_SIZE) {
      availableBytes = RX_FIFO_SIZE - rxFifoIndex;
    }
    RAIL_ReadRxFifo(rail_handle, rxAppFifo + rxFifoIndex, availableBytes);
    rxFifoIndex += availableBytes;
    if (rxFifoIndex >= RX_FIFO_SIZE) {
      state = S_END_OF_RECEPTION;
    }
  }
}

/******************************************************************************
 * Button callback, called if any button is pressed or released.
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if ((sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)
      && (state == S_IDLE)) {
    rx_requested = true;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void printf_rx_buffer()
{
  iq_data_t *iq_data = rxAppFifo;
  for (uint16_t i = 0; i < RX_FIFO_SIZE / 4; ++i) {
    if(RX_DATA_SOURCE == RX_IQDATA_FILTLSB)
      printf("%6d %6d\n", iq_data[i].I, iq_data[i].Q);
    else
      printf("%6d %6d\n", iq_data[i].Q, iq_data[i].I);
  }
}
