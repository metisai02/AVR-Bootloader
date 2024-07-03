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
    xTaskCreate(UartTaskHanlder, "task uart handler", 2094, NULL, 10,
                &xtask_uart_handle);
}

static void UartTaskHanlder(void *para)
{
    uint32_t gpio_pin = 0;
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);

    char *data = (char *)malloc(RX_BUF_SIZE + 1); // Allocated outside the loop

    while (1)
    {
        const int rxBytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 70 / portTICK_PERIOD_MS);

        if (rxBytes > 0)
        {
            control_t temp;
            data[rxBytes] = '\0';
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            temp.is_uart = true;
            if (strcmp(data, "RELAY1_ON") == 0)
            {
                temp.relay = RELAY_1;
                temp.state = ON;
                xQueueSend(xQueue_gpio, &temp, 10);
            }
            else if (strcmp(data, "RELAY1_OFF") == 0)
            {
                temp.relay = RELAY_1;
                temp.state = OFF;
                xQueueSend(xQueue_gpio, &temp, 10);
            }
            else if (strcmp(data, "RELAY2_ON") == 0)
            {
                temp.relay = RELAY_2;
                temp.state = ON;
                xQueueSend(xQueue_gpio, &temp, 10);
            }
            else if (strcmp(data, "RELAY2_OFF") == 0)
            {
                temp.relay = RELAY_2;
                temp.state = OFF;
                xQueueSend(xQueue_gpio, &temp, 10);
            }
            else if (strcmp(data, "RELAY3_ON") == 0)
            {
                temp.relay = RELAY_3;
                temp.state = ON;
                xQueueSend(xQueue_gpio, &temp, 10);
            }
            else if (strcmp(data, "RELAY3_OFF") == 0)
            {
                temp.relay = RELAY_3;
                temp.state = OFF;
                xQueueSend(xQueue_gpio, &temp, 10);
            }
        }

        vTaskDelay(10);
    }

    free(data); // Deallocate data when done
}
