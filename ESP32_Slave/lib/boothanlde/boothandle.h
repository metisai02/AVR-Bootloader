#ifndef BOOTHANDLE_H
#define BOOTHANDLE_H

#include <string>
#include <FS.h>
#include "HardwareSerial.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "main.h"
#include <vector>
struct Flags {
  bool cmdEarseOldFirmware;
  bool cmdReadOldFirmware;
  bool updateReadOldFirmware;
  bool newFirmwareUpdate;
  bool newBootloaderUpdate;
};
typedef struct __attribute__((packed))
{
    uint8_t start;
    uint8_t length;
    uint8_t command;
} boot_common_t;
typedef struct __attribute__((packed))
{
    uint16_t address;
    uint16_t crc;
} boot_erase_t;
typedef struct __attribute__((packed))
{
    boot_common_t common;
    boot_erase_t b_erase;
} boot_erase_frame_t;

class BootHandle
{
public:
    BootHandle(fs::FS &filesystem, const std::string &firmwarePath, const std::string &bootloaderPath, std::string pathOldFirmware);
    bool updateFirmware();
    bool readFirmware();
    bool eraseFirmware();
    bool updateBootloader();

    SemaphoreHandle_t xSerialSemaphoreUpdate;
    SemaphoreHandle_t xSerialSemaphoreCmd;
    SemaphoreHandle_t xMutexRTDB;

private:
    bool sendAndReceiveAck(uint8_t *pageIntUpdateFirmware, uint16_t sendBufferLength, size_t transmitCount);
    bool checkESPHeader(uint8_t *buffer, const char *charPage, uint16_t length);
    void sendToAVR(uint8_t *data, size_t length);
    // uint16_t crc16(const uint8_t *data, uint16_t length);
    static void updateFirmwareTask(void *param);
    static void CmdBootTask(void *param);
    static void RTDBTask(void *param);
    bool storageFrame2Txt(const std::string &pathOldFirmware, uint8_t *data);
    fs::FS &fs;
    std::string pathFirmware;
    std::string pathBootloader;
    std::string pathOldFirmware;
    bool flagFi;
    bool flagRe;
    bool flagEr;
    TaskHandle_t updateFirmwareHandle;
    TaskHandle_t CmdBootHandle;
};

#endif // BOOTHANDLE_H
