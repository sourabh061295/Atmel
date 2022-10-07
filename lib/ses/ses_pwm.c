/* INCLUDES ******************************************************************/
#include "ses_pwm.h"

/*FUNCTION DEFINITION *************************************************/
void pwm_init(void)
{
    DDRG |= (1 << DDG5);
    PRR0 &= ~(1 << PRTIM0);                // Enable Timer 0
    TCCR0A |= (1 << WGM01) | (1 << WGM00); // Enable FAST PWM with TOP as OCRA value
    TCCR0B &= (~(1 << WGM02));
    TCCR0A |= (1 << COM0B0) | (1 << COM0B1);
    TCCR0B |= (((1 << CS01) | (1 << CS00)) & ~(1 << CS02)); // Set prescaler to 64
    OCR0B = 0; // Setting the initial value for compare output mode to zero
}

void pwm_SetDutyCycle(uint8_t tc)
{
    OCR0B = tc;
}