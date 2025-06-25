/*
 * Copyright (c) 2023 DHT22 Temperature and Humidity Sensor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

/**
 * @brief DHT sensor data structure
 */
struct dht_data {
    struct sensor_value temperature;
    struct sensor_value humidity;
    bool valid;
};

/**
 * @brief Initialize DHT sensor
 * 
 * @return 0 on success, negative error code on failure
 */
int dht_sensor_init(void);

/**
 * @brief Check if DHT sensor is ready
 * 
 * @return true if ready, false otherwise
 */
bool dht_sensor_is_ready(void);

/**
 * @brief Read temperature and humidity from DHT sensor
 * 
 * @param data Pointer to store sensor data
 * @return 0 on success, negative error code on failure
 */
int dht_sensor_read(struct dht_data *data);

/**
 * @brief Print sensor data to log
 * 
 * @param data Sensor data to print
 */
void dht_sensor_print_data(const struct dht_data *data);

/**
 * @brief Get device name
 * 
 * @return Device name string
 */
const char *dht_sensor_get_name(void);

#endif /* DHT_SENSOR_H */