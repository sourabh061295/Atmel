#include "ses_led.h"
#include "ses_lcd.h"
#include "ses_button.h"
#include <util/delay.h>

int main(void)
{
	led_redInit();
	led_greenInit();
	led_yellowInit();
	button_init();
	lcd_init();

	led_redOff();
	led_greenOff();
	led_yellowOff();
	while (1)
	{
		if (button_isJoystickPressed())
		{
			led_redOn();
			_delay_ms(100);
			led_redOff();
		}
		if (button_isRotaryPressed())
		{
			led_greenOn();
			_delay_ms(100);
			led_greenOff();
		}
		fprintf(lcdout, "1000\n\n\n\n");
	}
	return 0;
}
