#include "mathesp.h"

void MathESP::stringToHex(uint8_t *buffer, const char *char_str, uint16_t buffer_size)
{
    uint16_t count = 0;

    while (*char_str && count < buffer_size / 2)
    {
        char byte_str[3]; // Lưu trữ một byte hex (hai ký tự hex + ký tự kết thúc chuỗi)
        byte_str[0] = *char_str++;
        byte_str[1] = *char_str++;
        byte_str[2] = '\0'; // Ký tự kết thúc chuỗi

        buffer[count++] = strtol(byte_str, NULL, 16);
    }
}