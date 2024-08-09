

#ifndef MATH_H
#define MATH_H
#include "Arduino.h"
class MathESP
{
    public:
    static void stringToHex(uint8_t *buffer, const char *char_str, uint16_t buffer_size);
};

#endif