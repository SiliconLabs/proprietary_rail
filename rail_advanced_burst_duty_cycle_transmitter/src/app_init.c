/***************************************************************************//**
 * @file
 * @brief app_init.c
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
#include "sl_rail_advanced_burst_dc_transmitter_config.h"
#include "sl_rail_packet_streamer.h"
#include "sl_common.h"

#include "app_log.h"
#include "app_assert.h"

#include "em_gpcrc.h"
#include "em_cmu.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define S_TO_US                       1000000

#define BYTE_TO_BIT                   8
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief This function is used to configure GPCRC module
 *****************************************************************************/
static void gpcrc_init();

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
extern volatile packet_streamer_info_t packet_info_streamer_ldc;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
SL_RAIL_DECLARE_FIFO_BUFFER(tx_fifo,
                            SL_ADVANCED_BURST_DC_TRANSMITTER_BUFFER_LENGTH);

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * @brief The function is used for some basic initialization relates to the app.
 *****************************************************************************/
void app_init(void)
{
  sl_rail_status_t status;

  // Get RAIL handle, used later by the application
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  status = sl_rail_set_tx_fifo(rail_handle,
                               tx_fifo,
                               SL_ADVANCED_BURST_DC_TRANSMITTER_BUFFER_LENGTH,
                               0,
                               0);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_set_tx_fifo return status failed.");

  packet_streamer_init(rail_handle);

  gpcrc_init();

  // Get syncword details
  sl_rail_sync_word_config_t sync_word_config;
  status = sl_rail_get_sync_words(rail_handle, &sync_word_config);
  app_assert(status == SL_RAIL_STATUS_NO_ERROR,
             "sl_rail_get_sync_words return status failed.");

  // To calculate byte time
  uint32_t bit_rate = sl_rail_get_bit_rate(rail_handle);
  packet_info_streamer_ldc.priority =
    SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_PRIORITY;
  packet_info_streamer_ldc.slip_time =
    SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_SLIPTIME_US;
  packet_info_streamer_ldc.syncword = sync_word_config.sync_word_0;
  packet_info_streamer_ldc.syncword_len = sync_word_config.sync_word_bits;
  packet_info_streamer_ldc.byte_time = S_TO_US * BYTE_TO_BIT / bit_rate;
  packet_info_streamer_ldc.preamble_pattern =
    SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATTERN;
  packet_info_streamer_ldc.preamble_pattern_len =
    SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATT_LEN;
  packet_info_streamer_ldc.channel =
    SL_ADVANCED_BURST_DC_TRANSMITTER_DEFAULT_CHANNEL;
  packet_info_streamer_ldc.preamble_length =
    SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_LEN;
  packet_info_streamer_ldc.time_anchor_after_burst =
    SL_ADVANCED_BURST_DC_TRANSMITTER_ANCHOR_DELAY_US;

  // Welcome message
  app_log(
    "Advanced Burst Duty Cycle Transmitter example\n"
    "Press PB0 (right) to SEND packets\n");
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void gpcrc_init()
{
  CMU_ClockEnable(cmuClock_GPCRC, true);
  GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;
  init.crcPoly = SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_POLYNOMIAL;
  init.autoInit = true;
  init.initValue = SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_SEED;
  GPCRC_Init(GPCRC, &init);
  GPCRC_Start(GPCRC);
}
