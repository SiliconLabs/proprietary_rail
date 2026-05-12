/***************************************************************************//**
 * @file
 * @brief EzRadio transmit plug-in managed by the plug-in manager if enabled.
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

#include <stddef.h>

#include "em_device.h"

#include "ezradio_cmd.h"
#include "ezradio_prop.h"
#include "ezradio_api_lib.h"
#include "ezradio_api_lib_add.h"
#include "ezradio_plugin_manager.h"
#include "ezradio_transmit_plugin.h"

#if (EZRADIO_PLUGIN_TRANSMIT)

/**************************************************************************//**
 * @brief Set Radio to TX mode with packet configuration in pktLengthConf.
 *
 *  @param radioHandle Handler of the ezradiodrv instance where packet length
 *         and channel configurations are set.
 *  @param pktLengthConf Packet length configuration structure configuration
 *         of the packet.
 *
 *  @return
 *    @ref ECODE_EMDRV_EZRADIODRV_OK on success. On failure an appropriate EZRADIODRV
 *    @ref Ecode_t is returned.
 *****************************************************************************/
Ecode_t ezradioLengthConfig(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_PacketLengthConfig_t pktLengthConf)
{
  ezradio_cmd_reply_t ezradioReply;

  if ( radioHandle == NULL ) {
    return ECODE_EMDRV_EZRADIODRV_ILLEGAL_HANDLE;
  }

  /* Request and check radio device state */
  ezradio_request_device_state(&ezradioReply);

  if (ezradioReply.REQUEST_DEVICE_STATE.CURR_STATE == EZRADIO_CMD_REQUEST_DEVICE_STATE_REP_CURR_STATE_MAIN_STATE_ENUM_TX) {
    return ECODE_EMDRV_EZRADIODRV_TRANSMIT_FAILED;
  }

    radioHandle->packetTx.lenConfig.lenMode = pktLengthConf.lenMode;
    radioHandle->packetTx.lenConfig.pktLen  = 0;
    radioHandle->packetTx.lenConfig.pktLen  += radioHandle->packetTx.lenConfig.fieldLen.f1 = pktLengthConf.fieldLen.f1;
    radioHandle->packetTx.lenConfig.pktLen  += radioHandle->packetTx.lenConfig.fieldLen.f2 = pktLengthConf.fieldLen.f2;
    radioHandle->packetTx.lenConfig.pktLen  += radioHandle->packetTx.lenConfig.fieldLen.f3 = pktLengthConf.fieldLen.f3;
    radioHandle->packetTx.lenConfig.pktLen  += radioHandle->packetTx.lenConfig.fieldLen.f4 = pktLengthConf.fieldLen.f4;
    radioHandle->packetTx.lenConfig.pktLen  += radioHandle->packetTx.lenConfig.fieldLen.f5 = pktLengthConf.fieldLen.f5;

#if (RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY == RADIO_CHIP_FAMILY_EZRADIOPRO)

    ezradio_set_property(EZRADIO_PROP_GRP_ID_PKT, 2u,
                         EZRADIO_PROP_GRP_INDEX_PKT_FIELD_1_LENGTH,
                         0, pktLengthConf.fieldLen.f1
                         );

    ezradio_set_property(EZRADIO_PROP_GRP_ID_PKT, 2u,
                         EZRADIO_PROP_GRP_INDEX_PKT_FIELD_2_LENGTH,
                         0, pktLengthConf.fieldLen.f2
                         );

    ezradio_set_property(EZRADIO_PROP_GRP_ID_PKT, 2u,
                         EZRADIO_PROP_GRP_INDEX_PKT_FIELD_3_LENGTH,
                         0, pktLengthConf.fieldLen.f3
                         );

    ezradio_set_property(EZRADIO_PROP_GRP_ID_PKT, 2u,
                         EZRADIO_PROP_GRP_INDEX_PKT_FIELD_4_LENGTH,
                         0, pktLengthConf.fieldLen.f4
                         );

    ezradio_set_property(EZRADIO_PROP_GRP_ID_PKT, 2u,
                         EZRADIO_PROP_GRP_INDEX_PKT_FIELD_5_LENGTH,
                         0, pktLengthConf.fieldLen.f5
                         );
#endif //#if !(RADIO_CONFIG_DATA_RADIO_TYPE == 4455)


  return ECODE_EMDRV_EZRADIODRV_OK;
}
/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
//  Note: These are internal routines used by ezradio plugin manager.

/**************************************************************************//**
 * @brief EZRadioDrv transmit plug-in handler routine.
 *
 *  @param radioHandle EzRadio driver instance handler.
 *  @param radioReplyHandle EZRadio communication reply handler.
 *
 *  @return
 *    @ref ECODE_EMDRV_EZRADIODRV_OK on success. On failure an appropriate EZRADIODRV
 *    @ref Ecode_t is returned.
 *****************************************************************************/
Ecode_t ezradioHandleTransmitPlugin(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_ReplyHandle_t radioReplyHandle)
{
  if ( radioHandle == NULL ) {
    return ECODE_EMDRV_EZRADIODRV_ILLEGAL_HANDLE;
  }

  if ( radioReplyHandle->GET_INT_STATUS.PH_PEND & EZRADIO_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT ) {
    if ( radioHandle->packetTx.userCallback != NULL ) {
      radioHandle->packetTx.userCallback(radioHandle, ECODE_EMDRV_EZRADIODRV_OK);
    }
  }

  return ECODE_EMDRV_EZRADIODRV_OK;
}

/// @endcond

#endif //#if ( EZRADIO_PLUGIN_TRANSMIT )
