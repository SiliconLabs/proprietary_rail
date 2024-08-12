/***************************************************************************//**
 * @file
 * @brief led_control.c
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
 ******************************************************************************/

#include "led_control.h"
#include "sl_sleeptimer.h"
#include "sl_simple_led_instances.h"

static uint32_t blinky_led_on_ticks, blinky_led_off_ticks;

static sl_sleeptimer_timer_handle_t blinky_timer_handle;
static int current_instance;

static void blinky_off(sl_sleeptimer_timer_handle_t *handle, void *data);
static void blinky_on(sl_sleeptimer_timer_handle_t *handle, void *data);

void blinky_start(int instance, int period, int duty_cycle)
{
  current_instance = instance;
  blinky_led_on_ticks = sl_sleeptimer_ms_to_tick(period * duty_cycle / 100);
  blinky_led_off_ticks = sl_sleeptimer_ms_to_tick(period) - blinky_led_on_ticks;
  blinky_on(0, 0);
}

void blinky_stop(void)
{
  sl_sleeptimer_stop_timer(&blinky_timer_handle);
  sl_led_turn_off(SL_SIMPLE_LED_INSTANCE(current_instance));
}

static void blinky_on(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle; (void)data; // unused parameters

  sl_led_turn_on(SL_SIMPLE_LED_INSTANCE(current_instance));
  sl_sleeptimer_start_timer(&blinky_timer_handle,
                            blinky_led_on_ticks,
                            blinky_off,
                            0,
                            0,
                            0);
}

static void blinky_off(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle; (void)data; // unused parameters

  sl_led_turn_off(SL_SIMPLE_LED_INSTANCE(current_instance));
  sl_sleeptimer_start_timer(&blinky_timer_handle,
                            blinky_led_off_ticks,
                            blinky_on,
                            0,
                            0,
                            0);
}

void led_on(int instance)
{
  sl_led_turn_on(SL_SIMPLE_LED_INSTANCE(instance));
}

void led_off(int instance)
{
  sl_led_turn_off(SL_SIMPLE_LED_INSTANCE(instance));
}
