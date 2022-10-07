/* INCLUDES ******************************************************************/
#include <stdlib.h>
#include <avr/interrupt.h>
#include "ses_timer.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC 1249
#define TIMER2_CYC_FOR_1MILLISEC 250 // 0x06

void (*volatile Timer2InterruptHandler)(void); // Global pointer for Timer 2 to store the callback pointer
void (*volatile Timer1InterruptHandler)(void); // Global pointer Timer 1 to store the callback pointer

/*FUNCTION DEFINITION ********************************************************/
void timer2_setCallback(pTimerCallback cb)
{
	if (cb != NULL)
	{
		Timer2InterruptHandler = cb; // Assigning the global pointer with passed callback
	}
}

void timer2_start()
{
	OCR2A = TIMER2_CYC_FOR_1MILLISEC;					 // To count upto 1ms
	TCCR2A &= (1 << WGM21) & ~(1 << WGM20);				 // To enable CTC mode
	TIMSK2 |= (1 << OCIE2A);							 // To enable Compare Match A interrupt & Overflow interrupt
	TIFR2 |= (1 << OCF2A);								 // To reset the interrupt flag
	TCCR2B |= (1 << CS22) & ~(1 << CS21) & ~(1 << CS20); // To set Prescaler value to 64
}

void timer2_stop()
{
	PRR0 |= (1 << PRTIM2); // To shut down the Timer/Counter2 module
}

void timer1_setCallback(pTimerCallback cb)
{
	if (cb != NULL)
	{
		Timer1InterruptHandler = cb; // Assigning the global pointer with passed callback
	}
}

void timer1_start()
{
	OCR1A = TIMER1_CYC_FOR_5MILLISEC;		  // To count upto 5ms
	TCCR1A &= ~((1 << WGM10) | (1 << WGM11)); // To enable CTC mode
	TCCR1B |= (1 << WGM12) & ~(1 << WGM13);
	TIMSK1 |= (1 << OCIE1A);								// To enable Timer/Counter1 Output Compare A Match interrupt & Overflow interrupt
	TIFR1 |= (1 << OCF1A);									// To reset the interrupt flag
	TCCR1B |= ((1 << CS12) & (~((1 << CS10) | (1 << CS11)))); // To set Prescaler value to 64
}

void timer1_stop()
{
	PRR0 |= (1 << PRTIM1); // To shut down the Timer/Counter1 module
}

ISR(TIMER1_COMPA_vect)
{
	Timer1InterruptHandler(); // Calling the timer1 Function pointer to raise the callback funtion
}

ISR(TIMER2_COMPA_vect)
{
	Timer2InterruptHandler(); // Calling the timer2 Function pointer to raise the callback funtion
}