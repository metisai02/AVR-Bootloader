/*
 * timer.h
 *
 * Created: 4/9/2024 9:31:31 PM
 *  Author: ADMIN
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include "stdint.h"
#include <avr/interrupt.h>
void timer1_init();
uint32_t getSystick();

#endif /* TIMER_H_ */