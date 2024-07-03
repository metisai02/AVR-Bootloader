#ifndef __lINUART_H
#define __lINUART_H
#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "main.h"
#include "driver/uart.h"
#define BUTTON_1 35
#define BUTTON_2 34
#define BUTTON_3 4

#define RELAY_1 12
#define RELAY_2 14
#define RELAY_3 27

#define LED_1 2
#define LED_2 15
#define LED_3 13

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#endif
void uart_init();
