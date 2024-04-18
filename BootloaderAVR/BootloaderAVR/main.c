#ifndef F_CPU
#define F_CPU 16000000UL // telling controller crystal frequency (16 MHz AVR ATMega328P)
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
uint8_t count = 0;
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

void send_hex_byte(uint8_t byte)
{
    uint8_t hex_digit;
    char hex_char[3];

    hex_digit = (byte >> 4) & 0x0F;
    hex_char[0] = hex_digit < 10 ? hex_digit + '0' : hex_digit - 10 + 'A';

    hex_digit = byte & 0x0F;
    hex_char[1] = hex_digit < 10 ? hex_digit + '0' : hex_digit - 10 + 'A';

    hex_char[2] = '\0';

    UART_Transmit((uint8_t *)hex_char, 2, 1000);
}

void send_hex_buffer(uint8_t *buffer, uint8_t length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        send_hex_byte(buffer[i]);
    }
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
    boot_write_t *d_write = NULL;
    boot_res_read_frame_t *res_data = NULL;
    while (1)
    {
        do
        {
            UART_Receive(boot_data, 1, 20000);
            if ((PIND & (1 << PORTD4)) == 0)
                __asm__ volatile("jmp 0x0000");

        } while ((boot_data[0] != BOOT_START));
        UART_Receive(&boot_data[1], 1, MAX_TIME_OUT);
        UART_Receive(&boot_data[2], (boot_data[1] + 3), MAX_TIME_OUT);
        uint16_t converted_data = (boot_data[3] << 8) | boot_data[4];
        // check command
        switch (boot_data[2])
        {
        case BOOT_WRITE_CMD:
            d_write = (boot_write_t *)&boot_data[3];
            toggle_led();
            if (boot_write_handler(converted_data, d_write->data, boot_data[1]) == BOOT_OK)
            {
                boot_send_ack();
            }
            else
                boot_send_nack();

            break;
        case BOOT_READ_ADD_CMD:
            toggle_led();
            res_data = (boot_res_read_frame_t *)boot_data;
            if (boot_read_handler(converted_data, BOOT_READ_ADD_CMD, res_data) == BOOT_OK)
                boot_send_ack();

            else
                boot_send_nack();

            break;
        case BOOT_READ_PAGE_CMD:
            res_data = (boot_res_read_frame_t *)boot_data;
            boot_send_ack();
            if (boot_read_handler(converted_data, BOOT_READ_PAGE_CMD, res_data) != BOOT_OK)
                boot_send_nack();
            break;
        case BOOT_READ_ALL_CMD:
            toggle_led();
            boot_send_ack();
            res_data = (boot_res_read_frame_t *)boot_data;
            if (boot_read_handler(converted_data, BOOT_READ_ALL_CMD, res_data) == BOOT_OK)
                boot_send_ack();

            else
                boot_send_nack();
            break;
        case BOOT_ERASE_CMD:
            boot_send_ack();
            if (boot_erase_handler() != BOOT_OK)
                boot_send_nack();

            else
                boot_send_ack();

            break;
        default:
            break;
        }
        memset(boot_data, 0, sizeof(boot_data));
        d_write = NULL;
        _delay_ms(1);
    }
}
void startup_code(void)
{
    main_cs();
}