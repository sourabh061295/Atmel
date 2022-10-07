#include "ses_led.h"
#include <util/delay.h>

/**Toggles the red LED of the SES-board*/
int main(void)
{
	led_redInit();
	led_greenInit();
	led_yellowInit();

	led_redOn();
	led_greenOn();
	led_yellowOn();
	_delay_ms(3000);
	led_redOff();
	led_greenOff();
	led_yellowOff();
	while (1)
	{
		led_redToggle();
		_delay_ms(1000);
		led_greenToggle();
		_delay_ms(1000);
		led_yellowToggle();
	}
	return 0;
}
