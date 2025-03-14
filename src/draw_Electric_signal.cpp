#define USE_LCD 1
//@ UART setting
#define BAUD_RATE 57600 // 38400//57600//115200

//@Timer 2 setting
#define PRE_SCALE 8.0
// #define CONV_DELAY 0.5 // in milisecond
//-------------====== setting for tha app =======--------------------------------//

#define CHRGING_PIN 0
#define CHARGING_PORT_DDR DDRB
#define CHARGING_PORT PORTB

#define DIS_CHRGING_PIN 7
#define DIS_CHARGING_PORT_DDR DDRD
#define DIS_CHARGING_PORT PORTD
//________________________/
#define SETUP_CHARGING_PORT (CHARGING_PORT_DDR |= 1 << CHRGING_PIN)
// #define SET_CHRG_PORT() (SET_CHARGING_PORT |= 1 << CHRGING_PIN);
#define START_CHARGING (CHARGING_PORT |= (1 << CHRGING_PIN))   //() (CHARGING_PORT |= 1 << CHRGING_PIN);
#define STOP_CHARGING (CHARGING_PORT &= (~(1 << CHRGING_PIN))) //() (CHARGING_PORT &= ~(1 << CHRGING_PIN));

#define SETUP_DIS_CHARGING_PORT (DIS_CHARGING_PORT_DDR |= 1 << DIS_CHRGING_PIN)
//For PNP Transistor
#define STOP_DIS_CHARGING (DIS_CHARGING_PORT |= (1 << DIS_CHRGING_PIN))
#define START_DIS_CHARGING (DIS_CHARGING_PORT &= (~(1 << DIS_CHRGING_PIN)))

//_------------------------------------------------------------------------
void Transmit_Charge_Discharge_Data();
//_==============================================================================================================================_//
#include "KH_Atmega8.hpp"
#include "KH_USART_CPP.hpp"
#include "KH_CPP_AVR_TIMER2.hpp"
#include "KH_ADC.hpp"
#include "Global_Stufs.hpp"
#if USE_LCD == 1
#include "LCDHD44780_AVRLib.hpp"

#endif
// global variables to deal with the LCD defined and initialized in the USART source file
// LCD_HD44780 *TLCD_ptr;
//!extern LCD_HD44780 *LCD_ptr;
//!extern LineNumber g_Line;
//!extern uint8_t g_Pos;
//!extern Page g_Page;
#if GET_REAL_TIME == 1
//---------------=========== Extern Global variables ============---------------//
extern volatile float current_Time;
extern volatile float ticks;
extern volatile float tick_time;
//_the time required for the counter to count 256 digit (over flow) in seconds
extern volatile float ticks_time_256;
#endif
//_UART Global Variables
extern ATMG8A_USART *mSerPtr;

//---------------=========== Global variables ============---------------//
ADC_Cls *ADC_ptr;
Atmega8Timer2 *timerPtr;
volatile uint16_t conv_result = 0;
bool loop4Ever = true;
volatile float getting_time = 0.0f;
volatile int float_Nums_Count = 0;
volatile int int_Nums_Count = 0;
bool charging_completed = false;

int main()
{
  // DDRD |= 1 << PD1 ;
  // DDRD &= ~(1 << PD4);
  
  ATMG8A_USART mSerial_Port(BAUD_RATE, true, true);
  Atmega8Timer2 mTimer(true, false);
  timerPtr = &mTimer;
  ADC_Cls mADC(PC5);
  ADC_ptr = &mADC;
  // long rxed_lng = 0;
  SETUP_CHARGING_PORT; // CHARGING_PORT_DDR |= 1 << CHRGING_PIN;
  SETUP_DIS_CHARGING_PORT;
  Set_Leds();
#if USE_LCD == 1
  // LCD constants are defined in
  LCD_HD44780 main_LCD(
      DP4, DP5, DP6, DP7,
      DPP4, DPP5, DPP6, DPP7,
      DRS, DE, DRW,
      PORT_D_, PORT_D_, PORT_D_);
  //LCD_ptr = &main_LCD;
  //-main_LCD.LCD_Show_Character('A');
  //-main_LCD.LCD_Show_Character('B',1,Line_2);
  //!printf("Test Message");
  //!g_Line = Line_2;
  //!printf("I:%d  F:%f", 9, 12.236);
  //!g_Line = Line_1;
#endif

  // Wink_Led3(90000);
  while (1)
  {
    ////_ if the switch is pressed
    // _delay_ms(900);
    // PORTD ^= 1 << PD6;
    // Wink_Led3(150000);

    // my_Delay_Fn(50000);
    _delay_ms(300);

    if (!(PINC & (1 << PC4)))
    {
      //- if(disccharge_cmd)
      //- {
      //-   START_DIS_CHARGING;
      //-   disccharge_cmd = false;
      //- }
      //- else
      
      { 
        //- STOP_DIS_CHARGING;
        main_LCD.LCD_Clear();
       Wink_Led2();
        //(LED_2_Outp) ^= 1 << (LED_2_Pin);
        //_delay_ms(300);
        //(LED_2_Outp) ^= 1 << (LED_2_Pin);
        
        Transmit_Charge_Discharge_Data();
      }
    }
  }
}

//@ The constant below is defined in the timer header file
#if USE_INTERRUPT == 1

ISR(TIMER2_OVF_vect)
{
  cli();
  ticks++;
  current_Time = ((ticks_time_256 * ticks) + (tick_time * TCNT2));

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

void Transmit_Charge_Discharge_Data()
{
  LCD_Ptr->LCD_Clear();
  //_reset the ticks and the counter
  ticks = 0;
  TCNT2 = 0;
//-Reset the current time and float and integer numbers count
  current_Time = 0.0;
  float_Nums_Count = 0;
  int_Nums_Count = 0;

  //* in case of free running conversion remove comment to start adc cct
  //  ADC_ptr->Start_ADC(true)     ;
  
  // Needed for PNP transistors when used to discharge the capacitor
  STOP_DIS_CHARGING;
  //* in case of free run conversion remove comment
  loop4Ever = true;
  ADC_ptr->Start_Converting(); // dummy 1 conversion
  _delay_us(2);

  if(!charging_completed)
  //@ start charging the capacitor
    START_CHARGING; // CHARGING_PORT |= 1 << CHRGING_PIN; // PORTC |= 1 << PC5;
  else
    START_DIS_CHARGING;

  //_ Start Timer 2
  timerPtr->Start_Timer2(timerPtr->PreSc_8); //(timer_2.PreSc_1024);

  do
  { //* in case of manual conversion remove comment
    // mSerPtr->Send_Nums<float>("%2.3f", current_Time);
    // mSerPtr->Transmit_Number(SOB_F, "%2.3f", current_Time, 0);
    ADC_ptr->Start_Converting();
    //! _delay_us(1);
    
    //_Wait until Conversion Completes
    while ((ADCSRA & (1 << ADSC)))
    {
    };
    
    mSerPtr->Transmit_Number(SOB_F, "%2.4f", current_Time, 0);
    float_Nums_Count++;
    
    conv_result = ADC_ptr->ADC_GetResult();
    _delay_us(SERIES_TX_DELAY); //_delay_us(100); //.1 ms
    mSerPtr->Transmit_Number(SOB_I, "%ld", 0, conv_result);
    int_Nums_Count++;
    
    // _delay_ms(0.1);  //0.25 ms
    _delay_us(SERIES_TX_DELAY); // _delay_us(100); //.1 ms //100us
    // TLCD_ptr->LCD_Clear();
    //-g_Pos=1;
    //-printf("%d",conv_result);
    //-LCD_ptr->LCD_Clear();
  
    if(!charging_completed)
    {
      g_Pos = 1;
      printf("C:%d", conv_result);
      if (conv_result >=1023) // 1023 /*|| loop4Ever == false*/ //-if (!(PIND & (1 << PD6)))
      {

        //@ Stoptimer 2
        timerPtr->Start_Timer2(timerPtr->Pre_OFF);
        loop4Ever = false;
        //@ stop charging the capacitor
        STOP_CHARGING; // CHARGING_PORT &= (~(1 << CHRGING_PIN)); // PORTC &= (~(1 << PC5));
        // update charging flag
        charging_completed = true;
        Wink_Led2();
      }
    }

    else if (charging_completed)
    {
      g_Pos = 9;
      printf("D:%d", conv_result);
      if(conv_result <= 10) 
        {
          //@ Stoptimer 2
          timerPtr->Start_Timer2(timerPtr->Pre_OFF);
          loop4Ever = false;
          //@ stop disscharging the capacitor
          STOP_DIS_CHARGING; 
        // update charging flag
          charging_completed = false;
          Wink_Led2();
      }
    }
      my_Delay_Fn(20000);
    }
    

while(loop4Ever); // for live data plot
//needed for PNP transistors when used to discharge the capacitor
START_DIS_CHARGING;
// Reset the ADC converted result value 
conv_result = 0;
 // Display the totan number of float and integer numbers transmitted
 g_Line = Line_1;
 g_Pos = 1;
 printf("F:%d", float_Nums_Count);
 g_Pos = 9;
 printf("I:%d", int_Nums_Count);

 // Display the total transmssion time period
 g_Line = Line_2;
 g_Pos=1;
 printf("T %f",(double)current_Time);
 
}
