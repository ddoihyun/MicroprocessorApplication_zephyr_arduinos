#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include <stdio.h>
#include "uart_co2.h"
#include "LCD.h"

int main(void)
{
    /* LCD 초기화 */
    LCD_TWI_init();
    LCD_begin(16, 2, LCD_5x8DOTS);
    LCD_clear();
    LCD_backlight();

    /* UART CO2 센서 초기화 */
    const struct device *uart_dev = DEVICE_DT_GET(DT_ALIAS(myserial));
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return;
    }
    uart_irq_callback_user_data_set(uart_dev, uart_co2_serial_callback, NULL);
    uart_irq_rx_enable(uart_dev);

    char packet[MSG_SIZE];
    while (1) {
        /* CO2 측정 요청 전송 */
        uart_co2_send_request();

        /* 메시지 큐에서 응답 대기 */
        if (k_msgq_get(&uart_co2_msgq, packet, K_MSEC(2000)) == 0) {
            int ppm = uart_co2_get_ppm(packet);
            char display[32];

            /* 체크섬 오류 또는 데이터 이상 시 에러 표시 */
            if (ppm < 0) {
                snprintf(display, sizeof(display), "CO2 Error");
            } else {
                snprintf(display, sizeof(display), "CO2: %4d ppm", ppm);
            }

            /* LCD에 출력 */
            LCD_clear();
            LCD_setCursor(0, 0);
            LCD_print((uint8_t *)display, strlen(display) + 1);
        } else {
            /* 타임아웃 시 표시 */
            const char *timeout_msg = "No Data";
            LCD_clear();
            LCD_setCursor(0, 0);
            LCD_print((uint8_t *)timeout_msg, strlen(timeout_msg) + 1);
        }

        /* 2초 대기 후 반복 */
        k_msleep(2000);
    }

    return 0;
}
