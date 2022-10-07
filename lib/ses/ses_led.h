#ifndef SES_LED_H_
#define SES_LED_H_

/* TYPES ********************************************************************/
typedef enum
{
    GREEN,
    YELLOW,
    RED
}LEDColour;

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * @brief Generic method to initialize LED
 * 
 * @param ledColour Green, Red, or Yellow
 */
void led_Init(LEDColour ledColour);

/**
 * @brief Generic method to toggle LED
 * 
 * @param ledColour Green, Red, or Yellow
 */
void led_Toggle(LEDColour ledColour);

/**initializes red led
 */
void led_redInit(void);

/**toggles red led
 */
void led_redToggle(void);

/**enabled red led
 */
void led_redOn(void);

/**disables red led
 */
void led_redOff(void);

/**initializes yellow led
 */
void led_yellowInit(void);

/**toggles yellow led
 */
void led_yellowToggle(void);

/**enabled yellow led
 */
void led_yellowOn(void);

/**disables yellow led
 */
void led_yellowOff(void);

/**initializes green led
 */
void led_greenInit(void);

/**toggles green led
 */
void led_greenToggle(void);

/**enabled green led
 */
void led_greenOn(void);

/**disables green led
 */
void led_greenOff(void);

/**Control Yellow led with joystick button and turnoff on timer reach
 */
void yellowLed_stateControl(void);

#endif /* SES_LED_H_ */
