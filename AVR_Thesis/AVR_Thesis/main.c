#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "flash.h"
#include <avr/fuse.h>
#include <avr/boot.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "uart.h"
#include "bootLib.h"
#include "timer.h"

#define ADDRESS_DATA (0x0000)
#define ADDRESS_F_DATA ADDRESS_DATA

void startup_code(void) __attribute__((naked)) __attribute__((section(".init8")));
void boot_main(void);

void gpio_init()
{
    DDRC = 0xFFu;
    DDRD &= ~(1 << DDD4);
    PORTD |= (1 << PORTD4);
}

void toggle_led()
{
    PORTC ^= (1 << DDC0);
}

void main_cs(void)
{
    UART_init(38400);
    timer1_init();
    gpio_init();
    uint8_t dataReset[] = "BOOTLOADER RESET\n";
    UART_Transmit(dataReset, sizeof(dataReset), 10000);
    boot_main();
}

void boot_main()
{
    uint8_t boot_data[sizeof(boot_write_frame_t)] = {0};
    boot_read_frame_t *boDataRead = NULL;
    boot_write_frame_t *boDataWrite = NULL;
    boot_erase_frame_t *boDataErase = NULL;

    while (1)
    {
        // Nhận và xử lý dữ liệu từ UART
        do
        {
            UART_Receive(boot_data, 1, 20000);
            if ((PIND & (1 << PORTD4)) == 0)
                __asm__ volatile("jmp 0x0000");

        } while ((boot_data[0] != BOOT_START));

        UART_Receive(&boot_data[1], 1, MAX_TIME_OUT);
        UART_Receive(&boot_data[2], (boot_data[1] + 5), MAX_TIME_OUT);

        switch (boot_data[2])
        {
        case BOOT_WRITE_CMD:

            if (boot_data[1] == 128)
            {
                boDataWrite = (boot_write_frame_t *)boot_data;
            }
            else
            {
                uint16_t crc = (boot_data[6 + boot_data[1]] << 8) | boot_data[5 + boot_data[1]];
                boDataWrite = (boot_write_frame_t *)boot_data;
                boDataWrite->b_write.crc = crc;
            }

            toggle_led();
            if (boot_write_handler(boDataWrite) == BOOT_OK)
            {
                boot_send_ack();
            }
            else
            {
                boot_send_nack();
            }
            break;

        case BOOT_READ_ADD_CMD:
        case BOOT_READ_PAGE_CMD:
        case BOOT_READ_ALL_CMD:
            toggle_led();
            boDataRead = (boot_read_frame_t *)boot_data;
            if (boot_read_handler(boDataRead) == BOOT_OK)
            {
                boot_send_ack();
            }
            else
            {
                boot_send_nack();
            }
            break;

        case BOOT_ERASE_CMD:
            toggle_led();
            boDataErase = (boot_erase_frame_t *)boot_data;
            if (boot_erase_handler(boDataErase) != BOOT_OK)
            {
                boot_send_nack();
            }
            else
            {
                boot_send_ack();
            }
            break;

        default:
            break;
        }
        memset(boot_data, 0, sizeof(boot_data));
        _delay_ms(1);
    }
}

void startup_code(void)
{
    main_cs();
}



