#include "ses_led.h"
#include "ses_adc.h"
#include "ses_lcd.h"
#include <util/delay.h>

int main(void)
{
	uart_init(9600);
	lcd_init();
	adc_init();
	while (1)
	{
		// uint16_t temp_result = adc_getTemperature();
		fprintf(lcdout,"\n \n \n");
		// fprintf(lcdout,"Temperatrue result : %d \n \n \n",temp_result);
		// uint16_t light_result = adc_read(ADC_LIGHT_CH);
		// fprintf(lcdout,"Light Sensor result : %d \n",light_result);
	}
	return 0;
}