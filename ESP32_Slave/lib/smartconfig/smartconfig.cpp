#include "smartconfig.h"

SmartConfig::SmartConfig()
{
    eventGroup = xEventGroupCreate();
    memset(&wifiConfig, 0, sizeof(wifiConfig));
}

void SmartConfig::init()
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &SmartConfig::eventHandler, this);

    esp_wifi_start();
}

void SmartConfig::start()
{
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &SmartConfig::eventHandler, this);
    esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &SmartConfig::eventHandler, this);
    xTaskCreate(&SmartConfig::smartConfigTask, "smartConfigTask", 4096, this, 5, &smartConfigHandle);
}

void SmartConfig::smartConfigTask(void *param)
{
    SmartConfig *self = static_cast<SmartConfig *>(param);
    esp_smartconfig_set_type(SC_TYPE_ESPTOUCH);
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    esp_smartconfig_start(&cfg);
    Serial.println("SMARTCONFIG START CONFIG");
    vTaskDelete(NULL);
    vTaskDelay(10);
}

void SmartConfig::eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    SmartConfig *self = static_cast<SmartConfig *>(arg);

    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
            Serial.println("WIFI_EVENT_STA_START");
            self->start();
            break;
        case WIFI_EVENT_STA_CONNECTED:
            Serial.println("WIFI_EVENT_STA_CONNECTED");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            xEventGroupClearBits(self->eventGroup, WIFI_CONNECTED_BIT);
            Serial.println("WIFI_EVENT_STA_DISCONNECTED");
            break;
        default:
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        if (event_id == IP_EVENT_STA_GOT_IP)
        {
            Serial.println("IP_EVENT_STA_GOT_IP");
            xEventGroupSetBits(self->eventGroup, WIFI_CONNECTED_BIT);
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            String ip = String((event->ip_info.ip.addr >> 0) & 0xFF) + "." +
                        String((event->ip_info.ip.addr >> 8) & 0xFF) + "." +
                        String((event->ip_info.ip.addr >> 16) & 0xFF) + "." +
                        String((event->ip_info.ip.addr >> 24) & 0xFF);
            Serial.print("IP: ");
            Serial.println(ip);
        }
    }
    else if (event_base == SC_EVENT)
    {
        switch (event_id)
        {
        case SC_EVENT_SCAN_DONE:
            Serial.println("SC_EVENT_SCAN_DONE");
            break;
        case SC_EVENT_FOUND_CHANNEL:
            Serial.println("SC_EVENT_FOUND_CHANNEL");
            break;
        case SC_EVENT_GOT_SSID_PSWD:
        {
            Serial.println("SC_EVENT_GOT_SSID_PSWD");
            smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
            memcpy(self->wifiConfig.sta.ssid, evt->ssid, sizeof(self->wifiConfig.sta.ssid));
            memcpy(self->wifiConfig.sta.password, evt->password, sizeof(self->wifiConfig.sta.password));
            Serial.print("SSID: ");
            Serial.println((char *)evt->ssid);
            Serial.print("PASSWORD: ");
            Serial.println((char *)evt->password);
            esp_wifi_disconnect();
            esp_wifi_set_config(WIFI_IF_STA, &self->wifiConfig);
            esp_wifi_connect();
            break;
        }
        case SC_EVENT_SEND_ACK_DONE:
            Serial.println("SC_EVENT_SEND_ACK_DONE");
            esp_smartconfig_stop();
            break;
        default:
            break;
        }
    }
}
