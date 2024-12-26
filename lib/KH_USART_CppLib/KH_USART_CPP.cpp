// #include "KH_Atmega8.hpp"
#include "KH_Atmega8.hpp"
#include "KH_USART_CPP.hpp"
// #include "Global_Stufs.hpp"
// #include <avr/interrupt.h>
#include "util/delay.h"
#include <stdlib.h>
// #include <stdio.h>

ATMG8A_USART *mSerPtr;
//___________________ For Debug  ____________________________//
//-uint8_t tt = 1;

//-LCD_Ptr->LCD_Show_Character(data, tt);
//-if (tt > 16)
//-{
//-  tt = 0;
//-}
//-tt += 1;
//-_delay_ms(600);
//___________________ End for Debug _________________________//

ATMG8A_USART::ATMG8A_USART(unsigned long ubrr, bool en_tx, bool en_rx)
{
  //! volatile uint8_t * UCSRA_ptr = (volatile uint8_t *)0x2B;
  // FILE usart0_str = FDEV_SETUP_STREAM(USART_SendByte, Receive_ByteByte, _FDEV_SETUP_RW);
  //- fscanf_Rd_Stream.get = Read_Byte_From_Stdinput;
  //- fscanf_Rd_Stream.flags = _FDEV_SETUP_READ;
  //- fscanf_Rd_Stream.buf=ts;
  //- stdin = &fscanf_Rd_Stream;
  // initialize global variabled
  //  type_Detected = false;
  //  EOB_Flag = false;

  mSerPtr = this;

#if (USE_TX_RX_BUFFERS == 1)
  float_Nums_Buffer[EXT_F_BUFF_SIZE] = {'\0'};
  long_Nums_Buffer[EXT_L_BUFF_SIZE] = {'\0'};
  string_Buffer[EXT_S_BUFF_SIZE] = {'\0'};

  // Private pointers  for refrencing the float and the float buffers
  float_Nums_Buffer_Ptr = float_Nums_Buffer;
  long_Nums_Buffer_Ptr = long_Nums_Buffer;
  string_Buffer_Ptr = string_Buffer;

  // for read only
  read_F_Nums_Buffer_Ptr = float_Nums_Buffer;
  read_L_Nums_Buffer_Ptr = long_Nums_Buffer;
  read_String_Buffer_Ptr = string_Buffer;

  curr_Rx_F_Buff_Indx = -1;
  curr_Rx_L_Buff_Indx = -1;
  curr_RX_Str_Buff_Indx = -1;
  read_F_Ptr_Index = -1;
  read_L_Ptr_Index = -1;
  //  tmp_Str_Indx = -1;

  rxed_FNums_Count = 0;
  rxed_LNums_Count = 0;
  rxed_String_Count = 0;
  tmpCnt_F = 0,
  tmpCnt_L = 0;

  action_Executed = false;
  new_Data_Received_Flag = false;

#endif

  // initialize the Tx\Rx buffers
  Rx_Buffer[BUFFER_SIZE] = {'\0'};
  Tx_Buffer[BUFFER_SIZE] = {'\0'};
  new_Data_Received_Flag = false;
  end_of_Tx_Block = false;
  type_Detected = false;
  EOB_Flag = false;
  // initialize Rx/Tx buffers indexes
  used_Rx_Buff_Size = 0;
  transmitted_Chars_Count = 0;
  used_Tx_Buffer_Size = 0;
  // remaining_tx_size = BUFFER_SIZE;
  used_Buffer_Size = 0;

  // initialize the rx buffer pointer
  Rx_BufferPtr = Rx_Buffer;
  Tx_BufferPtr = Tx_Buffer;
  current_Tx_Ch_Ptr = Tx_BufferPtr;
  ;

  // Initalize the received numbers s used to store the received numbers
  rxed_Float_Number = 0.0;
  rxed_Long_Number = 0;
  rxed_Number_Type = ' ';

#define MYUBRR ((F_CPU / (ubrr * 16UL)) - 1) //(((F_CPU / (ubrr * 16UL))) - 1)
                                             // Set the Baud rate
  //!     UCSRC &= ~(1 << URSEL);
  UBRRH = (unsigned char)(MYUBRR >> 8);
  UBRRL = (unsigned char)MYUBRR;

  // Set data frame formate 8 Data bits 2 stop pits , no parity check
  //    UCSRC |= ((1 << URSEL) | (1 << USBS) | (1 << UCSZ0 )| (1 << UCSZ1 ));

  // Set data frame formate 8 Data bits 1 stop pits , no parity check
  UCSRC |= ((1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1));

  UCSRC &= (~(1 << USBS) | (1 << URSEL)); //! 1 stop bit ->   USBC =0

  // UCSRC= 0b10000110;
  // Enable Transmitter
  if (en_tx)
    UCSRB |= (1 << TXEN);

  // Enable Receiver
  if (en_rx)
    UCSRB |= (1 << RXEN);
#if USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 1
  USART_Setup_Interrupt(true, false, false);
#endif
}

// void ATMG8A_USART::Clear_TX_Buffer()
//{
//     Tx_Buffer[BUFFER_SIZE]='\0';
//  }

// void ATMG8A_USART::ResetPtr()
//   {
//    Tx_BufferPtr = Tx_Buffer;
//   }

ATMG8A_USART::~ATMG8A_USART()
{
}

// void ATMG8A_USART::Reset_Buffer()
//{
//   for (uint8_t i=0;i<BUFFER_SIZE;i++)
//   {
//     Tx_Buffer[i]= '\0';
//     transmitted_Chars_Count=0;
//     used_Tx_Buffer_Size=0;
//   }
// }

#if USE_TX_RX_BUFFERS == 1

/// @brief To Get the a stored float number from the float buffer
///@param parameter-fn a variable to store the read float number from the float buffer
/// @return the read number index if the buffer not empty other wise false
int8_t ATMG8A_USART::Get_Rxed_F_Num(float &fn)
{
  bool ret_res =false;
  // if the buffer is empty
  if (curr_Rx_F_Buff_Indx < 0)
  {
    ret_res = -1;//return false;
  }
  if (read_F_Ptr_Index < rxed_FNums_Count)

  {
    fn = *read_F_Nums_Buffer_Ptr++; //-fn = float_Nums_Buffer[curr_Rx_F_Buff_Indx];
    ret_res = curr_Rx_F_Buff_Indx;
  }
  return ret_res;
}

/// @brief To Get the a stored long number from the long buffer
///@param parameter-fn a variable to store the read long number from the long buffer
/// @return the read number index if the buffer not empty other wise false

int8_t ATMG8A_USART::Get_Rxed_L_Num(long &ln)
{
  int8_t ret_res = -1;
  if (curr_Rx_L_Buff_Indx < 0)
  {
    ret_res = -1;
  }
  if (read_L_Ptr_Index < rxed_LNums_Count)
  {
    ln = *read_L_Nums_Buffer_Ptr++; //-ln = long_Nums_Buffer[curr_Rx_L_Buff_Indx];
    ret_res = read_L_Ptr_Index;
  }
  return ret_res;
}

/// @brief
/// @param nty is the type of buffer's numbers (float or long)
/// @return return the current size of the required buffer
uint8_t ATMG8A_USART::Get_Rxed_Nums_Count(unsigned char nty)
{
  if (nty == SOB_F)
    return rxed_FNums_Count;
  else
    return rxed_LNums_Count;
}
/// @brief Reset the main buffers pointers to store numbers from the begining and over write the existing numbers
/// @param pt: pointer type to be reset float or long
void ATMG8A_USART::Reset_Main_Rx_Ptr(unsigned char pt)
{

  if (pt == SOB_F)
  {
    float_Nums_Buffer_Ptr = float_Nums_Buffer;
    read_F_Nums_Buffer_Ptr = float_Nums_Buffer;
    rxed_FNums_Count = 0;
    curr_Rx_F_Buff_Indx = -1;
    read_F_Ptr_Index = -1;
    tmpCnt_F = 0;
  }
  else if (pt == SOB_I)
  {
    long_Nums_Buffer_Ptr = long_Nums_Buffer;
    read_L_Nums_Buffer_Ptr = long_Nums_Buffer;
    rxed_LNums_Count = 0;
    curr_Rx_L_Buff_Indx = -1;
    read_L_Ptr_Index = -1;
    tmpCnt_L = 0;
  }
  else // in case of SOB_S
  {
    string_Buffer_Ptr = string_Buffer;
    read_String_Buffer_Ptr = string_Buffer;
    rxed_String_Count = 0;
    curr_RX_Str_Buff_Indx = -1;
    tmpCnt_S = 0;
  }
  action_Executed = false;
}
#endif
/**
 * @brief Detect the sign of beginning transmit/receive numbers
 * @param sf: is the character that we search for
 * @param ptr: pointer to the start location for detecting
 * @return uint8_t  the index at which the sign was detected
 */

//>uint8_t ATMG8A_USART::Detect_Sign(unsigned char** ptr)
//>{
//>  uint8_t detect_ind=0;
//>  for (;detect_ind < used_Rx_Buff_Size;detect_ind++)
//>    {   //ptr is a pointer to a unsigned char pointer
//>      if(*(*ptr)  == SOB_F || *(*ptr)  == SOB_I || *(*ptr)  == SOB_S)
//>         {
//>          Rx_BufferPtr = *ptr;
//>          rxed_Number_Type = *(*ptr) ;
//>          break;
//>          }
//>        (*ptr)++;
//>    }
//> return detect_ind;
//>}

//.uint8_t ATMG8A_USART::Detect_Sign(char sf,unsigned char** ptr)
//.{
//.  uint8_t detect_ind=0;
//.  for (;detect_ind < used_Rx_Buff_Size;detect_ind++)
//.    {   //ptr is a pointer to a unsigned char pointer
//.      if(*(*ptr)  == sf)
//.         {
//.          Rx_BufferPtr = *ptr;
//.          break;
//.          }
//.        (*ptr)++;
//.    }
//. return detect_ind;
//.}

/// @brief Transmit a char using polling tecnique (thout interrupt)
/// @param data : the chrarcter that will be transmitted
/// @return
bool ATMG8A_USART::Transmit_Char(unsigned char &data)
{
  //$ Wait for Empty Transmit buffer (UDR)/ UDRE UART Data Transmit Buffer empty flag
  // if(! USE_USART_INTERRUPTS)
  // dissable interrupt
  // Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 0;
  // USART_Setup_Interrupt();

  while (!(UCSRA & (1 << UDRE)))
  {
  }; // Do nothing

  UDR = data;
  _delay_us(TX_DELAY);

  return true;
}

/// @brief Transmit a char pointed to by Tx Buffer pointer using interrupt tecnique
/// @return int byte : The number of transmitted charaters till now
//*int8_t ATMG8A_USART::Transmit_Char()
//*{
//*
//*   //dissable the interrupt after executing it to prevent infinite tx toutin trigger
//*      Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 0;
//*      USART_Setup_Interrupt();
//*
//*   //the current index of the next buffer location = the total number of transmitted chars
//*   if(transmitted_Chars_Count >= BUFFER_SIZE)
//*   {
//*      transmitted_Chars_Count=0;
//*      Tx_BufferPtr=Tx_Buffer;
//*     return false ;
//*   }
//*
//*      UDR = *Tx_BufferPtr++;
//*     //transmitted_Chars_Count point to the next character that will be transmitted
//*     //at this point the number of transmitted charaters is equal to the value of current Tx index (transmitted_Chars_Count)
//*     transmitted_Chars_Count++;
//*
//*    //return the number of transmitted charaters till now
//*   return transmitted_Chars_Count;
//*}

// working function
/// @brief Transmit a char using interrupt tecnique
/// @return  int byte : The number of transmitted charaters till now
//>int8_t ATMG8A_USART::Transmit_Char()
//>{
//>   //dissable the interrupt after executing it
//>       Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 0;
//>       USART_Setup_Interrupt();
//>
//>   //the current index of the next buffer location = the total number of transmitted chars
//>   if(transmitted_Chars_Count >= BUFFER_SIZE)
//>   {
//>      transmitted_Chars_Count=0;
//>   //  return -1 ;
//>   }
//>     //!ptrLCD->LCD_Show_Character(Page_2,pos,Line_1,Tx_Buffer[transmitted_Chars_Count]);
//>
//>
//>     //!    `2();
//>     //!   pos++;
//>     //! if(pos >16)
//>     //! pos=1;
//>     //UDR = *Tx_BufferPtr++;
//>      UDR = Tx_Buffer[transmitted_Chars_Count];
//>     //transmitted_Chars_Count point to the next character that will be transmitted
//>     //at this point the number of transmitted charaters is equal to the value of current Tx index (transmitted_Chars_Count)
//>     transmitted_Chars_Count++;
//>    //!  ptrLCD->LCD_ShowInt(Page_2,15,Line_2,transmitted_Chars_Count,global_buffer);
//>     //_delay_ms(5000);
//>   //return the number of transmitted charaters till now
//>   return transmitted_Chars_Count;
//>}
/// @brief Transmit a string using olling techniqu (no interrupt)
/// @param strPtr
void ATMG8A_USART::Transmit_Msg(unsigned char *strPtr)
{
  //  unsigned char * res_msg_ptr = Rx_Buffer;
  unsigned char m_ch = SOB_S; //*strPtr;

  while (m_ch != '\0')
  {
    // transmit the current value of m_ch
    Transmit_Char(m_ch);
    // upadate m_ch value from the current pointer (strPtr) location
    m_ch = *strPtr; //*res_msg_ptr++;
    // set the pointer postion to the next character
    strPtr++;

    //! thi sdelay time is essential for successive transmition
    _delay_us(250); //! Optimum value 250us
  }

  m_ch = EOB;
  Transmit_Char(m_ch);
}

//-void ATMG8A_USART::Send_Msg(const char *frmt ,uint8_t arg_cnt,...)
//-{
//-  va_list varList;
//-  uint8_t varCnt =0;
//-  unsigned char data_Type;
//-  while (*frmt != '\0')
//-  {
//-    if(*frmt++ == '%')
//-    {
//-      varCnt++;
//-      switch(*(++frmt))
//-      {
//-        case 'f':
//-          data_Type = SOB_F;
//-        break;
//-
//-        case 'l':
//-          data_Type = SOB_I;
//-        break;
//-
//-        case 's':
//-          data_Type = SOB_S;
//-        break;
//-      }
//-    }
//-  }
//-  va_start(varList,arg_cnt);
//-    arg_cnt = varCnt;
//-  va_start(varList,arg_cnt);
//-
//-  for (uint8_t a = 0 ; a < arg_cnt;a++)
//-  {
//-    switch (data_Type)
//-    {
//-    case SOB_F:
//-      /* code */
//-      break;
//-     case SOB_I:
//-      /* code */
//-      break;
//-       case SOB_S:
//-      const unsigned char *stPt = va_arg(varList,const unsigned char *);
//-
//-         unsigned char ccc= SOB_S;
//-          Transmit_Char(ccc);
//-
//-        while(*stPt++ != '\0')
//-        {
//-         ccc = *stPt;
//-         Transmit_Char(ccc);
//-
//-        }
//-
//-             ccc= EOB;
//-              Transmit_Char(ccc);
//-      break;
//-
//-    }
//-  }
//-
//-
//-
//-  if(*(++frmt) == 's')
//-  {
//-    const unsigned char *stPt = va_arg(varList,const unsigned char *);
//-
//-         unsigned char ccc= SOB_S;
//-   Transmit_Char(ccc);
//-
//-        while(*stPt++ != '\0')
//-        {
//-         ccc = *stPt;
//-         Transmit_Char(ccc);
//-
//-        }
//-
//-             ccc= EOB;
//-              Transmit_Char(ccc);
//-  }
//-  va_end(varList);
//-   //! thi sdelay time is essential for successive transmition
//-    _delay_us(250); //! Optimum value 250us
//-}

#if USE_USART_INTERRUPTS == 0
bool ATMG8A_USART::Receive_Char(unsigned char &data)
{
  // wait for data to be received
  if (USE_USART_INTERRUPTS != 1)
  {
    while (!(UCSRA & (1 << RXC)))
    {
    };
  }

  data = UDR; //(unsigned char)

  return true;
}

void ATMG8A_USART::Receive_Msg()
{
  Rx_BufferPtr = Rx_Buffer;
  unsigned char rx_chr = ' ';
  // const char *res_l_num = (const char *)Rx_Buffer;
  //  long rv=0;
  //  uint8_t ind = 0;

  // begining of receiving a long int  number sign
  do
  {
    Receive_Char(rx_chr);

  } while (rx_chr != SOB_S);

  // while end of transmission not detected
  do
  {
    // get the next one
    Receive_Char(rx_chr);
    // store the received char
    (*Rx_BufferPtr++) = rx_chr;

  } while (rx_chr != EOB);

  Rx_BufferPtr++;
  (*Rx_BufferPtr) = '\0'; // add null termainating charcter (or any non digit char) to make strtol function work properly
}

#endif
// overloading Rx_Char function

/// @brief   :retrive the received char or byte from the Rx buffer UDR or 0 if no data in the UDR rx buffer
/// @return  : the total numbers of the received bytes or char in the rx buffer
uint8_t ATMG8A_USART::Receive_Char()
{
#if (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))
  //@ check if char or a byte was received . if no data then return
  if (!(UCSRA & (1 << RXC)))
    return 0;
#endif //_(USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))

  // points to the the next rx empty buffer location = the total number of received chars
  if (used_Rx_Buff_Size > BUFFER_SIZE)
  {
    Rx_Buffer[BUFFER_SIZE] = {'\0'};
    used_Rx_Buff_Size = 0;
    Rx_BufferPtr = Rx_Buffer;
  }

  // copy the received byte to the Rx buffer and  set the pointer to the current empty rx buffer location (only if data type was detected)
  *Rx_BufferPtr = UDR;

  //@ If received data type have been detected
  if ((type_Detected == false) && ((*Rx_BufferPtr == SOB_F) || (*Rx_BufferPtr == SOB_I) || (*Rx_BufferPtr == SOB_S)))
  {
    rxed_Number_Type = *Rx_BufferPtr;
    type_Detected = true;
    // if the received type was detected it will be re-written in the next reading cause we don't need it any more
    return used_Rx_Buff_Size;
  }

  // if the receving data is a string message
  //.if( (rxed_Number_Type == SOB_S) && (type_Detected == true))
  //. {
  //. curr_RX_Str_Buff_Indx++;
  //. string_Buffer_Ptr++;
  //. }

  // if the end of receiving data was detected
  if (*Rx_BufferPtr == EOB && (type_Detected == true))
  {

    //> replace EOB with end of number sign (null character '\0') cause python will not send it, for success of number conversion
    *Rx_BufferPtr = '\0';
    used_Rx_Buff_Size++;
    EOB_Flag = true;
    return used_Rx_Buff_Size;
  }
  Rx_BufferPtr++;
  // update current rx used buffer size
  used_Rx_Buff_Size++;
  // return the size of the used rx buffer (the size of the total written characters in the buffer)
  //  remaining_tx_size -=1;
  return used_Rx_Buff_Size;
}

#if (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 1)

void ATMG8A_USART::USART_Setup_Interrupt(bool rxc, bool dre, bool txc)
{
#if USE_TX_INTERRUPT == 1
  Rx_Tx_Intrerrupts.U_DataRegisterEmpty_INTR = dre;
  Rx_Tx_Intrerrupts.U_Transmit_Completed_INTR = txc;
#endif

#if USE_RX_INTERRUPT == 1
  Rx_Tx_Intrerrupts.U_Receive_Complete_INTR = rxc;
#endif

#if USE_TX_INTERRUPT == 1
  if (Rx_Tx_Intrerrupts.U_DataRegisterEmpty_INTR) // Data Register is empty interrupt
  {
    UCSRB |= 1 << UDRIE;
  }
  else
  {
    UCSRB &= ~(1 << UDRIE);
  }

  if (Rx_Tx_Intrerrupts.U_Transmit_Completed_INTR) // Transmit Complete interrupt
  {
    UCSRB |= 1 << TXCIE;
  }
  else
  {
    UCSRB &= ~(1 << TXCIE);
  }
#endif

#if USE_RX_INTERRUPT == 1
  if (Rx_Tx_Intrerrupts.U_Receive_Complete_INTR) // Receive Complete interrupt
  {
    UCSRB |= 1 << RXCIE;
  }
  else
  {
    UCSRB &= ~(1 << RXCIE);
  }
#endif

  if // Enable or dissable the general interrupt
      (
#if USE_TX_INTERRUPT == 1
          Rx_Tx_Intrerrupts.U_DataRegisterEmpty_INTR ||
          Rx_Tx_Intrerrupts.U_Transmit_Completed_INTR ||
#endif

#if USE_RX_INTERRUPT == 1
          Rx_Tx_Intrerrupts.U_Receive_Complete_INTR
#endif
      )

  {
    sei();
  }

  else
  {
    cli();
  }
}
#endif

#if (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 1)
ISR(USART_RXC_vect)
{

  serPortPtr->new_Data_Received_Flag = false;
  serPortPtr->Receive_Char();

  if (serPortPtr->EOB_Flag)
  {
    serPortPtr->Data_Received();
    // has been moved to Receive_Char() fn
    //> replace EPF with end of number sign (null character '\0') cause python will not send it for success to number conversion
    serPortPtr->new_Data_Received_Flag = true;
    // reset the data type or the next data received
    serPortPtr->type_Detected = false;
    serPortPtr->EOB_Flag = false;

    // has been moved to Data_Received()function
    //>serPortPtr->used_Rx_Buff_Size = 0;
    // serPortPtr -> Data_Received();
    serPortPtr->used_Rx_Buff_Size = 0;
    // reset the Rx buffer pointer and the toral received bytes count
    serPortPtr->Rx_BufferPtr = serPortPtr->Rx_Buffer;
  }
  //  #endif
}
#endif //@ USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 1

//====================================================================================================================================//

//! uint8_t ATMG8A_USART::Receive_Char()
//!{
//!     //the current index of the next buffer location = the total number of transmitted chars
//!    if(used_Rx_Buff_Size >= BUFFER_SIZE)
//!    {
//!       used_Rx_Buff_Size=0;
//!       Rx_BufferPtr = Rx_Buffer;
//!    }
//!
//!   *Rx_BufferPtr = UDR;
//!    //point to the next write character location
//!   Rx_BufferPtr++ ;
//!   // update current rx index  to the next empty buffer locaton
//!   used_Rx_Buff_Size++;
//!
//!   // return the size of the used rx buffer (the size of the total written characters in the buffer)
//!   //the current index of the next empty buffer location
//!   //  remaining_tx_size -=1;
//!   return used_Rx_Buff_Size;
//! }

/**
 * @brief Transmit string over a serial port
 */

// This code depends on interrupt in the transmit fn
/**
 * @brief Transmit a string that's stored in the Tx Buffer using interrupt technique
 *
 * @return true : in case of success
 * @return false : in case of fail
 */
//*bool ATMG8A_USART::Transmit_String()
//*{
//*  //reset the tx index buffer (consider it empty)
//*  transmitted_Chars_Count = 0;
//*  Tx_BufferPtr = Tx_Buffer;
//*  uint8_t ttx_indx=0;
//*
//*
//* //this will set the Tx buffer pointer to thhe start location for transmitting
//* while (*Tx_BufferPtr != SOB_S)
//*   {
//*       Tx_BufferPtr++;
//*       ttx_indx++;
//*        //if string sign not detected
//*       if(ttx_indx > BUFFER_SIZE)
//*        return false;
//*  }
//* //.for(uint8_t i =0 ; i < used_Tx_Buffer_Size; i++)
//*   //. {
//*   //.
//*   //.   ptrLCD->LCD_Show_Character(Tx_Buffer[i],Page_1,pos,Line_1);
//*   //.   pos++;
//*   //.   if(pos >used_Tx_Buffer_Size)
//*   //.   pos=1;
//*   //. }
//* // while end of transmission not detected
//* //I did't increment the tx buffer pointer since it will be incremented y Tx_char function that will be run by the tx interrupt routine
//*   //while (*Tx_BufferPtr != EOB)
//*
//*    //reset end of transmission flag
//*  end_of_Tx_Block = false;
//*   while (!end_of_Tx_Block)
//*   {//end_of_Tx_Block flag will be  updated in the Tx interrupt routin
//*
//*        //ptrLCD->LCD_Show_Character(Tx_Buffer[ttx_indx],Page_1,pos,Line_1);
//*        //pos++;
//*        //if(pos >used_Tx_Buffer_Size)
//*        //pos=1;
//*        ttx_indx++;
//*      //case of ovelflow of the buffer
//*       if(ttx_indx > BUFFER_SIZE)
//*        return false;
//*        //below statements will fire the tx interrupt service routin
//*       Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//*       USART_Setup_Interrupt();
//*       _delay_us(INTRPT_DEALY);
//*
//*   }
//*    return true;
//*}

/// @brief Fill the Tx Buffer with data to be send
/// @param *tx_buf : is unsigned char pointer to the source of data that will be send
//*void ATMG8A_USART::Fill_Tx_Buffer(unsigned char *tx_buf )
//*  {
//*    while (*tx_buf != '\0')
//*    {
//*      if (*tx_buf == ' ')
//*      {
//*        tx_buf++;
//*        continue;
//*      }
//*      Add_Tx_Character(*tx_buf++);
//*    }
//*  }
#if USE_TX_FUNCTION_TEMPLATE == 0
//-bool ATMG8A_USART::Transmit_Number(const char * fmt,uint8_t arg_cnt,...)
//-{
//-  va_list varlist;
//-  // arg_cnt = 1 because i want the function to have only one argument in the va_list
//-  arg_cnt = 1;
//-  va_start(varlist,arg_cnt);
//-  double tx_d_num ;
//-  long tx_l_num;
//-  unsigned char num_typ;
//-  const char *fmt_cpy= fmt;
//-
//-   used_Tx_Buffer_Size = 0;
//-   Tx_Buffer[BUFFER_SIZE]={'\0'};
//-
//-    //char tx_Num_Buffer[Tx_NUM_SIZE] = {'\0'};
//-  Tx_BufferPtr = Tx_Buffer;
//- //skip % in the format var
//- fmt_cpy++;
//-  while (* fmt_cpy++ != '\0')
//-  {
//-    switch (*fmt_cpy)
//-        {
//-         case 'f':
//-         case 'F':
//-         num_typ = SOB_F;
//-         tx_d_num  = va_arg(varlist,double);
//-         snprintf((char *)Tx_Buffer, BUFFER_SIZE, fmt, tx_d_num);
//-
//-          break;
//-
//-        case 'l':
//-         case 'L':
//-         num_typ = SOB_I;
//-         tx_l_num = va_arg(varlist,long);
//-         snprintf((char *)Tx_Buffer,BUFFER_SIZE, fmt,tx_l_num);
//-
//-          break;
//-
//-        }
//-
//-  }
//-
//-
//-  //essetial initialisations
//-  transmitted_Chars_Count = 0;
//-  Tx_BufferPtr = Tx_Buffer;
//-  // reset end of sending a tx block flag
//-  end_of_Tx_Block = false;
//-
//-Transmit_Char(num_typ);
//-while(*Tx_BufferPtr != '\0')
//-{
//-  if(*Tx_BufferPtr == ' ')
//-  continue;
//-
//- Transmit_Char(*Tx_BufferPtr);
//-     Tx_BufferPtr++;
//-   // _delay_us(300);//(INTRPT_DEALY); //while (!(UCSRA & (1 << TXC))){};
//-   transmitted_Chars_Count++;
//-}
//-num_typ = EOB ;
//-Transmit_Char(num_typ);
//-end_of_Tx_Block =true;
//-
//-return end_of_Tx_Block;
//-}

bool ATMG8A_USART::Transmit_Number(unsigned char num_type, const char *fmt, float fval, long l_value)
{
  used_Tx_Buffer_Size = 0;
  Tx_Buffer[BUFFER_SIZE] = {'\0'};

  // char tx_Num_Buffer[Tx_NUM_SIZE] = {'\0'};
  Tx_BufferPtr = Tx_Buffer;
  //  char tx_Num_Buffer[Tx_NUM_SIZE] = {'\0'};

  //  ptrLCD -> LCD_Show_Float(fval,1,1,4,Line_2);
  if (num_type == SOB_I)
  {
    snprintf((char *)Tx_Buffer, BUFFER_SIZE, fmt, l_value);
  }

  else // num_type == SOB_F
  {
    snprintf((char *)Tx_Buffer, BUFFER_SIZE, fmt, fval); //
  }

  // essetial initialisations
  transmitted_Chars_Count = 0;
  Tx_BufferPtr = Tx_Buffer;
  // reset end of sending a tx block flag
  end_of_Tx_Block = false;

  Transmit_Char(num_type);
  while (*Tx_BufferPtr != '\0')
  {
    if (*Tx_BufferPtr == ' ')
    {
      Tx_BufferPtr++;
      continue;
    }

    Transmit_Char(*Tx_BufferPtr);
    Tx_BufferPtr++;
    // _delay_us(300);//(INTRPT_DEALY); //while (!(UCSRA & (1 << TXC))){};
    transmitted_Chars_Count++;
  }
  num_type = EOB;
  Transmit_Char(num_type);
  end_of_Tx_Block = true;

  return end_of_Tx_Block;
}
#endif

#if USE_TX_RX_BUFFERS == 1
/// @brief  rturn if no data was recived
/// @return
bool ATMG8A_USART::Data_Received()
{
  bool data_rxed = false;
  volatile unsigned char *tmpCharPtr = Rx_Buffer;
  uint8_t rd_StrSize = used_Rx_Buff_Size;
  // if no buffer used hto store the received numbers
#if (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))
  do
  {
    Receive_Char();
  }
  // continoue to get chars until eob is received which mean a complete number  or data was received
  while (type_Detected && !EOB_Flag);

  if (EOB_Flag) // new_Data_Received_Flag
#endif          //@ (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))
  {
    switch (rxed_Number_Type)
    {
    case SOB_F:
      rxed_Float_Number = strtod((const char *)Rx_Buffer, NULL);
      //  #if (USE_TX_RX_BUFFERS == 1)
      // in case of overflow
      if (rxed_FNums_Count > EXT_F_BUFF_SIZE)
      {
        Reset_Main_Rx_Ptr(rxed_Number_Type);
      }

      *float_Nums_Buffer_Ptr++ = rxed_Float_Number;
      // update the current index value to the last received number
      curr_Rx_F_Buff_Indx++;
      rxed_FNums_Count++;
      data_rxed = true;
      // #endif

      break;

    case SOB_I:
      rxed_Long_Number = strtol((const char *)Rx_Buffer, NULL, 10);
      // if using numbers buffer to store thhe eceived numbs was enabled
      //  #if (USE_TX_RX_BUFFERS == 1)

      // in case of overflow
      if (rxed_LNums_Count > EXT_L_BUFF_SIZE)
      {
        Reset_Main_Rx_Ptr(rxed_Number_Type);
      }

      // update the current index value
      *long_Nums_Buffer_Ptr++ = rxed_Long_Number;
      curr_Rx_L_Buff_Indx++;
      rxed_LNums_Count++;
      data_rxed = true;
      // #endif
      break;

    case SOB_S:
      //  #if (USE_TX_RX_BUFFERS == 1)

      // in case of overflow
      if (EXT_S_BUFF_SIZE - curr_RX_Str_Buff_Indx < used_Rx_Buff_Size)
      {
        Reset_Main_Rx_Ptr(rxed_Number_Type);
      }

      // update the read only received string buffer pointer to the start location of the received message
      read_String_Buffer_Ptr = string_Buffer_Ptr;
      // Rx_BufferPtr = Rx_Buffer;
      while (rd_StrSize)
      // for (uint8_t x=0;x < used_Rx_Buff_Size;x++)
      {
        *string_Buffer_Ptr++ = *tmpCharPtr++;
        curr_RX_Str_Buff_Indx++;
        rd_StrSize--;
      }
      // update the main string buffer pointer to point to the start location of the new received string message
      string_Buffer_Ptr++;
      rxed_String_Count++;
      data_rxed = true;
      //  #endif
      break;
    }
    // if no interrupts was used to receive data
#if (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))
    // Reset the important parameters to receive a new data
    {
      mSerPtr->new_Data_Received_Flag = true;
      // reset the data type for the next data received
      mSerPtr->type_Detected = false;
      mSerPtr->EOB_Flag = false;
      mSerPtr->used_Rx_Buff_Size = 0;
      // reset the Rx buffer pointer and the toral received bytes count
      mSerPtr->Rx_BufferPtr = mSerPtr->Rx_Buffer;
    }
#endif //- (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))
    //@ to run the action function defined in onDataReceived function on the new received data
    action_Executed = false;
  }

  return data_rxed;
}
#endif
//>bool ATMG8A_USART::Data_Received()
//>{
//>  g_Pos = 1;
//>  g_Line =Line_1;
//>  // a temp pointer to the main received char buffer used in case o receiving a string message
//>  volatile unsigned char * tmpCharPtr=Rx_Buffer;
//>  bool return_var = false;
//>  //if no buffer used hto store the received numbers
//>     #if (USE_TX_RX_BUFFERS == 0)
//>        if (EOB_Flag) //new_Data_Received_Flag
//>     #endif
//>  {
//>    // dont do that you'll delete the received data
//>    // Rx_Buffer[BUFFER_SIZE] = {'\0'};
//>
//>    // point the converting pointer used by the converting function to the start location of converting which is the current value of Rx_BufferPtr
//>    //-const char *result_num_ptr = (const char *) Rx_Buffer;
//>    //already set in the rx interrupt routin
//>    //  Rx_BufferPtr =  Rx_Buffer;
//>
//>    if (rxed_Number_Type == SOB_F)
//>    {
//>      //the statment below under test
//>       // sscanf(result_num_ptr,"%f",&rxed_Float_Number); //(sentence,"%s %*s %d",str,&i);
//>    //-  fscanf(&fscanf_Rd_Stream,"%f",&rxed_Float_Number);
//>
//>      //-rxed_Float_Number = strtod(result_num_ptr,NULL);    //(result_num_ptr, &endPtr);
//>      rxed_Float_Number = strtod((const char *)Rx_BufferPtr,NULL);
//>      //if using numbers buffer to store thhe eceived numbs was enabled
//>          #if (USE_TX_RX_BUFFERS == 1)
//>              if (rxed_FNums_Count > EXT_BUFF_SIZE)
//>                {
//>                  Reset_Main_Rx_Ptr(rxed_Number_Type);
//>                  ////float_Buffer_Size = 0;
//>                  // curr_Rx_F_Buff_Indx = -1;
//>                  // float_Nums_Buffer_Ptr = float_Nums_Buffer;
//>                  // read_F_Nums_Buffer_Ptr = float_Nums_Buffer;
//>                  // rxed_FNums_Count = 0;
//>
//>                  //return false;
//>                }
//>
//>                *float_Nums_Buffer_Ptr++ = rxed_Float_Number;
//>                ////float_Buffer_Size++;
//>                                // update the current index value
//>                curr_Rx_F_Buff_Indx++;
//>                rxed_FNums_Count++;
//>                //! should I update the Read pointer here (read_F_Nums_Buffer_Ptr)
//>                return_var = true;
//>          #endif
//>      //-return true;
//>    }
//>
//>    else if (rxed_Number_Type == SOB_I)
//>    {
//>        //in case of overflow
//>                if (rxed_LNums_Count > EXT_BUFF_SIZE)
//>                {
//>                  Reset_Main_Rx_Ptr(rxed_Number_Type);
//>                  /////long_Buffer_Size = 0;
//>                  // curr_Rx_L_Buff_Indx = -1;
//>                  // long_Nums_Buffer_Ptr = long_Nums_Buffer;
//>                  // read_L_Nums_Buffer_Ptr = long_Nums_Buffer_Ptr;
//>                  // rxed_LNums_Count = 0;
//>                  // return false;
//>
//>                }
//>//>max number of digits that can be received as integer number correctly is 9 digits,
//>      //>max valu can be received correctly as integer number is 999999999
//>      //>you can receive 10 digit int number ith a maximum value 1999999999
//>       //the statment below is under test
//>       //sscanf(result_num_ptr,"%ld",&rxed_Long_Number);
//>      //- fscanf(&fscanf_Rd_Stream,"%ld",&rxed_Long_Number);
//>      rxed_Long_Number = strtol((const char *)Rx_BufferPtr, NULL, 10);
//>      //>max number of digits that can be received as integer number correctly is 7 digits,
//>      //>max valu can be received correctly as integer number is 9999999
//>
//>        //if using numbers buffer to store thhe eceived numbs was enabled
//>          #if (USE_TX_RX_BUFFERS == 1)
//>                // update the current index value
//>                *long_Nums_Buffer_Ptr++ = rxed_Long_Number;
//>                curr_Rx_L_Buff_Indx++;
//>                rxed_LNums_Count++;
//>                 return_var = true;
//>               ///// long_Buffer_Size++;
//>               // reset needed parameters for the next number conversion
//>                    //-Rx_BufferPtr = Rx_Buffer;
//>                    //-used_Rx_Buff_Size = 0;
//>                    //-new_Data_Received_Flag = false;
//>
//>          #endif
//>
//>      //-return true;
//>    }
//>
//>     else  //received data is string message
//>    {
//>       //!check the if statement below it should be removed regardless using buffer to store recived data or not because in this case the data is string message
//>       #if (USE_TX_RX_BUFFERS == 1)
//>
//>      //in case of overflow
//>                if (curr_RX_Str_Buff_Indx >= EXT_BUFF_SIZE)
//>                {
//>                  Reset_Main_Rx_Ptr(rxed_Number_Type);
//>                    // rxed_String_Count = 0;
//>                    // curr_RX_Str_Buff_Indx = -1;
//>                    // string_Buffer_Ptr = string_Buffer;
//>                    // read_String_Buffer_Ptr = string_Buffer;
//>                  Wink_Led2();
//>                   Wink_Led2();
//>                    Wink_Led2();
//>                     Wink_Led2();
//>              //    return false;
//>                }
//>
//>       //update the read only received string buffer pointer to the start location of the received message
//>       read_String_Buffer_Ptr = string_Buffer_Ptr;
//>
//>       //while ( *tmpCharPtr != '\0' ) //(* string_Buffer_Ptr != '\0')
//>       for (uint8_t x=1;x<used_Rx_Buff_Size;x++)
//>       {
//>         *string_Buffer_Ptr = *tmpCharPtr ;
//>         curr_RX_Str_Buff_Indx++;
//>         string_Buffer_Ptr++ ;
//>         tmpCharPtr++;
// for debug
//>       //********************
//>        printf("%c",*read_String_Buffer_Ptr++);
//>       //**********************
//>       }
//>       //  read_String_Buffer_Ptr = string_Buffer;
//>       //update the main string buffer pointer to point to the start location of the new received string message
//>       //string_Buffer_Ptr++;
//>
//>      //    //sign for the beginning of a string message block
//>      //  *string_Buffer_Ptr++ = START_OF_MSG;
//>      //   curr_RX_Str_Buff_Indx++;
//>
//>       return_var = true;
//>
//>                 // reset needed parameters for the next number conversion
//>                      //-Rx_BufferPtr = Rx_Buffer;
//>                      //-used_Rx_Buff_Size = 0;
//>                      //-new_Data_Received_Flag = false;
//>          #endif
//>
//>      //-return true;
//>    }
//>
//>    //Rx_BufferPtr = Rx_Buffer;
//>    //used_Rx_Buff_Size = 0;
//>   // new_Data_Received_Flag = false;
//>  }
//>
//>  if (!EOB_Flag)
//>  {
//>    used_Rx_Buff_Size = 0;
//>    //Rx_Buffer[BUFFER_SIZE] = {'\0'};
//>  }
//>action_Executed = false;
//>return return_var;
//>
//>}

// return  strtod(endPtr,NULL);
//   return atof(result_num_ptr);
// return CustomStrtoF(result_num_ptr);

/**
 * @brief Function to get the received number
 *
 * @return true
 * @return false
 */
//*bool ATMG8A_USART::Receive_Number()
//*{
//*  //if no buffer used hto store the received numbers
//*     #if (USE_TX_RX_BUFFERS == 0)
//*        if (new_Data_Received_Flag)
//*     #endif
//*  {
//*    // dont do that you'll delete the received dT
//*    // Rx_Buffer[BUFFER_SIZE] = {'\0'};
//*
//*    Rx_BufferPtr = Rx_Buffer;
//*
//*    const char *result_num_ptr = nullptr; // (const char *)Rx_Buffer;
//*
//*    // detect receving float number sign (begining of receiving a float  number)
//*    uint8_t rxf_ind = Detect_Sign(&Rx_BufferPtr);
//*
//*    // skip adding the float sign if it was detected to the converting function
//*    rxf_ind++;
//*    Rx_BufferPtr++;
//*
//*    // below is the condition if the received int number sign not detected and not found
//*    if (rxf_ind >= used_Rx_Buff_Size)
//*    {
//*      return false;
//*    }
//*
//*    // point the converting pointer used by the converting function to the start location of converting which is the current value of Rx_BufferPtr
//*    result_num_ptr = (const char *)Rx_BufferPtr;
//*
//*    // reset needed parameters for the next number conversion
//*    Rx_BufferPtr = Rx_Buffer;
//*    used_Rx_Buff_Size = 0;
//*    new_Data_Received_Flag = false;
//*
//*    // this is just for test remove it in the final release
//*    //transmission_End = true;
//*    if (rxed_Number_Type == SOB_F)
//*    {
//*      rxed_Float_Number = strtod(result_num_ptr, &endPtr);
//*
//*      //if using numbers buffer to store thhe eceived numbs was enabled
//*          #if (USE_TX_RX_BUFFERS == 1)
//*                // update the current index value
//*                curr_Rx_F_Buff_Indx++;
//*                *float_Nums_Buffer_Ptr++ = rxed_Float_Number;
//*                float_Buffer_Size++;
//*                rxed_FNums_Count++;
//*
//*                   //in case of overflow
//*                if (float_Buffer_Size > EXT_BUFF_SIZE)
//*                {
//*                  float_Buffer_Size = 0;
//*                  curr_Rx_F_Buff_Indx = -1;
//*                  float_Nums_Buffer_Ptr = float_Nums_Buffer;
//*                  rxed_FNums_Count = 0;
//*                }
//*          #endif
//*      return true;
//*    }
//*
//*    else if (rxed_Number_Type == SOB_I)
//*    {
//*      //>max number of digits that can be received as integer number correctly is 9 digits,
//*      //>max valu can be received correctly as integer number is 999999999
//*      //>you can receive 10 digit int number ith a maximum value 1999999999
//*      rxed_Long_Number = strtol(result_num_ptr, NULL, 10);
//*      //>max number of digits that can be received as integer number correctly is 7 digits,
//*      //>max valu can be received correctly as integer number is 9999999
//*
//*        //if using numbers buffer to store thhe eceived numbs was enabled
//*          #if (USE_TX_RX_BUFFERS == 1)
//*                // update the current index value
//*                *long_Nums_Buffer_Ptr++ = rxed_Long_Number;
//*                curr_Rx_L_Buff_Indx++;
//*                long_Buffer_Size++;
//*                rxed_LNums_Count++;
//*
//*                //in case of overflow
//*                if (long_Buffer_Size > EXT_BUFF_SIZE)
//*                {
//*                  long_Buffer_Size = 0;
//*                  curr_Rx_L_Buff_Indx = -1;
//*                  long_Nums_Buffer_Ptr = long_Nums_Buffer;
//*                  rxed_LNums_Count = 0;
//*                }
//*          #endif
//*
//*      return true;
//*    }
//*  }
//*
//*  return false;
//*  // return  strtod(endPtr,NULL);
//*  //   return atof(result_num_ptr);
//*  // return CustomStrtoF(result_num_ptr);
//*}
//?bool ATMG8A_USART::Receive_Number()
//?{
//?  if (new_Data_Received_Flag)
//?  {
//?    // dont do that you'll delete the received dT
//?    // Rx_Buffer[BUFFER_SIZE] = {'\0'};
//?
//?    Rx_BufferPtr = Rx_Buffer;
//?
//?    const char *result_num_ptr = nullptr; // (const char *)Rx_Buffer;
//?
//?    // detect receving float number sign (begining of receiving a float  number)
//?    uint8_t rxf_ind = Detect_Sign(&Rx_BufferPtr);
//?
//?    // skip adding the float sign if it was detected to the converting function
//?    rxf_ind++;
//?    Rx_BufferPtr++;
//?
//?    // below is the condition if the received int number sign not detected and not found
//?    if (rxf_ind >= used_Rx_Buff_Size)
//?    {
//?      return false;
//?    }
//?
//?    //point the converting pointer used by the converting function to the start location of converting which is the current value of Rx_BufferPtr
//?    result_num_ptr = (const char *)Rx_BufferPtr;
//?
//?    // reset needed parameters for the next number conversion
//?    Rx_BufferPtr = Rx_Buffer;
//?    used_Rx_Buff_Size = 0;
//?    new_Data_Received_Flag = false;
//?
//?    // this is just for test remove it in the final release
//?    //transmission_End = true;
//?    if (rxed_Number_Type == SOB_F)
//?    {
//?      rxed_Float_Number = strtod(result_num_ptr, &endPtr);
//?      return true;
//?    }
//?
//?    else if (rxed_Number_Type == SOB_I)
//?    {
//?      //>max number of digits that can be received as integer number correctly is 9 digits,
//?      //>max valu can be received correctly as integer number is 999999999
//?      //>you can receive 10 digit int number ith a maximum value 1999999999
//?      rxed_Long_Number = strtol(result_num_ptr, NULL, 10);
//?      //>max number of digits that can be received as integer number correctly is 7 digits,
//?      //>max valu can be received correctly as integer number is 9999999
//?      return true;
//?    }
//?  }
//?
//?  return false;
//?  // return  strtod(endPtr,NULL);
//?  //   return atof(result_num_ptr);
//?  // return CustomStrtoF(result_num_ptr);
//?}

// #include <math.h>
// #include <string.h>
// using namespace std;
// # define FPRECISION 5;
// static double PRECISION = 0.00000000000001;
// static int MAX_NUMBER_STRING_SIZE = 50;//32;

/*
static int
   uart_putchar(char c, FILE *stream)
   {

     if (c == '\n')
       uart_putchar('\r', stream);
     loop_until_bit_is_set(UCSRA, UDRE);
     UDR = c;
     return 0;
   }

   int uart_putchar(char c, FILE *stream)
   {

     if (c == '\n')
       uart_putchar('\r', stream);
     loop_until_bit_is_set(UCSRA, UDRE);
     UDR = c;
     return 0;
   }*/

// void ATMG8A_USART::Transmit_String()
// {
//   //reset the tx index buffer
//   transmitted_Chars_Count = 0;
//   unsigned char eb= EOB;
//  do
//   {
//     if (Tx_Buffer[transmitted_Chars_Count] == ' ')
//       break;
//       //Transmit chars stored in the Tx buffer using the Tx interrupt routin
//     //? if (transmitted_Chars_Count < used_Tx_Buffer_Size)
//     {
//        //below statements will fire the tx interrupt service routin
//        Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//        USART_Setup_Interrupt();
//     }

//     // _delay_us(330); // instead of 2 ms   //>Optimum value  = 330us
//   }
//   //while (transmitted_Chars_Count < used_Tx_Buffer_Size);

//   while (Tx_Buffer[transmitted_Chars_Count] != '\0');
//  //? update the last element in the tx buffer with the End of transmision without using interrupt
//   //Tx_Buffer[transmitted_Chars_Count] = EOB;
//   Transmit_Char(eb);

// }

// This code doesn't use Interrupt
//  void ATMG8A_USART::Transmit_String()
//  {
//    //> reset the pointer to the 1st element in the buffer
//    Tx_BufferPtr = Tx_Buffer;
//    //reset the tx index buffer
//    transmitted_Chars_Count = 0;

//   do
//   {
//     if (*Tx_BufferPtr == ' ')
//       break;
//     Transmit_Char(*Tx_BufferPtr);
//     //point to the next char
//     Tx_BufferPtr++;
//     _delay_us(330); // instead of 2 ms   //>Optimum value  = 330us
//   }

//   while (*Tx_BufferPtr != '\0');

//   *Tx_BufferPtr = EOB; //? update the last element in the tx buffer with the End of transmision
//   Transmit_Char(*Tx_BufferPtr);

// }

//_void ATMG8A_USART::Transmit_String()
//_{
//_  //> reset the pointer to the 1st element in the buffer
//_  Tx_BufferPtr = Tx_Buffer;
//_  unsigned char tch = 0;
//_  do
//_  {
//_    tch = *Tx_BufferPtr++;
//_    if (tch == ' ')
//_      break;
//_    Transmit_Char(tch);
//_    _delay_us(330); // instead of 2 ms   //>Optimum value  = 330us
//_  }
//_
//_  while (*Tx_BufferPtr != '\0');
//_
//_  tch = EOB; //? End of transmision
//_  Transmit_Char(tch);
//_  { // buff[30]={'\0'}; //clear the buffer
//_    /* for(uint8_t i=0; i <30;i++)
//_     {
//_       Tx_Buffer[i] = '\0';
//_     }*/}
//_}
/**
 * @brief Transmit string using interrupt& pointer technique
 *
 * @param strptr : pointer to the string that will be transmitted
 */
//*void ATMG8A_USART::Transmit_Message(unsigned char * strptr)
//*{
//*  Tx_BufferPtr=Tx_Buffer;
//*  //transmitted_Chars_Count =0;
//*  used_Tx_Buffer_Size=0;
//* //! for(uint8_t x=0;x<BUFFER_SIZE;x++)
//* //! {
//* //!   *Tx_BufferPtr = '\0';
//* //!
//* //! }
//* //! Tx_BufferPtr=Tx_Buffer;
//*  //*Tx_BufferPtr++ = SOB_S;
//*  Add_Tx_Character(SOB_S);
//* // unsigned char  * delPtr =Tx_BufferPtr;
//*  while (* strptr != '\0')
//*  {
//*
//*    Add_Tx_Character(*strptr++);
//*
//*
//*       // Wink_Led3();
//*        //_delay_ms(900);
//*  }
//*
//* //!python don't need this
//*// Add_Tx_Character('\0');
//*//add end of transmission sign
//* Add_Tx_Character(EOB);
//*
//*
//* //> ptrLCD->LCD_Show_Int(used_Tx_Buffer_Size,Page_1,15,Line_1);
//* //>  //_delay_ms(500);
//* //>  //>pos=1;
//* //.for(uint8_t i =0 ; i < used_Tx_Buffer_Size; i++)
//* //. {
//* //.
//* //.   ptrLCD->LCD_Show_Character(Tx_Buffer[i],Page_1,pos,Line_1);
//* //.   pos++;
//* //.   if(pos >used_Tx_Buffer_Size)
//* //.   pos=1;
//* //. }
//* Transmit_String();
//* //>
//* //> //reset the Tx buffer pointer once againn to begin sending the string
//*//!  transmitted_Chars_Count =0;
//*//!  Tx_BufferPtr=Tx_Buffer;
//*//!  pos = 1;
//*//!  for(uint8_t t_ind =0 ; t_ind < used_Tx_Buffer_Size; t_ind++)
//*//!    {
//*//!      ptrLCD->LCD_Show_Character(Tx_Buffer[t_ind],Page_1,pos,Line_1);
//*//!      pos++;
//*//!      if(pos >used_Tx_Buffer_Size)
//*//!      pos=1;
//*//!      Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//*//!      USART_Setup_Interrupt();
//*//!   //   Wink_Led2();
//*//!     // _delay_ms(500);
//*//!
//*//! //?         USART_Setup_Interrupt();
//*//!    }
//* //>//? while (* strptr != '\0')
//* //>//?
//* //>//? {
//* //>//?    // enabling the tx interrrupt will fire the tx interrupt routine and send a charcter by a charcter
//* //>//?     if(end_of_Tx_Block_Block)
//* //>//?       {
//* //>//?        // Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//* //>//?        // USART_Setup_Interrupt();
//* //>//?         strptr++;
//* //>//?         Wink_Led3();
//* //>//?       }
//* //>//? }
//* //>
//* //> transmitted_Chars_Count =0;
//* //> used_Tx_Buffer_Size=0;
//* //> Tx_BufferPtr=Tx_Buffer;
//*
//*}

// If Enabled use buffers to store the received numbers
//! #if (USE_TX_RX_BUFFERS == 1)
//!
//!     /// @brief To Get the last stored float number in the buffer
//!     ///@param parameter-fn store the last float number stored in the buffer
//!     /// @return true if the buffer not empty other wise false
//!     bool ATMG8A_USART::Get_Rxed_F_Num(float & fn)
//!     {
//!        // if the buffer is empty
//!        if (curr_Rx_F_Buff_Indx < 0)
//!        {
//!          return false;
//!        }
//!
//!         fn =float_Nums_Buffer[curr_Rx_F_Buff_Indx];
//!         return true ;
//!     }
//!
//!     /// @brief To Get the last stored long number in the buffer
//!     ///@param parameter-ln store the last long number stored in the buffer
//!     /// @return true if the buffer not empty other wise false
//!
//!     bool ATMG8A_USART::Get_Rxed_L_Num(long& ln)
//!     {
//!       if (curr_Rx_L_Buff_Indx < 0)
//!       {
//!         return false;
//!       }
//!
//!         ln = long_Nums_Buffer[curr_Rx_L_Buff_Indx];
//!
//!         return true;
//!     }
//!
//!     /// @brief
//!     /// @param nty is the type of buffer's numbers (float or long)
//!     /// @return return the current size of the required buffer
//!     uint8_t ATMG8A_USART::Get_Rxed_Nums_Count(unsigned char nty)
//!     {
//!         if (nty == SOB_F)
//!           return rxed_FNums_Count;
//!         else
//!           return rxed_LNums_Count;
//!     }
//!        /// @brief Reset the main buffers pointers to store numbers from the begining and over write the existing numbers
//!        /// @param pt: pointer type to be reset float or long
//!        void ATMG8A_USART::Reset_Main_Rx_Ptr(unsigned char pt)
//!         {
//!
//!             if(pt == SOB_F)
//!             {
//!               float_Nums_Buffer_Ptr = float_Nums_Buffer;
//!               rxed_FNums_Count = 0;
//!                   curr_Rx_F_Buff_Indx = -1;
//!                    //tmpCnt_F =0;
//!             }
//!             else if(pt == SOB_I)
//!             {
//!               long_Nums_Buffer_Ptr = long_Nums_Buffer;
//!                rxed_LNums_Count = 0;
//!                   curr_Rx_L_Buff_Indx = -1;
//!                   //tmpCnt_L =0;
//!             }
//!             else //in case of SOB_S
//!             {
//!               string_Buffer_Ptr = string_Buffer;
//!                rxed_String_Count = 0;
//!                   curr_RX_Str_Buff_Indx = -1;
//!                   //tmpCnt_L =0;
//!             }
//!
//!         }
//! #endif
//*void ATMG8A_USART::Transmit_long(long l_value)
//*{
//*   char long_buf_tx[Tx_NUM_SIZE] = {'\0'};
//*  char * tmp_l_ptr = long_buf_tx;
//*  // below point to the location index will be written in the next add operation also equal to the number of charcters that added to the buffer
//*  used_Tx_Buffer_Size = 0;
//*
//*   //> Clear Tx buffer
//*  // Tx_Buffer[BUFFER_SIZE] = {'\0'};
//*
//*     Tx_BufferPtr = Tx_Buffer;
//*     sprintf(long_buf_tx, "%-12ld", l_value);
//*    //add a int number sighn as the 1st charcter to be send
//*    Add_Tx_Character(SOB_I);
//*    // { // sprintf(strPtr,"%-6d", value);
//*    // dtostre(value, strPtr, 4, DTOSTR_ALWAYS_SIGN | DTOSTR_UPPERCASE); // note that precision is defined in the top of this file
//*    //'I';
//* // }
//*
//*  while ((*tmp_l_ptr != '\0')  ) // & (*Tx_BufferPtr != ' ')
//*  {
//*   if(*tmp_l_ptr  == ' ')
//*   {
//*     tmp_l_ptr++ ;
//*       continue; //{break;}
//*   }
//*   Add_Tx_Character(*tmp_l_ptr++) ;
//*  }
//*   //add the end of transmission sighn
//*  Add_Tx_Character(EOB);
//*
//*  //reset the current Tx index to send the converted number
//*
//*  transmitted_Chars_Count=0;
//*   Tx_BufferPtr = Tx_Buffer;
//*    end_of_Tx_Block = false;
//*  //send the converted float number
//*
//* while (!end_of_Tx_Block)
//*    {
//*        //
//*       // enabling the tx interrrupt will fire the tx interrupt routine and send a charcter by a charcter
//*          Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//*          USART_Setup_Interrupt();
//*             _delay_us(INTRPT_DEALY);
//*    }
//* //! for (uint8_t i = 0; i < used_Tx_Buffer_Size; i++)
//* //!  {
//* //!    // enabling the tx interrrupt will fire the tx interrupt routine and send a charcter by a charcter
//* //!    Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//* //!    USART_Setup_Interrupt();
//* //!  }
//* //reset the current add index
//* used_Tx_Buffer_Size=0;
//*
//*}

//>const char * ATMG8A_USART::Receive_Message()
//>{
//> if(!new_Data_Received_Flag)
//> {
//>    _delay_ms(800);
//>    //Wink_Led2();
//>   return nullptr;
//>
//> }
//> // ptrLCD->LCD_Show_Int(used_Rx_Buff_Size,Page_1,Line_2,15,global_buffer);
//>
//>
//>   //for (uint8_t i =0;i <= used_Buffer_Size;i++)
//>   //{
//>   // ptrLCD->LCD_Show_Character(Page_2,pos,Line_1,Rx_Buffer[i]);
//>   //     pos++;
//>   //     if(pos >used_Buffer_Size)
//>   //     pos=1;
//>   //}
//>
//> // unsigned char * tptr= Rx_BufferPtr;
//> //for(uint8_t i=0;i<BUFFER_SIZE;i++)
//> //
//> //{ *tptr++ = '\0';}
//>   ///reset to receive a new block
//>
//>      Rx_BufferPtr=Rx_Buffer;
//>      used_Rx_Buff_Size =0;
//>
//>//  if(new_Data_Received_Flag)
//>   {
//>    //rx buffer pointer has reset in the rx interrupt routin
//>     uint8_t start_str_indx = Detect_Sign(&Rx_BufferPtr);
//>
//>      //skip  the string sign if it was detected
//>      Rx_BufferPtr++;
//>
//>     // below is the condition if the received string  sign not detected and not found
//>    if((start_str_indx) >= used_Rx_Buff_Size)
//>    {
//>      return nullptr;
//>    }
//>    //set the pointer to the start location of the received string
//>     unsigned char *str_BeginingPtr  = Rx_BufferPtr;
//>   //!didn't executed
//>   //ptrLCD->LCD_Show_Int(used_Rx_Buff_Size,Page_1,Line_2,15,global_buffer);
//>
//>   //ptrLCD->LCD_Show_Int(start_str_indx,Page_1,Line_2,11,global_buffer);
//>
//>   //<pos=1;
//>   //skip string sign detected
//>   //< for (uint8_t i =1;i < used_Buffer_Size;i++)
//>   //< {
//>   //<  ptrLCD->LCD_Show_Character(Page_1,pos,Line_2,Rx_Buffer[i]);
//>   //<      pos++;
//>   //<      if(pos >used_Buffer_Size)
//>   //<      pos=1;
//>   //< }
//>
//>   //.char strMsg[used_Buffer_Size];
//>   //.char *str_BeginingPtr  = strMsg;
//>   //.for (uint8_t i =0;i <= used_Buffer_Size;i++)
//>   //.  {
//>   //.
//>   //.    *str_BeginingPtr =  *Rx_BufferPtr;
//>   //.     if(*str_BeginingPtr == '\0')
//>   //.     {break;}
//>   //.          str_BeginingPtr++;
//>   //.    Rx_BufferPtr++;
//>   //.
//>   //.  }
//>
//>      return (const char *)str_BeginingPtr;
//>    }
//>    //return (const char *)str_BeginingPtr;
//>
//>
//>//  (*Rx_BufferPtr) = '\0'; // add null termainating charcter (or any non digit char) to make strtol function work properly
//>
//>}

/// @brief Add a new char to the tx buffer
/// @param d_chr the tended char to be added into the tX BUFFER
/// @return The remaining buffer size ready for new added characters
int8_t ATMG8A_USART::Add_Tx_Character(unsigned char d_chr)
{
  // buffer overflow case
  if (used_Tx_Buffer_Size > BUFFER_SIZE)
  { // reset the add index
    used_Tx_Buffer_Size = 0;
    Tx_BufferPtr = Tx_Buffer;
  }
  *Tx_BufferPtr++ = d_chr;
  // This is the current empty buffer location or the next location to be written which is equal to the total characters stored in the buffer
  used_Tx_Buffer_Size++;

  // the total number of characters added to the tx buffer to be send
  return used_Tx_Buffer_Size; // remaining_tx_size;
}

bool ATMG8A_USART::Is_New_Data()
{
  return action_Executed;
}
void ATMG8A_USART::On_Data_Received(void (*FnPtr)(void))
{
  if (action_Executed)
    return;
  // tmpCnt_F = 0;
  // tmpCnt_L =0;
  // tmpCnt_S =0;
  switch (rxed_Number_Type)
  {
  case SOB_F:
    // if new numbers were recived
    // action exxecuted until tmpCnt_F = to the index o the last stored number in the buffer
    while (tmpCnt_F < rxed_FNums_Count)
    {
      // Add here Action to be taken
      FnPtr();
      tmpCnt_F++;
      // update tmpCnt_F to be equal to rxed_FNums_Count to mean that the action is executed as the count of the received numbers   in the buffer
    }
    //  tmpCnt_F = rxed_FNums_Count;
    //- you can add anoter action here to be done after processing the new received data
    //@ to prevent looping function execution
    action_Executed = true;
    break;

  case SOB_I:
    // if new numbers were recived
    //  action exxecuted until tmpCnt_F or tmpCnt_L  equals to the index of the last stored number in the buffer
    while (tmpCnt_L < rxed_LNums_Count)
    {
      // Add here Action to be taken
      FnPtr();
      tmpCnt_L++;
    }
    // tmpCnt_L = rxed_LNums_Count;
    //- you can add anoter action here to be done after processing the new received data
    //@ to prevent looping function execution
    action_Executed = true;
    break;

  case SOB_S:
    // if new message were recived
    //  action exxecuted until tmpCnt_S equals to the index of the last stored number in the buffer

    while (tmpCnt_S <= curr_RX_Str_Buff_Indx) // if (tmpCnt_S < rxed_String_Count) //
    {
      // Add here Action to be taken
      FnPtr();
      // Wink_Led3();
      tmpCnt_S++;
    }
    // tmpCnt_S = curr_RX_Str_Buff_Indx;
    //-  you can add anoter action here to be done after processing the new received data
    //@ to prevent looping function execution
    action_Executed = true;
    break;

    //- default:
    //-   // to prevent looping function execution
    //-     action_Executed = true;
    //-break;
  }
}

//>void ATMG8A_USART::On_Data_Received(void (*FnPtr)(void))
//>{
//>  if(action_Executed)
//>  return ;
//>
//>  {
//>    if (rxed_Number_Type == SOB_F) // if (ntyp == SOB_F)
//>    {
//>      //> if (tmpCnt_F > EXT_BUFF_SIZE)
//>      //>  {
//>      //>    tmpCnt_F = 0;
//>      //>    Reset_Main_Rx_Ptr(rxed_Number_Type);
//>      //>    return;
//>      //>  }
//>
//>      // if new numbrs were recived
//>      //  action exxecuted until tmpCnt_F = to the index o the last stored number in the buffer
//>      while (tmpCnt_F < rxed_FNums_Count)
//>      {
//>        // Add here Action to be taken
//>        FnPtr();
//>        tmpCnt_F++;
//>        // update tmpCnt_F to be equal to rxed_FNums_Count to mean that the action is executed as the count of the received numbers   in the buffer
//>      }
//>    //  tmpCnt_F = rxed_FNums_Count;
//>      // you can add anoter action here to be done after processing the new received data
//>    }
//>
//>    else if (rxed_Number_Type == SOB_I) //(ntyp == SOB_I)//number type is long int
//>    {
//>
//>      //  t_sz = rxed_LNums_Count;
//>      //>if (tmpCnt_L > EXT_BUFF_SIZE)
//>      //>  {
//>      //>    tmpCnt_L = 0;
//>      //>    Reset_Main_Rx_Ptr(rxed_Number_Type);
//>      //>    return;
//>      //>  }
//>
//>      // if new numbers were recived
//>      //  action exxecuted until tmpCnt_F or tmpCnt_L  equals to the index of the last stored number in the buffer
//>      while (tmpCnt_L < rxed_LNums_Count)
//>      {
//>        // Add here Action to be taken
//>        FnPtr();
//>        tmpCnt_L++;
//>      }
//>     // tmpCnt_L = rxed_LNums_Count;
//>      // you can add anoter action here to be done after processing the new received data
//>    }
//>
//>    else if (rxed_Number_Type == SOB_S) // in case of string message //(ntyp == SOB_I)//number type is long int
//>    {
//>      //  t_sz = rxed_LNums_Count;
//>      //>if (tmpCnt_S > EXT_BUFF_SIZE)
//>      //>  {
//>      //>    tmpCnt_S = 0;
//>      //>    Reset_Main_Rx_Ptr(rxed_Number_Type);
//>      //>    return;
//>      //>  }
//>      // if new message were recived
//>      //  action exxecuted until tmpCnt_S equals to the index of the last stored number in the buffer
//>
//>
//>      while (tmpCnt_S <= curr_RX_Str_Buff_Indx) // if (tmpCnt_S < rxed_String_Count) //
//>      {
//>        // Add here Action to be taken
//>        FnPtr();
//>        // Wink_Led3();
//>        tmpCnt_S++;
//>      }
//>      // tmpCnt_S = curr_RX_Str_Buff_Indx;
//>      //  you can add anoter action here to be done after processing the new received data
//>    }
//>    // to prevent looping function execution
//>    action_Executed = true;
//>
//>  }
//>}

//-int Read_Byte_From_Stdinput(FILE * indev)
//-{
//-
//-
//-    for(uint8_t t =0; t <1;t++)
//-    {
//-
//-      *indev->buf++ = '1';
//-       *indev->buf++ = '2';
//-     *indev->buf++= '3';
//-     *indev->buf++= '4';
//-       *indev->buf++= '.';
//-       *indev->buf++ = '5';
//-     *indev->buf++ = '6';
//-         *indev->buf++= '\n';
//-
//-
//-    }
//-    return 8;
//-}

// float fval, int8_t nw = 12, uint8_t np = 4
/**
 * @brief Transmit float number using interrupt technique
 *
 * @param fval float number wished to transmit
 * @param nw min float number width include dot and the sighn recommended = 1
 * @param np the number of digits after the period
 */
//*void ATMG8A_USART::Transmit_Float(float fval, int8_t nw = 1, uint8_t np = 4) //(float fval,int8_t nw = 12,uint8_t np = 4)
//*{
//*  char tx_Num_Buffer[Tx_NUM_SIZE]={'\0'};
//*
//*  // below point to the location will be written in the next add operation also equal to the number of charcters that added to the buffer
//*  used_Tx_Buffer_Size = 0;
//*  Tx_BufferPtr = Tx_Buffer;
//*
//*  dtostrf(fval, nw, np,tx_Num_Buffer); //original
//*   char * tmp_f_ptr=tx_Num_Buffer;
//*//> Tx_BufferPtr++;
//* // while(*tmp_f_ptr++ != '\0')
//* //{
//* //add a float number sighn as the 1st charcter to be send
//*  Add_Tx_Character(SOB_F);
//*  // add the converted number to the Tx buffer
//*  // don't add and send '\0' char to python, it will make an error when trying to convert the transmitted string to a float number
//*  while(*tmp_f_ptr != '\0')
//*  {
//*   if(*tmp_f_ptr  == ' ')
//*    {
//*       tmp_f_ptr++ ;
//*       continue; //{break;}
//*    }
//*   Add_Tx_Character(*tmp_f_ptr++) ;
//*
//*  }
//*
//*   //add the end of transmission sighn
//*  Add_Tx_Character(EOB);
//*   transmitted_Chars_Count=0;
//*   Tx_BufferPtr = Tx_Buffer;
//*  // reset end of tx bloak flag
//*  end_of_Tx_Block = false;
//*
//* // unsigned char * cp = Tx_Buffer;
//* //for(uint8_t i = 0;i< used_Tx_Buffer_Size;i++)
//* //{
//* //   //ptrLCD -> LCD_Show_Character(Page_1,pos,Line_1,*cp++);
//* //   //   pos++;
//* //   //   if(pos > used_Tx_Buffer_Size)
//* //   //   pos=1;
//* //       Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//* //       USART_Setup_Interrupt();
//* //       _delay_us(180);
//* //}
//*    while (!end_of_Tx_Block)
//*    {
//*        //
//*       // enabling the tx interrrupt will fire the tx interrupt routine and send a charcter by a charcter
//*          Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//*          USART_Setup_Interrupt();
//*             _delay_us(INTRPT_DEALY);
//*    }
//*
//*  //?for (uint8_t i = 0; i < used_Tx_Buffer_Size; i++)
//*  //? {
//*  //?   // enabling the tx interrrupt will fire the tx interrupt routine and send a charcter by a charcter
//*  //?   Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//*  //?   USART_Setup_Interrupt();
//*  //? }
//* //reset the current add index
//* used_Tx_Buffer_Size=0;
//* //this is just for test remove it in the final release
//*    stop = true;
//*
//*
//* //un complete idea
//*//  unsigned char st1,st2;
//*// for(uint8_t j=0;j<used_Tx_Buffer_Size;j++)
//*// {  if(Tx_Buffer[j+1] = '\0')
//*//          break;
//*//    st1 =Tx_Buffer[j+1];
//*//    Tx_Buffer[j+1]= Tx_Buffer[j];
//*//
//*// }
//*
//*//uncomplete idea
//*//>  Tx_BufferPtr = Tx_Buffer;
//*//>unsigned char *str_BeginingPtr  = Tx_BufferPtr+1;
//*//>  unsigned char st1,st2;
//*//>  while(*Tx_BufferPtr != '\0')
//*//> {
//*//>  //backup location 2
//*//>  st1= *str_BeginingPtr;
//*//>  //copy location to location 2
//*//>  *str_BeginingPtr=*Tx_BufferPtr;
//*//>  //increment pointers by 2 to point to location 3 and 4
//*//>  Tx_BufferPtr+=2;
//*//>  str_BeginingPtr +=2;
//*//>  //backup location 3
//*//>  st2 = *Tx_BufferPtr;
//*//>  //copy content of location 2 to location 3
//*//>  *Tx_BufferPtr = st1;
//*//>
//*//>  //
//*//>
//*//> }
//*
//*  //reset the current Tx index to send the converted number
//*
//*
//*}

// working function
//  void ATMG8A_USART::Transmit_Float(float fval, int8_t nw = 25, uint8_t np = 12) //(float fval,int8_t nw = 12,uint8_t np = 4)
//  {
//    char float_buf[Tx_NUM_SIZE]={'\0'};
//    // below point to the location will be written in the next add operation also equal to the number of charcters that added to the buffer
//    used_Tx_Buffer_Size = 0;

//    //> Clear Tx buffer
//   // Tx_Buffer[BUFFER_SIZE] = {'\0'};
//      Tx_BufferPtr = Tx_Buffer;
//  //add a float number sighn as the 1st charcter to be send
//   Add_Tx_Character(SOB_F);

//  //ptrLCD-> LCD_ShowFloat(Page_1,1,Line_1,fval,1,4,(char *)global_buffer);
//  //! *Tx_BufferPtr++ = SOB_F;
//  //! used_Tx_Buffer_Size++;
//     dtostrf(fval, nw, np,float_buf); //original

//  for (uint8_t i = 0; i < Tx_NUM_SIZE; i++)
//  {
//     // don't add and send '\0' char to python, it will make an error when trying to convert the transmitted string to a float number
//     if(float_buf[i] == '\0')
//      break;
//     Add_Tx_Character(float_buf[i]);
//    //! *Tx_BufferPtr=float_buf[i];
//    //! used_Tx_Buffer_Size++;
//    //! Tx_BufferPtr++;

//  }

//  //add the end of transmission sighn
//   Add_Tx_Character(EOB);
//   //!*Tx_BufferPtr = EOB;
//   //! Tx_BufferPtr++;
//   //!used_Tx_Buffer_Size++;

//   //reset the current Tx index to send the converted number

//   transmitted_Chars_Count=0;
//   //Tx_BufferPtr = Tx_Buffer;

//   //send the converted float number
//   for (uint8_t i = 0; i < used_Tx_Buffer_Size; i++)
//    {
//      // enabling the tx interrrupt will fire the tx interrupt routine and send a charcter by a charcter
//      Rx_Tx_Intrerrupts.U_DataRegisterEmpty_IINTR = 1;
//      USART_Setup_Interrupt();
//    }

// }

// under setup

// under test

//*long ATMG8A_USART::Receive_Int_Num()
//*{
//* Rx_Buffer[BUFFER_SIZE] = {'\0'};
//* Rx_BufferPtr = Rx_Buffer;
//*
//*const char *res_l_num = nullptr;// (const char *)Rx_Buffer;
//*
//*// detect receving int number sighn (begining of receiving a long int  number)
//* uint8_t rxi_ind= Detect_Sign(SOB_I,&Rx_BufferPtr);
//*
//*
//*// detect receving int number sighn (begining of receiving a long int  number)
//*
//* //?for (;rxi_ind < used_Rx_Buff_Size;rxi_ind++)
//* //?{
//* //? //receving int number sighn
//* //? if(*Rx_BufferPtr  == SOB_I)
//* //?   {break;}
//* //?   Rx_BufferPtr++;
//* //?}
//*
//* //skip adding  the int sighn if it was detected to the converting function
//* rxi_ind ++;
//* Rx_BufferPtr++;
//*
//* // below is the condition if the received int number sighn not detected and not found
//* if((rxi_ind)  >= used_Rx_Buff_Size)
//*  return 0;
//* // point the converting pointer used by the converting function to the start location of converting which is the current value of Rx_BufferPtr
//* res_l_num =(const char *)Rx_BufferPtr;
//* //reset needed parameters for the next number conversion
//* Rx_BufferPtr = Rx_Buffer;
//* used_Rx_Buff_Size =0;
//*
//* //>max number of digits that can be received as integer number correctly is 9 digits,
//* //>max valu can be received correctly as integer number is 999999999
//* //>you can receive 10 digit int number ith a maximum value 1999999999
//* return strtol(res_l_num, NULL,10);
//*
//*  //>max number of digits that can be received as integer number correctly is 7 digits,
//*  //>max valu can be received correctly as integer number is 9999999
//*  //return strtod(res_l_num,&endPtr);
//*}

// long ATMG8A_USART::Receive_Int_Num()
// {
//   Rx_BufferPtr = Rx_Buffer;

//   const char *res_l_num = (const char *)Rx_Buffer;
//   // long rv=0;
//   // uint8_t ind = 0;

//   // begining of receiving a long int  number sign
//   do
//   {
//     Receive_Char();
//     //reset the buffer pointer tell detecting beggining of receiving sign
//     Rx_BufferPtr = Rx_Buffer;
//   } while (*Rx_BufferPtr != SOB_I);
//    //reset the current Rx index which is also the current used Rx buffer size
//       used_Rx_Buff_Size = 0;
//    // while end of transmission not detected
//     do
//     {
//       // get the next one
//       Receive_Char();

//     } while ( *Rx_BufferPtr++ != EOB);

//   Rx_BufferPtr++;
//   (*Rx_BufferPtr) = '\0'; // add null termainating charcter (or any non digit char) to make strtol function work properly

//   return strtod(res_l_num, NULL); // atof(res_l_num);
// }

// long ATMG8A_USART::Receive_Int_Num()
// {
//   Rx_BufferPtr = Rx_Buffer;
//   unsigned char rx_chr = ' ';
//   const char *res_l_num = (const char *)Rx_Buffer;
//   // long rv=0;
//   // uint8_t ind = 0;

//   // begining of receiving a long int  number sign
//   do
//   {
//     Receive_Char(rx_chr);
//     //  Wink_Both();
//   } while (rx_chr != SOB_I);

//    // while end of transmission not detected
//     do
//     {
//       // get the next one
//       Receive_Char(rx_chr);
//       // store the received char
//       (*Rx_BufferPtr++) = rx_chr;

//     } while (rx_chr != EOB);

//   Rx_BufferPtr++;
//   (*Rx_BufferPtr) = '\0'; // add null termainating charcter (or any non digit char) to make strtol function work properly

//   return strtod(res_l_num, NULL); // atof(res_l_num);
// }

//*double ATMG8A_USART::Receive_Float_Num()
//*{
//*  if(new_Data_Received_Flag)
//*  {
//*
//*
//*    //dont do that you'll delete the received dT
//*    //Rx_Buffer[BUFFER_SIZE] = {'\0'};
//*    // unsigned char * tmp_rxPtr = Rx_Buffer;
//*    Rx_BufferPtr = Rx_Buffer;
//*
//*    // unsigned char rx_fchr = 0;
//*    const char *res_F_num = nullptr; // (const char *)Rx_Buffer;
//*
//*    // detect receving float number sign (begining of receiving a float  number)
//*    uint8_t rxf_ind = Detect_Sign(SOB_F, &Rx_BufferPtr);
//*
//*    // skip adding  the float sign if it was detected to the converting function
//*    rxf_ind++;
//*    Rx_BufferPtr++;
//*
//*    // below is the condition if the received int number sighn not detected and not found
//*    if (rxf_ind >= used_Rx_Buff_Size)
//*    {
//*      return 0;
//*    }
//*
//*    // point the converting pointer used by the converting function to the start location of converting which is the current value of Rx_BufferPtr
//*    res_F_num = (const char *)Rx_BufferPtr;
//*
//*    // reset needed parameters for the next number conversion
//*    Rx_BufferPtr = Rx_Buffer;
//*    used_Rx_Buff_Size = 0;
//*    new_Data_Received_Flag = false;
//*
//*    //this is just for test remove it in the final release
//*    transmission_End = true;
//*    return strtod(res_F_num, &endPtr);
//*  }
//*  return 0;
//* //return  strtod(endPtr,NULL);
//* //  return atof(res_F_num);
//*    //return CustomStrtoF(res_F_num);
//*}

//! working function
//! double ATMG8A_USART::Receive_Float_Num()
//!{
//!  // Wink_All();
//!//  Rx_Buffer[BUFFER_SIZE] = {'\0'};
//!   unsigned char * tmp_rxPtr=Rx_Buffer;
//!   // unsigned char rx_fchr = 0;
//!   const char *res_F_num = (const char *)Rx_Buffer;
//!   uint8_t trx_indx=0;
//!
//!  //enable the Rx interrupt rotin and  executing it  (get the received character)
//!    //    Rx_Tx_Intrerrupts.U_Receive_Complete_INTR = 1;
//!    //    USART_Setup_Interrupt();
//!   // begining of receiving a long int  number sign
//!
//!   while (*tmp_rxPtr != SOB_F)
//!    {
//!        tmp_rxPtr++;
//!        trx_indx++;
//!         //if float number sign not detected
//!        if(trx_indx > BUFFER_SIZE)
//!         return 0;
//!   }
//!
//!   // while end of transmission not detected
//!    while (*tmp_rxPtr != '\0')
//!    {
//!       //discard start and end of transmision sign by overwriting them
//!       *tmp_rxPtr=*(tmp_rxPtr+1);
//!        tmp_rxPtr++;
//!        trx_indx++;
//!       //case of ovelflow of the buffer
//!        if(trx_indx > BUFFER_SIZE)
//!         return 0;
//!      // End of receiving signal was not received
//!    }
//!//add this for the converting fn  to work well
//!*tmp_rxPtr = '\0';
//!
//! Rx_BufferPtr = Rx_Buffer;
//! used_Rx_Buff_Size = 0;
//!
//! return strtod(res_F_num,&endPtr);
//!  //return  strtod(endPtr,NULL);
//!  //  return atof(res_F_num);
//!     //return CustomStrtoF(res_F_num);
//! }

//  double ATMG8A_USART::Receive_Float_Num()
//  {
//    Rx_Buffer[BUFFER_SIZE] = {'\0'};
//    Rx_BufferPtr = Rx_Buffer;

//    // unsigned char rx_fchr = 0;
//    const char *res_F_num = (const char *)Rx_Buffer;
//    // float rv=0.0;
//     uint8_t last_ind = 0;
//    // begining of receiving a long int  number sign
//    do
//    {
//      Receive_Char();
//      //reset the buffer pointer tell detecting beggining of receiving sign
//      Rx_BufferPtr = Rx_Buffer;
//    } while (*Rx_BufferPtr != SOB_F);
//    //reset the used buffer size
//    used_Rx_Buff_Size = 0;

//     // while end of transmission not detected
//      do
//      {
//        // get the next one
//        Receive_Char();

//      } while ( *Rx_BufferPtr++ != EOB);
//    Rx_BufferPtr++;
//    (*Rx_BufferPtr) = '\0'; // add null termainating charcter (or any non digit char) to make strtol function work properly
//    // convert the rx buffer to const char pointer to convert it to float
//    // sscanf(res_F_num, "%f", &rv);
//    // return rv;
//    return strtod(res_F_num,NULL);
//    // return atof(res_F_num);
//    //return CustomStrtoF(res_F_num);
//  }

//*double ATMG8A_USART::Receive_Float_Num_1( )
//* {
//*  //Rx_Buffer[BUFFER_SIZE]={'\0'};

//*  //index=0;
//*  // for(uint8_t i=0; i<BUFFER_SIZE ;i++)
//*  // {
//*  //   Rx_Buffer[i]='\0';
//*  // }
//*   Rx_BufferPtr = Rx_Buffer;
//*   unsigned char rx_fchr = 0;
//*   const char * res_F_num = (const char *)Rx_Buffer;
//*   //float rv=0.0;
//*   //uint8_t ind = 0;

//*   //begining of receiving a float  number sign
//*   do
//*   {
//*     Receive_Char(rx_fchr);

//*   } while (rx_fchr !=SOB_F);

//*   //while end of transmission not detected
//*   do
//*   {
//*      //get the next one
//*       Receive_Char(rx_fchr);
//*      //store the received char
//*       (*Rx_BufferPtr++) = rx_fchr ;

//*     //  //Rx_BufferPtr
//*   } while (rx_fchr != EOB && rx_fchr != '\0');

//*   Rx_BufferPtr++;
//*   (*Rx_BufferPtr) = '\0';//add null termainating charcter (or any non digit char) to make strtol function work properly

//*   //convert the rx buffer to const char pointer to convert it to float

//*     //sscanf(res_F_num, "%f", &rv);
//*    //return rv;
//*    return atof(res_F_num);//strtod(res_F_num,NULL);
//*    //return CustomStrtoF(res_F_num);
//* }

// working function
//! int8_t ATMG8A_USART::Add_Tx_Character(unsigned char d_chr)
//!{
//!   if(used_Tx_Buffer_Size >= BUFFER_SIZE)
//!    { //reset the add index
//!     used_Tx_Buffer_Size = 0;
//!     // buffer overflow case
//!     return -1;//remaining_tx_size=0;
//!    }
//!   Tx_Buffer[used_Tx_Buffer_Size] = d_chr;
//!   //This is the current empty buffer location or the next location to be written which is equal to the total characters stored in the buffer
//!   used_Tx_Buffer_Size++;
//!  // remaining_tx_size-- ;
//!   //used_Buffer_Size++;
//!
//!   //The remaining buffer size ready for new added characters
//!   return 0;//remaining_tx_size;
//! }
//?float ATMG8A_USART::CustomStrtoF(const char *str)
//?{
//?  float result = 0.0;
//?  float decimalFactor = 0.1;
//?  int sign = 1;
//?  int i = 0;
//?
//?  if (str[i] == '-')
//?  {
//?    sign = -1;
//?    i++;
//?  }
//?
//?  while (str[i] != '\0')
//?  {
//?    if (str[i] == '.')
//?    {
//?      i++;
//?      while (str[i] != '\0')
//?      {
//?        result += (str[i] - '0') * decimalFactor;
//?        decimalFactor *= 0.1;
//?        i++;
//?      }
//?    }
//?    else
//?    {
//?      result = result * 10 + (str[i] - '0');
//?      i++;
//?    }
//?  }
//?
//?  return sign * result;
//?}
//?
//?/**

/* Double to ASCII
 */
//?char * ATMG8A_USART::dtoa(char *s, double n)
//?{
//?    // handle special cases
//?    if (isnan(n)) {
//?        strcpy(s, "nan");
//?    } else if (isinf(n)) {
//?        strcpy(s, "inf");
//?    } else if (n == 0.0) {
//?        strcpy(s, "0");
//?    } else {
//?        int digit, m, m1;
//?        char *c = s;
//?        int neg = (n < 0);
//?        if (neg)
//?            n = -n;
//?        // calculate magnitude
//?        m = log10(n);
//?        int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
//?        if (neg)
//?            *(c++) = '-';
//?        // set up for scientific notation
//?        if (useExp) {
//?            if (m < 0)
//?               m -= 1.0;
//?            n = n / pow(10.0, m);
//?            m1 = m;
//?            m = 0;
//?        }
//?        if (m < 1.0) {
//?            m = 0;
//?        }
//?        // convert the number
//?        while (n > PRECISION || m >= 0) {
//?            double weight = pow(10.0, m);
//?            if (weight > 0 && !isinf(weight)) {
//?                digit = floor(n / weight);
//?                n -= (digit * weight);
//?                *(c++) = '0' + digit;
//?            }
//?            if (m == 0 && n > 0)
//?                *(c++) = '.';
//?            m--;
//?        }
//?        if (useExp) {
//?            // convert the exponent
//?            int i, j;
//?            *(c++) = 'e';
//?            if (m1 > 0) {
//?                *(c++) = '+';
//?            } else {
//?                *(c++) = '-';
//?                m1 = -m1;
//?            }
//?            m = 0;
//?            while (m1 > 0) {
//?                *(c++) = '0' + m1 % 10;
//?                m1 /= 10;
//?                m++;
//?            }
//?            c -= m;
//?            for (i = 0, j = m-1; i<j; i++, j--) {
//?                // swap without temporary
//?                c[i] ^= c[j];
//?                c[j] ^= c[i];
//?                c[i] ^= c[j];
//?            }
//?            c += m;
//?        }
//?        *(c) = '\0';
//?    }
//?    return s;
//?}
//?
//?//it's supposed to be good one
//?//https://stackoverflow.com/questions/2302969/convert-a-float-to-a-string
//?char*  ATMG8A_USART::floatToString(float num)
//?{
//?   int whole_part = num;
//?   int digit = 0, reminder =0;
//?   int log_value = log10(num), index = log_value;
//?   long wt =0;
//?
//?   // String containg result
//?  // char* str = new char[BUFFER_SIZE];
//?   //char str [BUFFER_SIZE];
//?   char* str = (char *) Tx_Buffer;//strcnt;
//?
//?   //Initilise stirng to zero
//?   memset(str, 0 ,BUFFER_SIZE);
//?    //Tx_Buffer[BUFFER_SIZE] = {'\0'};
//?
//?   //Extract the whole part from float num
//?   for(int  i = 1 ; i < log_value + 2 ; i++)
//?   {
//?       wt  =  pow(10.0,i);
//?       reminder = whole_part  %  wt;
//?       digit = (reminder - digit) / (wt/10);
//?
//?       //Store digit in string
//?       str[index--] = digit + 48;              // ASCII value of digit  = digit + 48
//?       if (index == -1)
//?          break;
//?   }
//?
//?    index = log_value + 1;
//?    str[index] = '.';
//?
//?   float fraction_part  = num - whole_part;
//?   float tmp1 = fraction_part,  tmp =0;
//?
//?   //Extract the fraction part from  num
//?   for( int i= 1; i < 15 ; i++)//FPRECISION
//?   {
//?      wt =10;
//?      tmp  = tmp1 * wt;
//?      digit = tmp;
//?
//?      //Store digit in string
//?      str[++index] = digit +48;           // ASCII value of digit  = digit + 48
//?      tmp1 = tmp - digit;
//?   }
//?
//? //? for(int i=0;i< BUFFER_SIZE;i++)
//? //? {
//? //?   Tx_Buffer[i] = str[i];
//? //? }
//?   return str;
//?}

//_bool ATMG8A_USART::Send_Big_Float(float num)
//_{
//_  char* str = (char*)malloc(20); // Assuming a maximum of 20 characters for the float
//_    sprintf(str, "%f", num);
//_    Tx_BufferPtr=(unsigned char *)str;
//_      // Free the allocated memory
//_    free(str);
//_    return 0;
//_}
// bool ATMG8A_USART::Transmit_Byte (uint8_t & data)
//  {
//     //$ Wait for Empty Transmit buffer (UDR)/ UDRE UART Data Transmit Buffer empty flag
//      while (! (UCSRA & (1 << UDRE)));       //Do nothing
//        //  return false;
//       UDR = data;
//       return true;
//  }
//
//  bool ATMG8A_USART::Receive_Byte (uint8_t & data)
//  {
//      //wait for data to be received
//      while (! (UCSRA & (1 << RXC)));
//        //  return false;
//      data = UDR;
//      return true;
//  }
//
//
//  /// @brief  this funcion will receive bytes until it receive \0 escape character
//  /// @return the number of characters or bytes it received
//  char ATMG8A_USART::Data_Received()
//  {
//     uint8_t x = 0;
//     unsigned char RxByte = 0;
//
//        do
//        {
//          while (! (UCSRA & (1 << RXC)));
//          RxByte = UDR; //Receive_Byte();
//            if(RxByte != '\0')
//          {
//              Rx_Buffer[x] = RxByte;
//                  x++;
//          }
//          else
//            return x;
//
//        //    if(x>19) return Rx_Tx_Buffer;
//        }
//        while (RxByte != '\0');
//     return x;
//  }
//
// id ATMG8A_USART::Receive_Int_num(int &rxn_int)
//
//  char tmp_ch='\0';
//       index=0;
//      Rx_Buffer[BUFFER_SIZE] = '\0';
//
//
//
// while (!(UCSRA & (1 << RXC))){};  //do nothingif no sata received
//      tmp_ch  = UDR;
//  Rx_Buffer[index] = tmp_ch;
//  index++;
//   PORTD ^= 1 << PD2;
//
// ile (tmp_ch != '\0');
//
//*     index++;
// Rx_Tx_Buffer[index]= '\0';  //end of receiving*/
//
// rxn_int =  atoi(Rx_Buffer);
//
//  index=0;
//     return ;//true;
//
//
// id ATMG8A_USART::Send_Int_Num(int& txn_int)
//
//  //Clear_TX_Buffer();
//  Tx_Buffer[BUFFER_SIZE] = '\0';
//  sprintf(Tx_Buffer,"%-6d", txn_int);
//      index=0;
//
//       while(Tx_Buffer[index] != '\0')
//       {
//         while (! (UCSRA & (1 << UDRE)));       //Do nothing
//                   UDR =(unsigned char) Tx_Buffer[index];
//                   index++;
//                      PORTC ^= 1<< PC0;
//       }
//
//  while (! (UCSRA & (1 << UDRE)));       //Do nothing
//               //    UDR = '\r';   //end of transmission \0
//                   UDR = '\n';
//                    index=0;
//
//
//
// id ATMG8A_USART::Send_Int(int tx_int)
//
/// static int intData{0};
// static bool higherByteSent{false};// static char ind{1};
// //unsigned char snd_int{0};
//
//  while (! (UCSRA & (1 << UDRE)));
// // return; //wait untill UDR register is empty
// if(! higherByteSent)
//   {
//
//      UDR = (tx_int >> 8);    // first send the higher byte
//      higherByteSent = true; // to send the low byte in the next call
//   //      _delay_ms(25);     // wait some time to make the receiver able to receive
//      Send_Int(tx_int) ;
//
//   }
//
// else
// {
//
//    UDR=tx_int;     // send the lower byte
//    higherByteSent=false;          // rset to send a new int in the next call
//  //   _delay_ms(100);
//    return ;
// }
//
//
//
//
// ol ATMG8A_USART::Receive_Int(int & rx_int)
//
//
// static bool higherBytereceived{false};
//
//  //wait for data to be received
//       while(!(UCSRA & (1 << RXC))) ;
// //return false;
//
//    if(! higherBytereceived)
//    {
//      rx_int = 0; // to make sure it's clear before use
//
//      rx_int = UDR ;   //receive the Higher byte first
//      rx_int = (rx_int << 8);  // store the higher byte
//
//      higherBytereceived= true;     //to get the lower byte in the next call to the function
//  //    _delay_ms(25);   // give some time to the snder to send the lower byte
//      Receive_Int(rx_int);  //get the lower byte of the received int
//    }
//
//  else
//    {
//        //Low Byte must coverted to unsighned char or it wil gve wrong result at hiher int value (max 32639,it's suposed to be 32768 in case of  signed int type)
//          rx_int|=(unsigned char) UDR;   //  finaly receive the lower byte
//          higherBytereceived=false;  // to get the higher byte of the next number (a new number)
//          return true;
//    }
//      return false;
//
//
// oid ATMG8A_USART::Send_Float_Num(float fn,uint8_t n_width ,uint8_t n_pression )
//{
//     dtostrf(fn, n_width, n_pression, Tx_Buffer);
//      index=0;
//
//       bool endOftransmt = false;
//       uint8_t tmpch = Tx_Buffer[index];
//
//      while (!endOftransmt  )
//        {
//            if(Tx_Buffer[index] == '\0')
//                  {
//                    endOftransmt =true;
//                  }
//
//                Transmit_Byte(tmpch);
//                index++;
//                tmpch =Tx_Buffer[index];
//
//        }
//  index=0;
//}
//
// ol ATMG8A_USART::Receive_Float_Num(float & fnum)
//{
//  index=0;
//  bool endOfreceive = false;
//  uint8_t tmpch{0};
//
//            //   Receive_Byte(tmpch);
//                  while(! endOfreceive )
//
//                    {
//                      Receive_Byte(tmpch);
//                      Rx_Buffer[index]=tmpch;
//
//                       if(tmpch == '\0')
//                         {
//                            endOfreceive = true;
//                            break;
//                         }
//
//                      index++;
//
//                    }
//  fnum = atof(Rx_Buffer);
//  index=0;
//     return true;
//
//}
//
// int8_t ATMG8A_USART::Transmit_String(const char strArr[])
//{
//  uint8_t num_of_Chars = 0;
//   const char* strPtr=strArr;
//
//  do
//       {
//          while (! (UCSRA & (1 << UDRE))){};       //Do nothing
//          UDR =(unsigned char) *strPtr++;
//           PORTC ^= 1<< PC0;
//
//         num_of_Chars += 1;
//       }
//
//   while (*strPtr != '\n');//while (*strPtr != '\0');
//  /* while (! (UCSRA & (1 << UDRE))){};       //Do nothing
//          UDR =(unsigned char) '\n';*/
//
//
//   return num_of_Chars;
//}
//
// ol ATMG8A_USART::Receive_String()
//
//  char rc_char='\0';
//  uint8_t pos =0;
//  //> Reset the rx buffer
//  Rx_Buffer[BUFFER_SIZE] = '\0';
//    do
//    {//wait for data to be received
//      while (! (UCSRA & (1 << RXC))){};
//
//      rc_char = (unsigned char)UDR;
//      Rx_Buffer[pos]=rc_char;
//      pos+=1;
//      //PORTD ^= 1 << PD2;
//
//      }
//   while(rc_char != '\0');
//
//
//  return true;
//
//* working with python
// void ATMG8A_USART::USART_Setup_Interrupt(bool DataREI, bool Tx_intr,bool Rx_Intr)

/*void ATMG8A_USART::My_Delay(double t)
{
  double lim= t ;//(1/16);

  while(lim)
{
 lim-=1;
}
}*/