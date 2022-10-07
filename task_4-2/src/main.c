/* INCLUDES ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "ses_lcd.h"
#include "ses_led.h"
#include "ses_scheduler.h"
#include "ses_button.h"

#define BASEPERIOD 5 // Polling time of task scheduler is 5ms
#define TOTALTAKS 2	 // Total taks which will be added to the scheduler
#define LEDTOGGLETASKINDEX 1
#define BUTTONSTATETASKINDEX 2
#define YELLOWLEDAUTOOFFTASK 3

static uint16_t localTimerForLEDControl = 0; // Global flag to maintain timer for Led control
static uint16_t localStopWatchTimer = 0;	 // Global flag to maintain stopwatch timer
static uint8_t seconds = 0;					 // Global flag to maintain seconds of a stopwatch timer
static bool isJoystickButtonPressed = false; // Global flag to maintain state of joystick
static bool isRotaryButtonPressed = false;	 // Global flag to maintain state of rotary
static task_descriptor_t tasks[TOTALTAKS];

void *ledToggleSelect(LEDColour ledColour)
{
	switch (ledColour)
	{
	case RED:
		return (&led_redToggle);
	case YELLOW:
		return (&led_yellowToggle);
	case GREEN:
		return (&led_greenToggle);
	}
}

void *ledOFFSelect(LEDColour ledColour)
{
	switch (ledColour)
	{
	case RED:
		return (&led_redOff);
	case YELLOW:
		return (&led_yellowOff);
	case GREEN:
		return (&led_greenOff);
	}
}

/**
 * Toggle LED on Joystick button press
 * Turn off the LED by default after 5sec
 */
void yellowLed_stateControl(void)
{
	isJoystickButtonPressed = !isJoystickButtonPressed;
	localTimerForLEDControl += BASEPERIOD;
	// Toggling the yellow led accordingly with the button state
	if (isJoystickButtonPressed)
	{
		led_yellowOn();

		// Scheduling a task for Turning off the yellow after 5sec(5000ms/BASEPERIOD)
		tasks[YELLOWLEDAUTOOFFTASK].task = (task_t)ledOFFSelect(YELLOW);
		tasks[YELLOWLEDAUTOOFFTASK].period = BASEPERIOD;
	}
	else
	{
		led_yellowOff();
	}
}

void display_StopWatch()
{
	isRotaryButtonPressed = !isRotaryButtonPressed;
	if (isRotaryButtonPressed)
	{
		localStopWatchTimer += BASEPERIOD;
		uint16_t tenthOfSec = 0;
		tenthOfSec = localStopWatchTimer % 10 == 0 ? localStopWatchTimer : localStopWatchTimer - BASEPERIOD;
		if (tenthOfSec >= 100)
		{
			seconds += 1;
			tenthOfSec = 0;
		}
		lcd_setCursor(0, 0);
		fprintf(lcdout, "%d . %d", seconds, tenthOfSec);
	}
}

int main(void)
{
	led_Init(GREEN);
	button_portConfig();
	button_setJoystickButtonCallback(yellowLed_stateControl);
	button_setRotaryButtonCallback(display_StopWatch);
	lcd_init();
	lcd_clear();
	scheduler_init();
	// List of tasks to be scheduled
	tasks[LEDTOGGLETASKINDEX].task = (task_t)ledToggleSelect(GREEN);
	tasks[LEDTOGGLETASKINDEX].period = 2000;
	tasks[BUTTONSTATETASKINDEX].task = (task_t)&button_checkState;
	tasks[BUTTONSTATETASKINDEX].period = BASEPERIOD;
	for (uint8_t i = 0; i < 4; i++)
	{
		scheduler_add(&tasks[i]);
	}
	sei();
	while (1)
	{
		scheduler_run();
	}
	return 0;
}