/***************************************************************************//**
 * @file
 * @brief This file contains the plug-in manager for the EZRadio and
 * EZRadioPRO families.
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
#include <stdio.h>
#include <stdbool.h>

#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "ustimer.h"

#include "ezradio_cmd.h"
#include "ezradio_prop.h"
#include "ezradio_hal.h"
#include "ezradio_api_lib.h"
#include "ezradio_api_lib_add.h"
#include "ezradio_plugin_manager.h"

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/* Radio configuration data array. */
static const uint8_t Radio_Configuration_Data_Array[]  = \
  RADIO_CONFIGURATION_DATA_ARRAY;

/* Radio interrupt receive flag */
static bool    ezradioIrqReceived = false;

#if (EZRADIO_PLUGIN_TRANSMIT)
Ecode_t ezradioHandleTransmitPlugin(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_ReplyHandle_t radioReplyHandle);
#endif

#if (EZRADIO_PLUGIN_RECEIVE)
Ecode_t ezradioHandleReceivePlugin(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_ReplyHandle_t radioReplyHandle);
#endif

#if (EZRADIO_PLUGIN_CRC_ERROR)
Ecode_t ezradioHandleCrcErrorPlugin(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_ReplyHandle_t radioReplyHandle);
#endif

static void ezradioPowerUp(void);

static void GPIO_EZRadio_INT_IRQHandler(uint8_t pin);

/// @endcond

/**************************************************************************//**
 * @brief  Radio Initialization.
 *
 * @param[in] handle EzRadio driver instance handler.
 *****************************************************************************/
void ezradioInit(EZRADIODRV_Handle_t handle)
{
  (void)handle;

  /* Initialize radio GPIOs and SPI port */
  ezradio_hal_GpioInit(GPIO_EZRadio_INT_IRQHandler);

  /* Power Up the radio chip */
  ezradioPowerUp();

  /* Load radio configuration */
  while (EZRADIO_CONFIG_SUCCESS != ezradio_configuration_init(Radio_Configuration_Data_Array)) {
    /* Error hook */
#ifdef ERROR_HOOK
    ERROR_HOOK;
#else
    printf("ERROR: Radio configuration failed!\n");
#endif

    USTIMER_Init();
    /* Delay for 10ms time */
    USTIMER_Delay(10000u);
    /* Deinit ustimer */
    USTIMER_DeInit();

    /* Power Up the radio chip */
    ezradioPowerUp();
  }

  /* Read ITs, clear pending ones */
  ezradio_get_int_status(0u, 0u, 0u, NULL);
}

/**************************************************************************//**
 * @brief  EzRadio plugin manager state machine handler. This function should
 *         be called in the application in an infinite loop, in order to
 *         manage the EzRadio plug-ins properly.
 *
 * @param[in] radioHandle EzRadio driver instance handler.
 *****************************************************************************/
Ecode_t ezradioPluginManager(EZRADIODRV_Handle_t radioHandle)
{
  /* EZRadio response structure union */
  ezradio_cmd_reply_t radioReplyData;
  EZRADIODRV_ReplyHandle_t radioReplyHandle = &radioReplyData;

  if ( radioHandle == NULL ) {
    return ECODE_EMDRV_EZRADIODRV_ILLEGAL_HANDLE;
  }

  /* Check is radio interrupt is received. */
  if (ezradioIrqReceived) {
    /* Accept interrupt before clearing IT in the radio, so prevent race conditions. */
    ezradioIrqReceived = false;

    /* Read ITs, clear all pending ones */
    ezradio_get_int_status(0x0, 0x0, 0x0, radioReplyHandle);

#if (EZRADIO_PLUGIN_TRANSMIT)
    ezradioHandleTransmitPlugin(radioHandle, radioReplyHandle);
#endif

#if (EZRADIO_PLUGIN_RECEIVE)
    ezradioHandleReceivePlugin(radioHandle, radioReplyHandle);
#endif

#if (EZRADIO_PLUGIN_CRC_ERROR)
    ezradioHandleCrcErrorPlugin(radioHandle, radioReplyHandle);
#endif
  }

  return ECODE_EMDRV_EZRADIODRV_OK;
}

/**************************************************************************//**
 * @brief  Resets both the TX and RX FIFOs.
 *****************************************************************************/
void ezradioResetTRxFifo(void)
{
#if (EZRADIO_PLUGIN_RECEIVE)
  ezradio_fifo_info(EZRADIO_CMD_FIFO_INFO_ARG_FIFO_RX_BIT, NULL);
#endif //#if ( EZRADIO_PLUGIN_RECEIVE )

#if (EZRADIO_PLUGIN_TRANSMIT)
  ezradio_fifo_info(EZRADIO_CMD_FIFO_INFO_ARG_FIFO_TX_BIT, NULL);
#endif //#if ( EZRADIO_PLUGIN_TRANSMIT )
}

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/**************************************************************************//**
 * @brief  Power up the Radio.
 *****************************************************************************/
static void ezradioPowerUp(void)
{
  /* Hardware reset the chip */
  ezradio_reset();

  /* Initialize ustimer */
  USTIMER_Init();
  /* Delay for preconfigured time */
  USTIMER_Delay(RADIO_CONFIG_DATA_RADIO_DELAY_AFTER_RESET_US);
  /* Deinit ustimer */
  USTIMER_DeInit();
}

/**************************************************************************//**
 * @brief  Radio nIRQ GPIO interrupt.
 *****************************************************************************/
static void GPIO_EZRadio_INT_IRQHandler(uint8_t pin)
{
  (void)pin;

  /* Sign radio interrupt received */
  ezradioIrqReceived = true;
}

/// @endcond
