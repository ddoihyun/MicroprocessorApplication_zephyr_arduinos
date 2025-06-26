#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>
#include <string.h>
#include "LCD.h"

void main(void) {
    // 초기화 로그
    printk("LCD 테스트 시작\n");

    // I2C 및 LCD 초기화
    LCD_TWI_init();               // I2C 디바이스 초기화
    LCD_begin(16, 2, 0);          // 16x2 LCD, 기본 폰트

    // 문자열 출력
    const char *msg1 = "Hello, world!";
    const char *msg2 = "Zephyr + LCD";

    LCD_clear();
    LCD_setCursor(0, 0);          // 1행
    LCD_print((uint8_t *)msg1, strlen(msg1) + 1);

    LCD_setCursor(0, 1);          // 2행
    LCD_print((uint8_t *)msg2, strlen(msg2) + 1);

    // 이후 유동적 출력: 문자열 변경
    while (1) {
        k_msleep(2000);
        
        LCD_clear();
        LCD_setCursor(0, 0);

        char buffer[20];
        static int count = 0;
        snprintf(buffer, sizeof(buffer), "Count: %d", count++);
        LCD_print((uint8_t *)buffer, strlen(buffer) + 1);

        LCD_setCursor(0, 1);
        LCD_print((uint8_t *)"Updating...", 11);
    }
}
