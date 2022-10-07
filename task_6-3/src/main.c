/* INCLUDES ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ses_lcd.h"
#include "ses_led.h"
#include "ses_button.h"
#include "ses_scheduler.h"
#include "ses_fsm.h"

/* DEFINES & MACROS **********************************************************/

#define TIME_CONST_250MILLISEC         	250
#define TIME_CONST_1SEC         		1000
#define TIME_CONST_5SEC         		5 * TIME_CONST_1SEC
#define TOTAL_MINUTES         		    60
#define TOTAL_HOURS         		    24


static Fsm alarmClock_fsm;
static bool isHourSet = false, isMinSet = false, clockDisp = false;

static task_descriptor_t alarmTask, timer5secTask, compareTask, displayTask;

/* INLINE FUNCTION DEFINITION *************************************************/

/* Dispatches events to state machine, called in application*/
inline static void fsm_dispatch(Fsm *fsm, const Event *event)
{
	// Entry and exit events for a state
    static Event entryEvent = {.signal = ENTRY};
    static Event exitEvent = {.signal = EXIT};
	// Store the previous state pointer to perform exit actions
    State s = fsm->state;
    fsmReturnStatus r = fsm->state(fsm, event);
	// If transition is requested, then perfrom exit action of previous state and entry action of the new state
    if (r == RET_TRANSITION)
    {
        s(fsm, &exitEvent);           //< call exit action of last state
        fsm->state(fsm, &entryEvent); //< call entry action of new state
    }
}

/* Sets and calls initial state of state machine */
inline static void fsm_init(Fsm *fsm, State init)
{
    // fsm->timeSet = getClockTime();
    fsm->isAlarmEnabled = false;
    fsm->isClockOn = false;
    fsm->timeSet.hour = 0;
    fsm->timeSet.minute = 0;
    fsm->timeSet.second = 0;
    fsm->timeSet.milli = 0;
    fsm->state = clock_init;
    Event entryEvent = {.signal = ENTRY};
    fsm->state(fsm, &entryEvent);
}

/* FUNCTION DEFINITION *************************************************/

/**
 * Event handler to dispatch the joystick button press event to the fsm
 */
static void joystickEventHandler()
{
	Event e = {.signal = JOYSTICK_PRESS};
	// Dispatch button press event to the fsm
	fsm_dispatch(&alarmClock_fsm, &e);
}

/**
 * Event handler to dispatch the rotary button press event to the fsm
 */
static void rotaryEventHandler()
{
	Event e = {.signal = ROTARY_PRESS};
	// Dispatch button press event to the fsm
	fsm_dispatch(&alarmClock_fsm, &e);
}

/**
 * Clock display task
 */
void clockDisplay(void)
{
    if (alarmClock_fsm.isClockOn && clockDisp)
    {
        // Get the curret system time
        time_t time = getClockTime();
        // Dispaly on LCD
        lcd_clear();
        lcd_setCursor(0,0);
        fprintf(lcdout,"%d:%d:%d\n",time.hour,time.minute,time.second);
        fprintf(lcdout,"Alarm: %d:%d\n",alarmClock_fsm.timeSet.hour,alarmClock_fsm.timeSet.minute);
        lcd_setCursor(0,0);
        // Toggle green led with seconds
        led_greenToggle();
    }
}

/**
 * Compare the alarm time with clock time and generate an event
 */
void compareAlarm(void *fsm)
{
    Fsm *f = (Fsm *) fsm;
	// Get the curret system time
    time_t time = getClockTime();
	// Perform time checking if alarm is enabled only
    if(f->isAlarmEnabled)
    {
		// Generate an event if the hour and minutes match the currrent system time
        if((f->timeSet.hour == time.hour) && (f->timeSet.minute == time.minute))
        {
            Event e = {.signal = ALARM_MATCH};
	        fsm_dispatch(f, &e);
        }
    }
}

/**
 * 5 second timeout funciton for alamr ring
 */
void alarmCounterElapse(void *fsm)
{
    Fsm *f = (Fsm *) fsm;
    Event e = {.signal = ALARM_OFF};
	// Dispatch alarm off event to the fsm after the 5 sec timer elapses
    fsm_dispatch(f, &e);
}


/**
 * Clock init state - called on power up or fsm init
 */
fsmReturnStatus clock_init(Fsm *fsm, const Event *e)
{
	// Event handling switch
    switch(e->signal)
    {
        case ENTRY:
			// Display on init 
            lcd_clear();
            lcd_setCursor(0,0);
            fprintf(lcdout,"%d:%d\n",fsm->timeSet.hour,fsm->timeSet.minute);
            fprintf(lcdout,"Press R to set time");
            return RET_HANDLED;
        case ROTARY_PRESS:
            fsm->state = set_time;
            return RET_TRANSITION;
        case EXIT:
            return RET_HANDLED;
        default:
            return RET_IGNORED;
    }
}

/**
 * Set time state - called to set the clock/alarm time
 */
fsmReturnStatus set_time(Fsm *fsm, const Event *e)
{
	// Event handling switch
    switch(e->signal)
    {
        case ENTRY:
            clockDisp = false;
			// Display the time being set
            lcd_clear();
            lcd_setCursor(0,0);
            fprintf(lcdout,"%d:%d\n Set time\n",fsm->timeSet.hour,fsm->timeSet.minute);
            return RET_HANDLED;
        case ROTARY_PRESS:
			// Handling of hour and minute set flags to reuse the same state for clock and alarm time setting
            if(isHourSet)
            {
				// Increement minute and wrap around
                fsm->timeSet.minute = (fsm->timeSet.minute + 1) % TOTAL_MINUTES;
            }
            else
            {
				// Increement hour and wrap around
                fsm->timeSet.hour = (fsm->timeSet.hour + 1) % TOTAL_HOURS;
            }
            fsm->state = set_time;
            return RET_TRANSITION;
        case JOYSTICK_PRESS:
			// If only hour is met, move to setting minute state, else exit set time mode
            if(isHourSet)
            {
                isMinSet = true;
            }
            else
            {
                isHourSet = true;
            }
			// If both hour and minute are set, proceed to the next state
            if (isHourSet && isMinSet)
            {
                isHourSet = false;
                isMinSet = false;
				// If clock is not initialized at init, set the clock time
                if (!fsm->isClockOn)
                {
                    fsm->timeSet.second = 0;
                    fsm->timeSet.milli = 0;
                    scheduler_setTime(clock_to_systemTime(fsm->timeSet));
                    fsm->isClockOn = true;
                }
                fsm->state = clock_mode;
            }
			// Stay in set time mode if either minute or hour is not set
            else
            {
                fsm->state = set_time;
            }
            return RET_TRANSITION;
        case EXIT:
            return RET_HANDLED;
        default:
            return RET_IGNORED;
    }
}

/**
 * Normal clock operation state - Display the clock every second
 */
fsmReturnStatus clock_mode(Fsm *fsm, const Event *e)
{
	// Event handling switch
    switch(e->signal)
    {
        case ENTRY:
            clockDisp = true;
            return RET_HANDLED;
        case ROTARY_PRESS:
            fsm->state = alarm_mode;
            return RET_TRANSITION;
        case JOYSTICK_PRESS:
			// Jump to alarm time setting mode on joystick press and remove the display task
            fsm->state = set_time;
            return RET_TRANSITION;
        case EXIT:
            return RET_HANDLED;
        default:
            return RET_IGNORED;
    }
}

/**
 * Alarm compare state - compare the alarm time with clock time
 */
fsmReturnStatus alarm_mode(Fsm *fsm, const Event *e)
{
	// Event handling switch
    switch(e->signal)
    {
        case ENTRY:
			// Perform alarm active actions
            led_yellowOn();
            fsm->isAlarmEnabled = true;
            return RET_HANDLED;
        case ROTARY_PRESS:
            // Perform alarm inactive actions
            led_yellowOff();
            fsm->isAlarmEnabled = false;
            fsm->state = clock_mode;
            return RET_TRANSITION;
        case JOYSTICK_PRESS:
			// Jump to alarm time setting mode on joystick press and remove the display task
            fsm->state = set_time;
            return RET_TRANSITION;
        case ALARM_MATCH:
            fsm->state = alarm_ring;
            return RET_TRANSITION;
        case EXIT:
            return RET_HANDLED;
        default:
            return RET_IGNORED;
    }
}

/**
 * Alarm ring state - perform the alarm ringing operation
 */
fsmReturnStatus alarm_ring(Fsm *fsm, const Event *e)
{
    switch(e->signal)
    {
        case ENTRY:
			// Add red led toggling task for alarm to the scheduler
            alarmTask.task = (task_t)led_redToggle;
            alarmTask.expire = TIME_CONST_250MILLISEC;
            alarmTask.period = TIME_CONST_250MILLISEC;
            scheduler_add(&alarmTask);

			// Add 5 second timeout task for alarm timeout to the scheduler
            timer5secTask.task = (task_t)alarmCounterElapse;
            timer5secTask.expire = TIME_CONST_5SEC;
            timer5secTask.period = 0;
            scheduler_add(&timer5secTask);
            return RET_HANDLED;
        case ROTARY_PRESS:
        case JOYSTICK_PRESS:
        case ALARM_OFF:
            // Exit actions for alarm ring mode
			led_redOff();
            alarmTask.period = 0;
            // scheduler_remove(&alarmTask);
            // scheduler_remove(&timer5secTask);
            fsm->state = alarm_mode;
            return RET_TRANSITION;
        case EXIT:
            return RET_HANDLED;
        default:
            return RET_IGNORED;
    }
}


/* MAIN *************************************************/

int main(void)
{
	// Display interfaces inti
	lcd_init();
	led_redInit();
	led_greenInit();
	led_yellowInit();
	// Button peripherals initializations
	button_init(true);
	// Event generator callback setting								   
	button_setJoystickButtonCallback(joystickEventHandler); 
	button_setRotaryButtonCallback(rotaryEventHandler); 
	// State machine initialization
	fsm_init(&alarmClock_fsm, NULL);

	// Scheduler init
	scheduler_init();

    // Add a display task to continuosly display system time with 1 sec period
    displayTask.task = (task_t)clockDisplay;
    displayTask.expire = TIME_CONST_1SEC;
    displayTask.period = TIME_CONST_1SEC;
    scheduler_add(&displayTask);

	// Adding the compare task to the scheduler
	compareTask.task = (task_t)compareAlarm;
	compareTask.param = (void *)(&alarmClock_fsm);
	compareTask.expire = TIME_CONST_1SEC;
	compareTask.period = TIME_CONST_1SEC;
	scheduler_add(&compareTask);

	// Enable global interrupt
	sei();

	// Super loop
	while (1)
	{
		// Run the scheduler
		scheduler_run();
	}
	return EXIT_SUCCESS;
}