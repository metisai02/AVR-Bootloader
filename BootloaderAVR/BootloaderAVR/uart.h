/*
 * uart.h
 *
 * Created: 4/9/2024 9:31:47 PM
 *  Author: ADMIN
 */

#ifndef UART_H_
#define UART_H_

#include <avr/boot.h>
#include <avr/io.h>
#include "timer.h"

#define READ_FUSE (_BV(__SPM_ENABLE) | _BV(BLBSET))
#define CPU_N 16000000UL
#define MAX_TIME_OUT CPU_N >> 1
typedef enum
{
    AVR_OK = 0,
    AVR_ERROR,
    AVR_TIMEOUT 
} AVR_status_t;

uint8_t is_external_clock();
uint8_t is_CKDIV8();
uint8_t UART_init(uint16_t baudrate);
AVR_status_t UART_Transmit(const uint8_t *data, uint16_t size, uint32_t timeout);
AVR_status_t UART_Receive(uint8_t *data, uint16_t size, uint32_t timeout);
uint8_t read_low_fuse();

#endif /* UART_H_ */