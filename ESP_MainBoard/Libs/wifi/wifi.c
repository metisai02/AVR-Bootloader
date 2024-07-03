#include "wifi.h"
#include "esp_log.h"
#include "esp_smartconfig.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "main.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <string.h>

#define DEFAULT_SCAN_LIST_SIZE 5
#define WIFI_CONNECTED_BIT (1 << 0)

TaskHandle_t smartConfigHandle;
static void smartConfig_handler(void *para);
extern EventGroupHandle_t event_group;
extern SSD1306_t dev;
static void wifi_event_handler(void *event_handler_arg,
                               esp_event_base_t event_base, int32_t event_id,
                               void *event_data);
wifi_config_t wifi_configuration = {
    .sta = {.ssid = "PIF_CLUB", .password = "chinsochin"

    }};
void wifi_init()
{
    // pharse 1
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init));

    // pharse 2
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    const uint8_t protocol = WIFI_PROTOCOL_11B;
    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, protocol));

    // pharse 3

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler,
                               NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler,
                               NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler,
                               NULL));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration));

    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}
static void wifi_event_handler(void *event_handler_arg,
                               esp_event_base_t event_base, int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
            ESP_LOGI("WIFI_EVENT", "WIFI_EVENT_STA_START");
            ESP_ERROR_CHECK(esp_wifi_connect());
            xTaskCreate(smartConfig_handler, "smartConfig_handler", 1024 * 3, NULL,
                        11, &smartConfigHandle);
            /* code */
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI("WIFI_EVENT", "WIFI_EVENT_STA_CONNECTED");
            /* code */
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_ERROR_CHECK(esp_wifi_connect());
            xEventGroupClearBits(event_group, WIFI_CONNECTED_BIT);
            ESP_LOGI("WIFI_EVENT", "WIFI_EVENT_STA_DISCONNECTED");
            /* code */
            break;
        case WIFI_EVENT_STA_BEACON_TIMEOUT:
            ESP_LOGI("WIFI_EVENT", "WIFI_EVENT_STA_BEACON_TIMEOUT");
            /* code */
            break;
        case WIFI_EVENT_STA_STOP:
            ESP_LOGI("WIFI_EVENT", "WIFI_EVENT_STA_STOP");
            /* code */
            break;

        default:
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI("IP_EVENT", "IP_EVENT_STA_GOT_IP");
            xEventGroupSetBits(event_group, WIFI_CONNECTED_BIT);

            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

            char ip_address_str[16]; // Chuỗi để lưu địa chỉ IP
            // Chuyển địa chỉ IP từ ip4_addr_t sang chuỗi
            inet_ntoa_r(event->ip_info.ip, ip_address_str, sizeof(ip_address_str));
            // Hiển thị địa chỉ IP lên màn hình OLED
            ip_address_str[16] = '\0';
            ESP_LOGI("IP_EVENT", "IP: %s", ip_address_str);
            break;
        default:
            break;
        }
    }
    else if (event_base == SC_EVENT)
    {
        switch (event_id)
        {
        case SC_EVENT_SCAN_DONE:
            ESP_LOGI("SC_EVENT", "SC_EVENT_SCAN_DONE");
            break;
        case SC_EVENT_FOUND_CHANNEL:
            ESP_LOGI("SC_EVENT", "SC_EVENT_FOUND_CHANNEL");
            break;
        case SC_EVENT_GOT_SSID_PSWD:
            uint8_t ssid[33] = {0};
            uint8_t password[65] = {0};
            smartconfig_event_got_ssid_pswd_t *evt =
                (smartconfig_event_got_ssid_pswd_t *)event_data;
            bzero(&wifi_configuration, sizeof(wifi_config_t));
            memcpy(wifi_configuration.sta.ssid, evt->ssid,
                   sizeof(wifi_configuration.sta.ssid));
            memcpy(wifi_configuration.sta.password, evt->password,
                   sizeof(wifi_configuration.sta.password));
            memcpy(ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(password, evt->password, sizeof(evt->password));
            ESP_LOGI("SC_EVENT", "SSID:%s", ssid);
            ESP_LOGI("SC_EVENT", "PASSWORD:%s", password);
            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration));
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        case SC_EVENT_SEND_ACK_DONE:
            break;

        default:
            break;
        }
    }
}
static void smartConfig_handler(void *para)
{
    esp_smartconfig_set_type(SC_TYPE_ESPTOUCH);
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
    ESP_LOGI("SMARTCONFIG", "SMARTCONFIG START CONFIG");
    vTaskDelete(NULL);
}
