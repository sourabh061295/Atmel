/* INCLUDES ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "ses_led.h"
#include <stddef.h>
#include "ses_button.h"
#include "ses_timer.h"

/* DEFINES & MACROS **********************************************************/

// Rotary button wiring on SES board
#define ROTARY_BUTTON_PORT PORTB
#define ROTARY_BUTTON_DDR DDRB
#define ROTARY_BUTTON_BIT 6

// Joystick button wiring on SES board
#define JOYSTICK_BUTTON_PORT PORTB
#define JOYSTICK_BUTTON_DDR DDRB
#define JOYSTICK_BUTTON_BIT 7

// Rotary Encoder wiring on SES board
#define ROTARY_ENCODER_INPUTA_PORT PORTB
#define ROTARY_ENCODER_INPUTB_PORT PORTG
#define ROTARY_ENCODER_INPUTA_DDR DDRB
#define ROTARY_ENCODER_INPUTB_DDR DDRG
#define ROTARY_ENCODER_INPUTA_BIT 5
#define ROTARY_ENCODER_INPUTB_BIT 2
#define ROTARY_A_PIN PINB
#define ROTARY_B_PIN PING

#define BUTTON_NUM_DEBOUNCE_CHECKS 3 // No. of times button needs to validated for debouncing
#define BUTTON_DEBOUNCE_POS_JOYSTICK 0x01
#define BUTTON_DEBOUNCE_POS_ROTARY 0x02

/* PRIVATE VARIABLES *********************************************************/
volatile pButtonCallback JoystickInterruptHandler;
volatile pButtonCallback RotaryInterrupHandler;
volatile pTypeRotaryCallback RotaryClockwiseInterrupHandler;
volatile pTypeRotaryCallback RotaryCounterClockwiseInterrupHandler;

/* FUNCTION DEFINITION *******************************************************/

/**
 * Debounce Handling of Rotary and Joystick button
 */
void button_checkState()
{
    static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {0};
    static uint8_t index = 0;
    static uint8_t debouncedState = 0;
    uint8_t lastDebouncedState = debouncedState;

    state[index] = 0; // each bit in every state byte represents one button
    if (button_isJoystickPressed())
    {
        state[index] |= BUTTON_DEBOUNCE_POS_JOYSTICK;
    }
    if (button_isRotaryPressed())
    {
        state[index] |= BUTTON_DEBOUNCE_POS_ROTARY;
    }
    index++;
    if (index == BUTTON_NUM_DEBOUNCE_CHECKS)
    {
        index = 0;
    }
    // init compare value and compare with ALL reads, only if
    // we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1's" in the state
    // array, the button at this position is considered pressed
    uint8_t j = 0xFF;
    for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++)
    {
        j = j & state[i];
    }
    debouncedState = j;

    if (debouncedState != lastDebouncedState)
    {
        if (debouncedState == BUTTON_DEBOUNCE_POS_ROTARY)
        {
            RotaryInterrupHandler();
        }
        if (debouncedState == BUTTON_DEBOUNCE_POS_JOYSTICK)
        {
            JoystickInterruptHandler();
        }
        debouncedState = 0;
    }
}

RotaryEncoderDirection rotaryEncoder_CheckDirection()
{
    uint8_t stateAValue = (ROTARY_A_PIN & (1 << ROTARY_ENCODER_INPUTA_BIT));
    uint8_t stateBValue = (ROTARY_B_PIN & (1 << ROTARY_ENCODER_INPUTB_BIT));
    RotaryEncoderDirection direction = NA;
    if (stateAValue == 1)
    {
        direction = stateBValue != stateAValue ? CCW : CW;
    }
    return direction;
}

/**
 * Debounce Handling of Rotary Encoder
 */
void rotaryEncoder_checkState()
{
    static RotaryEncoderDirection encoderState[BUTTON_NUM_DEBOUNCE_CHECKS] = {NA};
    static uint8_t index = 0;
    static RotaryEncoderDirection debouncedState = NA;
    RotaryEncoderDirection lastDebouncedState = debouncedState;
    uint8_t direction = rotaryEncoder_CheckDirection();
    if (direction != NA)
    {
        encoderState[index] = direction;
    }    
    index = ++index == BUTTON_NUM_DEBOUNCE_CHECKS ? 0 : index;
    
    // init compare value and compare with ALL reads, only if
    // we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1's" in the state
    // array, the button at this position is considered pressed
    uint8_t j = 0xFF;
    for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++)
    {
        j = j & encoderState[i];
    }
    debouncedState = j;

    if (debouncedState != lastDebouncedState)
    {
        if (debouncedState == CW)
        {
            RotaryClockwiseInterrupHandler();
        }
        if (debouncedState == CCW)
        {
            RotaryCounterClockwiseInterrupHandler();
        }
        debouncedState = NA;
    }
}



void handleDebounce()
{
    button_checkState();
    rotaryEncoder_checkState();
}

void button_portConfig()
{
    ROTARY_BUTTON_DDR &= ~(1 << ROTARY_BUTTON_BIT); // Configure Rotary button as input port
    ROTARY_BUTTON_PORT |= (1 << ROTARY_BUTTON_BIT); // Enable Pull up resistors for Rotary button

    JOYSTICK_BUTTON_DDR &= ~(1 << JOYSTICK_BUTTON_BIT); // Configure Joystick button as input port
    JOYSTICK_BUTTON_PORT |= (1 << JOYSTICK_BUTTON_BIT); // Enable Pull up resistors for joystick button

    ROTARY_ENCODER_INPUTA_DDR &= ~(1 << ROTARY_ENCODER_INPUTA_BIT); // Configure Rotary Encoder A as input port
    ROTARY_ENCODER_INPUTB_DDR &= ~(1 << ROTARY_ENCODER_INPUTB_BIT); // Configure Rotary Encoder B as input port
    ROTARY_ENCODER_INPUTA_PORT |= (1 << ROTARY_ENCODER_INPUTA_BIT); // Enable Pull up resistors for Rotary Encoder A
    ROTARY_ENCODER_INPUTB_PORT |= (1 << ROTARY_ENCODER_INPUTB_BIT); // Enable Pull up resistors for Rotary Encoder B
}

void button_init(bool debouncing)
{
    button_portConfig();
    if (debouncing)
    {
        timer1_setCallback(handleDebounce);
        timer1_start();
    }
    else
    {
        PCICR |= (1 << PCIE0);
        PCMSK0 |= (1 << PCINT7) | (1 << PCINT6);
        EIMSK = 0x00;
        EICRB |= (1 << ISC71) | (1 << ISC70) | (1 << ISC61) | (1 << ISC60);
        EIFR |= (1 << INTF7) | (1 << INTF6);
        EIMSK |= (1 << INT7) | (1 << INT6);
    }
}

bool button_isJoystickPressed(void)
{
    return ((PINB & (1 << JOYSTICK_BUTTON_BIT)) == 0);
}

bool button_isRotaryPressed(void)
{
    return ((PINB & (1 << ROTARY_BUTTON_BIT)) == 0);
}

ISR(PCINT0_vect)
{
    if (button_isRotaryPressed() && (EIMSK & (1 << INT6))) // confirming if rotary button is pressed
    {
        RotaryInterrupHandler();
    }
    if (button_isJoystickPressed() && (EIMSK & (1 << INT7))) // confirming if joystick button is pressed
    {
        JoystickInterruptHandler();
    }
}

void button_setJoystickButtonCallback(pButtonCallback callback)
{
    if (callback != NULL)
    {
        JoystickInterruptHandler = callback;
    }
}

void button_setRotaryButtonCallback(pButtonCallback callback)
{
    if (callback != NULL)
    {
        RotaryInterrupHandler = callback;
    }
}

void button_setRotaryClockwiseCallback(pTypeRotaryCallback callback)
{
    if (callback != NULL)
    {
        RotaryClockwiseInterrupHandler = callback;
    }
}

void button_setRotaryCounterClockwiseCallback(pTypeRotaryCallback callback)
{
    if (callback != NULL)
    {
        RotaryCounterClockwiseInterrupHandler = callback;
    }
}