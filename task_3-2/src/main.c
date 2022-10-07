#include <avr/io.h>
#include <avr/interrupt.h>
#include "ses_led.h"
#include "ses_timer.h"

static uint16_t TimerCount = 0;
void SoftwareTimer(void)
{
	if (TimerCount == 1000)
	{
		led_yellowToggle();
		TimerCount=0;
	}
	else
	{
		TimerCount++;
	}
}

int main(void)
{
	timer2_start();
	led_yellowInit();
	sei();
	timer2_setCallback(SoftwareTimer);
	while (1)
	{
	}
	return 0;
}