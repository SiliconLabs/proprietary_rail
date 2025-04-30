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
#include "sl_component_catalog.h"
#include "app_init.h"
#include "rail.h"
#include "sl_rail_util_init.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_button_instances.h"
#include "sl_cli_instances.h"
#include "app_log.h"
#include "app_assert.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * This function handles the transmission sequence
 *****************************************************************************/
static void schedule_next_tx(RAIL_Handle_t rail_handle);

/******************************************************************************
 * This function updates the length config and writes the packet to FIFO
 *****************************************************************************/
static RAIL_Status_t prepare_packet(RAIL_Handle_t rail_handle,
                                    uint8_t *payload,
                                    uint16_t packet_length);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// Indicates the current packet of the sequence to be transmitted
volatile uint8_t packet_counter;

// Indicates the length of the sequence
volatile uint8_t sequence_length = SL_RF_GENERATOR_SEQUENCE_LENGTH;
// Determines how many times the sequence is sent. 0 means infinite.
volatile uint32_t repeats = 0;
// Determines the time position of the packet that is scheduled for transmission
volatile uint32_t tx_time;
// Counts the number of times the sequence has been transmitted
volatile uint32_t sequence_counter;

// Flag indicating if BTN0 was pressed
volatile bool tx_toggle_flag = false;
// Enables packet info logging on transmission
volatile bool tx_log_enable = true;

// State of the application state machine
state_t state = S_IDLE;

extern packet_sequence_element_t sequence[SL_RF_GENERATOR_SEQUENCE_LENGTH];
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// Flag indicating if an error occured during calibration
static volatile bool calibration_error_flag = false;

// Flag indicating if an error occured during transmission
static volatile bool tx_error_flag = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Button callback, called if a Button event occurs
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (handle == &sl_button_btn0) {
      tx_toggle_flag = true;
    }
  }
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  if (calibration_error_flag) {
    calibration_error_flag = false;
    app_log_error("RAIL_Calibrate was unable to perform calibration!");
  }
  if (tx_error_flag) {
    tx_error_flag = false;
    app_log_error(
      "Packet transmission was not successful! "
      "The delay between packets could be too short!");
    state = S_IDLE;
  }
  switch (state) {
    case S_IDLE:
      if (tx_toggle_flag) {
        tx_toggle_flag = false;
        sequence_counter = 0;
        packet_counter = 0;
        state = S_SCHEDULE_NEXT_TX;
      }
      break;
    case S_SCHEDULE_NEXT_TX:
      schedule_next_tx(rail_handle);
      break;
    case S_TX_STARTED:
      if (tx_log_enable) {
        app_log_info("Packet %u Tx has been started\n",
                     sequence[packet_counter].packet->id);
      }
      break;
    case S_WAITING_FOR_TX:
      if (tx_toggle_flag) {
        tx_toggle_flag = false;
        state = S_TX_ABORT_PENDING;
      }
      break;
    case S_TX_ABORT_PENDING:
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
  if (events & RAIL_EVENT_CAL_NEEDED) {
    // Calibrate if necessary
    if (RAIL_Calibrate(rail_handle,
                       NULL,
                       RAIL_CAL_ALL_PENDING) != RAIL_STATUS_NO_ERROR) {
      calibration_error_flag = true;
    }
  }
  if (events & RAIL_EVENT_SCHEDULED_TX_STARTED) {
    state = S_TX_STARTED;
  }
  if (events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      sl_led_toggle(&sl_led_led1);
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
      state = S_SCHEDULE_NEXT_TX;
    } else {
      tx_error_flag = true;
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * This function handles the transmission sequence
 *****************************************************************************/
static void schedule_next_tx(RAIL_Handle_t rail_handle)
{
  if ((sequence_counter < repeats) || !repeats) {
    if (prepare_packet(rail_handle, sequence[packet_counter].packet->payload,
                       sequence[packet_counter].packet->length)
        != SL_STATUS_OK) {
      state = S_IDLE;
      return;
    }
    RAIL_Status_t rail_status;
    if ((sequence_counter == 0) && (packet_counter == 0)) {
      rail_status = RAIL_StartTx(rail_handle,
                                 sequence[packet_counter].packet->channel,
                                 RAIL_TX_OPTIONS_DEFAULT,
                                 NULL);
      state = S_TX_STARTED;
    } else {
      RAIL_ScheduleTxConfig_t scheduled_tx_config = {
        .when = tx_time,
        .mode = RAIL_TIME_ABSOLUTE,
      };
      rail_status =
        RAIL_StartScheduledTx(rail_handle,
                              sequence[packet_counter].packet->channel,
                              RAIL_TX_OPTIONS_DEFAULT,
                              &scheduled_tx_config,
                              NULL);
    }
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("Tx can not be started! Result: %lu\n", rail_status);
      state = S_IDLE;
    } else if (state != S_TX_STARTED) {
      state = S_WAITING_FOR_TX;
    }
  } else {
    state = S_IDLE;
  }
}

/******************************************************************************
 * This function updates the length config and writes the packet to FIFO
 *****************************************************************************/
static RAIL_Status_t prepare_packet(RAIL_Handle_t rail_handle,
                                    uint8_t *payload,
                                    uint16_t packet_length)
{
  if (RAIL_SetFixedLength(rail_handle, packet_length)
      == RAIL_SETFIXEDLENGTH_INVALID) {
    app_log_error("RAIL_SetFixedLength() result: %u\n",
                  RAIL_SETFIXEDLENGTH_INVALID);
    return SL_STATUS_FAIL;
  } else {
    if (RAIL_WriteTxFifo(rail_handle,
                         payload,
                         packet_length,
                         false) != packet_length) {
      app_log_error("RAIL_WriteTxFifo was unable to write the required size.");
      return SL_STATUS_FAIL;
    }
  }
  return SL_STATUS_OK;
}
