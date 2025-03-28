/***************************************************************************//**
 * @file
 * @brief app_process.c
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
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_component_catalog.h"
#include "rail.h"
#include "app_log.h"
#include "sl_simple_button_instances.h"
#include "sl_sleeptimer.h"
#include "ota_dfu_host.h"
#include "led_control.h"
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
enum ota_dfu_state {
  idle,
  start,
  started,
} state;
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void resend_packet(sl_sleeptimer_timer_handle_t *handle, void *data);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static bool packet_received, receive_error;
static uint8_t rx_buffer[PAYLOAD_LENGTH];
static uint8_t command[PAYLOAD_LENGTH];
static sl_sleeptimer_timer_handle_t resend_timer_handle;
static RAIL_Handle_t saved_rail_handle;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  RAIL_RxPacketHandle_t packet_handle;
  RAIL_RxPacketInfo_t packet_info;

  if (state == start) {
    ota_dfu_start_packet(command);
    RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, false);
    RAIL_WriteTxFifo(rail_handle, command, PAYLOAD_LENGTH, false);
    RAIL_StartTx(rail_handle, 0, RAIL_TX_OPTIONS_DEFAULT, NULL);
    state = started;
  }
  if (packet_received) {
    packet_received = false;

    packet_handle = RAIL_GetRxPacketInfo(rail_handle,
                                         RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE,
                                         &packet_info);
    if (packet_handle != RAIL_RX_PACKET_HANDLE_INVALID) {
      uint32_t action;
      RAIL_CopyRxPacket(rx_buffer, &packet_info);
      RAIL_ReleaseRxPacket(rail_handle, packet_handle);
      app_log("<%08lx\r\n", *(uint32_t *)rx_buffer);
      action = ota_dfu_process_response(rx_buffer, command);

      if (action == ota_dfu_data) {
        RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, false);
        RAIL_WriteTxFifo(rail_handle, command, PAYLOAD_LENGTH, false);
        RAIL_StartTx(rail_handle, 0, RAIL_TX_OPTIONS_DEFAULT, NULL);
        app_log(">%08lx\r\n", *(uint32_t *)command);
      } else if (action == ota_dfu_finished) {
        app_log("Done!\r\n");
        blinky_stop();
        led_on(0);
        state = idle;
      } else { // something wrong
        blinky_stop();
        led_off(0);
        blinky_start(1, SLOW_PERIOD, SLOW_DUTY_CYCLE);
      }
    }
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  if (events & RAIL_EVENTS_RX_COMPLETION) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      RAIL_HoldRxPacket(rail_handle);
      packet_received = true;
      sl_sleeptimer_stop_timer(&resend_timer_handle);
    } else {
      receive_error = true;
    }
  }
  if (events & RAIL_EVENTS_TX_COMPLETION) {
    if (state == started) {
      RAIL_StartRx(rail_handle, 0, NULL);
      saved_rail_handle = rail_handle;
      sl_sleeptimer_start_timer(&resend_timer_handle,
                                sl_sleeptimer_ms_to_tick(100),
                                resend_packet,
                                &saved_rail_handle,
                                0,
                                0);
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * Button callback, called if a Button event occurs
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if ((sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)
      && (state == idle)) {
    state = start;
    led_off(0);
    blinky_start(0, SLOW_PERIOD, SLOW_DUTY_CYCLE);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static void resend_packet(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle;
  RAIL_Handle_t rail_handle = *(RAIL_Handle_t *)data;
  RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, false);
  RAIL_WriteTxFifo(rail_handle, command, PAYLOAD_LENGTH, false);
  RAIL_StartTx(rail_handle, 0, RAIL_TX_OPTIONS_DEFAULT, NULL);
  app_log("!%08lx", *(uint32_t *)command);
}
