/*
 * bootLib.h
 *
 * Created: 4/9/2024 9:32:20 PM
 *  Author: ADMIN
 */ 


#ifndef BOOTLIB_H_
#define BOOTLIB_H_



#include <avr/boot.h>
#include <stdio.h>
#include "flash.h"
#include "uart.h"
#define BOOT_HEADER 0xFFFF
// read command

typedef enum
{
	BOOT_WRITE_CMD = 0x96,
	BOOT_READ_ADD_CMD = 0x69,
	BOOT_READ_PAGE_CMD = 0x68,
	BOOT_READ_ALL_CMD = 0x67,
	BOOT_ERASE_CMD = 0x77
} boot_command;
#define UART_BOOT 0
#define UART_USER 0
#define BOOT_START 0x55
typedef enum
{
	BOOT_OK = 0,
	BOOT_ERROR,
	BOOT_TIMEOUT
} boot_status_t;
typedef struct
{
	uint8_t start;
	uint8_t length;
	uint8_t command;
} boot_common_t;
// frame for reading
typedef struct
{
	uint16_t address;
	uint32_t crc;
} boot_read_t;
typedef struct
{
	boot_common_t common;
	boot_read_t b_read;
} boot_read_frame_t;
// frame for writing
typedef struct
{
	uint16_t address;
	uint8_t data[128];
	// uint32_t crc;
} boot_write_t;
typedef struct
{
	boot_common_t common;
	boot_write_t b_write;
} boot_write_frame_t;

// frame for erasing
typedef struct
{
	uint16_t address;
	uint32_t crc;
} boot_erase_t;
typedef struct
{
	boot_common_t common;
	boot_erase_t b_earse;
} boot_earse_frame_t;
// response frame read of AVR
typedef struct
{
	uint8_t start;
	uint16_t page;
	uint8_t data[128];
	// uinr16_t crc;

} boot_res_read_frame_t;

void boot_send_ack();
void boot_send_nack();
uint8_t boot_write_handler(uint16_t address, uint8_t *d_write, uint8_t size);
boot_status_t boot_read_handler(uint16_t address, boot_command type, boot_res_read_frame_t *bufData);
uint8_t boot_erase_handler();

#endif /* INC_BOOT_H_ */
