/*INCLUDES-------------------------------------------------------------------*/
#include <inttypes.h>
#include <avr/io.h>
#include "ses_adc.h"

/* DEFINES & MACROS **********************************************************/
#define ADC_INVALID_CHANNEL 0xFFFF //to signal that the given channel was invalid
#define ADC_VREF_SRC 0x03
#define ADC_MUX_MASKBITS 0x1F
#define ADC_TEMP_RAW_LOW 648
#define ADC_TEMP_HIGH 345
#define ADC_TEMP_LOW 10
#define ADC_TEMP_RAW_HIGH 30

/* FUNCTION DEFINITION *******************************************************/
void adc_init(void)
{
    DDRF |= 0x00;
    PORTF |= 0x00;
    PRR0 &= ~(1 << PRADC);
    DDRF &= (~((1 << 2) | (1 << 4) | (1 << 5) | (1 << 0) | (1 << 1)));
    ADMUX |= (1 << REFS1) & (1 << REFS0);
    ADMUX = ADC_VREF_SRC;
    ADMUX &= ~(1 << ADLAR);
    ADCSRA |= ((1 << ADPS1) | (1 << ADPS0) & ~(1 << ADPS2));
    ADCSRA &= ~(1 << ADATE);
    ADCSRA |= (1 << ADEN);
}

uint16_t adc_read(uint8_t adc_channel)
{
    if (adc_channel >= ADC_NUM)
    {
        return ADC_INVALID_CHANNEL;
    }
    else
    {
        ADMUX |= ADC_MUX_MASKBITS & adc_channel;
        ADCSRA |= (1 << ADSC);
        while ((ADCSRA & (1 << ADSC)))
        {

        }
        return (int16_t)ADC;
    }
}

int16_t adc_getTemperature(void)
{
    int32_t adc = adc_read(ADC_TEMP_CH);
    return (int16_t)(((adc - ADC_TEMP_RAW_LOW) * (ADC_TEMP_HIGH - ADC_TEMP_LOW)) / (ADC_TEMP_RAW_HIGH - ADC_TEMP_RAW_LOW)) + ADC_TEMP_LOW;
}