#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_
/* INCLUDES ******************************************************************/
#include <stdbool.h>

typedef enum
{
    JOYSTICK,
    ROTARY
} ButtonType;

typedef enum
{
    NA = 0,
    CW,
    CCW
} RotaryEncoderDirection;

typedef void (*pButtonCallback)(void);     // user defined pointer type for button callback
typedef void (*pTypeRotaryCallback)(void); // user defined pointer type for rotary encoder callback

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes port configuration for rotary encoder and joystick button
 */
void button_portConfig();

/**
 * Initializes rotary encoder and joystick button with external interrupt or timer interrupt for debouncing
 */
void button_init(bool debouncing);

/**
 * Sets callback function to be executed on Rotary button press ISR
 */
void button_setRotaryButtonCallback(pButtonCallback callback);

/**
 * Sets callback function to be executed on Joystick button press ISR
 */
void button_setJoystickButtonCallback(pButtonCallback callback);

/**
 * checks if the button is pressed with debounce mechanism
 */
void button_checkState(void);

/**
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/**
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);

/**
 * Sets callback function to be executed on Rotary encoder clockwise rotation ISR
 */
void button_setRotaryClockwiseCallback(pTypeRotaryCallback);

/**
 * Sets callback function to be executed on Rotary encoder anti-clockwise rotation ISR
 */
void button_setRotaryCounterClockwiseCallback(pTypeRotaryCallback);

#endif /* SES_BUTTON_H_ */