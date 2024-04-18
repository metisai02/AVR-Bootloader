/*
 * flash.h
 *
 * Created: 4/9/2024 9:31:59 PM
 *  Author: ADMIN
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "avr/io.h"
#include <avr/interrupt.h>
#include <avr/boot.h>

void flash_erase_page(uint16_t address);
void flash_fill_buffer(uint16_t address, uint16_t data);
void flash_write_data(uint16_t address, const uint8_t *data, uint16_t size);
void flash_read_data(uint16_t address);

#endif /* FLASH_H_ */