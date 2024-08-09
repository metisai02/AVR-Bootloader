#include "boothandle.h"
#include "mathesp.h"
#include "Firebase_ESP_Client.h"
#include "esp_task_wdt.h"
#include "ispProgram.h"
extern HardwareSerial Serial2;
extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;
extern void fcsDownloadCallback1(FCS_DownloadStatusInfo info);
extern void fcsUploadCallback(FCS_UploadStatusInfo info);
#define ACK 0x22
#define READ_ALL 0x67
#define READ_PAGE 0x68
#define READ_ADDRESS 0x69
#define WRITE 0x96
#define ERASE 0x77
#define START 0x55

#define PAGE_AVR_APP 200

BootHandle::BootHandle(fs::FS &filesystem, const std::string &firmwarePath, const std::string &bootloaderPath, std::string oldFirmwarePath)
    : fs(filesystem), pathFirmware(firmwarePath), pathBootloader(bootloaderPath), pathOldFirmware(oldFirmwarePath), flagFi(false), flagRe(false), flagEr(false)
{
    Serial.begin(115200);
    // Serial2.begin(38400);
    Serial.println("Constructor of BootHandle");
    xSerialSemaphoreUpdate = xSemaphoreCreateBinary();
    xSerialSemaphoreCmd = xSemaphoreCreateBinary();
    xMutexRTDB = xSemaphoreCreateMutex();
    if (xSerialSemaphoreUpdate == NULL || xSerialSemaphoreCmd == NULL || xMutexRTDB == NULL)
    {
        Serial.println("Could not create semaphore for updating");
    }
    else
    {
        if (xTaskCreate(&BootHandle::updateFirmwareTask, "updateFirmwareTask", 6000, this, 10, &updateFirmwareHandle) != pdTRUE)
        {
            Serial.println("Could not create task");
        }
        else
        {
            Serial.println("Task created successfully");
        }
        if (xTaskCreate(&BootHandle::CmdBootTask, "CmdBootTask", 6000, this, 10, &CmdBootHandle) != pdTRUE)
        {
            Serial.println("Could not create task");
        }
        else
        {
            Serial.println("Task created successfully");
        }
    }
}

static uint16_t crc16(const uint8_t *data, uint16_t length)
{
    uint16_t poly = 0x8005;
    uint16_t crc = 0x0000;

    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i] << 8;
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ poly;
            }
            else
            {
                crc <<= 1;
            }
        }
        crc &= 0xFFFF;
    }
    return crc;
}

bool BootHandle::checkESPHeader(uint8_t *buffer, const char *charPage, uint16_t length)
{
    MathESP::stringToHex(buffer, charPage, length);
    if (buffer[0] != 0xaa || buffer[1] != 0xaa)
        return false;

    uint16_t receivedCRC = (buffer[length / 2 - 2] << 8) | buffer[length / 2 - 1];
    uint16_t calculatedCRC = crc16(buffer, length / 2 - 2);

    Serial.printf("Length of data: %u\n", length);
    Serial.printf("Received CRC: 0x%04X\n", receivedCRC);
    Serial.printf("Calculated CRC: 0x%04X\n", calculatedCRC);

    return receivedCRC == calculatedCRC;
}
bool BootHandle::updateBootloader()
{

    bool endUpdate = false;
    char pageCharUpdateFirmware[300] = {0};
    uint8_t pageIntUpdateFirmware[200] = {0};
    uint16_t sendBufferLength = 0;
    File file = this->fs.open(this->pathBootloader.c_str());
    Serial.println("Send to AVR ...................");
    Serial2.begin(38400);
    uint8_t count = 0;
    while (file.available() && !endUpdate)
    {
        IspProgram isp(SPI_BUARD);
        size_t dataLength = file.readBytesUntil('\n', pageCharUpdateFirmware, sizeof(pageCharUpdateFirmware));
        if (dataLength <= 0)
            break;
        sendBufferLength = dataLength / 2;
        if (checkESPHeader(pageIntUpdateFirmware, pageCharUpdateFirmware, dataLength))
        {
            // program isp
            isp.ispStartPro();
            uint8_t t_fuse = 0xff;
            t_fuse &= ~(1 << FUSE_HIGH_BOOTSZ0);
            t_fuse &= ~(1 << FUSE_HIGH_BOOTSZ1);
            t_fuse &= ~(1 << FUSE_HIGH_SPIEN);
            isp.ispWriteFuseHigh(t_fuse);
            t_fuse = 0xff;
            t_fuse &= ~(1 << FUSE_LOW_CKDIV8);
            t_fuse &= ~(1 << FUSE_LOW_CKSEL0);
            t_fuse &= ~(1 << FUSE_LOW_CKSEL2);
            t_fuse &= ~(1 << FUSE_LOW_CKSEL3);
            isp.ispWriteFuseLow(t_fuse);
            isp.ispErasePages();
            // find address from frame
            uint16_t addr;
            // send data and address
            isp.ispWritePage(addr, pageIntUpdateFirmware, sizeof(pageIntUpdateFirmware));

            // we need to remember SPI set clok for TFT and Other
        }
        else
        {
            Serial.println("...............................");
            return false;
        }
    }
    file.close();
    Serial.println("\nClose this shit oke\n");
    return true;
}
bool BootHandle::updateFirmware()
{
    bool endUpdate = false;
    Serial.println("Update firmware ....");
    char pageCharUpdateFirmware[300] = {0};
    uint8_t pageIntUpdateFirmware[200] = {0};
    uint16_t sendBufferLength = 0;
    File file = this->fs.open(this->pathFirmware.c_str());
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for updating firmware");
        return false;
    }

    while (file.available())
    {
        size_t dataLength = file.readBytesUntil('\n', pageCharUpdateFirmware, sizeof(pageCharUpdateFirmware));
        Serial.printf("dataLength %d\n", dataLength);
        if (dataLength <= 0)
            break;

        sendBufferLength = dataLength / 2;
        if (checkESPHeader(pageIntUpdateFirmware, pageCharUpdateFirmware, dataLength))
        {
            Serial.println("valid ESP32 header or CRC.");
        }
        else
        {
            Serial.println("Invalid ESP32 header or CRC.");
            file.close();
            return false;
        }
    }
    file.close();
    file = this->fs.open(this->pathFirmware.c_str());
    Serial.println("Send to AVR ...................");
    Serial2.begin(38400);
    uint8_t count = 0;
    while (file.available() && !endUpdate)
    {
        size_t dataLength = file.readBytesUntil('\n', pageCharUpdateFirmware, sizeof(pageCharUpdateFirmware));
        if (dataLength <= 0)
            break;
        sendBufferLength = dataLength / 2;
        if (checkESPHeader(pageIntUpdateFirmware, pageCharUpdateFirmware, dataLength))
        {
            count++;
            Serial.printf("\nCount frame is : %d\n", count);
            Serial.printf("Data to send: \n");
            for (size_t i = 0; i < sendBufferLength; i++)
            {
                Serial.printf("%02X ", pageIntUpdateFirmware[i]);
            }
            Serial.println();
            if (sendAndReceiveAck(&pageIntUpdateFirmware[2], (sendBufferLength - 4), 10) != true)
            {

                Serial.println("Check your connection, please.....");
                return false;
            }
        }
        else
        {
            Serial.println("...............................");
            return false;
        }
    }
    file.close();
    Serial.println("\nClose this shit oke\n");
    return true;
}

bool ackHandle(uint8_t *data, uint16_t length, uint8_t restransmit)
{
    bool isAckFrame = false;
    bool isInFrame = false;
    uint8_t cntRetransmit = 0;

    while (!isAckFrame)
    {
        Serial2.write(data, length);
        uint32_t timeStart = millis();
        cntRetransmit++;

        while (millis() - timeStart < 10)
        {
            uint8_t nByte = Serial2.available();
            if (nByte > 0)
            {
                uint8_t byte = Serial2.read();
                Serial.printf("\n Byte of uart: %d\n", nByte);
                Serial.print(byte);
                if (!isInFrame)
                {
                    if (byte == 0x55)
                    {
                        isInFrame = true;
                        Serial.println("\n Fram is oke with 85\n");
                    }
                }
                else
                {
                    if (byte == ACK)
                    {
                        isAckFrame = true;
                        Serial.println("\nOne frame is oke... buddy");
                        while (Serial2.available() > 0)
                        {
                            Serial2.read();
                        }
                        return true;
                    }
                    else
                    {
                        isInFrame = false;
                    }
                }
            }
        }

        if (cntRetransmit >= restransmit)
            return false;
        delay(10);
    }

    return false;
}

bool BootHandle::sendAndReceiveAck(uint8_t *pageIntUpdateFirmware, uint16_t sendBufferLength, size_t transmitCount = 10)
{
    return ackHandle(pageIntUpdateFirmware, sendBufferLength, transmitCount) ? true : false;
}

void BootHandle::updateFirmwareTask(void *param)
{
    BootHandle *self = static_cast<BootHandle *>(param);
    Flags flags;
    while (1)
    {
        if (xSemaphoreTake(self->xSerialSemaphoreUpdate, portMAX_DELAY))
        {
            Serial.println("IN update cmd");

            // we take mutex for this sitsuation
            if (xSemaphoreTake(self->xMutexRTDB, 10000))
            {
                // Firebase.RTDB.getBool(&fbdo, "/flag/newFirmwareUpdate");

                if (Firebase.RTDB.get(&fbdo, "/flag"))
                {
                    if (fbdo.dataType() == "json")
                    {
                        FirebaseJson &json = fbdo.jsonObject();
                        FirebaseJsonData jsonData;

                        if (json.get(jsonData, "newFirmwareUpdate") && jsonData.type == "bool")
                        {
                            flags.newFirmwareUpdate = jsonData.boolValue;
                        }
                        if (json.get(jsonData, "newBootloaderUpdate") && jsonData.type == "bool")
                        {
                            flags.newBootloaderUpdate = jsonData.boolValue;
                        }
                    }
                }
                else
                {
                    Serial.println("Failed to get data");
                    Serial.println(fbdo.errorReason());
                }
                xSemaphoreGive(self->xMutexRTDB);
                if (flags.newFirmwareUpdate)
                {
                    Serial.println("We have new version firmware....");
                    Serial.println("Updating new firmware.................");
                    Serial.println("\nDownload file...\n");
                    delay(1000);
                    if (!Firebase.Storage.download(&fbdo, STORAGE_BUCKET_ID, "storage/firmware.txt", "/firmware/firmware.txt", mem_storage_type_flash, fcsDownloadCallback1))
                    {
                        Serial.println(fbdo.errorReason());
                    }
                    else
                    {
                        if (self->updateFirmware())
                        {
                            Serial.println("Download complete.");
                            Firebase.RTDB.setBool(&fbdo, "/flag/newFirmwareUpdate", false);
                        }
                    }
                }
                else
                {
                    Serial.println("We have no new version firmware....");
                }
                if (flags.newBootloaderUpdate)
                {
                    Serial.println("We have new bootloader ....");
                    Serial.println("Updating new bootloader.................");
                    Serial.println("\nDownload file...\n");
                    delay(1000);
                    if (!Firebase.Storage.download(&fbdo, STORAGE_BUCKET_ID, "storage/bootloader.txt", "/boot/firmware.txt", mem_storage_type_flash, fcsDownloadCallback1))
                    {
                        Serial.println(fbdo.errorReason());
                    }
                    else
                    {
                        if (self->updateFirmware())
                        {
                            Serial.println("Download complete.");
                            Firebase.RTDB.setBool(&fbdo, "/flag/newBootloaderUpdate", false);
                        }
                    }
                }
                else
                {
                    Serial.println("We have no new version bootloader....");
                }
            }
            else
            {
                Serial.println("Waiting for taking mutex from Update task");
            }
        }
            vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
void BootHandle::CmdBootTask(void *param)
{
    BootHandle *self = static_cast<BootHandle *>(param);
    Flags flags;
    while (1)
    {
        if (xSemaphoreTake(self->xSerialSemaphoreCmd, portMAX_DELAY))
        {
            if (xSemaphoreTake(self->xMutexRTDB, 10000))
            {

                if (Firebase.RTDB.get(&fbdo, "/flag"))
                {
                    if (fbdo.dataType() == "json")
                    {
                        FirebaseJson &json = fbdo.jsonObject();
                        FirebaseJsonData jsonData;

                        if (json.get(jsonData, "cmdReadOldFirmware") && jsonData.type == "bool")
                        {
                            flags.cmdReadOldFirmware = jsonData.boolValue;
                        }
                        if (json.get(jsonData, "cmdEarseOldFirmware") && jsonData.type == "bool")
                        {
                            flags.cmdEarseOldFirmware = jsonData.boolValue;
                        }
                    }
                }
                else
                {
                    Serial.println("Failed to get data");
                    Serial.println(fbdo.errorReason());
                }
                xSemaphoreGive(self->xMutexRTDB);
                if (flags.cmdEarseOldFirmware)
                {
                    Serial.println("We have new earsing command ....");
                    Serial.println("Earse old firmware.................");
                    Serial.println("\n Earsing file.......\n");
                    if (self->eraseFirmware())
                    {
                        Serial.println("\nErase complete.\n");
                        Firebase.RTDB.setBool(&fbdo, "/flag/cmdEarseOldFirmware", false);
                    }
                    else
                    {
                        Serial.println("\n We could not erase firmware\n");
                    }
                }
                else
                {
                    Serial.println("\n We have no erase firmware command\n");
                }
                if (flags.cmdReadOldFirmware)
                {
                    Serial.println("We have new reading command ....");
                    Serial.println("Reading old firmware.................");
                    Serial.println("\n Reading file.......\n");
                    uint32_t now_test = millis();
                    if (self->readFirmware())
                    {
                        Serial.printf("\nTime consum for readata: %d\n", (millis() - now_test));
                        if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID, self->pathOldFirmware.c_str(), mem_storage_type_flash, "storage/OldFirmware.txt", "text/plain", fcsUploadCallback))
                        {
                            Serial.println("File uploaded oke");
                            Firebase.RTDB.setBool(&fbdo, "/flag/cmdReadOldFirmware", false);
                            Firebase.RTDB.setBool(&fbdo, "/flag/updateReadOldFirmware", true);
                        }
                        else
                        {
                            Serial.println("\n We could not upload read firmware\n");
                        }
                    }
                    else
                    {
                        Serial.println("\n We could not read firmware\n");
                    }
                }
                else
                {
                    Serial.println("\n We have no read firmware command\n");
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
bool BootHandle::storageFrame2Txt(const std::string &pathOldFirmware, uint8_t *data)
{
    const uint8_t LENGTH_SIZE = 132;
    const uint8_t ESP32_START_BYTE = 0x88;
    uint8_t wrappedData[LENGTH_SIZE + 3];

    wrappedData[0] = ESP32_START_BYTE;
    memcpy(&wrappedData[1], data, LENGTH_SIZE);
    uint16_t newCRC = crc16(wrappedData, LENGTH_SIZE + 1);

    wrappedData[LENGTH_SIZE + 1] = newCRC >> 8;   // CRC high byte
    wrappedData[LENGTH_SIZE + 2] = newCRC & 0xFF; // CRC low byte

    File file = this->fs.open(pathOldFirmware.c_str(), FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return false;
    }
    for (int i = 0; i < LENGTH_SIZE + 3; ++i)
    {
        char hex[3];
        sprintf(hex, "%02X", wrappedData[i]);
        file.print(hex);
    }
    file.println(); // Dòng mới sau khi viết xong dữ liệu
    file.close();
    Serial.println("Data written to file successfully");
    return true;
}

bool BootHandle::readFirmware()
{
    uint8_t dataReadFrame[7] = {START, 0x00, READ_ALL, 0xFF, 0xFF, 0x00, 0x00};
    uint16_t crc = crc16(dataReadFrame, sizeof(dataReadFrame) - 2);
    dataReadFrame[5] = uint8_t(crc >> 8);
    dataReadFrame[6] = uint8_t(crc);
    Serial.printf("\nCRC of ERAESING: 0x%04X \n", crc);
    bool isInFrame = false;
    const uint8_t LENGTH_SIZE = 132;
    uint8_t dataRead[LENGTH_SIZE] = {0};
    if (ackHandle(dataReadFrame, sizeof(dataReadFrame), 10))
    {
        Serial.println("\nWe received ack from AVR");
        File file = this->fs.open(this->pathOldFirmware.c_str(), FILE_WRITE);
        file.close();
        uint32_t now = millis();
        uint8_t cnt = 0;
        uint8_t page_now = 0;
        while ((millis() - now) < 15000)
        {
            delay(0);
            uint8_t nByte = Serial2.available();
            // Serial.printf("\nNumber of byte is : %d",nByte);
            if (nByte > 0)
            {
                uint8_t byte = Serial2.read();
                if (!isInFrame)
                {
                    if (byte == 0x55)
                    {
                        dataRead[cnt++] = byte;
                        isInFrame = true;
                    }
                }
                else
                {
                    dataRead[cnt++] = byte;
                    if (cnt == LENGTH_SIZE)
                    {
                        if (storageFrame2Txt(this->pathOldFirmware, dataRead) != true)
                            return false;
                        cnt = 0;
                        page_now++;
                        isInFrame = false;
                    }
                }
            }
            if (page_now == PAGE_AVR_APP)
                return true;
        }
        Serial.printf("\nWe just have %d pages, it not enough\n", page_now);
        return false;
    }
    return false;
}

bool BootHandle::eraseFirmware()
{
    uint8_t dataReadFrame[7] = {START, 0x00, ERASE, 0xFF, 0xFF, 0x00, 0x00};
    uint16_t crc = crc16(dataReadFrame, sizeof(dataReadFrame) - 2);
    dataReadFrame[5] = uint8_t(crc >> 8);
    dataReadFrame[6] = uint8_t(crc);
    return ackHandle(dataReadFrame, sizeof(dataReadFrame), 20);
}