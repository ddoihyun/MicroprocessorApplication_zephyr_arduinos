#ifndef LCD_H_
#define LCD_H_

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <stdint.h>
#include <stdio.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

/**
 * @brief Initialize LCD with I2C
 */
void LCD_TWI_init(void);

/**
 * @brief Initialize LCD display
 * @param cols Number of columns
 * @param rows Number of rows  
 * @param charsize Character size
 */
void LCD_begin(uint8_t cols, uint8_t rows, uint8_t charsize);

/**
 * @brief Clear LCD display
 */
void LCD_clear(void);

/**
 * @brief Set cursor position
 * @param col Column position
 * @param row Row position
 */
void LCD_setCursor(uint8_t col, uint8_t row);

/**
 * @brief Print string to LCD
 * @param string String to print
 * @param size String size
 */
void LCD_print(uint8_t *string, int size);

/**
 * @brief Print formatted temperature and humidity
 * @param temp_val1 Temperature integer part
 * @param temp_val2 Temperature decimal part
 * @param hum_val1 Humidity integer part  
 * @param hum_val2 Humidity decimal part
 */
void LCD_print_sensor_data(int32_t temp_val1, int32_t temp_val2, 
                          int32_t hum_val1, int32_t hum_val2);

// Core LCD functions
void LCD_home(void);
void LCD_display(void);
void LCD_noDisplay(void);
void LCD_cursor(void);
void LCD_noCursor(void);
void LCD_blink(void);
void LCD_noBlink(void);
void LCD_scrollDisplayLeft(void);
void LCD_scrollDisplayRight(void);
void LCD_leftToRight(void);
void LCD_rightToLeft(void);
void LCD_autoscroll(void);
void LCD_noAutoscroll(void);
void LCD_backlight(void);
void LCD_noBacklight(void);
void LCD_createChar(uint8_t location, uint8_t charmap[]);

// Low-level functions
void LCD_command(uint8_t value);
void LCD_write(uint8_t value);
void LCD_send(uint8_t value, uint8_t mode);
void LCD_write4bits(uint8_t value);
void LCD_expanderWrite(uint8_t data);
void LCD_pulseEnable(uint8_t data);

#endif