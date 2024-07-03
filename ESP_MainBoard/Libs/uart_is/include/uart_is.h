#ifndef __lINUART_H
#define __lINUART_H
#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "main.h"
#include "driver/uart.h"

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#endif
void uart_init();
