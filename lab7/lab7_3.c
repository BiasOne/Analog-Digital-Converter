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
		voltage = ((float)data/(float)2048)*(float)2.5;		
		int dig1 = (int) voltage;
		double voltage2 = 10*(voltage - dig1);
		int dig2 = (int) voltage2;
		double voltage3 = 10*(voltage2 - dig2); 
		int dig3 = (int) voltage3; 
				
		dig1=intToASCII(abs(dig1));
		dig2=intToASCII(abs(dig2));
		dig3=intToASCII(abs(dig3));			
				
		if(voltage < 0)
		usartd0_out_char('-');	
		else if(voltage >=0)
		usartd0_out_char('+');	
			
		usartd0_out_char(dig1);		
		usartd0_out_char('.');		
		usartd0_out_char(dig2);
		usartd0_out_char(dig3);		
		usartd0_out_char('V');		
		usartd0_out_char(' ');
			
		//deci to hex conversion
		int hexDig1 = (int)data/16;
		int hexDig2 = (int)data%16;
		int hexDig3 = (int)hexDig1/16;
		hexDig1 = hexDig1%16;		
		
		hexDig1 = deciToHex(abs(hexDig1));
		hexDig2 = deciToHex(abs(hexDig2));
		hexDig3 = deciToHex(abs(hexDig3));		
		//hex
		usartd0_out_char('(');
	    usartd0_out_char('0');
		usartd0_out_char('x');		
		usartd0_out_char(hexDig3);
		usartd0_out_char(hexDig1);
		usartd0_out_char(hexDig2);
		usartd0_out_char(')');
		//new line
		usartd0_out_char(0x0a);
		usartd0_out_char(0x0d);
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
