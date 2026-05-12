/***************************************************************************//**
 * @file
 * @brief This file contains EZRadio HAL.
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

#include <stdlib.h>
#include <stdint.h>

#include "em_gpio.h"

#include "spidrv.h"
#include "sl_spidrv_instances.h"
#include "gpiointerrupt.h"

#include "ezradiodrv_config.h"
#include "ezradio_hal.h"

#define ezradioSpiHandlePtr sl_spidrv_exp_handle

/**
 * Configures the EZRadio GPIO port and pins
 *
 * @param[in] ezradioIrqCallback EZRadio interrupt callback configuration
 */
void ezradio_hal_GpioInit(GPIOINT_IrqCallbackPtr_t ezradioIrqCallback)
{

#if !(EZRADIODRV_SPI_4WIRE_MODE)
  GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN, gpioModePushPull, 1);
#endif

  /* Setup enable and interrupt pins to radio */
  GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN, gpioModeInputPull, 1);

#if (EZRADIODRV_COMM_USE_GPIO1_FOR_CTS)
  //Enable GPIO1 for CTS input
  GPIO_PinModeSet((GPIO_Port_TypeDef)RF_GPIO1_PORT, RF_GPIO1_PIN, gpioModeInput, 0);
#endif

  if (NULL != ezradioIrqCallback) {
    /* Register callback and enable interrupt */
    GPIOINT_CallbackRegister(RF_INT_PIN, ezradioIrqCallback);
    GPIO_ExtIntConfig( (GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN, RF_INT_PIN, false, true, true);
  }
}

/**
 * Asserts SDN pin of the EZRadio device.
 */
void ezradio_hal_AssertShutdown(void)
{
  GPIO_PinOutSet( (GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN);
}

/**
 * Deasserts SDN pin of the EZRadio device.
 */
void ezradio_hal_DeassertShutdown(void)
{
  GPIO_PinOutClear( (GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN);
}

/**
 * Clears nSEL pin of the EZRadio device.
 */
void ezradio_hal_ClearNsel(void)
{
  GPIO_PinOutClear( (GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN);
}

/**
 * Sets nSEL pin of the EZRadio device.
 */
void ezradio_hal_SetNsel(void)
{
  GPIO_PinOutSet( (GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN);
}

/**
 * Reads nIRQ pin of the EZRadio device.
 *
 * @return Value of nIRQ pin.
 */
uint8_t ezradio_hal_NirqLevel(void)
{
  return GPIO_PinInGet( (GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN);
}

#if (EZRADIODRV_COMM_USE_GPIO1_FOR_CTS)
/**
 * Reads GPIO1 pin of the EZRadio device.
 *
 * @return Value of GPIO1 pin.
 */
uint8_t ezradio_hal_Gpio1Level(void)
{
  return GPIO_PinInGet( (GPIO_Port_TypeDef) RF_GPIO1_PORT, RF_GPIO1_PIN);
}
#endif

/**
 * Writes a single byte to the EZRadio SPI port.
 *
 * @param byteToWrite Byte to write.
 */
void ezradio_hal_SpiWriteByte(uint8_t byteToWrite)
{
  SPIDRV_MTransmitB(ezradioSpiHandlePtr, &byteToWrite, 1);
}

/**
 * Reads a single byte from the EZRadio SPI port.
 *
 * @param readByte Read byte.
 */
void ezradio_hal_SpiReadByte(uint8_t* readByte)
{
  SPIDRV_MReceiveB(ezradioSpiHandlePtr, readByte, 1);
}

/**
 * Writes byteCount number of bytes to the EZRadio SPI port.
 *
 * @param byteCount Number of bytes to write.
 * @param pData Pointer to the byte array.
 */
void ezradio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData)
{
  SPIDRV_MTransmitB(ezradioSpiHandlePtr, pData, byteCount);
}

/**
 * Reads byteCount number of bytes from the EZRadio SPI port.
 *
 * @param byteCount Number of bytes to write.
 * @param pData Pointer to the byte array.
 */
void ezradio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData)
{
  SPIDRV_MReceiveB(ezradioSpiHandlePtr, pData, byteCount);
}

/**
 * Reads byteCount number of bytes from the EZRadio SPI port.
 *
 * @param byteCount Number of bytes to write.
 * @param pData Pointer to the byte array.
 */
void ezradio_hal_SpiWriteReadData(uint8_t byteCount, uint8_t* txData, uint8_t* rxData)
{
  SPIDRV_MTransferB(ezradioSpiHandlePtr, txData, rxData, byteCount);
}
