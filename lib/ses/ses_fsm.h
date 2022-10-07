
#ifndef SES_FSM_H
#define SES_FSM_H

/* INCLUDES ******************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "ses_scheduler.h"

/* DEFINES & MACROS **********************************************************/

/** return values */
typedef enum
{
   RET_HANDLED = 0, // event was handled
   RET_IGNORED,     // event was ignored; not used in this implementation
   RET_TRANSITION   // event was handled and a state transition occurred
} fsmReturnStatus;

// typedef for alarm clock state machine
typedef struct fsm_s Fsm; 
// event type for alarm clock fsm
typedef struct event_s Event; 

/** typedef for state event handler functions */
typedef fsmReturnStatus (*State)(Fsm *, const Event *);

struct fsm_s
{
   State state;           // current state, pointer to event handler
   bool isAlarmEnabled;   // flag for the alarm status
   bool isClockOn;
   time_t timeSet; // multi-purpose var for system time and alarm time
};

struct event_s
{
   uint8_t signal; // identifies the type of event
};

/* typedef for State machine events */
typedef enum
{
   ENTRY = 0,
   ROTARY_PRESS,
   ROTARY_CLOCKWISE,
   ROTARY_COUNTER_CLOCKWISE,
   JOYSTICK_PRESS,
   ALARM_MATCH,
   ALARM_OFF,
   EXIT
} events;


/* FUNCTION DEFINITION *******************************************************/
/**
 * Clock init state - called on power up or fsm init
 */
fsmReturnStatus clock_init(Fsm *fsm, const Event *e);

/**
 * Set time state - called to set the clock/alarm time
 */
fsmReturnStatus set_time(Fsm *fsm, const Event *e);

/**
 * Normal clock operation state - Display the clock every second
 */
fsmReturnStatus clock_mode(Fsm *fsm, const Event *e);

/**
 * Alarm compare state - compare the alarm time with clock time
 */
fsmReturnStatus alarm_mode(Fsm *fsm, const Event *e);

/**
 * Alarm ring state - perform the alarm ringing operation
 */
fsmReturnStatus alarm_ring(Fsm *fsm, const Event *e);

#endif