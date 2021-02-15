#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"

#define dacRef		PIN0_bm
#define cdsPlus		PIN1_bm
#define analogOut	PIN3_bm
#define inPlus		PIN4_bm
#define inMinus		PIN5_bm
#define cdsMinus	PIN6_bm

int16_t data;
float voltage;

void adc_init(void)
{
	//7_1
	PORTA.OUTCLR = cdsPlus| cdsMinus | inPlus | inMinus; //0b01110010
	PORTA.DIRCLR = cdsPlus| cdsMinus | inPlus | inMinus;	
	ADCA.CTRLB = ADC_CONMODE_bm | ADC_RESOLUTION_12BIT_gc;	//0b00010000
	ADCA.REFCTRL = ADC_REFSEL_AREFB_gc;						//0b00000000 // not sure if its AREFA OR AREFB?
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;				//0b00000111
	//ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXPOS_PIN6_gc; //0b01000010 tried using cdsPlus and cdsMinus and they dont work??
	ADCA.EVCTRL = ADC_EVSEL_0123_gc | ADC_EVACT_CH0_gc; //0b00000100 not sure if its chanels 0-3 or just 1??
	
	//7_2
	ADCA.CH0.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_LO_gc;
	PMIC.CTRL=PMIC_LOLVLEN_bm;
	sei();
	
	ADCA.CTRLA = ADC_ENABLE_bm;			//0b00000101
}
void tcc0_init(void)
{
	TCC0.PER = 20000;
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;
	EVSYS_CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	
}

int randomFlag=0;
int main(void)
{
	
	adc_init();
	tcc0_init();
	usartd0_init();
	
	ADCA.CH0.CTRL = ADC_CH_START_bm | ADC_CH_INPUTMODE_DIFFWGAIN_gc; //0b10000011 in signed mode inputmode diff w/gain?
	while (1)
	{
	if(randomFlag)
		{
			usartd0_out_char(data & 0xFF);
			usartd0_out_char((data >> 8)& 0xFF);		
			randomFlag = 0;
		}
	}
}
ISR(ADCA_CH0_vect)
{
	data = ADCA_CH0_RES;
	ADCA.CH0.INTFLAGS = 1;
	randomFlag=1;
}

ISR(USARTD0_RXC_vect)
{
	int16_t temp=usartd0_in_char(); //same as usart data
	if(temp==0x43) //hex = 0x4A or dec=74 'c'
	{	
		ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_PIN6_gc; //cds+ and cds- ping 1 and 6

	}
	else if(temp==0x4A ) // hex = 43 dec=67 'j'
	{
		
		ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN4_gc | ADC_CH_MUXNEG_PIN5_gc; //in0+ and in0- pin 4/5
	}
	
}




