#include "ses_button.h"
#include "ses_lcd.h"
#include <util/delay.h>

int main(void)
{
	button_init();
	lcd_init();
	_delay_ms(3000);
	while (1)
	{
		fprintf(lcdout, button_isRotaryPressed() ? "Rotary Pressed\n\n" : "Rotary Unpressed\n\n");
		fprintf(lcdout, button_isJoystickPressed() ? "Joystick Pressed\n\n" : "Joystick Unpressed\n\n");
	}
	return 0;
}
