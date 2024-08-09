#include "main.h"
#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "filehandle.h"
#include "mathesp.h"
#include "smartconfig.h"
#include "driver/gpio.h"

#include "freertos/timers.h"
#include "freertos/task.h"

//
#include <TFT_eSPI.h>
#include <SPI.h>
#include <lvgl.h>
#include "ui.h"

#include "AiEsp32RotaryEncoder.h"
#include "boothandle.h"

#define TIME_RTDB_CMD 2
#define TIME_RTDB 10
#define pdSECOND pdMS_TO_TICKS(1000)
#define CONFIG_RESET_GPIO 15

/******************* */

#define ROTARY_ENCODER_A_PIN 13
#define ROTARY_ENCODER_B_PIN 15
#define ROTARY_ENCODER_BUTTON_PIN 33

#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_VCC_PIN -1
/***************************** */

bool taskCompleted = false;
bool downloadComplete1 = false;
bool downloadComplete2 = false;
unsigned long sendDataPrevMillis = 0;

BootHandle bootHandle(LittleFS, "/firmware/firmware.txt", "/boot/firmware.txt", "/oldRead/firmware.txt");

// Variable to config firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
// variable to config RTOS section
SmartConfig smartConfig;
TimerHandle_t xTimerRTDB_20s_handle;
TimerHandle_t xTimerRTDB_CMD;
StaticTimer_t xTimerRTDB_20s;

// init TFT
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */
// AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS, false);

/******************************************* */
#define TIMER_TASK_STACK_SIZE 2048

// call back function handler
extern void fcsDownloadCallback1(FCS_DownloadStatusInfo info);
extern void fcsDownloadCallback2(FCS_DownloadStatusInfo info);
extern void vTimerCallbackUpdate(TimerHandle_t xTimer);
extern void vTimerCallbackCmd(TimerHandle_t xTimer);
void tftLgvlTask(void *pvParameter);
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX = 0, touchY = 0;

    bool touched = false; // tft.getTouch( &touchX, &touchY, 600 );

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print("Data x ");
        Serial.println(touchX);

        Serial.print("Data y ");
        Serial.println(touchY);
    }
}
void IRAM_ATTR readEncoderISR()
{
    // rotaryEncoder.readEncoder_ISR();
}
void main_thesis()
{
    /**************************Init Rotary*********** */
    // rotaryEncoder.begin();
    //  rotaryEncoder.setup(readEncoderISR);
    //  set boundaries and if values should cycle or not
    //  in this example we will set possible values between 0 and 1000;
    bool circleValues = false;
    // rotaryEncoder.setBoundaries(0, 1000, circleValues);
    // rotaryEncoder.disableAcceleration(); // acceleration is now enabled by default - disable if you dont need it
    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

    tft.begin(); /* TFT init */
    // tft.setRotation(3); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();
    xTaskCreate(tftLgvlTask, "tftLgvlTask", 4096, NULL, 5, NULL);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");

    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {

        Serial.print(".");
        delay(200);
    }
    Serial.println();
    Serial.println(WiFi.localIP());
    Serial.println();
    //  Create a timer checking ever 20s from Realtime data base
    //  to get flag update firmware
    xTimerRTDB_20s_handle = xTimerCreate(
        "TimerRTDB",
        pdSECOND * TIME_RTDB,
        pdTRUE,
        (void *)0,
        vTimerCallbackUpdate);
    xTimerRTDB_CMD = xTimerCreate(
        "TimerCMD",
        pdSECOND * TIME_RTDB_CMD,
        pdTRUE,
        (void *)0,
        vTimerCallbackCmd);

    // Mount LittleFS
    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.println("LittleFS Mount Failed");
        return;
    }
    else
    {
        Serial.println("LittleFS Mount OK!!");
    }

    // Create directories and write files to LittleFS
    FileHanle::createDir(LittleFS, "/boot");
    FileHanle::writeFile(LittleFS, "/boot/firmware.txt", "Hello2");
    FileHanle::listDir(LittleFS, "/", 1); // Check directory listing

    FileHanle::createDir(LittleFS, "/firmware");
    FileHanle::writeFile(LittleFS, "/firmware/firmware.txt", "Hello2");

    FileHanle::listDir(LittleFS, "/", 1); // Check directory listing

    FileHanle::createDir(LittleFS, "/oldRead");
    FileHanle::writeFile(LittleFS, "/oldRead/firmware.txt", "Hello2");

    delay(200);

    // Initialize Firebase configuration
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.token_status_callback = tokenStatusCallback;

    // Set buffer sizes
    fbdo.setBSSLBufferSize(4096, 1024);
    config.fcs.download_buffer_size = 2048;
    config.database_url = DATABASE_URL;
    // Begin Firebase with configured settings
    Firebase.begin(&config, &auth);

    while (1)
    {
        // Check if Firebase is ready and tasks haven't completed
        if (Firebase.ready() && !taskCompleted)
        {
            if (xTimerStart(xTimerRTDB_20s_handle, 0) != pdPASS || xTimerStart(xTimerRTDB_CMD, 0) != pdPASS)
            {
                Serial.println("Could not start timer...");
                delay(300);
            }
            else
            {
                taskCompleted = true;
                // vTaskDelete(NULL);
            }
        }
        delay(500);
    }
}
void tftLgvlTask(void *pvParameter)
{
    // Task code goes here
    while (1)
    {
        // Do something
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5)); // Delay 1 second
        // Serial.println("Loop TFT");
    }
}
