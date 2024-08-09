#include "timer.h"

static uint32_t volatile countSystick = 0;
// init 1ms timer , clk 16Mhz
void timer1_init()
{
    // Set normal mode
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    // Set prescaler divide by 1024
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS11);
    TCCR1B |= (1 << CS12);
}
uint16_t getCNT()
{
    return TCNT1;
}
void reset_timer1()
{
    TCNT1 = 0;
}