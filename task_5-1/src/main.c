/* INCLUDES ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "ses_button.h"
#include "ses_pwm.h"
#include "ses_lcd.h"

#define BASEDUTYCYCLEVALUE 51 // Fan speed incremented by 25% on every button press -> (MAX Value of unit_8)*0.25

uint8_t dutyCycleValue = 0; // To preserve the dutycycle value on every button press

void fanSpeedControl()
{
	dutyCycleValue += BASEDUTYCYCLEVALUE;
	pwm_SetDutyCycle(dutyCycleValue);
	lcd_clear();
	lcd_setCursor(0, 0);
	fprintf(lcdout, "PWM Value: %d", dutyCycleValue);
	dutyCycleValue = dutyCycleValue >= 255 ? 0 : dutyCycleValue;
}

int main(void)
{
	button_init(true); // Configure port for Joystick with debounce check
	lcd_init();
	button_setJoystickButtonCallback(fanSpeedControl); // Setting callback to control fan speed on button press
	pwm_init();										   // Initialising the PWM mode on timer
	sei(); // Enable global interrupt
	while (1)
	{

	}
	return 0;
}