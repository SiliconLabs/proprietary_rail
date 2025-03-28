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
#include "sl_sleeptimer.h"
#include "ota_dfu_target.h"
#include "led_control.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void restart(sl_sleeptimer_timer_handle_t *handle, void *data);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static bool packet_received, receive_error;
static uint8_t rx_buffer[PAYLOAD_LENGTH];
static uint8_t response[PAYLOAD_LENGTH];
static sl_sleeptimer_timer_handle_t restart_timer_handle;
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

  if (packet_received) {
    packet_received = false;

    while ((packet_handle =
              RAIL_GetRxPacketInfo(rail_handle,
                                   RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE,
                                   &packet_info))
           != RAIL_RX_PACKET_HANDLE_INVALID) {
      enum ota_dfu_opcode opcode;

      RAIL_CopyRxPacket(rx_buffer, &packet_info);
      RAIL_ReleaseRxPacket(rail_handle, packet_handle);

      opcode = ota_dfu_process_command(rx_buffer, response);

      if (opcode == ota_dfu_go_ahead) {
        blinky_stop();
        blinky_start(0, SLOW_PERIOD, SLOW_DUTY_CYCLE);
      }

      RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, false);
      RAIL_WriteTxFifo(rail_handle, response, PAYLOAD_LENGTH, false);
      RAIL_StartTx(rail_handle, 0, RAIL_TX_OPTIONS_DEFAULT, NULL);

      if (opcode == ota_dfu_finished) {
        blinky_stop();
        led_on(0);
        app_log("Restarting in 3 seconds\r\n");
        sl_sleeptimer_start_timer(&restart_timer_handle,
                                  sl_sleeptimer_ms_to_tick(3000),
                                  restart,
                                  0,
                                  0,
                                  0);
      } else if ((opcode == ota_dfu_no_space)
                 || (opcode == ota_dfu_bootloader_error)
                 || (opcode == ota_dfu_unknown)) {
        blinky_stop();
        blinky_start(1, SLOW_PERIOD, SLOW_DUTY_CYCLE);
      }
    }
  }
  if (receive_error) {
    receive_error = false;
    RAIL_StartRx(rail_handle, 0, NULL);
    app_log("RX error\r\n");
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
    } else {
      receive_error = true;
    }
  }

  if (events & RAIL_EVENTS_TX_COMPLETION) {
    RAIL_StartRx(rail_handle, 0, NULL);
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void restart(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle;
  (void)data;
  led_off(1);
  ota_dfu_restart();
}
