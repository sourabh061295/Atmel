#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/* INCLUDES *****************************************************************/
#include <stdbool.h>
#include <stdint.h>

/* DEFINES & MACROS **********************************************************/
#define DAY_TO_MILLI          86400000UL  // 24H * 60Min * 60Sec * 1000ms 
#define HOUR_TO_MILLI         3600000UL   // 60Min * 60Sec * 1000ms
#define MINUTE_TO_MILLI       60000UL     // 60Sec * 1000ms
#define SECOND_TO_MILLI       1000UL      // 1000ms

/* TYPES ********************************************************************/

/**
 * Type of function pointer for tasks
 */
typedef void (*task_t)(void *);

/**
 * Task structure to schedule tasks
 */
typedef struct task_descriptor_s
{
   task_t task;                    // function pointer to call
   void *param;                    // pointer, which is passed to task when executed
   uint16_t expire;                // time offset in ms, after which to call the task
   uint16_t period;                // period of the timer after firing; 0 means exec once
   uint8_t execute : 1;            // for internal use
   uint8_t reserved : 7;           // reserved
   struct task_descriptor_s *next; // pointer to next task, internal use
} task_descriptor_t;

typedef uint32_t system_time_t;

typedef struct time_s
{
   uint8_t hour;
   uint8_t minute;
   uint8_t second;
   uint16_t milli;
} time_t;


/** typedef for state event handler functions */

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes the task scheduler. Uses hardware timer2 of the AVR.
 */
void scheduler_init(void);

/**
 * Runs scheduler in an infinite loop.
 */
void scheduler_run(void);

/**
 * Adds a new task to the scheduler.
 *
 * @param td   Pointer to taskDescriptor structure. The scheduler takes
 *             possesion of the memory pointed at by td until the task
 *             is removed by scheduler_remove or a non-periodic task is
 *             executed. td->expire and td->execute are written to by
 *             the task scheduler
 *
 * @return     false, if task is already present or invalid (NULL)
 *             true, if task was successfully added to scheduler and will be
 *             scheduled after td->expire ms
 */
bool scheduler_add(task_descriptor_t *td);

/**
 * Removes a timer task from the scheduler.
 *
 * @param td	pointer to task descriptor to remove
 * */
void scheduler_remove(const task_descriptor_t *td);
/**
 * Get the current system time.
 */
system_time_t scheduler_getTime();

/**
 * Set the system time.
 */
void scheduler_setTime(system_time_t time);

/**
 * Returns system time to human readable time 
 */
time_t getClockTime(void);

/**
 * Convert human readable time to system time 
 */
system_time_t clock_to_systemTime(time_t);

#endif /* SCHEDULER_H_ */