/* INCLUDES ******************************************************************/
#include <avr/io.h>
#include <stdbool.h>
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// Red LED wiring on SES board
#define LED_RED_PORT PORTG
#define LED_RED_DDR DDRG
#define LED_RED_BIT 1

// Yellow LED wiring on SES board
#define LED_YELLOW_PORT PORTF
#define LED_YELLOW_DDR DDRF
#define LED_YELLOW_BIT 7

// Green LED wiring on SES board
#define LED_GREEN_PORT PORTF
#define LED_GREEN_DDR DDRF
#define LED_GREEN_BIT 6

/* FUNCTION DEFINITION *******************************************************/
void led_Init(LEDColour ledColour)
{
    switch (ledColour)
    {
    case GREEN:
        led_greenInit();
        led_greenOff();
        break;
    case YELLOW:
        led_yellowInit();
        led_yellowOff();
        break;
    case RED:
        led_redInit();
        led_redOff();
        break;
    default:
        break;
    }
}

void led_Toggle(LEDColour ledColour)
{
    switch (ledColour)
    {
    case GREEN:
        led_greenToggle();
        break;
    case YELLOW:
        led_yellowToggle();
        break;
    case RED:
        led_redToggle();
        break;
    default:
        break;
    }
}

void led_redInit(void)
{
    LED_RED_DDR |= (1 << LED_RED_BIT);
}

void led_redToggle(void)
{
    LED_RED_PORT ^= (1 << LED_RED_BIT);
}

void led_redOn(void)
{
    LED_RED_PORT &= ~(1 << LED_RED_BIT);
}

void led_redOff(void)
{
    LED_RED_PORT |= (1 << LED_RED_BIT);
}

void led_yellowInit(void)
{
    LED_YELLOW_DDR |= (1 << LED_YELLOW_BIT);
}

void led_yellowToggle(void)
{
    LED_YELLOW_PORT ^= (1 << LED_YELLOW_BIT);
}

void led_yellowOn(void)
{
    LED_YELLOW_PORT &= ~(1 << LED_YELLOW_BIT);
}

void led_yellowOff(void)
{
    LED_YELLOW_PORT |= (1 << LED_YELLOW_BIT);
}

void led_greenInit(void)
{
    LED_GREEN_DDR |= (1 << LED_GREEN_BIT);
}

void led_greenToggle(void)
{
    LED_GREEN_PORT ^= (1 << LED_GREEN_BIT);
}

void led_greenOn(void)
{
    LED_GREEN_PORT &= ~(1 << LED_GREEN_BIT);
}

void led_greenOff(void)
{
    LED_GREEN_PORT |= (1 << LED_GREEN_BIT);
}