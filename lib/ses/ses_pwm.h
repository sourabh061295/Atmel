#ifndef SES_PWM_H
#define SES_PWM_H

/*INCLUDES *******************************************************************/

#include <inttypes.h>
#include <avr/io.h>

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * @Initializes the timer with PWM mode
 */
void pwm_init(void);

/**
 * Set the duty cycle for the PWM mode
 * @param tc duty cycle value
 */
void pwm_SetDutyCycle(uint8_t tc);

#endif