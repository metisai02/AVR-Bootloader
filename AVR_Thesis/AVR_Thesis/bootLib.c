
#include "bootLib.h"
#include "util/delay.h"
#include "avr/pgmspace.h"
#include <string.h>
#include <stdio.h>

// extern uint8_t count;

uint16_t crc16(const uint8_t *data, uint16_t length)
{
    uint16_t poly = 0x8005;
    uint16_t crc = 0x0000;
    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i] << 8;
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ poly;
            }
            else
            {
                crc <<= 1;
            }
        }
        crc &= 0xFFFF;
    }
    return crc;
}
void boot_send_ack()
{
    boot_ack_t ack = {BOOT_START, BOOT_ACK};
    UART_Transmit((uint8_t *)&ack, sizeof(ack), MAX_TIME_OUT);
}
void UART_Transmit_CRC(uint16_t crc_value) {
	char crc_str[5];  // 4 ký tự cho giá trị hex và 1 ký tự null-terminator
	snprintf(crc_str, sizeof(crc_str), "%04X", crc_value);

	UART_Transmit((uint8_t *)crc_str, strlen(crc_str), MAX_TIME_OUT);
}
void boot_send_nack()
{
    boot_ack_t nack = {BOOT_START, BOOT_NACK};
    UART_Transmit((uint8_t *)&nack, sizeof(nack), MAX_TIME_OUT);
}

boot_status_t boot_write_handler(boot_write_frame_t *booDataWrite)
{
    uint16_t crc_check = crc16((uint8_t *)booDataWrite, booDataWrite->common.length + 5);
    crc_check = (crc_check >> 8) | (crc_check << 8);
	
	//UART_Transmit_CRC(crc_check);  // Hiển thị CRC đã tính toán qua UART
	//UART_Transmit_CRC(booDataWrite->b_write.crc);
    booDataWrite->b_write.address = (booDataWrite->b_write.address >> 8) | (booDataWrite->b_write.address << 8);
    if (crc_check != booDataWrite->b_write.crc)
    {
        return BOOT_ERROR;
    }

    // Ghi dữ liệu vào flash
    flash_write_data(booDataWrite->b_write.address, booDataWrite->b_write.data, booDataWrite->common.length);
    return BOOT_OK;
}

boot_status_t boot_read_handler(boot_read_frame_t *booDataRead)
{
    uint16_t crc_check = crc16((uint8_t *)booDataRead, booDataRead->common.length + 5);
    crc_check = (crc_check >> 8) | (crc_check << 8);

    booDataRead->b_read.address = (booDataRead->b_read.address >> 8) | (booDataRead->b_read.address << 8);
    if (crc_check != booDataRead->b_read.crc)
    {
        return BOOT_ERROR;
    }
	else
	{
		// this ACK for cmd frame
		boot_send_ack();
	}

    boot_res_read_frame_t res_frame;
    res_frame.start = BOOT_START;
    res_frame.page = (uint8_t)(booDataRead->b_read.address / 128);
    uint16_t real_address = res_frame.page * 128;

    switch (booDataRead->common.command)
    {
    case BOOT_READ_ADD_CMD:
        res_frame.page = 0xFF;
        res_frame.data[0] = pgm_read_byte(booDataRead->b_read.address);
        if (UART_Transmit((uint8_t *)&res_frame, 5, MAX_TIME_OUT) == AVR_OK)
            return BOOT_OK;
        break;

    case BOOT_READ_PAGE_CMD:
        for (int i = 0; i < 128; i++)
        {
            res_frame.data[i] = pgm_read_byte(real_address + i);
        }
        res_frame.crc = crc16((uint8_t *)&res_frame, sizeof(res_frame) - 2);
        if (UART_Transmit((uint8_t *)&res_frame, sizeof(boot_res_read_frame_t), MAX_TIME_OUT) == AVR_OK)
            return BOOT_OK;
        break;

    case BOOT_READ_ALL_CMD:
        _delay_ms(30);
        for (int i = 0; i < 200; i++)
        {
            res_frame.page = i;
            for (int j = 0; j < 128; j++)
            {
                res_frame.data[j] = pgm_read_byte(i * 128 + j);
            }
            res_frame.crc = crc16((uint8_t *)&res_frame, sizeof(res_frame) - 2);

            if (UART_Transmit((uint8_t *)&res_frame, sizeof(boot_res_read_frame_t), MAX_TIME_OUT) != AVR_OK)
                return AVR_ERROR;
			_delay_ms(35);
        }
		// this for all of page in ATmega328
        return BOOT_OK;

    default:
        break;
    }
    return BOOT_ERROR;
}

boot_status_t boot_erase_handler(boot_erase_frame_t *booDataEarse)
{
    uint16_t crc_check = crc16((uint8_t *)booDataEarse, booDataEarse->common.length + 5);
    crc_check = (crc_check >> 8) | (crc_check << 8);

    booDataEarse->b_erase.address = (booDataEarse->b_erase.address >> 8) | (booDataEarse->b_erase.address << 8);
    if (crc_check != booDataEarse->b_erase.crc)
    {
        boot_send_nack();
        return BOOT_ERROR;
    }
    for (int i = 0; i < 20; i++)
    {
        uint16_t real_address = 128 * i;
        flash_erase_page(real_address);
    }
    return BOOT_OK;
}