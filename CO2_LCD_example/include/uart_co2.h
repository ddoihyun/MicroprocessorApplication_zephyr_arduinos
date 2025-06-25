// MH-Z14A sensor

#ifndef UART_CO2_H
#define UART_CO2_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 수신 타임아웃(ms) */
#define RECEIVE_TIMEOUT 1000
/* 패킷 크기 */
#define MSG_SIZE 9
/* CO₂ 농도 계산용 상수 */
#define CO2_MULTIPLIER 256

/* 메시지 큐 선언 (소스 파일에서 정의) */
extern struct k_msgq uart_co2_msgq;

/**
 * @brief UART 인터럽트 콜백 함수
 * @param dev      UART 디바이스 포인터
 * @param user_data  사용자 데이터 (unused)
 */
void uart_co2_serial_callback(const struct device *dev, void *user_data);

/**
 * @brief CO₂ 측정 명령 전송
 */
void uart_co2_send_request(void);

/**
 * @brief 패킷에서 CO₂ 농도(ppm) 계산
 * @param packet  수신된 9바이트 패킷
 * @return 체크섬 오류 시 -1, 정상 시 ppm 값
 */
int uart_co2_get_ppm(const char *packet);

#ifdef __cplusplus
}
#endif

#endif /* UART_CO2_H */
