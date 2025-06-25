#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include "LCD.h"

void main(void) {
    LCD_TWI_init();
    LCD_begin(16, 2, LCD_5x8DOTS);
    LCD_setCursor(0, 0);

    uint8_t hello[] = "Hello World";
    LCD_print(hello, sizeof(hello));
}
