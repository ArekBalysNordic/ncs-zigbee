/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *
 * @brief Simple Zigbee light bulb implementation.
 */

#include "app_task_zigbee.h"

#ifdef CONFIG_CHIP

#include "app_task.h"
#include <nrf_802154_callbacks_dispatcher.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, CONFIG_CHIP_APP_LOG_LEVEL);

/* Zephyr thread stack size and priority */
#define ZIGBEE_THREAD_STACK_SIZE 2048
#define ZIGBEE_THREAD_PRIORITY 3
#define MATTER_THREAD_STACK_SIZE 3072
#define MATTER_THREAD_PRIORITY 3

void zigbee_thread_fn(void) { ZigbeeStart(); }

void matter_thread_fn(void) { AppTask::Instance().StartApp(); }

K_THREAD_DEFINE(zigbee_thread_id, ZIGBEE_THREAD_STACK_SIZE, zigbee_thread_fn,
                NULL, NULL, NULL, ZIGBEE_THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(matter_thread_id, MATTER_THREAD_STACK_SIZE, matter_thread_fn,
                NULL, NULL, NULL, MATTER_THREAD_PRIORITY, 0, 0);

#endif

int main(void) {

#ifdef CONFIG_CHIP

#if defined(CONFIG_NRF_802154_CALLBACKS_DISPATCHER)
  int ret =
      nrf_802154_callbacks_dispatcher_switch("zigbee_nrf_802154_radio", true);
  if (ret != 0) {
    LOG_ERR("Failed to switch 802.15.4 radio to Zigbee: %d", ret);
    return ret;
  }
#endif

  k_thread_start(zigbee_thread_id);
  k_thread_start(matter_thread_id);

#else

  ZigbeeStart();

#endif

  return 0;
}
