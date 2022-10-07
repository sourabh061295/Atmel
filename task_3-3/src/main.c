#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_led.h"
#include "ses_timer.h"
#include <stdbool.h>

int main(void)
{
	button_init(true);
	led_redInit();
	led_greenInit();
	sei();
	button_setJoystickButtonCallback(led_greenToggle);
	button_setRotaryButtonCallback(led_redToggle);
	while (1)
	{
		
	}
	return 0;
}