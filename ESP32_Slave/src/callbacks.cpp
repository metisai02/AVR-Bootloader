
#include <Firebase_ESP_Client.h>
#include "boothandle.h"
extern bool downloadComplete1;
extern bool downloadComplete2;
// Khởi tạo đối tượng BootHandle
extern BootHandle bootHandle;
#include "main.h"
void fcsDownloadCallback1(FCS_DownloadStatusInfo info)
{
    if (info.status == firebase_fcs_download_status_init)
    {
        Serial.printf("Downloading file 1 %s (%d) to %s\n", info.remoteFileName.c_str(), info.fileSize, info.localFileName.c_str());
    }
    else if (info.status == firebase_fcs_download_status_download)
    {
        Serial.printf("Downloaded 1 %d%s, Elapsed time %d ms\n", (int)info.progress, "%", info.elapsedTime);
    }
    else if (info.status == firebase_fcs_download_status_complete)
    {
        Serial.println("Download completed 1 \n");
        downloadComplete1 = true;
    }
    else if (info.status == firebase_fcs_download_status_error)
    {
        Serial.printf("Download failed 1, %s\n", info.errorMsg.c_str());
    }
}
void fcsDownloadCallback2(FCS_DownloadStatusInfo info)
{
    if (info.status == firebase_fcs_download_status_init)
    {
        Serial.printf("Downloading file 2 %s (%d) to %s\n", info.remoteFileName.c_str(), info.fileSize, info.localFileName.c_str());
    }
    else if (info.status == firebase_fcs_download_status_download)
    {
        Serial.printf("Downloaded 2  %d%s, Elapsed time %d ms\n", (int)info.progress, "%", info.elapsedTime);
    }
    else if (info.status == firebase_fcs_download_status_complete)
    {
        Serial.println("Download completed 2\n");
        downloadComplete2 = true;
    }
    else if (info.status == firebase_fcs_download_status_error)
    {
        Serial.printf("Download failed, %s\n", info.errorMsg.c_str());
    }
}
void fcsUploadCallback(FCS_UploadStatusInfo info)
{
    if (info.status == firebase_fcs_upload_status_init)
    {
        Serial.printf("Uploading file %s (%d) to %s\n", info.localFileName.c_str(), info.fileSize, info.remoteFileName.c_str());
    }
    else if (info.status == firebase_fcs_upload_status_upload)
    {
        Serial.printf("Uploaded %d%s, Elapsed time %d ms\n", (int)info.progress, "%", info.elapsedTime);
    }
    else if (info.status == firebase_fcs_upload_status_complete)
    {
        Serial.println("Upload completed\n");
    }
    else if (info.status == firebase_fcs_upload_status_error)
    {
        Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
    }
}


void vTimerCallbackUpdate(TimerHandle_t xTimer)
{
    //Serial.println("Checking Realtime Database...");
    if (xSemaphoreGive(bootHandle.xSerialSemaphoreUpdate) != pdTRUE)
    {
        Serial.println("Could not give semaphore CMD......");
    }
    else{
        Serial.println("give one semaphore update......");
    }
}
void vTimerCallbackCmd(TimerHandle_t xTimer)
{
    //Serial.println("Checking Realtime Database...");
    if (xSemaphoreGive(bootHandle.xSerialSemaphoreCmd) != pdTRUE)
    {
        Serial.println("Could not give semaphore CMD......");
    }
    else{
        Serial.println("give one semaphore CMD......");
    }
}
 