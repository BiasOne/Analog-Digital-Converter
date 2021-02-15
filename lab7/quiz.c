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
	ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_PIN6_gc; //0b01000010 tried using cdsPlus and cdsMinus and they dont work??
	ADCA.EVCTRL = ADC_EVSEL_0123_gc | ADC_EVACT_CH0_gc; //0b00000100 not sure if its chanels 0-3 or just 1??

	//7_2
	ADCA.CH0.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_LO_gc;
	PMIC.CTRL=PMIC_LOLVLEN_bm;
	sei();

	ADCA.CTRLA = ADC_ENABLE_bm;			//0b00000101
}
void tcc0_init(void)
{
	TCC0.PER = 10417;
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
	EVSYS_CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	
}
//display as positive or negative
//voltage in decimal
//voltage in hex

int intToASCII(int temp)
{
	temp+=48;
	
	return temp;
}

int deciToHex(int temp)
{
	if(temp > 9)
	{
		temp +=55;
	}
	else
	{
		temp += 48;
	}
	
	return temp;
}
int dispTOP =0;
int dispBOT =0;
int dispRIGHT =0;
int randomFlag =0;
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
			//
			if (data > 700 && dispTOP == 0) 
			{
				char TopSideUp[8]= {'B','R','I','G','H','T','!',' '};
				for (int i=0; i < sizeof(TopSideUp); i++)
				{
					usartd0_out_char(TopSideUp[i]);
				}
				usartd0_out_char(0x0a);
				usartd0_out_char(0x0d);
				dispTOP =1;
				dispBOT =0;
				dispRIGHT =0;				
				
			}
			else if(data > 0 && data <=700 && dispBOT == 0)
			{
				char BottomSideUp[16]= {'S','O','M','E','W','H','A','T',' ','B','R','I','G','H','T',' '};
				for (int i=0; i < sizeof(BottomSideUp); i++)
				{
					usartd0_out_char(BottomSideUp[i]);
				}
				usartd0_out_char(0x0a);
				usartd0_out_char(0x0d);
				dispTOP =0;
				dispBOT=1;
				dispRIGHT =0;
				
				
			}
			else if(data <=-200 && dispRIGHT == 0)
			{
				
				char RightSideUp[6]= {'D','A','R','K','!',' '};
				for (int i=0; i < sizeof(RightSideUp); i++)
				{
					usartd0_out_char(RightSideUp[i]);
				}
				usartd0_out_char(0x0a);
				usartd0_out_char(0x0d);
				dispBOT=0;
				dispTOP =0;
				dispRIGHT =1;
				
			}
			
			
			
			randomFlag =0;
		}
		
	}
	
}
ISR(ADCA_CH0_vect)
{
	data = ADCA_CH0_RES;
	ADCA.CH0.INTFLAGS = 1;
	randomFlag = 1;
}
