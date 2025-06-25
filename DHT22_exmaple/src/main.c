/*
 * Copyright (c) 2023 DHT22 Temperature and Humidity Sensor Application
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "dht_sensor.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* Sensor reading interval in milliseconds */
#define SENSOR_READ_INTERVAL_MS 2000

int main(void)
{
    int ret;
    struct dht_data sensor_data;
    
    LOG_INF("DHT22 Temperature and Humidity Sensor Application");
    LOG_INF("=================================================");
    
    /* Initialize DHT sensor */
    ret = dht_sensor_init();
    if (ret) {
        LOG_ERR("Failed to initialize DHT sensor: %d", ret);
        return ret;
    }
    
    LOG_INF("Starting sensor readings every %d ms", SENSOR_READ_INTERVAL_MS);
    LOG_INF("Sensor device: %s", dht_sensor_get_name());
    LOG_INF("=================================================");
    
    /* Main sensor reading loop */
    while (1) {
        /* Read sensor data */
        ret = dht_sensor_read(&sensor_data);
        if (ret) {
            LOG_ERR("Failed to read sensor data: %d", ret);
        } else {
            /* Print sensor data */
            dht_sensor_print_data(&sensor_data);
            LOG_INF("---");
        }
        
        /* Wait before next reading */
        k_msleep(SENSOR_READ_INTERVAL_MS);
    }
    
    return 0;
}