#include <avr/io.h>
#include <avr/interrupt.h>


#define dacRef		PIN0_bm
#define cdsPlus		PIN1_bm
#define analogOut	PIN3_bm
#define inPlus		PIN4_bm
#define inMinus		PIN5_bm
#define cdsMinus	PIN6_bm

int16_t temp;

void adc_init(void)
{
	//7_1
	PORTA.OUTCLR = cdsPlus| cdsMinus | inPlus | inMinus; //0b01110010
	PORTA.DIRCLR = cdsPlus| cdsMinus | inPlus | inMinus;
	ADCA.CTRLB = ADC_CONMODE_bm | ADC_RESOLUTION_12BIT_gc;	//0b00010000
	ADCA.REFCTRL = ADC_REFSEL_AREFB_gc;						//0b00000000 // not sure if its AREFA OR AREFB?
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;				//0b00000111
	ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_PIN6_gc; //0b01000010 tried using cdsPlus and cdsMinus and they dont work??
	ADCA.EVCTRL = ADC_EVSEL_0123_gc | ADC_EVACT_CH0_gc; //0b00000100 not sure if its chanels 0-3 or just 1??
	
	ADCA.CTRLA = ADC_ENABLE_bm;			//0b00000101
}


int main(void)
{	
	
	adc_init();
		
	
    while (1) 
    {		
			ADCA.CH0.CTRL = ADC_CH_START_bm | ADC_CH_INPUTMODE_DIFFWGAIN_gc; //0b10000011 in signed mode inputmode diff w/gain?		
			if(ADCA.CH0.INTFLAGS && ADC_CH0IF_bm)
			{
			temp =ADCA_CH0_RES;
			ADCA.CH0.INTFLAGS = 1;
			}
    }
	
}



	
	