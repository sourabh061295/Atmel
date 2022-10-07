#include "ses_lcd.h"
#include "ses_uart.h"
#include <util/delay.h>

/**Toggles the red LED of the SES-board*/
int main(void)
{
	uart_init(57600);
	lcd_init();
	while (1)
	{
		fprintf(uartout, "UART TEST PRINT\n");
		fprintf(lcdout, "LCD TEST PRINT\n");
		_delay_ms(500);
	}

	return 0;
}
