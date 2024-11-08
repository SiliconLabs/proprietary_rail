/***************************************************************************//**
 * @file
 * @brief app_process.c
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
#include <stdio.h>

#include "sl_component_catalog.h"
#include "sl_simple_button_instances.h"

#include "rail.h"

#include "em_timer.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "app_log.h"
#include "app_init.h"
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

#include "sl_rail_direct_mode_detector_config.h"
#include "sl_emlib_gpio_init_algorithm_active_config.h"
#include "sl_emlib_gpio_init_async_detect_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#if SL_DIRECT_MODE_DEBUG_GPIO == 1
#define sl_algorithm_active_gpio_set() \
        GPIO_PinOutSet(SL_EMLIB_GPIO_INIT_ALGORITHM_ACTIVE_PORT, \
                       SL_EMLIB_GPIO_INIT_ALGORITHM_ACTIVE_PIN)
#define sl_algorithm_active_gpio_clear() \
        GPIO_PinOutClear(SL_EMLIB_GPIO_INIT_ALGORITHM_ACTIVE_PORT, \
                         SL_EMLIB_GPIO_INIT_ALGORITHM_ACTIVE_PIN)
#define sl_async_detect_gpio_toggle() \
        GPIO_PinOutToggle(SL_EMLIB_GPIO_INIT_ASYNC_DETECT_PORT, \
                          SL_EMLIB_GPIO_INIT_ASYNC_DETECT_PIN)

#else // SL_DIRECT_MODE_DEBUG_GPIO == 0
#define sl_algorithm_active_gpio_set()   (void)0
#define sl_algorithm_active_gpio_clear() (void)0
#define sl_async_detect_gpio_toggle()    (void)0
#endif // SL_DIRECT_MODE_DEBUG_GPIO

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

static void handle_timer_interrupt(TIMER_TypeDef *timer, bool is_high_pulse);
static void rearm_detector_algorithm();

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
extern uint32_t symbol_duration;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// Flag indicating packet has been detected
static volatile bool packet_detected = false;
#if SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
// Flag indicating packet has been received
static volatile  bool packet_ready = false;
#endif // SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE

// Initializes expected syncword pattern and mask
static uint32_t syncword_pattern = SL_DIRECT_MODE_DETECTOR_SYNCWORD_PATTERN;
static uint32_t syncword_mask = SL_DIRECT_MODE_DETECTOR_SYNCWORD_MASK;

// Variable to store the last 32 bits of the demodulated stream
// This is used to detect the syncword
static volatile uint32_t demod_stream = 0UL;

#if SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
// Payload buffer for received packet
static uint8_t rx_payload[SL_DIRECT_MODE_DETECTOR_PACKET_LENGTH];
#endif // SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE

// Flag indicating synchronization packet to be send
volatile bool transmit_pend = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Pressing a button initiate to transmit a packet
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (handle == &sl_button_btn0) {
    if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
      transmit_pend = true;
    }
  }
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t status;

  // Handles packet transmission requested by the user
  if (transmit_pend) {
    app_log_info("Packet transmission initiated.\n");

    // Disable Timers to stop the demodulation
    TIMER_Enable(TIMER0, false);
    TIMER_Enable(TIMER1, false);

    // Clears the flag
    transmit_pend = false;
    
    // Sends the packet - Resend option keeps the payload in the FIFO
    status = RAIL_StartTx(rail_handle,
                          SL_DIRECT_MODE_DETECTOR_DEFAULT_CHANNEL,
                          RAIL_TX_OPTION_RESEND,
                          NULL);
    if (status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartTx() failed!\n");
    }
  }

#if SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_DETECT_ONLY
  // In detection only mode the application only logs the packet detection
  // and rearms the radio for the next packet
  if (packet_detected) {
    app_log_info("Packet Detected.\n");
    packet_detected = false;
  }
#endif // SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_DETECT_ONLY

#if SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
  // In packet receive mode the application logs the received packet
  // and restart RX after the payload is logged
  if (packet_ready) {
    // Idle the radio manually while printing the packet
    RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, false);
    // Each byte takes 3 chars ("XX ") and one for null terminator
    static char log_buffer[SL_DIRECT_MODE_DETECTOR_PACKET_LENGTH * 3 + 1];

    int offset = 0;
    for (int i = 0; i < SL_DIRECT_MODE_DETECTOR_PACKET_LENGTH; i++) {
      offset += snprintf(log_buffer + offset,
                         sizeof(log_buffer) - offset,
                         "%02X ",
                         rx_payload[i]);
    }

    app_log_info("Packet received:\n%s\n", log_buffer);

    // Rearms the algorithm for the next packet
    rearm_detector_algorithm();

    // Restarts RX
    RAIL_StartRx(rail_handle, SL_DIRECT_MODE_DETECTOR_DEFAULT_CHANNEL, NULL);

    // Resets flags
    packet_ready = false;
    packet_detected = false;
  }
#endif // SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  if (events & RAIL_EVENTS_TX_COMPLETION) {
    app_log_info("Packet transmission completed.\n");
    rearm_detector_algorithm();
  }

  if (events & RAIL_EVENT_CAL_NEEDED) {
    if (RAIL_STATUS_NO_ERROR != RAIL_Calibrate(rail_handle,
                                               NULL,
                                               RAIL_CAL_ALL_PENDING)) {
      app_log_error("Calibration failed!\n");
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

void TIMER0_IRQHandler()
{
  handle_timer_interrupt(TIMER0, true);
}

void TIMER1_IRQHandler()
{
  handle_timer_interrupt(TIMER1, false);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void rearm_detector_algorithm()
{
  // Clear potentially pending interrupts
  TIMER_IntClear(TIMER0, _TIMER_IEN_CC0_MASK);
  TIMER_IntClear(TIMER1, _TIMER_IEN_CC0_MASK);

  // Reset demod buffer
  demod_stream = 0;

  // Enable Timers
  TIMER_Enable(TIMER0, true);
  TIMER_Enable(TIMER1, true);
}

static void handle_timer_interrupt(TIMER_TypeDef *timer, bool is_high_pulse)
{
  sl_algorithm_active_gpio_set();

  // Get the pulse duration in timer ticks
  uint32_t pulse_duration = TIMER_CaptureBufGet(timer, 0);

  // Round pulse duration to the nearest symbol duration multiple
  uint8_t number_of_symbols = (pulse_duration + (symbol_duration >>
                                                 1)) / symbol_duration;

#if SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
  // Variables to store write position in the payload buffer
  static uint8_t payload_byte_pos = 0;
  static uint8_t payload_bit_pos = 0;
#endif // SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE

  // If pulse duration is less than half of the symbol duration, ignore it
  if (number_of_symbols) {
    // Push the symbol to the demodulated stream one by one
    for (int i = 0; i < number_of_symbols; i++) {
      // Shift 0s or 1s to the demodulated stream based on which TIMER triggered
      // the interrupt
      demod_stream = demod_stream << 1 | (is_high_pulse ? 1 : 0);
#if SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
      // Search for the syncword pattern only if a packet was not detected yet
      if (!packet_detected) {
#endif // SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
      // Check for syncword pattern match with mask
      if ((demod_stream & syncword_mask) == syncword_pattern) {
        packet_detected = true;
        sl_async_detect_gpio_toggle();
#if SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
        // Detection resets the payload buffer write positions
        payload_byte_pos = 0;
        payload_bit_pos = 0;
#endif // SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
      }
#if SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
    } else {
      // Entering this branch means a packet was detected

      // Shift the bit position
      payload_bit_pos++;
      // Shift byte position if byte is complete
      if (payload_bit_pos == 8) {
        payload_bit_pos = 0;
        // Reverse the endianness of the demodulated stream to match
        // the endianness configuration
        rx_payload[payload_byte_pos] = (__RBIT(demod_stream) >> 24) & 0xFF;
        payload_byte_pos++;

        if (payload_byte_pos < SL_DIRECT_MODE_DETECTOR_PACKET_LENGTH) {
          payload_bit_pos = 0;
        } else {
          // Packet is complete
          packet_ready = true;
          // Disable Timers to stop the demodulation
          TIMER_Enable(TIMER0, false);
          TIMER_Enable(TIMER1, false);
          // Stop processing
          break;
        }
      }
    }
#endif // SL_DIRECT_MODE_DETECTOR_APP_MODE == SL_PACKET_RECEIVE
    }
  }
  // Clear the interrupt flag
  TIMER_IntClear(timer, _TIMER_IEN_CC0_MASK);
  sl_algorithm_active_gpio_clear();
}
