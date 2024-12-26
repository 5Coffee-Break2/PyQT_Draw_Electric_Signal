
//#include <avr/io.h>
#include "KH_ADC.hpp"
//#include "Global_Stufs.hpp"

ADC_Cls::ADC_Cls(uint8_t CHANNEL_NUMBER, enum refVoltage refVolt, enum preScale prscale, enum aLighnment alighn)
{

   Change_Ch_To(CHANNEL_NUMBER);
   

     switch (refVolt)
     {
     case ARef: //AREF,Internal Vref turned off
          ADMUX |= ((0 << REFS1) | (0 << REFS0));
          break;
     case AVcc: //AVcc with external capacitor at AREF
          ADMUX |= (1 << REFS0);
          ADMUX &= (~(1 << REFS1));
          break;
     case Int_Volt: //Internal 2.56V with external capacitor T AREF pin
          ADMUX |= (1 << REFS1 | 1 << REFS0);
          break;
    /* case 2:
     #if (REF_VOLTAGE == 2)
          #error "Refernce Voltage REF_VOLTAGE can't be equal 2"
     #endif
          break;*/
     }

     switch (alighn) //0 is Left  ------------  1 is Right
     {
     case Left: //Result Left Alighnment ADCH (R9...R2) and the reminder 2 bits in ADCL (R0,R1 at bit 6,7 respectively)
          ADMUX |= 1 << ADLAR;
          break;
     case Right: //this is the default Right adjusted  ADCL(R0---R7) and the reminder 2 bits in ADCH(R8,R9 at bit 0 and 1 respectively)
          ADMUX &= ~(1 << ADLAR);
          break;
     default:
          //_  #error "The value of RESULT_ALIGHNMENT is L or R only";
          
          break;
     }
     switch (prscale)
     {
     //_000 or 001  Freq/2
     case Pre2:          
          ADCSRA &= ~(1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0);
          break;
     //_010          Freq/4
     case Pre4:
          ADCSRA &= ~(1 << ADPS2 | 1 << ADPS0); 
          ADCSRA |= (1 << ADPS1);
          break;
     //_011           Freq/8
     case Pre8:
          ADCSRA &= ~(1 << ADPS2); 
          ADCSRA |= (1 << ADPS1 | 1 << ADPS0);
          break;
     //_100          Freq/16
     case Pre16:
          ADCSRA &= ~(1 << ADPS1 | 1 << ADPS0); 
          ADCSRA |= (1 << ADPS2);
          break;
      //_101          Freq/32
     case Pre32:
          ADCSRA &= ~(1 << ADPS1);
          ADCSRA |= (1 << ADPS2 | 1 << ADPS0);
          break;
     //_110          Freq/64
     case Pre64:
          ADCSRA &= ~(1 << ADPS0); 
          ADCSRA |= (1 << ADPS2 | 1 << ADPS1);
          break;
     //_111          Freq/128
     case Pre128: 
          ADCSRA |= (1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0);
          break;
     
     default:
          //*      #error "Prescaler value PRESCALE_VALUE is 2,4,8,16,32,64,128 only";
          break;
     }
     #if (ENABLE_INTURRUPT == 1)
     {
          //_  Enable Interrupt on complete
          ADCSRA |= 1 << ADIE;
          //_ Set general interrupt routain
          sei();
     }
     #endif
    
     #if (ENABLE_FREE_Running == 1) //enable free running mode
     {
          ADCSRA |= 1 << ADFR;
     }
     #endif
     //_ Enable ADC cct to be ready to convert if it's false no conversion will happen
     Start_ADC(true);
    /*//_ In free run mode it shold be called once, but in manual mode should be called every time conversion is required
     Start_Converting();*/
    /* #if (START_CONVERSION_IM == 1) //start one conversion immediately
     {
          ADCSRA |= 1 << ADSC;
     }
     #endif*/
}

void ADC_Cls::Change_Ch_To(uint8_t ch)
{
      DDRC &= ~(1 << ch); //configure the CHANNEL_NUMBER as input
     switch (ch)

     {
     case 0:
          ADMUX &= ~((1 << MUX3) | (1 << MUX2) | ~(1 << MUX1) | ~(1 << MUX0));
          break;
     case 1:
          ADMUX |= (1 << MUX0);
          ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1));
          break;
     case 2:
          ADMUX |= (1 << MUX1);
          ADMUX &= ~((1 << MUX3) | (1 << MUX0) | (1 << MUX2));
          break;
     case 3:
          ADMUX |= (1 << MUX1 | 1 << MUX0);
          ADMUX &= ~((1 << MUX3) | (1 << MUX2));
          break;
     case 4:
          ADMUX |= (1 << MUX2);
          ADMUX &= ~((1 << MUX3) | (1 << MUX0) | (1 << MUX1));
          break;
     case 5:
          ADMUX |= (1 << MUX2 | 1 << MUX0);
          ADMUX &= ~(1 << MUX3 | 1 << MUX1);
          break;
     case 6:
          ADMUX |= ((1 << MUX2) | 1 << MUX1);
          ADMUX &= ~((1 << MUX3) | (1 << MUX0));
          break;
     case 7:
          ADMUX |= (1 << MUX2 | 1 << MUX1 | 1 << MUX0);
          ADMUX &= ~(1 << MUX3);
          break;
          //default:
          //#error "CHANEL_NUMBER value between 0 till 7";
          //   break;
     }
}


void ADC_Cls::Start_ADC(bool start_f =false)
{
     if(start_f)
     //* Activate ADC cct
          ADCSRA |= 1 << ADEN;
     else
          ADCSRA &= ~ (1 << ADEN);

}

void ADC_Cls::Start_Converting()
{
  //start  conversion immediately
     {
          ADCSRA |= 1 << ADSC;
         
     }
}


uint16_t ADC_Cls::ADC_GetResult()
{ //ADLAR must be cleared ADLAR=0 (right alighnment)
     adc_result = ADCL;
     adc_result |= (uint16_t)ADCH << 8;
     return adc_result;
}
/*ISR(ADC_vect)
{
	adc_result=ADCL;
	adc_result |= ((uint16_t)ADCH) << 8;		//get the ADC result
	
    
//}*/

/*  switch (CHANNEL_NUMBER)

     {
     case 0:
          ADMUX &= ~((1 << MUX3) | (1 << MUX2) | ~(1 << MUX1) | ~(1 << MUX0));
          break;
     case 1:
          ADMUX |= (1 << MUX0);
          ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1));
          break;
     case 2:
          ADMUX |= (1 << MUX1);
          ADMUX &= ~((1 << MUX3) | (1 << MUX0) | (1 << MUX2));
          break;
     case 3:
          ADMUX |= (1 << MUX1 | 1 << MUX0);
          ADMUX &= ~((1 << MUX3) | (1 << MUX2));
          break;
     case 4:
          DDRC &= ~(1 << CHANNEL_NUMBER); //configure the CHANNEL_NUMBER as input
          ADMUX |= (1 << MUX2);
          ADMUX &= ~((1 << MUX3) | (1 << MUX0) | (1 << MUX1));
          break;
     case 5:
          ADMUX |= (1 << MUX2 | 1 << MUX0);
          ADMUX &= ~(1 << MUX3 | 1 << MUX1);
          break;
     case 6:
          ADMUX |= ((1 << MUX2) | 1 << MUX1);
          ADMUX &= ~((1 << MUX3) | (1 << MUX0));
          break;
     case 7:
          ADMUX |= (1 << MUX2 | 1 << MUX1 | 1 << MUX0);
          ADMUX &= ~(1 << MUX3);
          break;
          //default:
          //#error "CHANEL_NUMBER value between 0 till 7";
          //   break;
     }*/