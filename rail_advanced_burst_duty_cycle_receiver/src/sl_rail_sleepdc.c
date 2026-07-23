/***************************************************************************//**
 * @file
 * @brief sl_rail_sleepdc.c
 * @details Main sleep duty cycling implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_rail_util_init.h"
#include "sl_rail_sleepdc.h"
#include "sl_rail_advanced_burst_dc_receiver_config.h"
#include "app_process.h"
#include "sl_common.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief Helper function to abort duty cycling and report error reason
 * @param[in] reason Reason for aborting duty cycling
 *****************************************************************************/
static void abort_rx(sl_rail_sleepdc_event_t reason);

/******************************************************************************
 * @brief Schedule the next rx window in duty cycling mode
 *****************************************************************************/
static void start_next_rx();

/******************************************************************************
 * @brief Schedule the timed rx for the anchored frame
 *****************************************************************************/
static void schedule_timed_rx();

/******************************************************************************
 * @brief Process the burst packet, extract the timestamp and set up for the
 * next frame
 * @param[in] packet_handle Handle of the received packet
 * @param[in] p_packet_info Info of the received packet
 *****************************************************************************/
static sl_rail_sleepdc_event_t process_burst_packet(
  sl_rail_rx_packet_handle_t packet_handle,
  sl_rail_rx_packet_info_t *p_packet_info);

/******************************************************************************
 * @brief Process the received packet and decide if it's a burst packet or data
 * packet, and handle accordingly. Also responsible for generating the callback
 * to app about the wakeup or any error in processing, which is done in the
 * non-interrupt context to avoid doing too much in the interrupt
 *****************************************************************************/
static void process_packet();

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
extern volatile app_state_t state;

// Working mode of sl_rail_sleepdc
sl_rail_sleepdc_wakeup_mode_t mode = WAKEUP_MODE_OFF;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// RAIL handle used by sl_rail_sleepdc
static sl_rail_handle_t rail_handle = SL_RAIL_EFR32_HANDLE;

// Channel used by sl_rail_sleepdc
static uint16_t wakeup_channel;

// anchor time of the data packet
static volatile sl_rail_time_t anchor_time;

static volatile bool was_last_wakeup_accurate = false;

// these control the non-interrupt context callback
static volatile bool generate_callback = false;
static volatile sl_rail_sleepdc_event_t status_event =
  SL_RAIL_SLEEPDC_EVENT_WAKEUP;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void sl_rail_sleepdc_init(void)
{
  // get the rail handle for later use.
  rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
}

sl_status_t sl_rail_sleepdc_set_mode(sl_rail_sleepdc_wakeup_mode_t wakeup_mode,
                                     uint16_t channel)
{
  // we only accept OFF if wakeup is running already
  if ((mode == WAKEUP_MODE_OFF) && (wakeup_mode == WAKEUP_MODE_OFF)) {
    return SL_STATUS_IN_PROGRESS;
  }
  if (wakeup_mode == WAKEUP_MODE_OFF) {
    state = S_IDLE;
    if (sl_rail_idle(rail_handle, SL_RAIL_IDLE_ABORT,
                     true) != SL_RAIL_STATUS_NO_ERROR) {
      return SL_RAIL_STATUS_INVALID_STATE;
    }
  } else { // on burst
    wakeup_channel = channel;
    start_next_rx();
  }
  mode = wakeup_mode;
  return SL_STATUS_OK;
}

// process function is only used to generate callbacks
void sl_rail_sleepdc_process()
{
  if (generate_callback) {
    generate_callback = false;
    if (status_event == SL_RAIL_SLEEPDC_EVENT_WAKEUP) {
      sl_rail_sleepdc_cb_wakeup_received();
    } else {
      sl_rail_sleepdc_cb_wakeup_aborted(status_event);
    }
  }
}

void sl_rail_sleepdc_util_on_event(sl_rail_handle_t rail_handle,
                                   sl_rail_events_t events)
{
  if (events & SL_RAIL_EVENTS_RX_COMPLETION) {
    if (events & SL_RAIL_EVENT_RX_PACKET_RECEIVED) {
      process_packet();
      if (state == S_RX_WAITING_DATA) {
        state = S_RX_DATA_RECEIVED;
      }
    } else {
      if (events & SL_RAIL_EVENT_RX_FRAME_ERROR) {
        sl_rail_sleepdc_cb_wakeup_event_irq(SL_RAIL_SLEEPDC_EVENT_FRAME_ERROR);
      } else {
        sl_rail_sleepdc_cb_wakeup_event_irq(SL_RAIL_SLEEPDC_EVENT_RX_ERROR);
      }
    }
  }

  if (events
      & (SL_RAIL_EVENT_RX_SCHEDULED_RX_END
         | SL_RAIL_EVENT_RX_SCHEDULED_RX_MISSED)) {
    if (sl_rail_idle(rail_handle, SL_RAIL_IDLE,
                     true) != SL_RAIL_STATUS_NO_ERROR) {
      abort_rx(SL_RAIL_SLEEPDC_EVENT_SCHEDULE_RX_FAIL);
    }
    if (state == S_RX_WAITING_DATA) {
      sl_rail_sleepdc_cb_wakeup_event_irq(
        SL_RAIL_SLEEPDC_EVENT_DATA_TIMEOUT);
      state = S_RX_DUTY_CYCLE;
    }
    if ((state == S_RX_DUTY_CYCLE) || (state == S_RX_DATA_RECEIVED)) {
      start_next_rx();
    } else if (state == S_RX_WAITING_DATA_SETUP) {
      schedule_timed_rx();
    } else {
    }
  }

  // dmp errors.
  if (events & SL_RAIL_EVENT_SCHEDULER_STATUS) {
    sl_rail_scheduler_status_t status;
    if (sl_rail_get_scheduler_status(rail_handle, &status,
                                     NULL) != SL_RAIL_STATUS_NO_ERROR) {
      abort_rx(SL_RAIL_SLEEPDC_EVENT_DMP_ASSERT);
    }
    // scheduler status is 4 bit error and 4 bit operation, so we check the only
    // operation we support
    if (status & SL_RAIL_SCHEDULER_STATUS_MASK) {
      if (status & SL_RAIL_SCHEDULER_TASK_SCHEDULED_RX) {
        if ((status & SL_RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED)
            || (status & SL_RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL)) {
          // generic errors due to other protocols, we continue operation,
          // except if we already reported state
          if ((state == S_RX_WAITING_DATA)
              || (state == S_RX_WAITING_DATA_SETUP)) {
            sl_rail_sleepdc_cb_wakeup_event_irq(
              SL_RAIL_SLEEPDC_EVENT_DATA_DMP_ERROR);
            state = S_RX_DUTY_CYCLE;
          }
          if (state == S_RX_DUTY_CYCLE) {
            start_next_rx();
          }
        } else if (status & SL_RAIL_SCHEDULER_STATUS_TASK_FAIL) {
          // rail API call returned an error - most likely wrong channel
          abort_rx(SL_RAIL_SLEEPDC_EVENT_DMP_TASK_FAIL);
        } else {
          // serious issue: The scheduler is not running or reported unexpected
          // error
          abort_rx(SL_RAIL_SLEEPDC_EVENT_DMP_ASSERT);
        }
      } else {
        // error from an API we never called
        abort_rx(SL_RAIL_SLEEPDC_EVENT_DMP_ASSERT);
      }
    }
  }
}

int32_t sl_rail_sleepdc_get_anchor_delay()
{
  sl_rail_time_t now = sl_rail_get_time(rail_handle);
  return (int32_t)(anchor_time - now);
}

SL_WEAK void sl_rail_sleepdc_cb_wakeup_event_irq(
  sl_rail_sleepdc_event_t reason)
{
  (void)reason;
}

SL_WEAK void sl_rail_sleepdc_cb_wakeup_received()
{
}

SL_WEAK void sl_rail_sleepdc_cb_wakeup_aborted(sl_rail_sleepdc_event_t reason)
{
  (void)reason;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static void abort_rx(sl_rail_sleepdc_event_t reason)
{
  if (sl_rail_idle(rail_handle, SL_RAIL_IDLE_ABORT,
                   true) != SL_RAIL_STATUS_NO_ERROR) {
    NVIC_SystemReset();
  }
  state = S_IDLE;
  sl_rail_sleepdc_cb_wakeup_event_irq(reason);
  status_event = reason;
  generate_callback = true;
}

static void start_next_rx()
{
  sl_rail_scheduled_rx_config_t sched_config = {
    .start = SL_ADVANCED_BURST_DC_RECEIVER_BURST_CYCLE_TIME_US
             - SL_ADVANCED_BURST_DC_RECEIVER_BURST_ON_TIME_US,
    .start_mode = SL_RAIL_TIME_DELAY,
    .end = SL_ADVANCED_BURST_DC_RECEIVER_BURST_ON_TIME_US,
    .end_mode = SL_RAIL_TIME_DELAY,
    .hard_window_end = false,
  };
  sl_rail_scheduler_info_t sched_info = {
    .priority = SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_PRIORITY,
    .slip_time = SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_SLIPTIME_US,
    .transaction_time = SL_ADVANCED_BURST_DC_RECEIVER_BURST_ON_TIME_US,
  };
  state = S_RX_DUTY_CYCLE;
  sl_rail_status_t status = sl_rail_start_scheduled_rx(rail_handle,
                                                       wakeup_channel,
                                                       &sched_config,
                                                       &sched_info);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    abort_rx(SL_RAIL_SLEEPDC_EVENT_SCHEDULE_RX_FAIL);
  }
}

static void schedule_timed_rx()
{
  sl_rail_scheduled_rx_config_t sched_config = {
    .start = anchor_time
             - SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_FOR_DATA_MSG_US,
    .start_mode = SL_RAIL_TIME_ABSOLUTE,
    // we're timing out if sync is not received in 1ms, but the message can be
    // longer
    .end_mode = SL_RAIL_TIME_DELAY,
    .end = was_last_wakeup_accurate ? 1000 : 2000,
    .hard_window_end = false,
  };
  // To calculate byte time
  uint16_t byte_time = 8000000 / sl_rail_get_bit_rate(rail_handle);
  sl_rail_scheduler_info_t sched_info = {
    .priority = SL_ADVANCED_BURST_DC_RECEIVER_RX_PRIORITY,
    .slip_time = 0,             // no sliptime, we're tightly scheduled
    .transaction_time = byte_time
                        * (SL_ADVANCED_BURST_DC_RECEIVER_MAX_PACKET_LEN
                           + sizeof(((sl_rail_sleepdc_burst_frame_t){}).crc)),
    // longest message possible
  };
  state = S_RX_WAITING_DATA;
  sl_rail_status_t status = sl_rail_start_scheduled_rx(rail_handle,
                                                       wakeup_channel,
                                                       &sched_config,
                                                       &sched_info);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    abort_rx(SL_RAIL_SLEEPDC_EVENT_SCHEDULE_RX_FAIL);
  }
}

static sl_rail_sleepdc_event_t process_burst_packet(
  sl_rail_rx_packet_handle_t packet_handle,
  sl_rail_rx_packet_info_t *p_packet_info)
{
  sl_rail_sleepdc_burst_frame_t frame;
  if (sl_rail_copy_rx_packet(rail_handle, (void *)(&frame),
                             p_packet_info) != SL_RAIL_STATUS_NO_ERROR) {
    abort_rx(SL_RAIL_SLEEPDC_EVENT_BURST_PROCESS_FAIL);
  }
  sl_rail_sleepdc_event_t callback_reason = SL_RAIL_SLEEPDC_EVENT_WAKEUP;
  status_event = SL_RAIL_SLEEPDC_EVENT_WAKEUP;
  generate_callback = true;

  // with broadcast frame, we need to set up for the addressing frame. There's
  // no filtering at this stage
  state = S_RX_WAITING_DATA_SETUP; // we reschedule at duty cycle end

  // if we're going forward with the wakeup, we need to save the timestamp of
  // the received frame
  sl_rail_rx_packet_details_t packet_details = {
    .is_ack = false,
    .time_received.time_position = SL_RAIL_PACKET_TIME_AT_PACKET_END,
    // at least up until EFR32xG28, always the hw timestamp
    .time_received.total_packet_bytes = p_packet_info->packet_bytes
                                        + sizeof(((sl_rail_sleepdc_burst_frame_t)
                                                  {}).crc),
  };
  if (sl_rail_get_rx_packet_details(rail_handle, packet_handle,
                                    &packet_details)
      != SL_RAIL_STATUS_NO_ERROR) {
    abort_rx(SL_RAIL_SLEEPDC_EVENT_BURST_PROCESS_FAIL);
  }
  was_last_wakeup_accurate = false;
  // and calculate anchor_time with the timestamp and the delay saved in the
  // frame
  if ((packet_details.time_received.time_position
       == SL_RAIL_PACKET_TIME_AT_PACKET_END)
      || (packet_details.time_received.time_position
          == SL_RAIL_PACKET_TIME_AT_PACKET_END_USED_TOTAL)) {
    // successful timestamp conversion, switch to more accurate timing
    anchor_time = packet_details.time_received.packet_time + frame.time;
    was_last_wakeup_accurate = true;
  } else {
    // we have no timestamp. Use "now" as timestamp instead
    anchor_time = sl_rail_get_time(rail_handle) + frame.time
                  -
                  SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_ON_NO_TIMESTAMP_US;
  }
  return callback_reason;
}

static void process_packet()
{
  sl_rail_rx_packet_info_t packet_info;
  sl_rail_rx_packet_handle_t packet_handle = sl_rail_get_rx_packet_info(
    rail_handle,
    SL_RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE,
    &packet_info);
  sl_rail_sleepdc_event_t callback_reason =
    SL_RAIL_SLEEPDC_EVENT_BURST_PROCESS_FAIL;

  // we try to decide frame processing based on length. We know exactly burst
  // frame length and we know a minimum length of data frames
  if (packet_info.packet_bytes
      > SL_ADVANCED_BURST_DC_RECEIVER_MAX_PACKET_LEN) {
    callback_reason = SL_RAIL_SLEEPDC_EVENT_PACKET_TOO_LONG;
  } else if (packet_info.packet_bytes
             == sizeof(sl_rail_sleepdc_burst_frame_t)
             - sizeof(((sl_rail_sleepdc_burst_frame_t){}).crc)) {
    // burst frame, but without CRC
    callback_reason = process_burst_packet(packet_handle, &packet_info);
  } else if (packet_info.packet_bytes
             >= SL_ADVANCED_BURST_DC_RECEIVER_MIN_DATA_LEN) {
    sl_rail_hold_rx_packet(rail_handle);
    callback_reason = SL_RAIL_SLEEPDC_EVENT_DATA_RECEIVED;
  } else {
    // too short to be a valid packet
    callback_reason = SL_RAIL_SLEEPDC_EVENT_FRAME_ERROR;
  }
  sl_rail_sleepdc_cb_wakeup_event_irq(callback_reason);
}
