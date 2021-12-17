/***************************************************************************//**
 * @file rail_config.c
 * @brief RAIL Configuration
 * @details
 *   WARNING: Auto-Generated Radio Config  -  DO NOT EDIT
 *   Radio Configurator Version: x.y.z
 *   RAIL Adapter Version: x.y.z
 *   RAIL Compatibility: x.y
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_device.h"
#include "rail_config.h"

uint32_t RAILCb_CalcSymbolRate(RAIL_Handle_t railHandle)
{
  (void) railHandle;
  return 0U;
}

uint32_t RAILCb_CalcBitRate(RAIL_Handle_t railHandle)
{
  (void) railHandle;
  return 0U;
}

void RAILCb_ConfigFrameTypeLength(RAIL_Handle_t railHandle,
                                  const RAIL_FrameType_t *frameType)
{
  (void) railHandle;
  (void) frameType;
}

static const uint8_t irCalConfig[] = {
  25, 76, 6, 6, 4, 16, 0, 1, 1, 3, 0, 2, 2, 0, 0, 0, 0, 5, 0, 1, 1, 0, 0, 0, 0, 0
};

static RAIL_ChannelConfigEntryAttr_t channelConfigEntryAttr = {
  { 0xFFFFFFFFUL }
};

static const uint32_t phyInfo[] = {
  10UL,
  0x00009D49UL, // 0.6144000000000001
  (uint32_t) NULL,
  (uint32_t) irCalConfig,
#ifdef RADIO_CONFIG_ENABLE_TIMING
  (uint32_t) &PHY_Longrange_490M_OQPSK_DSSS8_9p6kbps_timing,
#else
  (uint32_t) NULL,
#endif
  0x00000000UL,
  8985600UL,
  29952000UL,
  76800UL,
  (32UL << 8) | 4UL,
  0x0A003A85UL,
  (uint32_t) NULL,
  (uint32_t) NULL,
  (uint32_t) NULL,
  0UL,
  0UL,
};

const uint32_t PHY_Longrange_490M_OQPSK_DSSS8_9p6kbps_cohdsa_modemConfigBase[] = {
  0x01041FF0UL, 0x003F003FUL,
  /*    1FF4 */ 0x00000000UL,
  /*    1FF8 */ (uint32_t) &phyInfo,
  /*    1FFC */ 0x00000000UL,
  0x00020004UL, 0x00158001UL,
  /*    0008 */ 0x0000007FUL,
  0x00020018UL, 0x00000000UL,
  /*    001C */ 0x00000000UL,
  0x00070028UL, 0x00000000UL,
  /*    002C */ 0x00000000UL,
  /*    0030 */ 0x00000000UL,
  /*    0034 */ 0x00000000UL,
  /*    0038 */ 0x00000000UL,
  /*    003C */ 0x00000000UL,
  /*    0040 */ 0x000007A0UL,
  0x00010048UL, 0x00000000UL,
  0x00020054UL, 0x00000000UL,
  /*    0058 */ 0x00000000UL,
  0x000400A0UL, 0x00004000UL,
  /*    00A4 */ 0x00004CFFUL,
  /*    00A8 */ 0x00004100UL,
  /*    00AC */ 0x00004DFFUL,
  0x00012000UL, 0x00000704UL,
  0x00012010UL, 0x00000000UL,
  0x00012018UL, 0x00008408UL,
  0x00013008UL, 0x0100AC13UL,
  0x00023030UL, 0x00103A85UL,
  /*    3034 */ 0x00000005UL,
  0x00013040UL, 0x00000000UL,
  0x000140A0UL, 0x0F0027AAUL,
  0x000140B8UL, 0x0023C000UL,
  0x000140F4UL, 0x00001020UL,
  0x00024134UL, 0x00000880UL,
  /*    4138 */ 0x000087F6UL,
  0x00024140UL, 0x00880020UL,
  /*    4144 */ 0x4D52E6C1UL,
  0x00044160UL, 0x00000000UL,
  /*    4164 */ 0x00000000UL,
  /*    4168 */ 0x00000006UL,
  /*    416C */ 0x00000006UL,
  0x00086014UL, 0x00000010UL,
  /*    6018 */ 0x0413F920UL,
  /*    601C */ 0x0072000BUL,
  /*    6020 */ 0x00007000UL,
  /*    6024 */ 0x00000000UL,
  /*    6028 */ 0x03000000UL,
  /*    602C */ 0x0F014800UL,
  /*    6030 */ 0x4020B840UL,
  0x00066050UL, 0x00FE3E03UL,
  /*    6054 */ 0x00000841UL,
  /*    6058 */ 0x000000C8UL,
  /*    605C */ 0x000A07B0UL,
  /*    6060 */ 0x00000A70UL,
  /*    6064 */ 0x00000000UL,
  0x000C6078UL, 0x0B80631EUL,
  /*    607C */ 0x744AC39BUL,
  /*    6080 */ 0x00000034UL,
  /*    6084 */ 0x00008003UL,
  /*    6088 */ 0x00000000UL,
  /*    608C */ 0x30100101UL,
  /*    6090 */ 0x7F7F7050UL,
  /*    6094 */ 0x00000000UL,
  /*    6098 */ 0x00000000UL,
  /*    609C */ 0x00000000UL,
  /*    60A0 */ 0x00000000UL,
  /*    60A4 */ 0x00000000UL,
  0x000760E4UL, 0x8C8F0890UL,
  /*    60E8 */ 0x00000B59UL,
  /*    60EC */ 0x07830464UL,
  /*    60F0 */ 0x3AC81388UL,
  /*    60F4 */ 0x000A209CUL,
  /*    60F8 */ 0x00206100UL,
  /*    60FC */ 0x123556B7UL,
  0x00036104UL, 0x00104639UL,
  /*    6108 */ 0x29043020UL,
  /*    610C */ 0x0040BB88UL,
  0x00016120UL, 0x00000000UL,
  0x00016128UL, 0x0C660664UL,
  0x000C6130UL, 0x00FA53E8UL,
  /*    6134 */ 0x008C4B00UL,
  /*    6138 */ 0x1A150F09UL,
  /*    613C */ 0x36302B23UL,
  /*    6140 */ 0x4432423CUL,
  /*    6144 */ 0x0BA98765UL,
  /*    6148 */ 0x0000501EUL,
  /*    614C */ 0x00000001UL,
  /*    6150 */ 0x231A1403UL,
  /*    6154 */ 0x7F5A4614UL,
  /*    6158 */ 0x50C89540UL,
  /*    615C */ 0x093220A1UL,
  0x10017014UL, 0x0007F800UL,
  0x30017014UL, 0x000000F5UL,
  0x00067018UL, 0x00001800UL,
  /*    701C */ 0x05050060UL,
  /*    7020 */ 0xAB500003UL,
  /*    7024 */ 0x00000082UL,
  /*    7028 */ 0x00000000UL,
  /*    702C */ 0x000000D5UL,
  0x00027048UL, 0x0000383EUL,
  /*    704C */ 0x000025BCUL,
  0x00037070UL, 0x00010103UL,
  /*    7074 */ 0x00039C0FUL,
  /*    7078 */ 0x00624006UL,
  0xFFFFFFFFUL,
};

const RAIL_ChannelConfigEntry_t PHY_Longrange_490M_OQPSK_DSSS8_9p6kbps_cohdsa_channels[] = {
  {
    .phyConfigDeltaAdd = NULL,
    .baseFrequency = 490000000,
    .channelSpacing = 10000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 10,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr,
#ifdef RADIO_CONFIG_ENABLE_CONC_PHY
    .entryType = 0
#endif
  },
};

const RAIL_ChannelConfig_t PHY_Longrange_490M_OQPSK_DSSS8_9p6kbps_cohdsa_channelConfig = {
  .phyConfigBase = PHY_Longrange_490M_OQPSK_DSSS8_9p6kbps_cohdsa_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = PHY_Longrange_490M_OQPSK_DSSS8_9p6kbps_cohdsa_channels,
  .length = 1U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t *channelConfigs[] = {
  &PHY_Longrange_490M_OQPSK_DSSS8_9p6kbps_cohdsa_channelConfig,
  NULL
};

uint32_t phyAccelerationBuffer[207];
