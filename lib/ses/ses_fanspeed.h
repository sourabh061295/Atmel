#ifndef SES_FANSPEED_H
#define SES_FANSPEED_H

/*INCLUDES *******************************************************************/

#include <inttypes.h>
#include <avr/io.h>

typedef void (*pTimerCallback)(void);

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initialise Fanspeed Measurement with Timer 5 with a time period of 1s
 */
void fanspeed_init(void);

/**
 * Get the recent speed of fan
 * @return Recent fan speed in rotations per second (RPM)
 */
uint16_t fanspeed_getRecent(void);

/**
 * Set callback function of timer interrupt trigger
/ * @param timerCallback Callback function to be executed
 */
void fanSpeed_setTimer5Callback(pTimerCallback timerCallback);

/**
 * Returns Filtered Fan speed value 
 * @return uint16_t Filetred Fan speed value in rotations per second (RPM)
 */
uint16_t fanspeed_getFiltered(void);

#endif