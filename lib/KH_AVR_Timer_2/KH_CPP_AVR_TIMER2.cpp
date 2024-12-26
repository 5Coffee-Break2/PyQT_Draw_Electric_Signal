//@ Pre scale value for the timer setup
#define PRE_SCALE 8.0

#include "KH_Atmega8.hpp"
#include "global_vars.hpp"
#include "KH_CPP_AVR_TIMER2.hpp"
//#include "avr/interrupt.h"
    // #include "Global_Stufs.hpp"
#if USE_INTERRUPT == 1 && GET_REAL_TIME == 1
// //---------------=========== Global variables ===========-------------//
// volatile float current_Time; //= 0.0f;
// volatile float ticks;        // = 0.0;
// //_the time required for the counter to count 256 digit (over flow) in seconds
// volatile float ticks_time_256; //= (PRE_SCALE / F_CPU) * 256; // (PRE_SCALE / 16000000L) * 256
// volatile float tick_time;      // = (PRE_SCALE / F_CPU);
#endif

Atmega8Timer2::Atmega8Timer2(bool ovi, bool oci)
{
  current_Time = 0.0f;
   ticks = 0.0;
  //_the time required for the counter to count 256 digit (over flow) in seconds
   ticks_time_256 = (PRE_SCALE / F_CPU) * 256; // (PRE_SCALE / 16000000L) * 256
   tick_time = (PRE_SCALE / F_CPU);
  
  timer_Op_Mode = NormalMode_OP;
   //Timer2InterruptsSetup.En_Tim2OCI_Intr = oci;
  //Timer2InterruptsSetup.En_Tim2OvrFlow_Intr = ovi;
  #if USE_INTERRUPT == 1
    Setup_Timer2Interrupt(ovi,oci);
  #endif
    
}

Atmega8Timer2::Atmega8Timer2(uint8_t value)
{
  timer_Op_Mode = PWMMode_OP;
  DDRB |= 1 << PB3; // set OC2 pin as an output
  OCR2 = value; 
}

//Atmega8Timer2::~Atmega8Timer2()
//{
//}

void Atmega8Timer2::Setup_Timer2PWM(Timer2PWM_Modes pwm_mode,Timer2PWM_OC2_Modes oc2_modes)
{
   switch (pwm_mode)
    {
      case PWM_PhCorrect : 
      //Phase Correct PWM 
        TCCR2 |= 1 << WGM20;
        TCCR2 &= ~(1 << WGM21);
      break;
    
      // Fast PWM
      case PWM_Fast : 
        TCCR2 |= (1 << WGM21 | 1 << WGM20);
      break;
    
      //Normal operation for the counter TCNT2
      default:
        TCCR2 &= ~(1 << WGM20 | 1 << WGM21); 
      break;
    }
   
  switch(oc2_modes)
    {
          //clear OC2 on count matching between TCNT2 and OCR2 and set OC2 in case Fast mode at TCNT2=0 and in case phase correct mode at matching when counting down
          case PWM_OC2_NonInvertingMode :        
            TCCR2 |= 1 << COM21;
            TCCR2 &= ~(1 << COM20);
          break;
         //set the OC2 on counting match between TCNT2 & OCR2  and clear OC2 in case Fast mode at TCNT2=0 and in case phase correct moode at matching when counting down
          case PWM_OC2_InvertingMode :           
            TCCR2 |= 1 << COM21;
            TCCR2 |= (1 << COM20 | 1 << COM21);
          break;
        // Normal operation OC2 is disconnected   
          default:
             TCCR2 &= ~(1 << COM20 | 1 << COM21); 
          break;
    }
}

void Atmega8Timer2::Setup_Timer2CTC(Timer2CTC_OC2_Modes oc2_ctc_mode)
  {

      //setup TCNT2 for CTC mode
        TCCR2 |= 1 << WGM21;
        TCCR2 &= ~(1 << WGM20);
      
      // select one of tha available operation mode     
        switch (oc2_ctc_mode)
        {
                case CTC_OC2_ToggleMode : //Toggle OC2 on compare match
                  TCCR2 |= 1 << COM20;
                  TCCR2 &= ~(1 << COM21);
                break;

                case CTC_OC2_ClearMode : // Clear OC2 on compare match and clear TCNT2
                  TCCR2 |= 1 << COM21;
                  TCCR2 &= ~(1 << COM20);
                break;

                case CTC_OC2_SetMode : //Set OC2 on compare match and clear TCNT2
                  TCCR2 |= (1 << COM20 | 1 << COM21);
                break;
                
                default:
                  TCCR2 &= ~(1 << COM20 | 1 << COM21); // Normal operation OC2 is disconnected
                break;
        }
  } 

#if USE_INTERRUPT == 1
void Atmega8Timer2::Setup_Timer2Interrupt(bool ovi,bool oci)
     {
              
        Timer2InterruptsSetup.En_Tim2OCI_Intr = oci;
        Timer2InterruptsSetup.En_Tim2OvrFlow_Intr = ovi;
        // Enable the General interrupt Flage
        sei();
        // SREG |= 1<<7;  //  sei();               
        //*SREG_ |= 1 << 7;
        if(Timer2InterruptsSetup.En_Tim2OCI_Intr)
          TIMSK |= 1 << OCIE2;         //Enable interrupt on output compare match
        if(Timer2InterruptsSetup.En_Tim2OvrFlow_Intr)
          TIMSK |= 1 << TOIE2;           // Enable timer over flow for Timer2  
     }
#endif

void  Atmega8Timer2::Start_Timer2(Timer2_ClkScaling t2sel)
{
   switch (t2sel)
      {
           case PreSc_1:
                TCCR2 |= 1 << CS20;
                TCCR2 &= ~(1 << CS21 | 1 << CS22);
            break;

            case PreSc_8:
                TCCR2 |= 1 << CS21;
                TCCR2 &= ~(1 << CS20 | 1 << CS22);
            break;

            case PreSc_64:
                TCCR2 |= (1 << CS22);
                TCCR2 &= ~(1 << CS20 | 1 << CS21);
            break;
            
            case PreSc_128:
                 TCCR2 |=  ( 1 << CS20);
                 TCCR2 &= ~(1 << CS22 | 1 << CS21) ;
            break;

            case PreSc_256:
                TCCR2 |= (1 << CS22 | 1 << CS21) ;
                TCCR2 &= ~( 1 << CS20);
            break;

            case PreSc_1024:
                TCCR2 |= (1 << CS22 |1 << CS21| 1 << CS20);
            break;

          //! check the 2 case statment below
            case ExtClk_FallingEdge:
                TCCR2 |= (1 << CS22 | 1 << CS21);
                TCCR2 &= ~(1 << CS20);
            break;

            case ExtClk_RisingEdge:
                TCCR2 |= (1 << CS20 | 1 << CS22 | 1 << CS21);
            break;

            case Pre_OFF:
                TCCR2 &= ~(1 << CS20 | 1 << CS22 | 1 << CS21);
     }     
}

#if (GET_REAL_TIME == 1 && USE_INTERRUPT == 1)
void Atmega8Timer2::Get_Current_Time()
{
  cli();
  ticks++;
  current_Time = ((ticks_time_256 * ticks)+(tick_time *TCNT2)) ;


  //_ ADC_ptr->Start_Converting();
  //_  //   //_Wait until Conversion Completes
  //_ while (!(ADCSRA & (1<<ADSC))){};
  //_ conv_result = ADC_ptr->ADC_GetResult();
  //_ serPortPtr-> Transmit_long(conv_result);
  //_ PORTB ^= 1 << PB1;
  //_  _delay_ms(0.5);
  //_   serPortPtr-> Transmit_Float(ticks,12,10);
  //_     _delay_ms(0.5);
  sei();
}
#endif
