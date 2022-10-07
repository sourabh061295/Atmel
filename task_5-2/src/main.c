/* INCLUDES ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "ses_lcd.h"
#include "ses_led.h"
#include "ses_button.h"
#include "ses_pwm.h"
#include "ses_fanspeed.h"
#include "ses_scheduler.h"

#define BASEDUTYCYCLEVALUE 51 // Fan speed incremented by 25% on every button press -> (MAX Value of unit_8)*0.25

static uint8_t dutyCycleValue = 0; // To preserve the dutycycle value on every button press

// Increase the PWM value for fan speed control in steps of BASEDUTYCYCLEVALUE
void fanSpeedControl()
{
	dutyCycleValue += BASEDUTYCYCLEVALUE;
	pwm_SetDutyCycle(dutyCycleValue);
	dutyCycleValue = dutyCycleValue >= 255 ? 0 : dutyCycleValue;
}

// Display fan speed on LCD
void DisplayFanSpeed(void)
{
	lcd_clear();
	lcd_setCursor(0, 0);
	uint16_t fanSpeed = fanspeed_getRecent();
	fprintf(lcdout, "Fan Speed: %d RPM\n", fanSpeed);
	fprintf(lcdout,"PWM Value: %d \n",dutyCycleValue);
}

// Detect if fan speed is 0RPM and turn on RED led
void FanHaltDetection(void)
{
	uint16_t fanSpeed = fanspeed_getRecent();
	led_redOff();
	if (!fanSpeed)
	{
		led_redOn();
	}
}

int main(void)
{
	lcd_init();
	led_Init(RED);
	button_init(true);											  // Configure port for Joystick with debounce check
	button_setJoystickButtonCallback(fanSpeedControl);			  // Setting callback to control fan speed on button press
	pwm_init();													  // Initialising the PWM mode on timer
	fanspeed_init();											  // Initialising the measurement of fanspeed
	fanSpeed_setTimer5Callback((pTimerCallback)FanHaltDetection); // Set timer callback to execute Display fanspeed on interrupt
	scheduler_init();											  // Initialize the scheduler
	task_descriptor_t DisplayTask;								  // Create a new task to Dsiplay fan speed at a regular interval of 1sec
	DisplayTask.task = (task_t)DisplayFanSpeed;
	DisplayTask.period = 1000;
	scheduler_add(&DisplayTask);
	sei(); // Enable global interrupt

	while (1)
	{
		scheduler_run();
	}
	return 0;
}