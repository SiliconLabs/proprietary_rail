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
#include <stdint.h>
#include <stdio.h>

#include "rail.h"

#include "sl_component_catalog.h"
#include "sl_rail_direct_to_buffer_config.h"
#include "sl_simple_button_instances.h"

#include "app_init.h"
#include "app_log.h"

#if (defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_8))
#define SL_DIRECT_TO_BUFFER_IQ_SWAP 1
#else
#define SL_DIRECT_TO_BUFFER_IQ_SWAP 0
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define SL_BUFFER_LENGTH (SL_DIRECT_TO_BUFFER_RX_FIFO_SIZE / 2)

// Auxiliary structure for typecasting the Rx buffer
typedef struct iq_data {
  int16_t I;
  int16_t Q;
} iq_data_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * The function prints the rx buffer. The format depends on the data source.
 *
 * @returns None
 *****************************************************************************/
static void printf_rx_buffer();

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Flag, indicating print request
static volatile bool print_requested = false;

// Flag, indicating RX FIFO overflow
static volatile bool fifo_error = false;

// Allocates a ping-pong style application buffer
uint8_t rx_buffers[2][SL_BUFFER_LENGTH];

// Marks the active half of rx_buffers the next data will be written to
uint8_t active_buffer = 0;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Contains the last events the last calibration attempt failed with
static volatile uint64_t cal_error_events = 0;

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

  // After FIFO overflow the radio stays in Idle state regardless of the
  // auto state transition config
  if (fifo_error) {
    app_log_warning("RX FIFO overflow\n");
    fifo_error = false;
  }

  // While printing the radio is unable to collect data
  if (print_requested) {
    // Stop the radio to avoid data corruption
    rail_status = RAIL_Idle(rail_handle, RAIL_IDLE, true);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_Idle failed with status %d\n", rail_status);
    }

    // Prepare the RX FIFO for the next iteration
    rail_status = RAIL_ResetFifo(rail_handle, false, true);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_ResetFifo failed with status %d\n", rail_status);
    }

    printf_rx_buffer();

    // Restart the radio
    rail_status =
      RAIL_StartRx(rail_handle, SL_DIRECT_TO_BUFFER_DEFAULT_CHANNEL, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartRx failed with status %d\n", rail_status);
    }

    active_buffer = 0;
    // Clear the flag at the end of this sequence to avoid re-triggering during
    //  printing is ongoing
    print_requested = false;
  }

  if (cal_error_events) {
    app_log_warning("RAIL event mask for calibration failed: %lld\n",
                    cal_error_events);
    cal_error_events = 0;
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
      cal_error_events = (events & RAIL_EVENT_CAL_NEEDED);
    }
  }

  // In FIFO mode these events are coincident with each other
  if ((events & (RAIL_EVENT_RX_FIFO_OVERFLOW | RAIL_EVENT_RX_FIFO_FULL))) {
    fifo_error = true;
  }

  // Half of the RX FIFO is reay to be read
  // For simplicity RX FIFO size is equal to the rx_buffers size, and rx_buffers
  // boundary is aligned with the RX FIFO boundary
  if (events & RAIL_EVENT_RX_FIFO_ALMOST_FULL) {
    uint16_t bytes_read;
    // Read the RX FIFO into the active buffer
    bytes_read = RAIL_ReadRxFifo(rail_handle,
                                 rx_buffers[active_buffer],
                                 SL_BUFFER_LENGTH);
    if (bytes_read != SL_BUFFER_LENGTH) {
      // handle by idling and printing what's available
      print_requested = true;
    }
    // Switch the active buffer for the next iteration
    active_buffer ^= 1;
  }
}

/******************************************************************************
 * Button callback, called if any button is pressed or released.
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    print_requested = true;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void printf_rx_buffer()
{
  // Get data source from configuration
  RAIL_RxDataSource_t data_source = SL_DIRECT_TO_BUFFER_RX_SOURCE;
  switch (data_source) {
    case (RX_IQDATA_FILTLSB):
    case (RX_IQDATA_FILTMSB):
      app_log_info("IQ Data:\n");
      iq_data_t *iq_data = (iq_data_t *)rx_buffers[active_buffer];
#if SL_DIRECT_TO_BUFFER_IQ_SWAP
      if (data_source == RX_IQDATA_FILTLSB) {
        for (uint16_t i = 0; i < SL_BUFFER_LENGTH / 4; ++i) {
          app_log_append_info("%6d %6d\n", iq_data[i].I, iq_data[i].Q);
        }
        iq_data = (iq_data_t *)rx_buffers[active_buffer ^ 1];
        for (uint16_t i = 0; i < SL_BUFFER_LENGTH / 4; ++i) {
          app_log_append_info("%6d %6d\n", iq_data[i].I, iq_data[i].Q);
        }
      } else {
        for (uint16_t i = 0; i < SL_BUFFER_LENGTH / 4; ++i) {
          app_log_append_info("%6d %6d\n", iq_data[i].Q, iq_data[i].I);
        }
        iq_data = (iq_data_t *)rx_buffers[active_buffer ^ 1];
        for (uint16_t i = 0; i < SL_BUFFER_LENGTH / 4; ++i) {
          app_log_append_info("%6d %6d\n", iq_data[i].Q, iq_data[i].I);
        }
      }
#else
      for (uint16_t i = 0; i < SL_BUFFER_LENGTH / 4; ++i) {
        app_log_append_info("%6d %6d\n", iq_data[i].I, iq_data[i].Q);
      }
      iq_data = (iq_data_t *)rx_buffers[active_buffer ^ 1];
      for (uint16_t i = 0; i < SL_BUFFER_LENGTH / 4; ++i) {
        app_log_append_info("%6d %6d\n", iq_data[i].I, iq_data[i].Q);
      }
#endif
      break;
    case RX_DEMOD_DATA:
      app_log_info("Demodulated Data:\n");
      int8_t *demod_data = (int8_t *)rx_buffers[active_buffer];
      for (uint16_t i = 0; i < SL_BUFFER_LENGTH; ++i) {
        app_log_append_info("%02d ", demod_data[i]);
        // line break after every 16th sample
        if ((i + 1) % 16 == 0) {
          app_log_append_info("\n");
        }
      }
      demod_data = (int8_t *)rx_buffers[active_buffer ^ 1];
      for (uint16_t i = 0; i < SL_BUFFER_LENGTH; ++i) {
        app_log_append_info("%02d ", demod_data[i]);
        if ((i + 1) % 16 == 0) {
          app_log_append_info("\n");
        }
      }
      break;
    case RX_DIRECT_MODE_DATA:
    case RX_DIRECT_SYNCHRONOUS_MODE_DATA:
      app_log_info("Direct Mode Data:\n");
      for (uint16_t i = 0; i < SL_BUFFER_LENGTH; ++i) {
        app_log_append_info("%02x ", rx_buffers[active_buffer][i]);
        // line break after every 16th sample
        if ((i + 1) % 16 == 0) {
          app_log_append_info("\n");
        }
      }
      for (uint16_t i = 0; i < SL_BUFFER_LENGTH; ++i) {
        app_log_append_info("%02x ", rx_buffers[active_buffer ^ 1][i]);
        if ((i + 1) % 16 == 0) {
          app_log_append_info("\n");
        }
      }
      break;
    default:
      app_log_info("Unknown data source\n");
  }
  app_log_append_info("\n");
}
