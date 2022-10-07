/*INCLUDES *******************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "ses_lcd.h"
#include "ses_fanspeed.h"
#include "util/atomic.h"

/* DEFINES & MACROS **********************************************************/
#define TACHO_PORT PORTD
#define TACHO_DDR DDRD
#define TACHO_PIN 0

#define TIMER5_CYC_FOR_1SEC 0xC2F6 // Reducing actual value 0xC2F7 by one to compensate the time spent in reset
#define BUFFER_SIZE 5              // Size of ring buffer maintained for fan speed
/* PRIVATE VARIABLES *********************************************************/

volatile uint16_t edgeCount = 0;                // Global variable to capture the edge count
volatile pTimerCallback Timer5Callback;         // function to execute on Timer 5 ISR
volatile uint16_t edgeCountBuffer[BUFFER_SIZE]; // Ring buffer to maintain fan speed
volatile uint8_t bufferIndex = 0;               // Initial index of ring buffer

/* FUNCTION PROTOTYPES *******************************************************/

// Initializes the Tacho
void Tacho_Init()
{
    TACHO_DDR &= ~(1 << TACHO_PIN);       // Initialising TACHO pin as Input
    PCMSK0 |= (1 << PCINT0);              // Enabling pin change interrupt
    EIMSK = 0x00;                         // Clearing Interrupt Enable bit to disable interrupts on EICRA bits change
    EICRA |= (1 << ISC01) | (1 << ISC00); // Enabling interrupt on rising edge of INT0
    EIFR &= ~(1 << INTF0);                // Clearing the interrupt flag
    EIMSK |= (1 << INT0);                 // Enable external interrupt on PD0
}

// Initializes the Timer 5
void Timer5_Init()
{
    PRR1 &= ~(1 << PRTIM5);                   // Enable Timer 0
    TCCR5A &= ~((1 << WGM51) | (1 << WGM50)); // Enable CTC mode with TOP as OCRA value
    TCCR5B |= (1 << WGM52) & ~(1 << WGM53);
    TCCR5B |= (1 << WGM02);
    OCR5A = TIMER5_CYC_FOR_1SEC;                          // Setting the TOP value for compare
    TIMSK5 |= (1 << OCIE5A);                              // To enable Timer/Counter1 Output Compare A Match interrupt & Overflow interrupt
    TIFR5 |= (1 << OCF5A);                                // To reset the interrupt flag
    TCCR5B |= ((1 << CS52) | (1 << CS50)) & ~(1 << CS51); // Set prescaler to 1024
}

void fanspeed_init(void)
{
    Tacho_Init();  // Enable external interrupt on Tach connected PIN
    Timer5_Init(); // Enable Timer 5 to calculate speed of fan per second
}

// Returns speed in RPS from edge count
uint16_t fanspeed_getScaledSpeed(uint16_t edge)
{
    uint16_t actualEdgeCount = 0;
    if (edge % 2 == 0)
    {
        actualEdgeCount = (edge / 2);
    }
    else
    {
        actualEdgeCount = (edge - 1) / 2;
    }
    return actualEdgeCount;
}

uint16_t fanspeed_getRecent(void)
{
    return fanspeed_getScaledSpeed(edgeCount) * 60;
}

void fanSpeed_setTimer5Callback(pTimerCallback timerCallback)
{
    Timer5Callback = timerCallback;
}

// Returns median of the Fanspeed array
float fanspeed_getMedian(uint16_t temp[])
{
    float median = 0;
    if (BUFFER_SIZE % 2 == 0)
    {
        median = (temp[(BUFFER_SIZE - 1) / 2] + temp[BUFFER_SIZE / 2]) / 2.0;
    }
    else
    {
        median = temp[BUFFER_SIZE / 2];
    }
    return median;
}

uint16_t fanspeed_getFiltered(void)
{
    uint16_t temp = 0;
    uint16_t tempEdgeCountBuffer[BUFFER_SIZE];
    float medianSpeed = 0.0;
    for (uint8_t i = 0; i < BUFFER_SIZE; i++)
    {
        tempEdgeCountBuffer[i] = edgeCountBuffer[i];
    }

    for (uint8_t x = 0; x < BUFFER_SIZE - 1; x++)
    {
        for (uint8_t y = 0; y < BUFFER_SIZE - x - 1; y++) // Sorting array in ascending order
        {
            if (tempEdgeCountBuffer[y] > tempEdgeCountBuffer[y + 1])
            {
                temp = tempEdgeCountBuffer[y];
                tempEdgeCountBuffer[y] = tempEdgeCountBuffer[y + 1];
                tempEdgeCountBuffer[y + 1] = temp;
            }
        }
    }
    medianSpeed = fanspeed_getMedian(tempEdgeCountBuffer);
    return fanspeed_getScaledSpeed(medianSpeed) * 60;
}

ISR(INT0_vect)
{
    edgeCount++; // Incrementing the previous edge count by one
}

ISR(TIMER5_COMPA_vect)
{
    Timer5Callback(); // Execute the function pointed by the callback pointer
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        edgeCountBuffer[(bufferIndex++) % BUFFER_SIZE] = edgeCount;
        edgeCount = 0;
    }
}