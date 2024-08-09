#include "crc16.h"

uint16_t crc16(const uint8_t *data, uint16_t length)
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