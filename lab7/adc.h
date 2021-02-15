#include <avr/io.h>


#ifndef ADC_H_
#define ADC_H_

#define dacRef		PIN0_bm
#define cdsPlus		PIN1_bm
#define analogOut	PIN3_bm
#define inPlus		PIN4_bm
#define inMinus		PIN5_bm
#define cdsMinus	PIN6_bm

void adc_init(void);

#endif /* ADC_H_ */