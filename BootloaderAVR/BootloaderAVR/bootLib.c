
#include "bootLib.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
// extern uint8_t count;
void boot_send_ack()
{
    boot_ack_t ack = {BOOT_START, BOOT_ACK};
    UART_Transmit((uint8_t *)&ack, sizeof(ack), MAX_TIME_OUT);
}
void boot_send_nack()
{
    boot_ack_t nack = {BOOT_START, BOOT_NACK};
    UART_Transmit((uint8_t *)&nack, sizeof(nack), MAX_TIME_OUT);
}
uint8_t boot_write_handler(uint16_t address, uint8_t *d_write, uint8_t size)
{
    flash_write_data(address, d_write, size);
    return BOOT_OK;
}
boot_status_t boot_read_handler(uint16_t address, boot_command type, boot_res_read_frame_t *res_frame)
{
    res_frame->start = BOOT_START;
    res_frame->page = (uint8_t)(address / 128);
    uint16_t real_address = res_frame->page * 128;

    switch (type)
    {
    case BOOT_READ_ADD_CMD:
        res_frame->page = 0xFF;
        res_frame->data[0] = pgm_read_byte(address);
        if (UART_Transmit((uint8_t *)res_frame, 5, MAX_TIME_OUT) == AVR_OK)
            return BOOT_OK;
        break;

    case BOOT_READ_PAGE_CMD:

        for (int i = 0; i < 128; i++)
        {
            res_frame->data[i] = pgm_read_byte(real_address + i);
        }
        if (UART_Transmit((uint8_t *)res_frame, sizeof(boot_res_read_frame_t), MAX_TIME_OUT) == AVR_OK)
            return BOOT_OK;
        break;

    case BOOT_READ_ALL_CMD:
		_delay_ms(5);
        for (int i = 0; i < 5; i++)
        {
            res_frame->page = i;
            for (int j = 0; j < 128; j++)
            {
                res_frame->data[j] = pgm_read_byte(i * 128 + j);
            }

            if (UART_Transmit((uint8_t *)res_frame, sizeof(boot_res_read_frame_t), MAX_TIME_OUT) != AVR_OK)
                return AVR_ERROR;
        }
        return BOOT_OK;

    default:
        break;
    }
    return BOOT_ERROR;
}

boot_status_t boot_erase_handler()
{
    for (int i = 0; i < 20; i++)
    {
        uint16_t real_address = 128 * i;
        flash_erase_page(real_address);
    }
    return BOOT_OK;
}
