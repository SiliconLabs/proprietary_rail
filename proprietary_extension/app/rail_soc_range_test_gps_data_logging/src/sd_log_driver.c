/***************************************************************************//**
 * @file
 * @brief sd_log_driver.c
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
#include <stdio.h>
#include "string.h"

#include "em_common.h"
#include "em_gpio.h"

#include "diskio.h"

#include "sl_spidrv_instances.h"
#include "sl_sdc_sd_card.h"

#include "sd_log_driver.h"

#include "app_assert.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
// These functions are used to control the chip select pin of the SD card SPI
// interface.
static void sd_card_spi_chip_select(void);
static void sd_card_spi_chip_deselect(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// Filesystem object structure
FATFS fs;
// File object structure
FIL log_file;

// Written byte count. Not checked in this example
unsigned int bytes_written;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
#if ((_SILICON_LABS_32B_SERIES_2_CONFIG == 5) \
  || (_SILICON_LABS_32B_SERIES_2_CONFIG == 8))
SPIDRV_Handle_t *sd_spidrv_handle = &sl_spidrv_eusart_microsd_handle;
#else
SPIDRV_Handle_t *sd_spidrv_handle = &sl_spidrv_microsd_handle;
#endif
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * The function is used for SD driver initialization.
 *****************************************************************************/
void sd_init(void)
{
  sl_spidrv_init_instances();
  sl_gpio_t pinport =
  { (*sd_spidrv_handle)->initData.portRx,
    (*sd_spidrv_handle)->initData.pinRx };
  sl_status_t status =
    sl_gpio_set_pin_mode(&pinport, SL_GPIO_MODE_INPUT_PULL, 1);
  app_assert_status(status);
#if (_SILICON_LABS_32B_SERIES_2_CONFIG == 5)
  (*sd_spidrv_handle)->portCs = SL_SPIDRV_EUSART_MICROSD_CS_PORT;
  (*sd_spidrv_handle)->pinCs = SL_SPIDRV_EUSART_MICROSD_CS_PIN;
#endif
  status = sd_card_spi_init((*sd_spidrv_handle));
  app_assert_status(status);
  pinport = (sl_gpio_t){ (*sd_spidrv_handle)->portCs,
                         (*sd_spidrv_handle)->pinCs };
  sl_gpio_set_pin_mode(&pinport, SL_GPIO_MODE_PUSH_PULL, 1);
}

/**************************************************************************//**
 * The function is used to start a new log entry on the SD card.
 * If the log file already exists, it will be overwritten.
 *****************************************************************************/
FRESULT sd_start_log_entry(const char *log_file_name)
{
  sd_card_spi_chip_select();
  FRESULT result = f_mount(&fs, "", 0);
  if (result == FR_OK) {
    result = f_open(&log_file,
                    log_file_name,
                    FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
  }
  sd_card_spi_chip_deselect();
  return result;
}

/**************************************************************************//**
 * The function is used to stop the current log entry on the SD card.
 *****************************************************************************/
FRESULT sd_stop_log_entry(void)
{
  sd_card_spi_chip_select();
  FRESULT result = f_close(&log_file);
  if (result == FR_OK) {
    result = f_mount(NULL, "", 1);
  }
  sd_card_spi_chip_deselect();
  return result;
}

/**************************************************************************//**
 * The function is used to log a string to the SD card.
 *****************************************************************************/
FRESULT sd_log(const char *str, unsigned int len)
{
  // Write a message
  sd_card_spi_chip_select();
  FRESULT result = f_write(&log_file, str, len, &bytes_written);
  sd_card_spi_chip_deselect();
  return result;
}

/**************************************************************************//**
 * The function is used to check if SD card is present or not.
 *****************************************************************************/
bool sd_is_present(void)
{
  DSTATUS sd_card_status = sd_card_disk_status();
  if (sd_card_status & STA_NODISK) {
    return false;
  } else {
    return true;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void sd_card_spi_chip_select(void)
{
  GPIO_PinOutClear((*sd_spidrv_handle)->portCs,
                   (*sd_spidrv_handle)->pinCs);
}

static void sd_card_spi_chip_deselect(void)
{
  GPIO_PinOutSet((*sd_spidrv_handle)->portCs,
                 (*sd_spidrv_handle)->pinCs);
}
