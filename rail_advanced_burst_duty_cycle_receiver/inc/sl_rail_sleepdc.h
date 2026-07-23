/***************************************************************************//**
 * @file
 * @brief sl_rail_sleepdc.h
 * @details Main sleep duty cycling implementation header.
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

#ifndef SL_RAIL_SLEEPDC_H_
#define SL_RAIL_SLEEPDC_H_

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "sl_status.h"
#include "sl_common.h"
#include "sl_rail_types.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/******************************************************************************
 * @typedef sl_rail_sleepdc_burst_frame_t
 * @brief Burst frame used in burst mode
 *****************************************************************************/
SL_PACK_START(1)
typedef struct {
  uint8_t length;         // Length of the frame, excluding the length
                          // byte and CRC
  uint32_t time;          // Timestamp. Time between the end of this frame
                          // and the anchor point
  uint16_t crc;           // CRC
} SL_ATTRIBUTE_PACKED sl_rail_sleepdc_burst_frame_t;
SL_PACK_END()

/******************************************************************************
 * @enum sl_rail_sleepdc_wakeup_mode_t
 * @brief Configurable modes for \ref sl_rail_sleepdc_set_mode()
 *****************************************************************************/
typedef enum {
  WAKEUP_MODE_OFF = 0,  // Off mode
  WAKEUP_BURST,         // Burst mode timing.
} sl_rail_sleepdc_wakeup_mode_t;

/******************************************************************************
 * @enum sl_rail_sleepdc_event_t
 * @brief Various status changes, mostly minor errors (like filtered frames)
 * signaled via \ref sl_rail_sleepdc_cb_wakeup_event_irq()
 *****************************************************************************/
typedef enum {
  // Wakeup event, interrupt-safe callback will be generated
  SL_RAIL_SLEEPDC_EVENT_WAKEUP,
  // Data message received. If this event is triggered, the received packet is
  // held by RAIL and must be released in the event handler!
  SL_RAIL_SLEEPDC_EVENT_DATA_RECEIVED,
  // Message received but with frame error (CRC or length decoding issue, or
  // incorrect detection)
  SL_RAIL_SLEEPDC_EVENT_FRAME_ERROR,
  // Message was received, but an unknown error happened
  SL_RAIL_SLEEPDC_EVENT_RX_ERROR,
  // Packet is too long for any processing (longer than
  // SL_ADVANCED_BURST_DC_RECEIVER_MAX_PACKET_LEN)
  SL_RAIL_SLEEPDC_EVENT_PACKET_TOO_LONG,
  // Packet length matched burst packet length, but could not be processed
  SL_RAIL_SLEEPDC_EVENT_BURST_PROCESS_FAIL,
  // Data message not received at the time configured
  SL_RAIL_SLEEPDC_EVENT_DATA_TIMEOUT,
  // DMP (Dynamic Multiprotocol) error (we didn't get access to the radio HW)
  // while waiting for the data message
  SL_RAIL_SLEEPDC_EVENT_DATA_DMP_ERROR,
  // Calling ScheduleRx to start duty cycle active time or timed rx failed
  SL_RAIL_SLEEPDC_EVENT_SCHEDULE_RX_FAIL,
  // DMP returned error when calling a RAIL task. Typically caused by wrong
  // arguments, e.g. non-existent channel
  SL_RAIL_SLEEPDC_EVENT_DMP_TASK_FAIL,
  // DMP returned an unexpected error.
  SL_RAIL_SLEEPDC_EVENT_DMP_ASSERT,
} sl_rail_sleepdc_event_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief Init function of the sleep duty cycle module. Should be called from
 * the app initialization.
 *****************************************************************************/
void sl_rail_sleepdc_init(void);

/******************************************************************************
 * @brief Event handle of the rail_sleepdc. Should be called from the RAIL
 * event handler.
 * @param[in] rail_handle RAIL handle
 * @param[in] events events triggered by RAIL
 *****************************************************************************/
void sl_rail_sleepdc_util_on_event(sl_rail_handle_t rail_handle,
                                   sl_rail_events_t events);

/******************************************************************************
 * @brief Sets the mode of sleep duty cycle, including turning it off.
 * @param wakeup_mode[in] The requested mode
 * @param channel[in] The channel to operate on
 * @return SL_STATUS_IN_PROGRESS if active mode is requested from an already
 * running mode. SL_STATUS_OK if mode was set up successfully.
 * SL_RAIL_STATUS_INVALID_STATE if the mode change failed.
 *****************************************************************************/
sl_status_t sl_rail_sleepdc_set_mode(sl_rail_sleepdc_wakeup_mode_t wakeup_mode,
                                     uint16_t channel);

/******************************************************************************
 * @brief  After burst packet a time anchor is set in the future
 *         when the wakeup sender completes the operation and able to receive
 *         messages again. The delay to this anchor can be requested with this
 *         function.
 * @return The delay to the anchor in microseconds. If negative, the anchor was
 *         in the past
 * @note   For accurate conversion between microseconds and the timestamp used
 *         by the main protocol, this function should be called from an atomic
 *         or critical block.
 *****************************************************************************/
int32_t sl_rail_sleepdc_get_anchor_delay();

/******************************************************************************
 * @brief  Process function. Should be called periodically. Callbacks
 *         \ref sl_rail_sleepdc_cb_wakeup_received() and
 *         \ref sl_rail_sleepdc_cb_wakeup_aborted() are generated from this
 *         function.
 *****************************************************************************/
void sl_rail_sleepdc_process();

/******************************************************************************
 * @brief  Callback called on any new status that might worth reporting.
 *         Can be ignored, as major events are always reported via \ref
 *         sl_rail_sleepdc_cb_wakeup_received or \ref
 *         sl_rail_sleepdc_cb_wakeup_aborted
 * @param  reason[in] The reason of the callback
 * @note   This callback is almost always called from interrupt context, so make
 *         sure to return from it quickly, and handle everything in
 *         interrupt-safe manner.
 *****************************************************************************/
void sl_rail_sleepdc_cb_wakeup_event_irq(sl_rail_sleepdc_event_t reason);

/******************************************************************************
 * @brief  Callback called after wakeup was received. Note
 *         that duty cycling is kept enabled.
 *****************************************************************************/
void sl_rail_sleepdc_cb_wakeup_received();

/******************************************************************************
 * @brief  Callback called if the duty cycling system cannot be continued.
 * @param  reason[in] The reason of the error.
 *****************************************************************************/
void sl_rail_sleepdc_cb_wakeup_aborted(sl_rail_sleepdc_event_t reason);

#endif /* SL_RAIL_SLEEPDC_H_ */
