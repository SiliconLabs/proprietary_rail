/***************************************************************************//**
 * @file
 * @brief EzRadio unmodulated carrier plug-in managed by the plug-in manager if enabled.
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

#if (EZRADIO_PLUGIN_UNMOD_CARRIER)


static uint8_t restore_mod_type;

/**************************************************************************//**
 * @brief Starts unmodulated carrier transmission with the radio configurations
 *        from the radioHandle parameter.
 *
 *  @param radioHandle Handler of the EzRadio driver instance where channel
 *         configuration is set.
 *
 *  @return
 *    @ref ECODE_EMDRV_EZRADIODRV_OK on success. On failure an appropriate EZRADIODRV
 *    @ref Ecode_t is returned.
 *****************************************************************************/
Ecode_t ezradioStartUnmodulatedCarrier(EZRADIODRV_Handle_t radioHandle)
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

  /* Save the original value of mod type property */
  ezradio_get_property(EZRADIO_PROP_GRP_ID_MODEM, 1, EZRADIO_PROP_GRP_INDEX_MODEM_MOD_TYPE, &ezradioReply);
  restore_mod_type = ezradioReply.GET_PROPERTY.DATA[0];

  /* Set MOD_TYPE to CW */
  ezradio_set_property(EZRADIO_PROP_GRP_ID_MODEM, 1, EZRADIO_PROP_GRP_INDEX_MODEM_MOD_TYPE, 0x08); //0x00 would be probably enough, but WDS generates 0x08

  /* Read ITs, clear pending ones */
  ezradio_get_int_status(0u, 0u, 0u, NULL);

  /* Start sending packet, channel 0, START immediately, Packet n bytes long, go READY when done */
  ezradio_start_tx(radioHandle->unmodCarrier.channel, 0u, 0u);

  return ECODE_EMDRV_EZRADIODRV_OK;
}

/**************************************************************************//**
 * @brief Stops unmodulated carrier transmission.
 *
 *  @return
 *    @ref ECODE_EMDRV_EZRADIODRV_OK on success. On failure an appropriate EZRADIODRV
 *    @ref Ecode_t is returned.
 *****************************************************************************/
Ecode_t ezradioStopUnmodulatedCarrier(void)
{
  ezradio_change_state(EZRADIO_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_READY);

  /* Restore MOD_TYPE */
  ezradio_set_property(EZRADIO_PROP_GRP_ID_MODEM, 1, EZRADIO_PROP_GRP_INDEX_MODEM_MOD_TYPE, restore_mod_type);

  return ECODE_EMDRV_EZRADIODRV_OK;
}

#endif //#if ( EZRADIO_PLUGIN_UNMOD_CARRIER )
