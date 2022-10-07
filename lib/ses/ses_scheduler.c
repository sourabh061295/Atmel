/*INCLUDES *******************************************************************/
#include <stdlib.h>
#include <util/atomic.h>
#include "ses_timer.h"
#include "ses_scheduler.h"
#include "ses_led.h"
#include "util/atomic.h"

/* DEFINES & MACROS **********************************************************/
#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)

/* PRIVATE VARIABLES *********************************************************/
static task_descriptor_t *taskList = NULL;
static system_time_t SystemTime = 0; // Global variable to store the time

/*FUNCTION DEFINITION *************************************************/
static void scheduler_update(void)
{
    task_descriptor_t *tempTaskpointer = taskList;
    // Loop until last element of the list is reached
    while (tempTaskpointer != NULL)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            // Check if the task has expired
            if (!taskList->expire)
            {
                tempTaskpointer->execute = 1;
                // Reset the expire time with period for periodic tasks
                tempTaskpointer->expire = tempTaskpointer->period;
            }
            else
            {
                tempTaskpointer->expire--;
            }
        }
        tempTaskpointer = tempTaskpointer->next; // fetch next task in the list
    }
    SystemTime++;
}

void scheduler_init()
{
    // Initializes the timer callaback to scheduler update
    timer2_setCallback(scheduler_update);

    // Initializes Timer2 for 1ms
    timer2_start();
}

void scheduler_run()
{
    task_descriptor_t *temp = taskList;
    // Loop until last element of the list is reached
    while (temp != NULL)
    {
        // Check if task can be executed
        if (temp->execute)
        {
            temp->task(temp->param); // execute the task
            if (temp->execute)
            {
                temp->task(temp->param);
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
                {
                    scheduler_remove(temp); // Remove the task from task list if its aperiodic
                }
            }
            if (!(temp->period))
            {
                scheduler_remove(temp);
            }
            
        }
        temp = temp->next; // fetch next task in the list
    }
}

bool scheduler_add(task_descriptor_t *toAdd)
{
    task_descriptor_t *tempTaskptr;
    bool isDuplicateElementPresent = false;
    /*
     * We make sure that the list is accessed only within atomic sections
     * protected by a memory barrier --> no volatile necessary
     */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Clearing the execute flag
        toAdd->execute = 0;
        // Initialize the expire timer with the period
        toAdd->expire = toAdd->period;
        // Initialize the next pointer of the task to NULL
        toAdd->next = NULL;
        // Check if the taskList has any previously added elements
        if (taskList != NULL)
        {
            tempTaskptr = taskList;
            // Loop until last element of the list is fetched
            while (tempTaskptr->next != NULL)
            {
                // Check if any matching element exists
                if (tempTaskptr == toAdd)
                {
                    isDuplicateElementPresent = true;
                }
                // Initialise the pointer to point next task
                tempTaskptr = tempTaskptr->next;
            }
            tempTaskptr->next = toAdd;
        }
        else
        {
            taskList = toAdd;
        }
    }
    return isDuplicateElementPresent;
}

void scheduler_remove(const task_descriptor_t *toRemove)
{
    task_descriptor_t *temp, *prev;
    /*
     * We make sure that the list is accessed only within atomic sections
     * protected by a memory barrier --> no volatile necessary
     */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (taskList == toRemove)
        {
            // Assign the head pointer to the next task address
            taskList = taskList->next;
        }
        else
        {
            // Set the task pointer to the head of the list
            temp = taskList;
            // Loop through the list to find the task to be removed
            while (temp != NULL)
            {
                // Find the key to be deleted
                if (temp != toRemove)
                {
                    // Make note of the previos task pointer
                    prev = temp;
                    // Assign the head pointer to the next task address
                    temp = temp->next;
                    // Breaking out after finding the task
                    break;
                }
            }
            // Severe the pointer of the previous task and make a new connection to the next of the removed task
            prev->next = temp->next;
        }
    }
}
/**
 * Get the current system time.
 */
system_time_t scheduler_getTime()
{
    return SystemTime;
}

/**
 * Set the system time.
 */
void scheduler_setTime(system_time_t time)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 
    {
        SystemTime = time;
    }
}

/**
 * Convert system time to human readable time 
 */
time_t getClockTime(void)
{
    time_t time;
    system_time_t currTime;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) 
    {
        currTime = SystemTime;
    }
    time.hour = (uint8_t)(currTime / HOUR_TO_MILLI);
    currTime -= (time.hour * HOUR_TO_MILLI);
    time.minute = (uint8_t)(currTime / MINUTE_TO_MILLI);
    currTime -= (time.minute * MINUTE_TO_MILLI);
    time.second = (uint8_t)(currTime / SECOND_TO_MILLI);
    currTime -= (time.second * MINUTE_TO_MILLI);
    time.milli = (uint16_t)currTime;
    return time;
}

/**
 * Convert human readable time to system time 
 */
system_time_t clock_to_systemTime(time_t time)
{
    return (system_time_t)((time.hour * HOUR_TO_MILLI) + 
                            (time.minute * MINUTE_TO_MILLI) +
                            (time.second * SECOND_TO_MILLI) +
                            time.milli);
}