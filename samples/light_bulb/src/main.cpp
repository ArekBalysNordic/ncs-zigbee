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

// K_THREAD_DEFINE(zigbee_thread_id, ZIGBEE_THREAD_STACK_SIZE, zigbee_thread_fn,
//                 NULL, NULL, NULL, ZIGBEE_THREAD_PRIORITY, 0, 0);

// K_THREAD_DEFINE(matter_thread_id, MATTER_THREAD_STACK_SIZE, matter_thread_fn,
//                 NULL, NULL, NULL, MATTER_THREAD_PRIORITY, 0, 0);

#include <nrf_802154_callbacks_dispatcher.h>
extern "C" {
#include <zb_nrf_platform.h>
#include <radio_nrf5.h>
}

static const struct nrf_802154_callbacks openthread_802154_callbacks = {
	.init = openthread_nrf_802154_radio_init,
	.received_timestamp_raw = openthread_nrf_802154_received_timestamp_raw,
	.receive_failed = openthread_nrf_802154_receive_failed,
	.tx_ack_started = openthread_nrf_802154_tx_ack_started,
	.transmitted_raw = openthread_nrf_802154_transmitted_raw,
	.transmit_failed = openthread_nrf_802154_transmit_failed,
	.energy_detected = openthread_nrf_802154_energy_detected,
	.energy_detection_failed = openthread_nrf_802154_energy_detection_failed,
#if defined(CONFIG_NRF_802154_SER_HOST)
	.serialization_error = openthread_nrf_802154_serialization_error,
#endif
};

NRF_802154_CALLBACKS_DISPATCHER_REGISTER(openthread_nrf_802154_radio, openthread_802154_callbacks);

static const struct nrf_802154_callbacks zigbee_802154_callbacks = {
	.init = zigbee_nrf_802154_radio_init,
	.received_timestamp_raw = zigbee_nrf_802154_received_timestamp_raw,
	.receive_failed = zigbee_nrf_802154_receive_failed,
	.tx_ack_started = zigbee_nrf_802154_tx_ack_started,
	.transmitted_raw = zigbee_nrf_802154_transmitted_raw,
	.transmit_failed = zigbee_nrf_802154_transmit_failed,
	.energy_detected = zigbee_nrf_802154_energy_detected,
	.energy_detection_failed = zigbee_nrf_802154_energy_detection_failed,
#if defined(CONFIG_NRF_802154_SER_HOST)
	.serialization_error = NULL,
#endif
};

NRF_802154_CALLBACKS_DISPATCHER_REGISTER(zigbee_nrf_802154_radio, zigbee_802154_callbacks);

#endif

int main(void) {

#ifdef CONFIG_CHIP

  int ret =
      nrf_802154_callbacks_dispatcher_activate("zigbee_nrf_802154_radio");
  if (ret != 0) {
    LOG_ERR("Failed to activate Zigbee 802.15.4 radio: %d", ret);
    return ret;
  }

  // k_thread_start(matter_thread_id);
  // k_thread_start(zigbee_thread_id);

  ZigbeeStart();

#else

  ZigbeeStart();

#endif

  return 0;
}
