// MH-Z14A sensor

#include "uart_co2.h"
#include <string.h>

/* 메시지 큐 정의: 최대 9바이트 메시지 9개 저장, align=4 */
K_MSGQ_DEFINE(uart_co2_msgq, MSG_SIZE, 9, 4);

/* UART 디바이스 바인딩 (DT_ALIAS 로 설정) */
static const struct device *const uart_dev = DEVICE_DT_GET(DT_N_ALIAS_myserial);

/* 수신 버퍼 및 상태 머신 변수 */
static char rx_buf[MSG_SIZE];
static int rx_pos;
static enum {
    UART_FSM_IDLE,
    UART_FSM_HEADER,
    UART_FSM_DATA,
    UART_FSM_CHECKSUM,
    UART_FSM_END,
} uart_fsm = UART_FSM_IDLE;

/* 상태 머신 업데이트 */
static void uart_co2_fsm_update(uint8_t byte)
{
    switch (uart_fsm) {
    case UART_FSM_IDLE:
        if (byte == 0xFF) uart_fsm = UART_FSM_HEADER;
        break;
    case UART_FSM_HEADER:
        if (byte == 0x86) uart_fsm = UART_FSM_DATA;
        else uart_fsm = UART_FSM_IDLE;
        break;
    case UART_FSM_DATA:
        if (rx_pos == MSG_SIZE - 2) uart_fsm = UART_FSM_CHECKSUM;
        break;
    case UART_FSM_CHECKSUM:
        if (rx_pos == MSG_SIZE - 1) uart_fsm = UART_FSM_END;
        break;
    case UART_FSM_END:
        uart_fsm = UART_FSM_IDLE;
        break;
    default:
        uart_fsm = UART_FSM_IDLE;
        break;
    }
}

/* 체크섬 계산 */
static uint8_t uart_co2_calc_checksum(const char *pkt)
{
    uint8_t sum = 0;
    for (int i = 1; i < 8; i++) {
        sum += pkt[i];
    }
    return (uint8_t)(0xFF - sum + 1);
}

/* BCD 형식 → 10진 변환 */
static uint8_t uart_co2_bcd_to_dec(uint8_t bcd)
{
    return (bcd / 16) * 10 + (bcd % 16);
}

void uart_co2_serial_callback(const struct device *dev, void *user_data)
{
    uint8_t byte;
    int read;

    if (!uart_irq_update(uart_dev) || !uart_irq_rx_ready(uart_dev)) {
        return;
    }

    /* FIFO에서 바이트 단위로 읽어들임 */
    while ((read = uart_fifo_read(uart_dev, &byte, 1)) == 1) {
        if (uart_fsm == UART_FSM_IDLE) {
            rx_pos = 0;
        }
        uart_co2_fsm_update(byte);

        if (rx_pos < MSG_SIZE) {
            rx_buf[rx_pos++] = byte;
        }
    }

    /* 패킷 완성되면 메시지 큐에 복사 */
    if (uart_fsm == UART_FSM_END && rx_pos == MSG_SIZE) {
        uint8_t cs = uart_co2_calc_checksum(rx_buf);
        if (cs == (uint8_t)rx_buf[8]) {
            /* 큐에 성공 패킷 전달 (타임아웃 적용 가능) */
            k_msgq_put(&uart_co2_msgq, rx_buf, K_MSEC(RECEIVE_TIMEOUT));
        }
        /* 버퍼 초기화 */
        memset(rx_buf, 0, MSG_SIZE);
        uart_fsm = UART_FSM_IDLE;
    }
}

void uart_co2_send_request(void)
{
    const uint8_t cmd[MSG_SIZE] = {0xFF,0x01,0x86,0,0,0,0,0,0x79};
    for (int i = 0; i < MSG_SIZE; i++) {
        uart_poll_out(uart_dev, cmd[i]);
    }
}

int uart_co2_get_ppm(const char *packet)
{
    if (uart_co2_calc_checksum(packet) != (uint8_t)packet[8]) {
        return -1;
    }
    uint8_t high = uart_co2_bcd_to_dec((uint8_t)packet[2]);
    uint8_t low  = uart_co2_bcd_to_dec((uint8_t)packet[3]);
    return high * CO2_MULTIPLIER + low;
}