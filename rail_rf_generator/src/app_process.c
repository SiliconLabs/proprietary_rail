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
#include "app_init.h"
#include "rail.h"
#include "sl_rail_util_init.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_button_instances.h"
#include "sl_cli_instances.h"
#include "app_log.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void tx_sequence(RAIL_Handle_t rail_handle);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
volatile uint8_t packet_counter;
volatile uint8_t sequence_length = SEQUENCE_LENGTH;
volatile uint32_t repeats = 0;
volatile uint32_t tx_time;
volatile uint32_t sequence_counter;

volatile bool tx_log_enable = true;
state_t state = S_IDLE;

extern packetSequenceElement_t sequence[SEQUENCE_LENGTH];
extern state_t state;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void tx_toggle()
{
  if (state == S_IDLE) {
    uint16_t packet_length = sequence[packet_counter].packet->length;
    RAIL_Handle_t rail_handle
      = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
    sequence_counter = 0;
    packet_counter = 0;
    state = S_TX_STARTED;
    if (RAIL_SetFixedLength(rail_handle, packet_length)
        == RAIL_SETFIXEDLENGTH_INVALID) {
      app_log_warning("RAIL_SetFixedLength() result:"
                      "RAIL_SETFIXEDLENGTH_INVALID\n");
      state = S_IDLE;
      return;
    }
    RAIL_WriteTxFifo(rail_handle,
                     sequence[packet_counter].packet->payload,
                     packet_length,
                     false);

    RAIL_Status_t rail_status = RAIL_StartTx(rail_handle,
                                             sequence[packet_counter].packet->channel,
                                             RAIL_TX_OPTIONS_DEFAULT,
                                             NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_StartScheduledTx() result:%d\n", rail_status);
      state = S_IDLE;
    }
  } else {
    state = (state == S_TX || state == S_TX_SEQUENCE || state == S_TX_STARTED)
            ? S_TX_ABORTED : S_IDLE;
  }
}

void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    tx_toggle();
  }
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  switch (state) {
    case S_IDLE:
      break;
    case S_TX_SEQUENCE:
      tx_sequence(rail_handle);
      break;
    case S_TX_STARTED:
      if (tx_log_enable) {
        app_log_info("Packet %u has been sent\n",
                     sequence[packet_counter].packet->id);
      }
      break;
    case S_TX:
      break;
    case S_TX_ABORTED:
      RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, false);
      app_log_info("Tx aborted\n");
      state = S_IDLE;
      break;
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  if (events & RAIL_EVENT_SCHEDULED_TX_STARTED) {
    state = S_TX_STARTED;
  }
  if (events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      sl_led_toggle(&sl_led_led0);
      RAIL_TxPacketDetails_t tx_packet_details = { 0 };
      tx_packet_details.timeSent.timePosition = RAIL_PACKET_TIME_AT_PACKET_END;
      RAIL_GetTxPacketDetails(rail_handle, &tx_packet_details);
      tx_time = tx_packet_details.timeSent.packetTime
                + sequence[packet_counter].delay;
      packet_counter++;
      if (packet_counter == sequence_length) {
        packet_counter = 0;
        sequence_counter++;
      }
      state = S_TX_SEQUENCE;
    }
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static void tx_sequence(RAIL_Handle_t rail_handle)
{
  if ((sequence_counter < repeats) || !repeats) {
    uint16_t packet_length = sequence[packet_counter].packet->length;
    if (RAIL_SetFixedLength(rail_handle, packet_length)
        == RAIL_SETFIXEDLENGTH_INVALID) {
      app_log_warning("RAIL_SetFixedLength() result:"
                      "RAIL_SETFIXEDLENGTH_INVALID\n");
      state = S_IDLE;
      return;
    }
    RAIL_ScheduleTxConfig_t scheduled_tx_config = {
      .when = tx_time,
      .mode = RAIL_TIME_ABSOLUTE,
    };
    RAIL_WriteTxFifo(rail_handle, sequence[packet_counter].packet->payload,
                     packet_length,
                     false);

    RAIL_Status_t rail_status = RAIL_StartScheduledTx(rail_handle,
                                                      sequence[packet_counter].packet->channel,
                                                      RAIL_TX_OPTIONS_DEFAULT,
                                                      &scheduled_tx_config,
                                                      NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_StartScheduledTx() result:%d\n", rail_status);
      state = S_IDLE;
    } else {
      state = S_TX;
    }
  } else {
    state = S_IDLE;
  }
}
