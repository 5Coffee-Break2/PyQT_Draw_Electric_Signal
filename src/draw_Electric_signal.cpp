#define USE_LCD 1
//@ UART setting
#define BAUD_RATE 57600 // 38400//57600//115200

//@Timer 2 setting
#define PRE_SCALE 8.0
// #define CONV_DELAY 0.5 // in milisecond
//-------------====== setting for tha app =======--------------------------------//
#define ENABLE_TX 1
#define ENABLE_RX 1

#define CHRGING_PIN 0
#define CHARGING_PORT_DDR DDRB
#define CHARGING_PORT PORTB

#define DIS_CHRGING_PIN 2          // 7
#define DIS_CHARGING_PORT_DDR DDRB // DDRD
#define DIS_CHARGING_PORT PORTB    // PORTD
//________________________/
#define SETUP_CHARGING_PORT (CHARGING_PORT_DDR |= 1 << CHRGING_PIN)
// #define SET_CHRG_PORT() (SET_CHARGING_PORT |= 1 << CHRGING_PIN);
#define START_CHARGING (CHARGING_PORT |= (1 << CHRGING_PIN))   //() (CHARGING_PORT |= 1 << CHRGING_PIN);
#define STOP_CHARGING (CHARGING_PORT &= (~(1 << CHRGING_PIN))) //() (CHARGING_PORT &= ~(1 << CHRGING_PIN));

#define SETUP_DIS_CHARGING_PORT (DIS_CHARGING_PORT_DDR |= 1 << DIS_CHRGING_PIN)
// For PNP Transistor revers maco name
#define START_DIS_CHARGING (DIS_CHARGING_PORT |= (1 << DIS_CHRGING_PIN))
#define STOP_DIS_CHARGING (DIS_CHARGING_PORT &= (~(1 << DIS_CHRGING_PIN)))

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
//_==============================================================================================================================_//
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
/// @brief ////////-/for debug only

//---------------=========== Global variables ============---------------//
ADC_Cls *ADC_ptr;
Atmega8Timer2 *timerPtr;
volatile uint16_t conv_result = 0;
bool loop4Ever = true;
volatile float getting_time = 0.0f;
volatile int float_Nums_Count = 0;
volatile int int_Nums_Count = 0;
bool charging_completed = false;
float f_val = 0.0;
long l_val = 0;
long charging_limit = 1023;
long discharging_limit = 50;

void Convert_Transmit_Data();
void Charg_Capacitor();
void Discharge_Capacitor();
void Initialize_Charging_Discharge();
void Adjust_Display(uint8_t);
// void Disp_Received_Data ()
//{//

//  LCD_Ptr->LCD_Clear();//

//   if (mSerPtr->rxed_Number_Type == (SOB_I))
//   {
//     g_Pos = 1;
//     mSerPtr->Get_Rxed_L_Num(l_val);
//         printf("%ld", l_val);
//     //g_Pos = 1;
//     //g_Line = Line_2;
//     //printf("I:%d", mSerPtr->Get_Rxed_Nums_Count(SOB_I));
//   }//
//  // g_Line = Line_1;
//  //!if (mSerPtr->rxed_Number_Type == (SOB_F))
//  //!{
//  //!  g_Pos = 1;
//  //!  mSerPtr->Get_Rxed_F_Num(f_val);
//  //!  printf("%f", (double)f_val);
//  //!  g_Pos = 1;
//  //!  g_Line = Line_2;
//  //!  printf("F:%d", mSerPtr->Get_Rxed_Nums_Count(SOB_F));
//  //!}
//
//  //if(mSerPtr->Get_Rxed_Nums_Count(SOB_I) > 0)//

//  //_if (mSerPtr->rxed_Number_Type == (SOB_S))
//  //_{
//  //_  g_Pos = 1;
//  //_  //Wink_Led2();
//  //_  g_Pos = 1;
//  //_  g_Line = Line_2;
//  //_  printf("S:%d", mSerPtr->rxed_Msg_Length);
//  //_  for(uint8_t i = 0; i < mSerPtr->rxed_Msg_Length; i++)
//  //_  {
//  //_    printf("%c", mSerPtr->Rx_Buffer[i]);
//  //_  }
//  //_  //printf("%s", mSerPtr->read_Message_Buffer_Ptr++);
//  //_  mSerPtr->Reset_Read_Ptr(SOB_S);
//  //_}
//
//}
unsigned char rx_ch_buff[BUFFER_SIZE] = {"\0"};
uint8_t prev_length_L1 = 0;
uint8_t prev_length_L2 = 0;
uint8_t prev_Spos_1 = 0;
uint8_t prev_Spos_2 = 0;

int main()
{
  // DDRD |= 1 << PD1 ;
  // DDRD &= ~(1 << PD4);

  ATMG8A_USART mSerial_Port(BAUD_RATE, true, true);
#if ENABLE_TX == 1
  Atmega8Timer2 mTimer(true, false);
  timerPtr = &mTimer;

  ADC_Cls mADC(PC5);
  ADC_ptr = &mADC;
  Set_Leds();
  // long rxed_lng = 0;
  SETUP_CHARGING_PORT; // CHARGING_PORT_DDR |= 1 << CHRGING_PIN;
  SETUP_DIS_CHARGING_PORT;
  // in case of using PNP transistor for discharging the capacitor
  // STOP_DIS_CHARGING;

  // Wink_Led2();
  // DDRB |= 1 << PB2;
  // PORTB |= 1 << PB2;
  //_delay_ms(3000);
  // Wink_Led2();
  // PORTB &= ~(1 << PB2);
  // Wink_Led2();

#endif

#if USE_LCD == 1
  // LCD constants are defined in
  LCD_HD44780 main_LCD(
      DP4, DP5, DP6, DP7,
      DPP4, DPP5, DPP6, DPP7,
      DRS, DE, DRW,
      PORT_D_, PORT_D_, PORT_D_);
#endif
  // unsigned char rx_ch = '0';
  //  Wink_Led3(90000);
  // Wink_Led2();
  // uint8_t i = 0;
  // Wink_Led2();
  // prev_length_L1 = printf("Khaled Sobhy1234");
  //
  ////prev_length_L1= printf("Khaled Sobhy1234");
  // g_Line = Line_2;
  // g_Pos = 1;
  //// printf("%s    i=%ld", strStreambuff,(long int)i);
  // prev_length_L1= printf("PL = %ld", (long)prev_length_L1);
  // g_Pos=9;
  // prev_length_L1 = printf("PL = %ld", (long)prev_length_L1);
  //_delay_ms(1500);
  //  g_Pos = 1;
  //  prev_length_L1 = 16;
  //  Adjust_Display(12);
  //  printf("Khaled Sobhy");
  uint8_t g_l = 0;
  while (1)
  {

    ////_ if the switch is pressed
    // _delay_ms(900);
    // PORTD ^= 1 << PD6;
    // Wink_Led3(150000);

    // my_Delay_Fn(50000);
    _delay_ms(300);

#if ENABLE_RX == 1
    //---if (mSerPtr->new_Data_Received_Flag)
    //---{
    //---  mSerPtr->new_Data_Received_Flag = false;
    //---  LCD_Ptr->LCD_Clear();
    //---mSerPtr->Get_Rxed_L_Num(l_val);
    //---
    //---g_Pos = 1;
    //---
    //---printf("%ld", (long)l_val);
    //---if(l_val==1)
    //---  {
    //---    charging_limit = 400;
    //---    Charg_Capacitor();
    //---  }
    //---else if(l_val==2)
    //---  {discharging_limit = 250;
    //---    Discharge_Capacitor();
    //---  }
    //---
    //---}
    // when a new data is received from the serial port  get the received data
    if (mSerPtr->new_Data_Received_Flag)
    {
      mSerPtr->new_Data_Received_Flag = false;
      // mSerPtr->new_Data_Received_Flag = false;
      if (mSerPtr->rxed_Data_Type == SOB_S)
      {
        g_Pos = l_val;
        g_Line = Line_1;
        // printf("%s  %ld", (const char *)mSerPtr->read_Message_Buffer_Ptr,(long int) *mSerPtr->read_Message_Buffer_Ptr);
        g_l=printf("%s  %ld", (const char *)mSerPtr->read_Message_Buffer_Ptr, l_val);
        // printf("%s", (const char *)mSerPtr->message_Buffer);
        LCD_Ptr->ManageDisplay(g_l,g_Pos, g_Line);

        //!  LCD_Ptr->LCD_SHOW_String((char *)mSerPtr->read_Message_Buffer_Ptr, g_Pos, g_Line, g_Page);
        // LCD_Ptr->LCD_Clear();
        //---uint8_t msg_length = strlen((const char *)mSerPtr->Rx_Buffer);
        //---/ uint8_t diff = 0;
        //---//uint8_t new_length=msg_length;
        //---strcpy((char *)rx_ch_buff, (const char *)mSerPtr->Rx_Buffer);

        //- // Using strcpy with dynamically allocated memory
        //- char *destination3;
        //- size_t source_len = strlen((const char *) mSerPtr->Rx_Buffer);
        //- destination3 = (char *)malloc(source_len + 1); // Allocate enough memory (+1 for null terminator)
        //-
        //- if (destination3 != NULL)
        //- {
        //-   strcpy(destination3, source);
        //-   g_Line = Line_1;
        //-   g_Pos = 1;
        //-   printf("Destination 3: %s", destination3);
        //-   free(destination3); // Free the dynamically allocated memory
        //- }
        //- else
        //- {
        //-   printf("Memory allocation failed for destination 3.\n");
        //- }
      }
      else if (mSerPtr->rxed_Data_Type == SOB_I)
      {
        g_Pos = 1;
        g_Line = Line_2;
        mSerPtr->Get_Rxed_L_Num(l_val);
        g_l = printf("%ld %s", (long)l_val, "Long #");
        LCD_Ptr->ManageDisplay(g_l ,g_Pos, g_Line);
      }
      //! else
      //!  {
      //!    mSerPtr->Get_Rxed_L_Num(l_val);
      //!    switch (l_val)
      //!    {
      //!    // if charging command received
      //!    case 1:
      //!      //-LCD_Ptr->LCD_Clear();
      //!      //-mSerPtr->Get_Rxed_F_Num(f_val);
      //!      //-g_Pos = 1;
      //!      //-g_Line = Line_2;
      //!      //-printf("%f", (double)f_val);
      //!      // if(!charging_completed)
      //!
      //!      Charg_Capacitor();
      //!
      //!      break;
      //!      // if discharging command received
      //!    case 2:
      //!      //if(charging_completed)
      //!       {
      //!         Wink_Led2();
      //!         START_DIS_CHARGING;
      //!         START_CHARGING;
      //!
      //!         //Discharge_Capacitor();
      //!       }
      //!       break;
      //!     //set the charging limit
      //!       default:
      //!       //-Wink_Led2();
      //!
      //!       if (charging_completed)
      //!          {
      //!            Wink_Led2();
      //!            discharging_limit = l_val;
      //!          }
      //!        else
      //!           {
      //!             charging_limit = l_val;
      //!           }
      //!
      //!        g_Line = Line_1;
      //!        g_Pos = 1;
      //!        printf("%ld", (long)l_val);
      //!        g_Line = Line_2;
      //!        g_Pos = 1;
      //!        printf("C:%ld    D:%ld", (long)charging_limit,discharging_limit);
      //!        break;
      //!        }
    }
  }
  //_ {

  //_   //if charging command received
  //_   if(l_val == 1)
  //_   {
  //_     main_LCD.LCD_Clear();
  //_     if(charging_completed)
  //_     {
  //_
  //_       Discharge_Capacitor();
  //_     }
  //_     else
  //_       {
  //_         Charg_Capacitor();
  //_
  //_       }
  //_     l_val = 0;
  //_   }
  //_   else
  //_   {
  //_     {
  //_       if(charging_completed)
  //_         discharging_limit = l_val;
  //_       else
  //_       charging_limit = l_val;
  //_     }

  //_   }
  //_ }

#endif

#if ENABLE_TX == 1

//! if (!(PINC & (1 << PC4)))
//!{
//!   //- if(disccharge_cmd)
//!   //- {
//!   //-   START_DIS_CHARGING;
//!   //-   disccharge_cmd = false;
//!   //- }
//!   //- else
//!
//!   {
//!     //- STOP_DIS_CHARGING;
//!     main_LCD.LCD_Clear();
//!    Wink_Led2();
//!     //(LED_2_Outp) ^= 1 << (LED_2_Pin);
//!     //_delay_ms(300);
//!     //(LED_2_Outp) ^= 1 << (LED_2_Pin);
//!
//!     Transmit_Charge_Discharge_Data();
//!   }
//! }
#endif
}

//@ The constant below is defined in the timer header file
#if USE_INTERRUPT == 1 && ENABLE_TX == 1

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
#if ENABLE_TX == 1

//! void Transmit_Charge_Discharge_Data()
//!{
//!   LCD_Ptr-> LCD_Clear();
//!   //_reset the ticks and the counter
//!   ticks = 0;
//!   TCNT2 = 0;
//!//-Reset the current time and float and integer numbers count
//!   current_Time = 0.0;
//!   float_Nums_Count = 0;
//!   int_Nums_Count = 0;
//!
//!   //* in case of free running conversion remove comment to start adc cct
//!   //  ADC_ptr->Start_ADC(true)     ;
//!
//!   // Needed for PNP transistors when used to discharge the capacitor
//!   STOP_DIS_CHARGING;
//!   //* in case of free run conversion remove comment
//!   loop4Ever = true;
//!   ADC_ptr->Start_Converting(); // dummy 1 conversion
//!   _delay_us(2);
//!
//!   if(!charging_completed)
//!   //@ start charging the capacitor
//!     START_CHARGING; // CHARGING_PORT |= 1 << CHRGING_PIN; // PORTC |= 1 << PC5;
//!   else
//!     START_DIS_CHARGING;
//!
//!   //_ Start Timer 2
//!   timerPtr->Start_Timer2(timerPtr->PreSc_8); //(timer_2.PreSc_1024);
//!
//!   do
//!   { //* in case of manual conversion remove comment
//!     // mSerPtr->Send_Nums<float>("%2.3f", current_Time);
//!     // mSerPtr->Transmit_Number(SOB_F, "%2.3f", current_Time, 0);
//!     ADC_ptr->Start_Converting();
//!     //! _delay_us(1);
//!
//!     //_Wait until Conversion Completes
//!     while ((ADCSRA & (1 << ADSC)))
//!     {
//!     };
//!
//!     mSerPtr->Transmit_Number(SOB_F, "%2.4f", current_Time, 0);
//!     float_Nums_Count++;
//!
//!     conv_result = ADC_ptr->ADC_GetResult();
//!     _delay_us(SERIES_TX_DELAY); //_delay_us(100); //.1 ms
//!     mSerPtr->Transmit_Number(SOB_I, "%ld", 0, conv_result);
//!     int_Nums_Count++;
//!
//!     // _delay_ms(0.1);  //0.25 ms
//!     _delay_us(SERIES_TX_DELAY); // _delay_us(100); //.1 ms //100us
//!     // TLCD_ptr->LCD_Clear();
//!     //-g_Pos=1;
//!     //-printf("%d",conv_result);
//!     //-LCD_ptr->LCD_Clear();
//!
//!     if(!charging_completed)
//!     {
//!       g_Pos = 1;
//!       printf("C:%d", conv_result);
//!       if (conv_result >=charging_limit) // 1023 /*|| loop4Ever == false*/ //-if (!(PIND & (1 << PD6)))
//!       {
//!
//!         //@ Stoptimer 2
//!         timerPtr->Start_Timer2(timerPtr->Pre_OFF);
//!         loop4Ever = false;
//!         //@ stop charging the capacitor
//!         STOP_CHARGING; // CHARGING_PORT &= (~(1 << CHRGING_PIN)); // PORTC &= (~(1 << PC5));
//!         // update charging flag
//!         charging_completed = true;
//!         Wink_Led2();
//!       }
//!     }
//!
//!     else if (charging_completed)
//!     {
//!       g_Pos = 9;
//!       printf("D:%d", conv_result);
//!       if (conv_result <= discharging_limit)
//!       {
//!         //@ Stoptimer 2
//!         timerPtr->Start_Timer2(timerPtr->Pre_OFF);
//!         loop4Ever = false;
//!         //@ stop disscharging the capacitor
//!         STOP_DIS_CHARGING;
//!         // update charging flag
//!           charging_completed = false;
//!           Wink_Led2();
//!       }
//!     }
//!       my_Delay_Fn(20000);
//!     }
//!
//!
//! while(loop4Ever); // for live data plot
//!//needed for PNP transistors when used to discharge the capacitor
//! START_DIS_CHARGING;
//!// Reset the ADC converted result value
//! conv_result = 0;
//!  // Display the totan number of float and integer numbers transmitted
//!  g_Line = Line_1;
//!  g_Pos = 1;
//!  printf("F:%d", float_Nums_Count);
//!  g_Pos = 9;
//!  printf("I:%d", int_Nums_Count);
//!
//!  // Display the total transmssion time period
//!  g_Line = Line_2;
//!  g_Pos=1;
//!  printf("T %f",(double)current_Time);
//!
//! }
void Initialize_Charging_Discharge()
{
  //_reset the ticks and the counter
  ticks = 0;
  TCNT2 = 0;
  //-Reset the current time and float and integer numbers count
  current_Time = 0.0;
  // To count the number of float and integer numbers transmitted
  float_Nums_Count = 0;
  int_Nums_Count = 0;
}
void Convert_Transmit_Data()
{
  //* in case of manual conversion remove comment
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

  _delay_us(SERIES_TX_DELAY); // _delay_us(100); //.1 ms //100us
}
// LCD_Ptr->LCD_Clear}
void Charg_Capacitor()
{

  Initialize_Charging_Discharge();
  //* in case of free running conversion remove comment to start adc cct
  //  ADC_ptr->Start_ADC(true)     ;

  // Needed for PNP transistors when used to discharge the capacitor to make it off
  // STOP_DIS_CHARGING;
  //* in case of free run conversion remove comment
  loop4Ever = true;
  ADC_ptr->Start_Converting(); // dummy 1 conversion
  _delay_us(2);
  g_Line = Line_1;
  START_CHARGING;

  //_ Start Timer 2
  timerPtr->Start_Timer2(timerPtr->PreSc_8); //(timer_2.PreSc_1024);

  do
  {
    Convert_Transmit_Data();
    LCD_Ptr->LCD_Clear();

    g_Pos = 1;
    printf("%ld", (long)conv_result);

    if (conv_result >= charging_limit) // 1023 /*|| loop4Ever == false*/ //-if (!(PIND & (1 << PD6)))
    {

      //@ Stoptimer 2
      timerPtr->Start_Timer2(timerPtr->Pre_OFF);
      loop4Ever = false;
      //@ stop charging the capacitor
      STOP_CHARGING; // CHARGING_PORT &= (~(1 << CHRGING_PIN)); // PORTC &= (~(1 << PC5));
      // update charging flag
      charging_completed = true;
      // Reset the ADC converted result value
      conv_result = 0;
      // Wink_Led2();
      g_Line = Line_1;
      g_Pos = 1;
      printf("%ld", (long)conv_result);
      g_Line = Line_2;
      g_Pos = 1;
      printf("%ld", (long)charging_limit);
    }

    my_Delay_Fn(20000);
  }

  while (loop4Ever); // for live data plot

  // Display the totan number of float and integer numbers transmitted
  //-g_Line = Line_1;
  //-g_Pos = 1;
  //-printf("F:%d", float_Nums_Count);
  //-g_Pos = 9;
  //-printf("I:%d", int_Nums_Count);
  //-
  //-// Display the total transmssion time period
  //-g_Line = Line_2;
  //-g_Pos = 1;
  //-printf("T %f", (double)current_Time);
}

void Discharge_Capacitor()
{
  Initialize_Charging_Discharge();

  //* in case of free running conversion remove comment to start adc cct
  //  ADC_ptr->Start_ADC(true)     ;

  //* in case of free run conversion remove comment
  loop4Ever = true;
  ADC_ptr->Start_Converting(); // dummy 1 conversion
  _delay_us(2);

  START_DIS_CHARGING;
  // Wink_Led2();
  //_ Start Timer 2
  timerPtr->Start_Timer2(timerPtr->PreSc_8); //(timer_2.PreSc_1024);

  do
  {
    Convert_Transmit_Data();
    if (conv_result <= discharging_limit) // 1023 /*|| loop4Ever == false*/ //-if (!(PIND & (1 << PD6)))
    {

      //@ Stoptimer 2
      timerPtr->Start_Timer2(timerPtr->Pre_OFF);
      loop4Ever = false;
      //@ stop charging the capacitor
      STOP_DIS_CHARGING; // CHARGING_PORT &= (~(1 << CHRGING_PIN)); // PORTC &= (~(1 << PC5));
      // update charging flag
      charging_completed = false;
      // Reset the ADC converted result value
      conv_result = 0;
      //  Wink_Led2();
    }

    my_Delay_Fn(20000);
  }

  while (loop4Ever && charging_completed); // for live data plot

  // Display the totan number of float and integer numbers transmitted
  g_Line = Line_1;
  g_Pos = 1;
  printf("F:%d", float_Nums_Count);
  g_Pos = 9;
  printf("I:%d", int_Nums_Count);

  // Display the total transmssion time period
  g_Line = Line_2;
  g_Pos = 1;
  printf("T %f", (double)current_Time);
}
//* in case of free running conversion remove comment to start adc cct
//  ADC_ptr->Start_ADC(true)     ;
#endif

//-void Adjust_Display(uint8_t msg_len)
//-{
//-  uint8_t tmp_len = 0;
//-  if (g_Line == Line_1)
//-  {
//-    tmp_len = prev_length_L1;
//-    prev_length_L1 = msg_len;
//-  }
//-
//-  else if (g_Line == Line_2)
//-  {
//-    tmp_len = prev_length_L2; // diff = prev_length - strlen((const char *)mSerPtr->Rx_Buffer);
//-    prev_length_L2 = msg_len;
//-  }
//-
//-  if (tmp_len > msg_len)
//-  {
//-
//-    for (uint8_t i = msg_len; i < tmp_len; i++)
//-    {
//-      rx_ch_buff[i] = ' '; // Clear the buffer after use
//-    }
//-    //   rx_ch_buff[prev_length_L1] = '\0';
//-    // strlen((const char *)mSerPtr->Rx_Buffer);}
//-  }
//-}

void Adjust_Display(uint8_t msg_len)
{

  uint8_t tmp_len = 0;
  uint8_t tmp_spos = 0;
  if (g_Line == Line_1)
  {
    tmp_len = prev_length_L1;
    prev_length_L1 = msg_len;
    tmp_spos = prev_Spos_1;
    prev_Spos_1 = g_Pos;
  }

  else if (g_Line == Line_2)
  {
    tmp_len = prev_length_L2; // diff = prev_length - strlen((const char *)mSerPtr->Rx_Buffer);
    prev_length_L2 = msg_len;
    tmp_spos = prev_Spos_2;
    prev_Spos_2 = g_Pos;
  }

  if (g_Pos < tmp_spos)
  {
    for (uint8_t i = g_Pos + msg_len + 1; i <= tmp_spos + tmp_len; i++)
    {
      printf(" "); // Clear the buffer after use
    }
    if (msg_len < tmp_len)
    {
      for (uint8_t i = g_Pos + msg_len + 1; i < tmp_spos + tmp_len; i++)
      {
        printf(" ");
      }
      //   rx_ch_buff[prev_length_L1] = '\0';
      // strlen((const char *)mSerPtr->Rx_Buffer);}
    }

    else if (g_Pos > tmp_spos)
    {
      for (uint8_t i = tmp_spos; i <= g_Pos - tmp_len - 1; i++)
      {
        printf(" ");
      }
      if (msg_len < tmp_len)
      {
        for (uint8_t i = g_Pos + msg_len + 1; i < tmp_spos + tmp_len; i++)
        {
          printf(" ");
        }
      }

      else if (g_Pos == tmp_spos && msg_len < tmp_len)

      {
        for (uint8_t i = tmp_len - msg_len + 1; i <= tmp_len; i++)
        {
          printf(" ");
        }
      }
    }
  }
}
