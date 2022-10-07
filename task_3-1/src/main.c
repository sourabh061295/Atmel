#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "ses_led.h"
#include "ses_button.h"

int main(void)
{
	button_init(false);
	led_redInit();
	led_greenInit();
	sei();
	button_setJoystickButtonCallback(led_redToggle);
	button_setRotaryButtonCallback(led_greenToggle);
	while (1)
	{
		
	}
	return 0;
}