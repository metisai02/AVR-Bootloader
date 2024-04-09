#include "uart.h"
uint8_t is_external_clock()
{
	uint8_t fuse_value = 1;
	// uint8_t fuse_value = read_low_fuse();
	return (fuse_value & 0x07) != 0x07;
}

uint8_t is_CKDIV8()
{
	uint8_t fuse_value = 1;
	// uint8_t fuse_value = read_low_fuse();
	return (fuse_value & 0x80) != 0; // Check if CKDIV8 bit is set
}

uint8_t UART_init(uint16_t baudrate)
{
	if (is_CKDIV8() && is_external_clock())
	return 1;

	uint16_t ubrr = (uint16_t)(CPU_N / (16UL * baudrate)) - 1; // Calculate UBRR value

	// Set baud rate
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)(ubrr);

	// Enable transmitter and receiver
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// Set frame format: 8 data bits, 1 stop bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	return 0;
}

AVR_status_t UART_Transmit(const uint8_t *data, uint16_t size, uint32_t timeout)
{
	uint32_t startTimer = getSystick()	;
	for (uint16_t i = 0; i < size; i++)
	{
		// Wait for transmit buffer to be empty
		while (!(UCSR0A & (1 << UDRE0)))
		{
			if ((getSystick() - startTimer) > timeout)
			{
				return AVR_TIMEOUT;
			}
		}

		// Load data into transmit register
		UDR0 = data[i];
	}
	return AVR_OK;
}

AVR_status_t UART_Receive(uint8_t *data, uint16_t size, uint32_t timeout)
{
	uint32_t startTimer = getSystick();
	for (uint16_t i = 0; i < size; i++)
	{
		// Wait for data to be received
		while (!(UCSR0A & (1 << RXC0)))
		{

			if ((getSystick() - startTimer) > timeout)
			{
				return AVR_TIMEOUT;
			}
		}

		// Get received data from buffer
		data[i] = UDR0;
	}
	return AVR_OK;
}

// uint8_t read_low_fuse()
// {
// 	uint8_t data;
// 	SPMCSR = READ_FUSE;
// 	__asm__ volatile(
// 	"ldi %0, %1\n"
// 	: "=r"(data)
// 	: "z"(0x0000)
// 	: "r30", "r31"
// 	);
// 	return data;
// }
