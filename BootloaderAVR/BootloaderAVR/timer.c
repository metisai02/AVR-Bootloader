#include "timer.h"

static uint32_t volatile countSystick = 0;
// init 1ms timer , clk 16Mhz
void timer1_init()
{
    // Set normal mode
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 65536 - 2000;
    // Set prescaler divide by 8
    TCCR1B |= (1 << CS11);
    // TIMSK1 |= (1 << TOIE1);
    // sei();
}
uint32_t getSystick()
{
    return countSystick;
}
// ISR(TIMER1_OVF_vect)
// {
//     countSystick++;
//     TCNT1 = 65536 - 2000;
// }