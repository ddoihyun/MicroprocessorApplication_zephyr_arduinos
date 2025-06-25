/*
 * Copyright (c) 2023 DHT22 + LCD Temperature and Humidity Monitor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include "dht_sensor.h"
#include "LCD.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* Sensor reading interval in milliseconds */
#define SENSOR_READ_INTERVAL_MS 3000

int main(void)
{
    int ret;
    struct dht_data sensor_data;
    char title[] = "DHT22 Monitor";
    
    LOG_INF("DHT22 + LCD Temperature and Humidity Monitor");
    LOG_INF("===============================================");
    
    /* Initialize LCD */
    LOG_INF("Initializing LCD...");
    LCD_TWI_init();
    k_msleep(100);
    LCD_begin(16, 2, 0);  // 16x2 LCD
    
    /* Display title on first line */
    LCD_setCursor(0, 0);
    LCD_print((uint8_t*)title, strlen(title) + 1);
    
    /* Initialize DHT sensor */
    LOG_INF("Initializing DHT22 sensor...");
    ret = dht_sensor_init();
    if (ret) {
        LOG_ERR("Failed to initialize DHT sensor: %d", ret);
        LCD_setCursor(0, 1);
        LCD_print((uint8_t*)"Sensor Error!", 14);
        return ret;
    }
    
    LOG_INF("Starting sensor readings every %d ms", SENSOR_READ_INTERVAL_MS);
    LOG_INF("Sensor device: %s", dht_sensor_get_name());
    LOG_INF("===============================================");
    
    /* Initial display */
    LCD_setCursor(0, 1);
    LCD_print((uint8_t*)"Initializing...", 16);
    
    /* Main sensor reading loop */
    while (1) {
        /* Read sensor data */
        ret = dht_sensor_read(&sensor_data);
        if (ret) {
            LOG_ERR("Failed to read sensor data: %d", ret);
            LCD_setCursor(0, 1);
            LCD_print((uint8_t*)"Read Error!     ", 17);
        } else {
            /* Print sensor data to log */
            dht_sensor_print_data(&sensor_data);
            
            /* Display sensor data on LCD */
            LCD_print_sensor_data(sensor_data.temperature.val1, 
                                 sensor_data.temperature.val2,
                                 sensor_data.humidity.val1, 
                                 sensor_data.humidity.val2);
            
            LOG_INF("---");
        }
        
        /* Wait before next reading */
        k_msleep(SENSOR_READ_INTERVAL_MS);
    }
    
    return 0;
}