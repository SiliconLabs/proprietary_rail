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

#include "em_timer.h"

#include "rail.h"

#include "app_log.h"

#include "sl_rail_state_transition_test_config.h"
#include "sl_simple_button_instances.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// Shortcuts to configurations
#define REPEAT_CYCLES            SL_RAIL_STATE_TRANSITION_TEST_REPEAT_CYCLES
#define PRIMARY_CHANNEL          SL_RAIL_STATE_TRANSITION_TEST_PRIMARY_CHANNEL
#define SECONDARY_CHANNEL        SL_RAIL_STATE_TRANSITION_TEST_SECONDARY_CHANNEL
#define IDLE_MODE                SL_RAIL_STATE_TRANSITION_TEST_IDLE_MODE
#define TX_FIFO_SIZE             SL_RAIL_STATE_TRANSITION_TEST_TX_FIFO_SIZE

#define ONE_SECOND_IN_US         (1000000)

// Definition to get the TIMER's buffer and convert to microseconds
#define GET_TIME(timer, channel) \
        ((float)TIMER_CaptureBufGet(timer, channel) * 1000000.0f) / timer_freq

// Shortcuts to get the time of the TIMERs
#define GET_ACTIVE_TO_ACTIVE_TIME() GET_TIME(TIMER0, 0)
#define GET_BUSY_TIME()             GET_TIME(TIMER1, 0)
#define GET_BUSY_OR_READY_TIME()    GET_TIME(TIMER2, 0)

// Wait for the radio to reach a specific state
#define WAIT_FOR_RADIO_STATE(state) while (RAIL_GetRadioStateAlt(rail_handle) \
                                           != state) {}

/*
 * Measurement mode types
 */
typedef enum mode {
  MANUAL_TRANSITIONS,
  AUTO_TRANSITIONS,
  CHANNEL_SWITCH,
  RX_COMPLETE,
  TX,
} mode_t;

/*
 * String representation of the measurement modes
 */
char *mode_str[] = {
  "MANUAL_TRANSITIONS",
  "AUTO_TRANSITIONS",
  "CHANNEL_SWITCH",
  "RX_COMPLETE",
  "TX for RX_COMPLETE",
};

/*
 * Statistics structure definition
 */
typedef struct statistics {
  float average;
  float min;
  float max;
} statistics_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/*
 * Calculate the statistics - min, max, average - of the measurements
 */
static void calc_statistics();

/*
 * Print the statistics of the measurements
 */
static void print_statistics();

/*
 * Measurement handlers for each mode
 */
static void handle_manual_transitions(RAIL_Handle_t rail_handle);
static void handle_auto_transitions(RAIL_Handle_t rail_handle);
static void handle_channel_switch(RAIL_Handle_t rail_handle);
static void handle_rx_complete(RAIL_Handle_t rail_handle);
static void handle_tx(RAIL_Handle_t rail_handle);

/*
 * Set the state transitions for the radio
 */
static void set_state_transitions(RAIL_Handle_t rail_handle,
                                  RAIL_RadioState_t rx_success,
                                  RAIL_RadioState_t rx_error,
                                  RAIL_RadioState_t tx_success,
                                  RAIL_RadioState_t tx_error);

/*
 * Assign the buffers to the pointers passed as arguments
 */
static void buffer_designation(int num_buffers, ...);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

extern float timer_freq;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Contains the last events the last calibration attempt failed with
static volatile uint64_t cal_error_events = 0;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

// Application starts in MANUAL_TRANSITIONS mode
static volatile mode_t current_mode = MANUAL_TRANSITIONS;

// Measurement requested and pending flags
static volatile bool measurement_requested = false;
static volatile bool measurement_pending = false;

// 2D buffer to store all measurements
static float buffer[8][REPEAT_CYCLES];

// Pointer to the buffers and statistics for each measurement
static float *request_to_RX_active;
static statistics_t request_to_RX_active_stats;
static float *request_to_LNA_active;
static statistics_t request_to_LNA_active_stats;
static float *RX_active_to_idle;
static statistics_t RX_active_to_idle_stats;
static float *LNA_active_to_idle;
static statistics_t LNA_active_to_idle_stats;

static float *request_to_TX_active;
static statistics_t request_to_TX_active_stats;
static float *request_to_PA_active;
static statistics_t request_to_PA_active_stats;
static float *TX_active_to_idle;
static statistics_t TX_active_to_idle_stats;
static float *PA_active_to_idle;
static statistics_t PA_active_to_idle_stats;

static float *LNA_to_PA;
static statistics_t LNA_to_PA_stats;
static float *RX_to_TX;
static statistics_t RX_to_TX_stats;
static float *PA_to_LNA;
static statistics_t PA_to_LNA_stats;
static float *TX_to_RX;
static statistics_t TX_to_RX_stats;

static float *RX_to_RX_p_s;
static statistics_t RX_to_RX_p_s_stats;
static float *RX_to_RX_s_p;
static statistics_t RX_to_RX_s_p_stats;
static float *TX_to_TX_p_s;
static statistics_t TX_to_TX_p_s_stats;
static float *TX_to_TX_s_p;
static statistics_t TX_to_TX_s_p_stats;

// TX FIFO buffer
__ALIGNED(RAIL_FIFO_ALIGNMENT)
static uint8_t tx_fifo[TX_FIFO_SIZE];

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

  if (cal_error_events) {
    app_log_warning("RAIL event mask for calibration failed: %llu\n",
                    cal_error_events);
    cal_error_events = 0;
  }

  if (measurement_requested) {
    measurement_pending = true;

    // Idle the radio before starting the measurements
    rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
    }

    if (RAIL_SetTxFifo(rail_handle, tx_fifo, TX_FIFO_SIZE,
                       TX_FIFO_SIZE) != TX_FIFO_SIZE) {
      app_log_error("RAIL_SetTxFifo failed\n");
    }

    switch (current_mode) {
      case MANUAL_TRANSITIONS:
        handle_manual_transitions(rail_handle);
        break;

      case AUTO_TRANSITIONS:
        handle_auto_transitions(rail_handle);
        break;

      case CHANNEL_SWITCH:
        handle_channel_switch(rail_handle);
        break;

      case RX_COMPLETE:
        handle_rx_complete(rail_handle);
        break;

      case TX:
        handle_tx(rail_handle);
        break;

      default:
        break;
    }

    measurement_pending = false;
    measurement_requested = false;
  }
}

static void handle_manual_transitions(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t rail_status;
  buffer_designation(8, &request_to_RX_active, &request_to_LNA_active,
                     &RX_active_to_idle, &LNA_active_to_idle,
                     &request_to_TX_active, &request_to_PA_active,
                     &TX_active_to_idle, &PA_active_to_idle);

  set_state_transitions(rail_handle,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE);

  for (int i = 0; i < REPEAT_CYCLES; i++) {
    rail_status = RAIL_StartRx(rail_handle, PRIMARY_CHANNEL, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
    }
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);
    request_to_RX_active[i] = GET_BUSY_TIME();
    request_to_LNA_active[i] = GET_BUSY_OR_READY_TIME();
    rail_status = RAIL_Idle(rail_handle, IDLE_MODE, true);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
    }
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_OFF);

    RX_active_to_idle[i] = GET_BUSY_TIME();
    LNA_active_to_idle[i] = GET_BUSY_OR_READY_TIME();

    rail_status = RAIL_StartTx(rail_handle,
                               PRIMARY_CHANNEL,
                               RAIL_TX_OPTION_RESEND,
                               NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartTx failed with code %lu\n", rail_status);
    }
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_TX);

    request_to_TX_active[i] = GET_BUSY_TIME();
    request_to_PA_active[i] = GET_BUSY_OR_READY_TIME();
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_OFF);
    TX_active_to_idle[i] = GET_BUSY_TIME();
    PA_active_to_idle[i] = GET_BUSY_OR_READY_TIME();
  }

  rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
  }
  calc_statistics();
  print_statistics();
}

static void handle_auto_transitions(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t rail_status;

  buffer_designation(4, &LNA_to_PA, &RX_to_TX, &PA_to_LNA, &TX_to_RX);

  set_state_transitions(rail_handle,
                        RAIL_RF_STATE_TX,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_RX,
                        RAIL_RF_STATE_RX);

  rail_status = RAIL_StartRx(rail_handle, PRIMARY_CHANNEL, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
  }

  for (int i = 0; i < REPEAT_CYCLES; i++) {
    rail_status = RAIL_StartTx(rail_handle,
                               PRIMARY_CHANNEL,
                               RAIL_TX_OPTION_RESEND,
                               NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartTx failed with code %lu\n", rail_status);
    }
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_TX);

    LNA_to_PA[i] = GET_BUSY_OR_READY_TIME();
    RX_to_TX[i] = GET_BUSY_TIME();

    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);

    PA_to_LNA[i] = GET_BUSY_OR_READY_TIME();
    TX_to_RX[i] = GET_BUSY_TIME();
  }

  rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
  }
  calc_statistics();
  print_statistics();
}

static void handle_channel_switch(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t rail_status;

  if (PRIMARY_CHANNEL == SECONDARY_CHANNEL) {
    app_log_warning("Primary and secondary channels are the same!\n");
    return;
  }

  buffer_designation(4, &RX_to_RX_p_s, &RX_to_RX_s_p, &TX_to_TX_p_s,
                     &TX_to_TX_s_p);
  set_state_transitions(rail_handle,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE);

  rail_status = RAIL_StartRx(rail_handle, PRIMARY_CHANNEL, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
  }
  WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);

  for (int i = 0; i < REPEAT_CYCLES; i++) {
    rail_status = RAIL_StartRx(rail_handle, SECONDARY_CHANNEL, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
    }
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);
    RX_to_RX_p_s[i] = GET_ACTIVE_TO_ACTIVE_TIME();
    rail_status = RAIL_StartRx(rail_handle, PRIMARY_CHANNEL, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
    }
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);
    RX_to_RX_s_p[i] = GET_ACTIVE_TO_ACTIVE_TIME();
  }

  rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
  }

  rail_status = RAIL_StartTx(rail_handle,
                             PRIMARY_CHANNEL,
                             RAIL_TX_OPTION_RESEND,
                             NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_StartTx failed with code %lu\n", rail_status);
  }
  WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_TXPD);

  for (int i = 0; i < REPEAT_CYCLES; i++) {
    rail_status = RAIL_StartTx(rail_handle,
                               SECONDARY_CHANNEL,
                               RAIL_TX_OPTION_RESEND,
                               NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartTx failed with code %lu\n", rail_status);
    }

    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_TXPD);
    TX_to_TX_p_s[i] = GET_ACTIVE_TO_ACTIVE_TIME();
    rail_status = RAIL_StartTx(rail_handle,
                               PRIMARY_CHANNEL,
                               RAIL_TX_OPTION_RESEND,
                               NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartTx failed with code %lu\n", rail_status);
    }
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_TXPD);
    TX_to_TX_s_p[i] = GET_ACTIVE_TO_ACTIVE_TIME();
  }

  rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
  }
  calc_statistics();
  print_statistics();
}

/*
 * Rx Completion measurements require a transmitter node. This measurement
 * can be interrupted if no compatible transmitter is available.
 */
static void handle_rx_complete(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t rail_status;

  buffer_designation(4, &RX_active_to_idle, &RX_to_TX, &RX_to_RX_p_s,
                     &RX_to_RX_s_p);

  set_state_transitions(rail_handle,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE);

  measurement_requested = false;

  bool exit_loop = false;

  app_log_warning("Transmitter node required!\n");
  rail_status = RAIL_StartRx(rail_handle, PRIMARY_CHANNEL, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
  }
  WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);
  for (int i = 0; i < REPEAT_CYCLES && !exit_loop; i++) {
    while (RAIL_GetRadioStateAlt(rail_handle) != RAIL_RAC_STATE_OFF) {
      if (measurement_requested) {
        exit_loop = true;
        break;
      }
    }
    if (exit_loop) {
      break;
    }
    RX_active_to_idle[i] = GET_BUSY_TIME();
    rail_status = RAIL_StartRx(rail_handle, PRIMARY_CHANNEL, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
    }
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);
  }
  if (exit_loop) {
    app_log_warning("Measurement interrupted!\n");
    rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
    }
    return;
  }

  // state transitions only configurable in Idle state
  rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
  }

  set_state_transitions(rail_handle,
                        RAIL_RF_STATE_TX,
                        RAIL_RF_STATE_RX,
                        RAIL_RF_STATE_RX,
                        RAIL_RF_STATE_RX);

  rail_status = RAIL_StartRx(rail_handle, PRIMARY_CHANNEL, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
  }
  WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);

  for (int i = 0; i < REPEAT_CYCLES && !exit_loop; i++) {
    if (RAIL_SetTxFifo(rail_handle, tx_fifo, TX_FIFO_SIZE,
                       TX_FIFO_SIZE) != TX_FIFO_SIZE) {
      app_log_error("RAIL_SetTxFifo failed\n");
    }
    bool error = false;
    while (RAIL_GetRadioStateAlt(rail_handle) != RAIL_RAC_STATE_RX2TX) {
      if (measurement_requested) {
        exit_loop = true;
        break;
      }
      if (RAIL_GetRadioStateAlt(rail_handle) == RAIL_RAC_STATE_OFF) {
        rail_status = RAIL_StartRx(rail_handle, PRIMARY_CHANNEL, NULL);
        if (rail_status != RAIL_STATUS_NO_ERROR) {
          app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
        }
        WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);
        i--;
        error = true;
        break;
      }
    }
    if (error) {
      continue;
    }
    if (exit_loop) {
      app_log_warning("Measurement interrupted!\n");
      rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
      if (rail_status != RAIL_STATUS_NO_ERROR) {
        app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
      }
      return;
    }

    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_TX);
    RX_to_TX[i] = GET_ACTIVE_TO_ACTIVE_TIME();
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);
  }
  if (exit_loop) {
    return;
  }

  rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
  }

  set_state_transitions(rail_handle,
                        RAIL_RF_STATE_RX,
                        RAIL_RF_STATE_RX,
                        RAIL_RF_STATE_IDLE,
                        RAIL_RF_STATE_IDLE);

  rail_status = RAIL_StartRx(rail_handle, PRIMARY_CHANNEL, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_StartRx failed with code %lu\n", rail_status);
  }
  WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);

  for (int i = 0; i < REPEAT_CYCLES && !exit_loop; i++) {
    while (RAIL_GetRadioStateAlt(rail_handle) != RAIL_RAC_STATE_RX2RX) {
      if (measurement_requested) {
        exit_loop = true;
        break;
      }
    }
    if (exit_loop) {
      break;
    }
    WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_RXSEARCH);
    // reuse RX_to_RX_p_s for RX to RX time
    RX_to_RX_p_s[i] = GET_ACTIVE_TO_ACTIVE_TIME();
  }
  if (exit_loop) {
    app_log_warning("Measurement interrupted!\n");
    rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
    }
    return;
  }
  rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
  }

  calc_statistics();
  print_statistics();
}

static void handle_tx(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t rail_status;

  uint32_t bitrate = RAIL_GetBitRate(rail_handle);

  if (RAIL_SetTxFifo(rail_handle, tx_fifo, TX_FIFO_SIZE,
                     TX_FIFO_SIZE) != TX_FIFO_SIZE) {
    app_log_error("RAIL_SetTxFifo failed\n");
  }
  rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_Idle failed with code %lu\n", rail_status);
  }

  uint32_t delay = (SL_RAIL_STATE_TRANSITION_TEST_TX_PERIOD
                    * ONE_SECOND_IN_US) / bitrate;

  if (delay > RAIL_MAXIMUM_TRANSITION_US) {
    delay = RAIL_MAXIMUM_TRANSITION_US - 1;
  } else if (delay < RAIL_MINIMUM_TRANSITION_US) {
    delay = RAIL_MINIMUM_TRANSITION_US + 1;
  }

  RAIL_TxRepeatConfig_t repeat_config = {
    .iterations = REPEAT_CYCLES * 3,
    .repeatOptions = RAIL_TX_REPEAT_OPTION_START_TO_START,
    .delayOrHop = { .delay = delay },
  };

  rail_status = RAIL_SetNextTxRepeat(rail_handle, &repeat_config);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_SetNextTxRepeat failed with code %lu\n",
                  rail_status);
  }

  rail_status = RAIL_StartTx(rail_handle,
                             PRIMARY_CHANNEL,
                             RAIL_TX_OPTION_RESEND,
                             NULL);

  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_StartTx failed with code %lu\n", rail_status);
  }

  app_log_info(
    "Transmission started with %d iterations and %lu bits delay\n",
    repeat_config.iterations,
    repeat_config.delayOrHop.delay);

  WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_TX);
  WAIT_FOR_RADIO_STATE(RAIL_RAC_STATE_OFF);

  app_log_info("Scheduled transmission completed for %d cycles.\n",
               REPEAT_CYCLES * 3);
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
      cal_error_events = (events & RAIL_EVENT_CAL_NEEDED);
    }
  }
}

/******************************************************************************
 * Button callback, called if any button is pressed or released.
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (SL_SIMPLE_BUTTON_INSTANCE(0) == handle) {
      measurement_requested = true;
    } else if (SL_SIMPLE_BUTTON_INSTANCE(1) == handle) {
      if (!measurement_pending) {
        current_mode++;
        if (current_mode > TX) {
          current_mode = MANUAL_TRANSITIONS;
        }
        app_log_info("Current mode: %s\n", mode_str[current_mode]);
      } else {
        app_log_warning("Measurement in progress!\n");
      }
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

void calc_statistics()
{
  switch (current_mode)
  {
    case MANUAL_TRANSITIONS:
      request_to_LNA_active_stats.min = request_to_LNA_active[0];
      request_to_RX_active_stats.min = request_to_RX_active[0];
      RX_active_to_idle_stats.min = RX_active_to_idle[0];
      LNA_active_to_idle_stats.min = LNA_active_to_idle[0];
      request_to_TX_active_stats.min = request_to_TX_active[0];
      request_to_PA_active_stats.min = request_to_PA_active[0];
      TX_active_to_idle_stats.min = TX_active_to_idle[0];
      PA_active_to_idle_stats.min = PA_active_to_idle[0];
      request_to_LNA_active_stats.max = request_to_LNA_active[0];
      request_to_RX_active_stats.max = request_to_RX_active[0];
      RX_active_to_idle_stats.max = RX_active_to_idle[0];
      LNA_active_to_idle_stats.max = LNA_active_to_idle[0];
      request_to_TX_active_stats.max = request_to_TX_active[0];
      request_to_PA_active_stats.max = request_to_PA_active[0];
      TX_active_to_idle_stats.max = TX_active_to_idle[0];
      PA_active_to_idle_stats.max = PA_active_to_idle[0];

      for (int i = 0; i < REPEAT_CYCLES; i++) {
        request_to_RX_active_stats.average += request_to_RX_active[i];
        request_to_LNA_active_stats.average += request_to_LNA_active[i];
        RX_active_to_idle_stats.average += RX_active_to_idle[i];
        LNA_active_to_idle_stats.average += LNA_active_to_idle[i];
        request_to_TX_active_stats.average += request_to_TX_active[i];
        request_to_PA_active_stats.average += request_to_PA_active[i];
        TX_active_to_idle_stats.average += TX_active_to_idle[i];
        PA_active_to_idle_stats.average += PA_active_to_idle[i];

        if (request_to_RX_active[i] < request_to_RX_active_stats.min) {
          request_to_RX_active_stats.min = request_to_RX_active[i];
        }
        if (request_to_RX_active[i] > request_to_RX_active_stats.max) {
          request_to_RX_active_stats.max = request_to_RX_active[i];
        }

        if (request_to_LNA_active[i] < request_to_LNA_active_stats.min) {
          request_to_LNA_active_stats.min = request_to_LNA_active[i];
        }
        if (request_to_LNA_active[i] > request_to_LNA_active_stats.max) {
          request_to_LNA_active_stats.max = request_to_LNA_active[i];
        }

        if (RX_active_to_idle[i] < RX_active_to_idle_stats.min) {
          RX_active_to_idle_stats.min = RX_active_to_idle[i];
        }
        if (RX_active_to_idle[i] > RX_active_to_idle_stats.max) {
          RX_active_to_idle_stats.max = RX_active_to_idle[i];
        }

        if (LNA_active_to_idle[i] < LNA_active_to_idle_stats.min) {
          LNA_active_to_idle_stats.min = LNA_active_to_idle[i];
        }
        if (LNA_active_to_idle[i] > LNA_active_to_idle_stats.max) {
          LNA_active_to_idle_stats.max = LNA_active_to_idle[i];
        }

        if (request_to_TX_active[i] < request_to_TX_active_stats.min) {
          request_to_TX_active_stats.min = request_to_TX_active[i];
        }
        if (request_to_TX_active[i] > request_to_TX_active_stats.max) {
          request_to_TX_active_stats.max = request_to_TX_active[i];
        }

        if (request_to_PA_active[i] < request_to_PA_active_stats.min) {
          request_to_PA_active_stats.min = request_to_PA_active[i];
        }
        if (request_to_PA_active[i] > request_to_PA_active_stats.max) {
          request_to_PA_active_stats.max = request_to_PA_active[i];
        }

        if (TX_active_to_idle[i] < TX_active_to_idle_stats.min) {
          TX_active_to_idle_stats.min = TX_active_to_idle[i];
        }
        if (TX_active_to_idle[i] > TX_active_to_idle_stats.max) {
          TX_active_to_idle_stats.max = TX_active_to_idle[i];
        }

        if (PA_active_to_idle[i] < PA_active_to_idle_stats.min) {
          PA_active_to_idle_stats.min = PA_active_to_idle[i];
        }
        if (PA_active_to_idle[i] > PA_active_to_idle_stats.max) {
          PA_active_to_idle_stats.max = PA_active_to_idle[i];
        }
      }

      request_to_RX_active_stats.average /= REPEAT_CYCLES;
      request_to_LNA_active_stats.average /= REPEAT_CYCLES;
      RX_active_to_idle_stats.average /= REPEAT_CYCLES;
      LNA_active_to_idle_stats.average /= REPEAT_CYCLES;
      request_to_TX_active_stats.average /= REPEAT_CYCLES;
      request_to_PA_active_stats.average /= REPEAT_CYCLES;
      TX_active_to_idle_stats.average /= REPEAT_CYCLES;
      PA_active_to_idle_stats.average /= REPEAT_CYCLES;

      break;
    case AUTO_TRANSITIONS:
      LNA_to_PA_stats.min = LNA_to_PA[0];
      RX_to_TX_stats.min = RX_to_TX[0];
      PA_to_LNA_stats.min = PA_to_LNA[0];
      TX_to_RX_stats.min = TX_to_RX[0];
      LNA_to_PA_stats.max = LNA_to_PA[0];
      RX_to_TX_stats.max = RX_to_TX[0];
      PA_to_LNA_stats.max = PA_to_LNA[0];
      TX_to_RX_stats.max = TX_to_RX[0];

      for (int i = 0; i < REPEAT_CYCLES; i++) {
        LNA_to_PA_stats.average += LNA_to_PA[i];
        RX_to_TX_stats.average += RX_to_TX[i];
        PA_to_LNA_stats.average += PA_to_LNA[i];
        TX_to_RX_stats.average += TX_to_RX[i];

        if (LNA_to_PA[i] < LNA_to_PA_stats.min) {
          LNA_to_PA_stats.min = LNA_to_PA[i];
        }
        if (LNA_to_PA[i] > LNA_to_PA_stats.max) {
          LNA_to_PA_stats.max = LNA_to_PA[i];
        }

        if (RX_to_TX[i] < RX_to_TX_stats.min) {
          RX_to_TX_stats.min = RX_to_TX[i];
        }
        if (RX_to_TX[i] > RX_to_TX_stats.max) {
          RX_to_TX_stats.max = RX_to_TX[i];
        }

        if (PA_to_LNA[i] < PA_to_LNA_stats.min) {
          PA_to_LNA_stats.min = PA_to_LNA[i];
        }
        if (PA_to_LNA[i] > PA_to_LNA_stats.max) {
          PA_to_LNA_stats.max = PA_to_LNA[i];
        }

        if (TX_to_RX[i] < TX_to_RX_stats.min) {
          TX_to_RX_stats.min = TX_to_RX[i];
        }
        if (TX_to_RX[i] > TX_to_RX_stats.max) {
          TX_to_RX_stats.max = TX_to_RX[i];
        }
      }

      LNA_to_PA_stats.average /= REPEAT_CYCLES;
      RX_to_TX_stats.average /= REPEAT_CYCLES;
      PA_to_LNA_stats.average /= REPEAT_CYCLES;
      TX_to_RX_stats.average /= REPEAT_CYCLES;

      break;
    case CHANNEL_SWITCH:
      RX_to_RX_p_s_stats.min = RX_to_RX_p_s[0];
      RX_to_RX_s_p_stats.min = RX_to_RX_s_p[0];
      TX_to_TX_p_s_stats.min = TX_to_TX_p_s[0];
      TX_to_TX_s_p_stats.min = TX_to_TX_s_p[0];
      RX_to_RX_p_s_stats.max = RX_to_RX_p_s[0];
      RX_to_RX_s_p_stats.max = RX_to_RX_s_p[0];
      TX_to_TX_p_s_stats.max = TX_to_TX_p_s[0];
      TX_to_TX_s_p_stats.max = TX_to_TX_s_p[0];

      for (int i = 0; i < REPEAT_CYCLES; i++) {
        RX_to_RX_p_s_stats.average += RX_to_RX_p_s[i];
        RX_to_RX_s_p_stats.average += RX_to_RX_s_p[i];
        TX_to_TX_p_s_stats.average += TX_to_TX_p_s[i];
        TX_to_TX_s_p_stats.average += TX_to_TX_s_p[i];

        if (RX_to_RX_p_s[i] < RX_to_RX_p_s_stats.min) {
          RX_to_RX_p_s_stats.min = RX_to_RX_p_s[i];
        }
        if (RX_to_RX_p_s[i] > RX_to_RX_p_s_stats.max) {
          RX_to_RX_p_s_stats.max = RX_to_RX_p_s[i];
        }

        if (RX_to_RX_s_p[i] < RX_to_RX_s_p_stats.min) {
          RX_to_RX_s_p_stats.min = RX_to_RX_s_p[i];
        }
        if (RX_to_RX_s_p[i] > RX_to_RX_s_p_stats.max) {
          RX_to_RX_s_p_stats.max = RX_to_RX_s_p[i];
        }

        if (TX_to_TX_p_s[i] < TX_to_TX_p_s_stats.min) {
          TX_to_TX_p_s_stats.min = TX_to_TX_p_s[i];
        }
        if (TX_to_TX_p_s[i] > TX_to_TX_p_s_stats.max) {
          TX_to_TX_p_s_stats.max = TX_to_TX_p_s[i];
        }

        if (TX_to_TX_s_p[i] < TX_to_TX_s_p_stats.min) {
          TX_to_TX_s_p_stats.min = TX_to_TX_s_p[i];
        }
        if (TX_to_TX_s_p[i] > TX_to_TX_s_p_stats.max) {
          TX_to_TX_s_p_stats.max = TX_to_TX_s_p[i];
        }
      }

      RX_to_RX_p_s_stats.average /= REPEAT_CYCLES;
      RX_to_RX_s_p_stats.average /= REPEAT_CYCLES;
      TX_to_TX_p_s_stats.average /= REPEAT_CYCLES;
      TX_to_TX_s_p_stats.average /= REPEAT_CYCLES;

      break;

    case RX_COMPLETE:
      // uses RX_active_to_idle RX_to_TX and RX_to_RX_p_s
      RX_active_to_idle_stats.min = RX_active_to_idle[0];
      RX_to_TX_stats.min = RX_to_TX[0];
      RX_to_RX_p_s_stats.min = RX_to_RX_p_s[0];
      RX_active_to_idle_stats.max = RX_active_to_idle[0];
      RX_to_TX_stats.max = RX_to_TX[0];
      RX_to_RX_p_s_stats.max = RX_to_RX_p_s[0];

      for (int i = 0; i < REPEAT_CYCLES; i++) {
        RX_active_to_idle_stats.average += RX_active_to_idle[i];
        RX_to_TX_stats.average += RX_to_TX[i];
        RX_to_RX_p_s_stats.average += RX_to_RX_p_s[i];

        if (RX_active_to_idle[i] < RX_active_to_idle_stats.min) {
          RX_active_to_idle_stats.min = RX_active_to_idle[i];
        }
        if (RX_active_to_idle[i] > RX_active_to_idle_stats.max) {
          RX_active_to_idle_stats.max = RX_active_to_idle[i];
        }

        if (RX_to_TX[i] < RX_to_TX_stats.min) {
          RX_to_TX_stats.min = RX_to_TX[i];
        }
        if (RX_to_TX[i] > RX_to_TX_stats.max) {
          RX_to_TX_stats.max = RX_to_TX[i];
        }

        if (RX_to_RX_p_s[i] < RX_to_RX_p_s_stats.min) {
          RX_to_RX_p_s_stats.min = RX_to_RX_p_s[i];
        }
        if (RX_to_RX_p_s[i] > RX_to_RX_p_s_stats.max) {
          RX_to_RX_p_s_stats.max = RX_to_RX_p_s[i];
        }
      }

      RX_active_to_idle_stats.average /= REPEAT_CYCLES;
      RX_to_TX_stats.average /= REPEAT_CYCLES;
      RX_to_RX_p_s_stats.average /= REPEAT_CYCLES;
      break;

    case TX:
    default:
      break;
  }
}

static void print_statistics()
{
  switch (current_mode)
  {
    case MANUAL_TRANSITIONS:
      app_log_info(
        "Request to RX active (BUSY + READY): average: %.2f us min: %.2f us max: %.2f us\n",
        request_to_RX_active_stats.average,
        request_to_RX_active_stats.min,
        request_to_RX_active_stats.max);
      app_log_info(
        "Request to LNA active (BUSY): average: %.2f us min: %.2f us max: %.2f us\n",
        request_to_LNA_active_stats.average,
        request_to_LNA_active_stats.min,
        request_to_LNA_active_stats.max);
      app_log_info(
        "RX active to idle (BUSY + READY): average: %.2f us min: %.2f us max: %.2f us\n",
        RX_active_to_idle_stats.average,
        RX_active_to_idle_stats.min,
        RX_active_to_idle_stats.max);
      app_log_info(
        "LNA active to idle (BUSY): average: %.2f us min: %.2f us max: %.2f us\n",
        LNA_active_to_idle_stats.average,
        LNA_active_to_idle_stats.min,
        LNA_active_to_idle_stats.max);
      app_log_info(
        "Request to TX active (BUSY + READY): average: %.2f us min: %.2f us max: %.2f us\n",
        request_to_TX_active_stats.average,
        request_to_TX_active_stats.min,
        request_to_TX_active_stats.max);
      app_log_info(
        "Request to PA active (BUSY): average: %.2f us min: %.2f us max: %.2f us\n",
        request_to_PA_active_stats.average,
        request_to_PA_active_stats.min,
        request_to_PA_active_stats.max);
      app_log_info(
        "TX active to idle (BUSY + READY): average: %.2f us min: %.2f us max: %.2f us\n",
        TX_active_to_idle_stats.average,
        TX_active_to_idle_stats.min,
        TX_active_to_idle_stats.max);
      app_log_info(
        "PA active to idle (BUSY): average: %.2f us min: %.2f us max: %.2f us\n",
        PA_active_to_idle_stats.average,
        PA_active_to_idle_stats.min,
        PA_active_to_idle_stats.max);
      break;
    case AUTO_TRANSITIONS:
      app_log_info(
        "LNA to PA (BUSY between): average: %.2f us min: %.2f us max: %.2f us\n",
        LNA_to_PA_stats.average,
        LNA_to_PA_stats.min,
        LNA_to_PA_stats.max);
      app_log_info(
        "RX to TX (READY + BUSY between): average: %.2f us min: %.2f us max: %.2f us\n",
        RX_to_TX_stats.average,
        RX_to_TX_stats.min,
        RX_to_TX_stats.max);
      app_log_info(
        "PA to LNA (BUSY between): average: %.2f us min: %.2f us max: %.2f us\n",
        PA_to_LNA_stats.average,
        PA_to_LNA_stats.min,
        PA_to_LNA_stats.max);
      app_log_info(
        "TX to RX (READY + BUSY between): average: %.2f us min: %.2f us max: %.2f us\n",
        TX_to_RX_stats.average,
        TX_to_RX_stats.min,
        TX_to_RX_stats.max);
      break;
    case CHANNEL_SWITCH:
      app_log_info(
        "RX to RX primary to secondary (READY + BUSY between): average: %.2f us min: %.2f us max: %.2f us\n",
        RX_to_RX_p_s_stats.average,
        RX_to_RX_p_s_stats.min,
        RX_to_RX_p_s_stats.max);
      app_log_info(
        "RX to RX secondary to primary (READY + BUSY between): average: %.2f us min: %.2f us max: %.2f us\n",
        RX_to_RX_s_p_stats.average,
        RX_to_RX_s_p_stats.min,
        RX_to_RX_s_p_stats.max);
      app_log_info(
        "TX to TX primary to secondary: average (READY + BUSY between): %.2f us min: %.2f us max: %.2f us\n",
        TX_to_TX_p_s_stats.average,
        TX_to_TX_p_s_stats.min,
        TX_to_TX_p_s_stats.max);
      app_log_info(
        "TX to TX secondary to primary: average (READY + BUSY between): %.2f us min: %.2f us max: %.2f us\n",
        TX_to_TX_s_p_stats.average,
        TX_to_TX_s_p_stats.min,
        TX_to_TX_s_p_stats.max);
      break;
    case RX_COMPLETE:
      app_log_info(
        "RX active to idle (BUSY + READY): average: %.2f us min: %.2f us max: %.2f us\n",
        RX_active_to_idle_stats.average,
        RX_active_to_idle_stats.min,
        RX_active_to_idle_stats.max);
      app_log_info(
        "RX to TX (BUSY + READY between): average: %.2f us min: %.2f us max: %.2f us\n",
        RX_to_TX_stats.average,
        RX_to_TX_stats.min,
        RX_to_TX_stats.max);
      app_log_info(
        "RX to RX (READY + BUSY between): average: %.2f us min: %.2f us max: %.2f us\n",
        RX_to_RX_p_s_stats.average,
        RX_to_RX_p_s_stats.min,
        RX_to_RX_p_s_stats.max);
      break;
    case TX:
    default:
      break;
  }
}

static void buffer_designation(int num_buffers, ...)
{
  va_list args;
  va_start(args, num_buffers);

  if (num_buffers > 8) {
    app_log_error("Too many buffers requested (%d), maximum is 8\n",
                  num_buffers);
    num_buffers = 8;
  }

  for (int i = 0; i < num_buffers; i++) {
    float **buffer_ptr = va_arg(args, float **);
    *buffer_ptr = buffer[i];
  }

  va_end(args);
}

static void set_state_transitions(RAIL_Handle_t rail_handle,
                                  RAIL_RadioState_t rx_success,
                                  RAIL_RadioState_t rx_error,
                                  RAIL_RadioState_t tx_success,
                                  RAIL_RadioState_t tx_error)
{
  RAIL_Status_t rail_status;
  RAIL_StateTransitions_t rx_transitions;
  RAIL_StateTransitions_t tx_transitions;

  rx_transitions.error = rx_error;
  rx_transitions.success = rx_success;
  tx_transitions.error = tx_error;
  tx_transitions.success = tx_success;

  rail_status = RAIL_SetRxTransitions(rail_handle, &rx_transitions);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_SetRxTransitions failed with code %lu\n", rail_status);
  }

  rail_status = RAIL_SetTxTransitions(rail_handle, &tx_transitions);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_SetTxTransitions failed with code %lu\n", rail_status);
  }
}
