#include "uart_is.h"
#include <stdio.h>
#include <string.h>
#define ON true
#define OFF false
static const int RX_BUF_SIZE = 1024;
extern QueueHandle_t xQueue_gpio;
TaskHandle_t xtask_uart_handle = NULL;

static void UartTaskHanlder(void *para);

void uart_init()
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    // xTaskCreate(UartTaskHanlder, "task uart handler", 2094, NULL, 10,
    //             &xtask_uart_handle);
}