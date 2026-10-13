/***************************************************************************//**
 * @file drv_uart.c
 * @brief mikroSDK 2.0 Click Peripheral Drivers - EUSART
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided \'as-is\', without any express or implied
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

#include <string.h>
#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_atomic.h"

#include "drv_uart.h"
#include "sl_status.h"
#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include "sl_iostream_eusart.h"
#include "em_eusart.h"

// Compatibility layer for peripheral and em lib
#if defined(LDMA0)
#define LDMA_PERIPH   LDMA0
#else
#define LDMA_PERIPH   LDMA
#endif

static void uart_config_baudrate(uart_t *obj);
static void uart_config_frame(uart_t *obj);

static uart_t *_owner = NULL;

static err_t _acquire(uart_t *obj, bool obj_open_state)
{
  err_t status = ACQUIRE_SUCCESS;

  if ((obj_open_state == true) && (_owner == obj)) {
    return ACQUIRE_FAIL;
  }

  if (_owner != obj) {
    _owner = obj;
  }

  return status;
}

void uart_configure_default(uart_config_t *config)
{
  config->tx_pin = 0xFFFFFFFF;
  config->rx_pin = 0xFFFFFFFF;

  config->baud = 115200;
  config->data_bits = UART_DATA_BITS_DEFAULT;
  config->parity = UART_PARITY_DEFAULT;
  config->stop_bits = UART_STOP_BITS_DEFAULT;
  config->tx_ring_size = 0;
  config->rx_ring_size = 0;
}

err_t uart_open(uart_t *obj, uart_config_t *config)
{
  uart_config_t *p_config = &obj->config;
  memcpy(p_config, config, sizeof(uart_config_t));
  sl_iostream_uart_t *ptr = (sl_iostream_uart_t *)obj->handle;

  // Configure baudrate.
  uart_config_baudrate(obj);

  // Configure frame.
  uart_config_frame(obj);

  // Don't use auto cr lf
  sl_iostream_uart_set_auto_cr_lf(ptr, false);

  // Default to blocking mode
  uart_set_blocking(obj, true);

  return _acquire(obj, true);
}

err_t uart_set_baud(uart_t *obj, uint32_t baud)
{
  if (baud > 0) {
    if (_acquire(obj, false) != ACQUIRE_FAIL) {
      obj->config.baud = baud;
      // Configure baudrate.
      uart_config_baudrate(obj);
      return UART_SUCCESS;
    }
  }
  return UART_ERROR;
}

err_t uart_set_parity(uart_t *obj, uart_parity_t parity)
{
  if (parity <= UART_PARITY_ODD) {
    if (_acquire(obj, false) != ACQUIRE_FAIL) {
      obj->config.parity = parity;

      // Configure frame.
      uart_config_frame(obj);
      return UART_SUCCESS;
    }
  }
  return UART_ERROR;
}

err_t uart_set_stop_bits(uart_t *obj, uart_stop_bits_t stop)
{
  if (stop <= UART_STOP_BITS_TWO) {
    if (_acquire(obj, false) != ACQUIRE_FAIL) {
      obj->config.stop_bits = stop;

      // Configure frame.
      uart_config_frame(obj);
      return UART_SUCCESS;
    }
  }
  return UART_ERROR;
}

err_t uart_set_data_bits(uart_t *obj, uart_data_bits_t bits)
{
  if (bits <= UART_DATA_BITS_9) {
    if (_acquire(obj, false) != ACQUIRE_FAIL) {
      obj->config.data_bits = bits;

      // Configure frame.
      uart_config_frame(obj);
      return UART_SUCCESS;
    }
  }
  return UART_ERROR;
}

void uart_set_blocking(uart_t *obj, bool blocking)
{
#if (defined(SL_CATALOG_KERNEL_PRESENT))
  sl_iostream_uart_t *ptr = (sl_iostream_uart_t *)obj->handle;
  sl_iostream_uart_set_read_block(ptr, blocking);
#endif
  obj->is_blocking = blocking;
}

err_t uart_write(uart_t *obj, uint8_t *buffer, size_t size)
{
  sl_status_t status;

  if (_acquire(obj, false) != ACQUIRE_FAIL) {
    sl_iostream_uart_t *ptr = (sl_iostream_uart_t *)obj->handle;
    status = sl_iostream_write(&(ptr->stream),
                               buffer,
                               size);
    if (SL_STATUS_OK == status) {
      return size;
    }
  }
  return UART_ERROR;
}

err_t uart_read(uart_t *obj, uint8_t *buffer, size_t size)
{
  sl_status_t status;
  size_t read_size;
  int32_t total_size = 0;

  if (_acquire(obj, false) != ACQUIRE_FAIL) {
    sl_iostream_uart_t *ptr = (sl_iostream_uart_t *)obj->handle;
    if (obj->is_blocking) {
      while (size) {
        status = sl_iostream_read(&(ptr->stream),
                                  buffer,
                                  size,
                                  &read_size);
        if (SL_STATUS_OK == status) {
          if (read_size > 0) {
            if (read_size <= size) {
              size -= read_size;
              buffer += read_size;
              total_size += read_size;
            } else {
              size = 0;
            }
          }
        }
      }
    } else {   // Non-blocking read
      status = sl_iostream_read(&(ptr->stream),
                                buffer,
                                size,
                                &read_size);
      if (SL_STATUS_OK == status) {
        total_size = read_size;
      }
    }
    return total_size;
  }
  return UART_ERROR;
}

err_t uart_print(uart_t *obj, char *text)
{
  size_t data_written = 0;

  if (_acquire(obj, false) != ACQUIRE_FAIL) {
    sl_iostream_uart_t *ptr = (sl_iostream_uart_t *)obj->handle;
    while (*text) {
      if (SL_STATUS_OK != sl_iostream_write(&(ptr->stream), text++, 1)) {
        return data_written;
      }
      data_written++;
    }
    return data_written;
  } else {
    return UART_ERROR;
  }
}

err_t uart_println(uart_t *obj, char *text)
{
  size_t data_written = 0;

  if (_acquire(obj, false) != ACQUIRE_FAIL) {
    sl_iostream_uart_t *ptr = (sl_iostream_uart_t *)obj->handle;
    while (*text) {
      if (SL_STATUS_OK != sl_iostream_write(&(ptr->stream), text++, 1)) {
        return data_written;
      }
      data_written++;
    }
    if (SL_STATUS_OK != sl_iostream_write(&(ptr->stream), (uint8_t *)"\r\n",
                                          2)) {
      return data_written;
    }
    data_written += 2;
    return data_written;
  } else {
    return UART_ERROR;
  }
}

size_t uart_bytes_available(uart_t *obj)
{
  (void) obj;
  // Currently this API is not supported
  return 0;
}

void uart_clear(uart_t *obj)
{
  sl_iostream_uart_t *ptr = (sl_iostream_uart_t *)obj->handle;
  sl_iostream_eusart_context_t *ctx =
    (sl_iostream_eusart_context_t *)ptr->stream.context;

  size_t read_size;
  uint8_t buffer[ctx->context.rx_buffer_len];

  // Try to read all available data in RX buffer
  sl_iostream_read(&(ptr->stream),
                   buffer,
                   ctx->context.rx_buffer_len,
                   &read_size);
}

void uart_close(uart_t *obj)
{
  obj->handle = NULL;
  _owner = NULL;
}

static void uart_config_baudrate(uart_t *obj)
{
  (void) obj;
}

static void uart_config_frame(uart_t *obj)
{
  (void) obj;
}

// ------------------------------------------------------------------------- END
