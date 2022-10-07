#include <avr/io.h>
#include <util/delay.h>

/**Toggles the red LED of the SES-board*/
int main(void)
{
	DDRG |= 0x02;
	DDRF |= 0x80;
	DDRF |= 0x40;
	while (1)
	{
		_delay_ms(500);
		PORTG ^= 0x02;
		_delay_ms(500);
		PORTF ^= 0x80;
		_delay_ms(500);
		PORTF ^= 0x40;
	}
	return 0;
}