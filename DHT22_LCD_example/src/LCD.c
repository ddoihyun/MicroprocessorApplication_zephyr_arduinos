#include "LCD.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(lcd, LOG_LEVEL_INF);

// I2C 디바이스 선언
#define I2C_NODE DT_NODELABEL(i2c0)
static const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);

// LCD 주소 설정
static uint8_t _Addr = 0x3F;

uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines;
uint8_t _cols;
uint8_t _rows;
uint8_t _backlightval = LCD_BACKLIGHT;  // 백라이트 ON 상태로 초기화

void LCD_TWI_init() {
    if (!device_is_ready(i2c_dev)) {
        LOG_ERR("I2C device not ready");
        return;
    }
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    LOG_INF("LCD I2C initialized");
}

void LCD_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
    _cols = cols;
    _rows = lines;
    
    if (lines > 1) {
        _displayfunction |= LCD_2LINE;
    }
    _numlines = lines;

    if ((dotsize != 0) && (lines == 1)) {
        _displayfunction |= LCD_5x10DOTS;
    }

    k_msleep(50);

    LCD_backlight();  // 백라이트를 초기화 시 바로 켬
    LCD_expanderWrite(_backlightval);
    k_msleep(1000);

    LCD_write4bits(0x03 << 4);
    k_busy_wait(4500);

    LCD_write4bits(0x03 << 4);
    k_busy_wait(4500);

    LCD_write4bits(0x03 << 4);
    k_busy_wait(150);

    LCD_write4bits(0x02 << 4);

    LCD_command(LCD_FUNCTIONSET | _displayfunction);

    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    LCD_display();

    LCD_clear();

    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    LCD_command(LCD_ENTRYMODESET | _displaymode);

    LCD_home();
    
    LOG_INF("LCD initialized: %dx%d", cols, lines);
}

void LCD_clear() {
    LCD_command(LCD_CLEARDISPLAY);
    k_busy_wait(2000);
}

void LCD_home() {
    LCD_command(LCD_RETURNHOME);
    k_busy_wait(2000);
}

void LCD_setCursor(uint8_t col, uint8_t row) {
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row > (_numlines - 1)) {
        row = _numlines - 1;
    }
    LCD_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_print(uint8_t *string, int size) {
    for (int i = 0; i < size - 1; i++) {
        LCD_write(string[i]);
    }
}

void LCD_print_sensor_data(int32_t temp_val1, int32_t temp_val2, 
                          int32_t hum_val1, int32_t hum_val2) {
    char buffer[17]; // 16 characters + null terminator
    
    // 두 번째 줄로 이동
    LCD_setCursor(0, 1);
    
    // 온습도 데이터 포맷: "T:23.4 H:65.2%"
    snprintf(buffer, sizeof(buffer), "T:%d.%dC H:%d.%d%%", 
             temp_val1, temp_val2/100000, hum_val1, hum_val2/100000);
    
    LCD_print((uint8_t*)buffer, strlen(buffer) + 1);
}

void LCD_noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_display() {
    _displaycontrol |= LCD_DISPLAYON;
    LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_cursor() {
    _displaycontrol |= LCD_CURSORON;
    LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_blink() {
    _displaycontrol |= LCD_BLINKON;
    LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD_scrollDisplayLeft(void) {
    LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCD_scrollDisplayRight(void) {
    LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LCD_leftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    LCD_command(LCD_ENTRYMODESET | _displaymode);
}

void LCD_rightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    LCD_command(LCD_ENTRYMODESET | _displaymode);
}

void LCD_autoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    LCD_command(LCD_ENTRYMODESET | _displaymode);
}

void LCD_noAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    LCD_command(LCD_ENTRYMODESET | _displaymode);
}

void LCD_createChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7;
    LCD_command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++) {
        LCD_write(charmap[i]);
    }
}

void LCD_noBacklight(void) {
    _backlightval = LCD_NOBACKLIGHT;
    LCD_expanderWrite(0);
}

void LCD_backlight(void) {
    _backlightval = LCD_BACKLIGHT;
    LCD_expanderWrite(0);
}

inline void LCD_command(uint8_t value) {
    LCD_send(value, 0);
}

inline void LCD_write(uint8_t value) {
    LCD_send(value, Rs);
}

void LCD_send(uint8_t value, uint8_t mode) {
    uint8_t highnib = value & 0xf0;
    uint8_t lownib = (value << 4) & 0xf0;
    LCD_write4bits((highnib) | mode);
    LCD_write4bits((lownib) | mode);
}

void LCD_write4bits(uint8_t value) {
    LCD_expanderWrite(value);
    LCD_pulseEnable(value);
}

void LCD_expanderWrite(uint8_t _data) {
    _data |= _backlightval;
    uint8_t buf[1] = { _data };
    i2c_write(i2c_dev, buf, 1, _Addr);
    k_msleep(2);
}

void LCD_pulseEnable(uint8_t _data) {
    LCD_expanderWrite(_data | En);
    k_busy_wait(1);
    LCD_expanderWrite(_data & ~En);
    k_busy_wait(50);
}