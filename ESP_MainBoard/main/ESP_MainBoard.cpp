#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "jsoncpp/value.h"
#include "jsoncpp/json.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define WIFI_SSID "1627"
#define WIFI_PASSWORD "16271627"
#define API_KEY "AIzaSyDvKWztVgwZCJwx52fc4EufKnomrq1jSrs"
#define USER_EMAIL "lequynga12345@gmail.com"
#define USER_PASSWORD "lequynga1"
#define STORAGE_BUCKET_ID "thesis-esp.appspot.com"

static const char *TAG = "FirebaseStorage";

// Function to handle HTTP events
static const char *TAG = "FirebaseAuth";

// Event handler for HTTP events
esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (evt->data_len > 0) {
                ESP_LOGI(TAG, "Data=%.*s", evt->data_len, (char *)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

esp_err_t firebase_authenticate() {
    esp_http_client_config_t config = {
        .url = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=" API_KEY,
        .event_handler = http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    std::string post_data = "{\"email\":\"" + std::string(USER_EMAIL) + "\",\"password\":\"" + std::string(USER_PASSWORD) + "\",\"returnSecureToken\":true}";
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data.c_str(), post_data.length());

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP POST request failed: %d", err);
    }

    esp_http_client_cleanup(client);
    return err;
}

extern "C" void app_main(void)
{

    // Wait for WiFi connection
    while (true)
    {
        if (esp_wifi_connect() == ESP_OK)
        {
            ESP_LOGI(TAG, "WiFi connected");
            break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    std::string id_token;
    firebase_authenticate()
}
