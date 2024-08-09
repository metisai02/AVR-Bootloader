#ifndef SMARTCONFIG_H
#define SMARTCONFIG_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_smartconfig.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include <string.h>
#include "Arduino.h"

#define WIFI_CONNECTED_BIT (1 << 0)
class SmartConfig
{
public:
    EventGroupHandle_t eventGroup;
    SmartConfig();
    void init();
    void start();

private:
    static void eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    static void smartConfigTask(void *param);

    wifi_config_t wifiConfig;
    TaskHandle_t smartConfigHandle;
};

#endif // SMARTCONFIG_H
