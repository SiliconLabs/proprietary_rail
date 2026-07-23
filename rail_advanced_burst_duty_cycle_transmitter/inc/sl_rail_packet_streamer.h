/***************************************************************************//**
 * @file
 * @brief sl_rail_packet_streamer.h
 * @details Interface to stream packets without gaps in between
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

#ifndef SL_RAIL_PACKET_STREAMER_H_
#define SL_RAIL_PACKET_STREAMER_H_

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "sl_rail_types.h"
#include "sl_status.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/******************************************************************************
 * @enum packet_streamer_tx_status_t
 * @brief Status codes for the packet streamer
 *****************************************************************************/
typedef enum {
  TX_SUCCESS = 0,                         // Packet was sent successfully
  TX_SUCCESS_INACCURATE_TIMING_LATE = 1,  // Packet was sent successfully,
                                          // but the timing of the anchor is
                                          // inaccurate and could only be set
                                          // for too late
  TX_SUCCESS_INACCURATE_TIMING_EARLY = 2, // Packet was sent successfully,
                                          // but the timing of the anchor is
                                          // inaccurate and could only be set
                                          // for too early
  TX_ERROR_DMP = 252,                     // DMP related errors most likely
                                          // caused that nothing was sent
  TX_ERROR_ABORTED = 253,                 // Packet was aborted (including
                                          // DMP priority task taking the radio)
  TX_ERROR_UNDERFLOW = 254,               // Packet was aborted due to
                                          // buffer underflow
  TX_ERROR_UNKNOWN = 255,                 // Packet was aborted due to an
                                          // unknown error
} packet_streamer_tx_status_t;

/******************************************************************************
 * @struct packet_streamer_info_t
 * @brief Initialization struct for the packet streamer
 *****************************************************************************/
typedef struct {
  uint32_t preamble_pattern;                  // Preamble pattern
  uint8_t preamble_pattern_len;               // Preamble pattern length
  uint32_t syncword;                          // Syncword in LSB first format
  uint8_t syncword_len;                       // Syncword length in bits
  uint16_t frame_len;                         // Frame length (as in every
                                              // byte after sync word)
  uint16_t channel;                           // Channel to transmit on
  uint32_t repeat;                            // Number of times to repeat the
                                              // packet. 0 means packet will be
                                              // send once
  uint32_t preamble_length;                   // Preamble length in bits
  bool time_anchor_enabled;                   // Enable or disable time anchor
                                              // function (timed event after the
                                              // burst)
  sl_rail_time_t time_anchor_after_burst;     // Sets the delay of the time
                                              // anchor after the burst
  sl_rail_time_t byte_time;                   // Time it takes to send one
                                              // byte in microseconds
  uint8_t priority;                           // RAIL DMP priority to use
  sl_rail_time_t slip_time;                   // RAIL DMP sliptime to use
} packet_streamer_info_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief Initialize the packet streamer
 *****************************************************************************/
void packet_streamer_init(sl_rail_handle_t rail_handle);

/******************************************************************************
 * @brief Configure the packet streamer
 * @param[in] packet_configuration Pointer to a \ref packet_streamer_info_t
 * struct
 *****************************************************************************/
void packet_streamer_configure_packet(
  volatile packet_streamer_info_t *packet_configuration);

/******************************************************************************
 * @brief Transmit a packet
 * @param[in] rail_handle RAIL handle
 * @return \ref sl_status_t
 *****************************************************************************/
sl_status_t packet_streamer_tx(sl_rail_handle_t rail_handle);

/******************************************************************************
 * @brief Transmit a packet
 * @param[in] rail_handle RAIL handle
 * @param[in] at Time to transmit
 * @return \ref sl_status_t
 * @note The API will call ScheduleTx first, and request payload/load fifo after
 * @note This API was tested working when called from tx_done callback
 *****************************************************************************/
sl_status_t packet_streamer_tx_at(sl_rail_handle_t rail_handle,
                                  sl_rail_time_t at);

/******************************************************************************
 * @brief Event handle of the packet streamer. Should be called from the RAIL
 * event handler. Call is ignored if the packet streamer is idle.
 * @param[in] rail_handle RAIL handle
 * @param[in] events events triggered by RAIL
 *****************************************************************************/
void packet_streamer_on_rail_event(sl_rail_handle_t rail_handle,
                                   sl_rail_events_t events);

/******************************************************************************
 * @brief Callback function that is called when a packet is sent
 * @param[in] status_code Status code of the packet transmission
 * @param[in] burst_counter Number of bursts left to send
 * @param[in] anchor_time Time of the anchor (if it was active)
 * @return Return true if the radio hw can be yielded or false if there's more
 * to do (e.g., scheduled message after the burst). Only valid on success, Radio
 * is automatically yielded on failure
 * @note If this returns success, the upper layer can call tx_at from the
 * callback directly
 *****************************************************************************/
bool packet_streamer_cb_tx_done(packet_streamer_tx_status_t status_code,
                                int32_t burst_counter,
                                sl_rail_time_t anchor_time);

/******************************************************************************
 * @brief Callback function that is called when a payload is sent. Can be used
 * to free the payload buffer and prepare the next payload while the streamer is
 * transmitting preamble/syncword. The callback won't be called if no more
 * payload is needed from the upper layer (i.e., after the last payload)
 * @param[in] payload Pointer to the payload that was sent
 *****************************************************************************/
void packet_streamer_cb_payload_done(uint8_t *payload);

/******************************************************************************
 * @brief Callback function that is called when a payload is requested.
 * @param[in] payload_len Length of the payload that is requested
 * @param[in] time_until_anchor Time until the anchor is triggered
 * @param[out] payload Pointer to the payload buffer. Buffer must be valid until
 * streamer calls \ref packet_streamer_cb_payload_done
 *****************************************************************************/
void packet_streamer_cb_request_payload(uint16_t payload_len,
                                        uint32_t time_until_anchor,
                                        uint8_t * *payload);

#endif // SL_RAIL_PACKET_STREAMER_H_
