#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"

void adc_init(void)
{
	PORTA.OUTCLR = cdsPlus| cdsMinus | inPlus | inMinus; //0b01110010
	PORTA.DIRCLR = cdsPlus| cdsMinus | inPlus | inMinus;	
	ADCA.CTRLA = ADC_CH0START_bm | ADC_ENABLE_bm;			//0b00000101
	ADCA.CTRLB = ADC_CONMODE_bm | ADC_RESOLUTION_12BIT_gc;	//0b00010000	
	ADCA.REFCTRL = ADC_REFSEL_AREFB_gc;						//0b00000000 // not sure if its AREFA OR AREFB?
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;				//0b00000111	
	ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_PIN6_gc; //0b01000010 tried using cdsPlus and cdsMinus and they dont work??	
	ADCA.EVCTRL = ADC_EVSEL_0123_gc | ADC_EVACT_CH0_gc; //0b00000100 not sure if its chanels 0-3 or just 1??
}