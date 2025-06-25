/*
 * Copyright (c) 2023 DHT22 Temperature and Humidity Sensor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dht_sensor.h"

LOG_MODULE_REGISTER(dht_sensor, LOG_LEVEL_INF);

/* DHT device instance */
static const struct device *dht_dev;

int dht_sensor_init(void)
{
    dht_dev = DEVICE_DT_GET_ONE(aosong_dht);
    
    if (!dht_dev) {
        LOG_ERR("Failed to get DHT device");
        return -ENODEV;
    }
    
    if (!device_is_ready(dht_dev)) {
        LOG_ERR("Device %s is not ready", dht_dev->name);
        return -ENODEV;
    }
    
    LOG_INF("DHT22 sensor initialized successfully");
    LOG_INF("Device: %s", dht_dev->name);
    
    return 0;
}

bool dht_sensor_is_ready(void)
{
    return (dht_dev != NULL) && device_is_ready(dht_dev);
}

int dht_sensor_read(struct dht_data *data)
{
    int ret;
    
    if (!data) {
        return -EINVAL;
    }
    
    if (!dht_sensor_is_ready()) {
        LOG_ERR("DHT sensor not ready");
        return -ENODEV;
    }
    
    /* Initialize data structure */
    data->valid = false;
    
    /* Fetch sensor data */
    ret = sensor_sample_fetch(dht_dev);
    if (ret) {
        LOG_ERR("Sensor fetch failed: %d", ret);
        return ret;
    }
    
    /* Get temperature */
    ret = sensor_channel_get(dht_dev, SENSOR_CHAN_AMBIENT_TEMP, &data->temperature);
    if (ret) {
        LOG_ERR("Failed to get temperature: %d", ret);
        return ret;
    }
    
    /* Get humidity */
    ret = sensor_channel_get(dht_dev, SENSOR_CHAN_HUMIDITY, &data->humidity);
    if (ret) {
        LOG_ERR("Failed to get humidity: %d", ret);
        return ret;
    }
    
    data->valid = true;
    
    return 0;
}

void dht_sensor_print_data(const struct dht_data *data)
{
    if (!data || !data->valid) {
        LOG_WRN("Invalid sensor data");
        return;
    }
    
    LOG_INF("Temperature: %d.%06d °C", 
            data->temperature.val1, 
            data->temperature.val2);
    
    LOG_INF("Humidity: %d.%06d %%RH", 
            data->humidity.val1, 
            data->humidity.val2);
}

const char *dht_sensor_get_name(void)
{
    if (dht_dev) {
        return dht_dev->name;
    }
    return "Unknown";
}