#ifndef KH_ADC_CPP
#define KH_ADC_CPP
#include "KH_Atmega8.hpp"
//#ifndef __AVR_ATmega8__         //change it if you use another AVR model
//  #define __AVR_ATmega8__     //determine the MicroContoller type this is necessary to load the definition from io.h file   
//#endif

//*     Right alignment          ADCH                           ADCL    
//*                       0 0 0 0 0 0 R9 R8             R7 R6 R5 R4  R3 R2 R1 R0
//*                       b7          b1 b0             b7............ .......b0

 //*    Left alignment            ADCH                            ADCL    
 //*                      R9 R8 R7 R6 R5 R4 R3 R2       R1 R0 0 0 0 0 0 0 
 //*                      b7................b1 b0       b7 b6... .......b0
// ADC Parameters
				
//@ Default settings for ADC					
#define ENABLE_INTURRUPT    0                       //*: Enable interrupt on conversion complete
#define START_CONVERSION_IM 0                       //*: START one CONVERSION Immediately after initialization
#define ENABLE_FREE_Running 0                       //*: enable free running mode of conversion  0: disable it

#ifndef _AVR_IO_H_  
	#include <avr/io.h>
#endif

#if(ENABLE_INTURRUPT == 1)
	#ifndef _AVR_INTERRUPT_H_
    #include "avr/interrupt.h"
  #endif
#endif
  
  //ADC Variables 
//> define the reference voltage source value 0,1 and 2 (default is 1 : AVCC as reference voltage)
enum refVoltage {ARef,AVcc,Int_Volt=3} ;   

//> value range 200k > F >50K
enum preScale {Pre2,Pre4,Pre8,Pre16,Pre32,Pre64,Pre128};      	

//>value L or R (Right alignment is the default) 0 is Left 1 is Right
enum aLighnment {Left,Right};             
//extern volatile uint16_t adc_result;
class ADC_Cls
{
 public:
 ADC_Cls (uint8_t adc_pin_no,enum refVoltage refvol = AVcc ,enum preScale prsc = Pre128,enum aLighnment align = Right);

uint16_t ADC_GetResult();
void Change_Ch_To(uint8_t ch);

void Start_ADC(bool);

void Start_Converting();


private:
volatile uint16_t adc_result;

};
#endif
