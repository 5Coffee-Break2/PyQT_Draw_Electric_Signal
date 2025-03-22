// #include "KH_Atmega8.hpp"
#include "KH_Atmega8.hpp"
#include "KH_USART_CPP.hpp"
#include "Global_Stufs.hpp"
// #include <avr/interrupt.h>
#include "util/delay.h"
#include <stdlib.h>
// #include <stdio.h>
#include "LCDHD44780_AVRLib.hpp"

ATMG8A_USART *mSerPtr;

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

  curr_Rx_Float_Buff_Indx = -1;
  curr_Rx_Long_Buff_Indx = -1;
  curr_RX_Str_Buff_Indx = -1;
  read_F_Ptr_Index = 0; //= -1;
  read_L_Ptr_Index = 0; //= -1;
  //  tmp_Str_Indx = -1;

  rxed_FNums_Count = 0;
  rxed_LNums_Count = 0;
  rxed_String_Count = 0;
#if EXECUTE_AN_ACTION == 1
  tmpCnt_F = 0,
  tmpCnt_L = 0;
  action_Executed = false;
#endif
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
  Rx_Buffer_Ptr = Rx_Buffer;
  Tx_BufferPtr = Tx_Buffer;
  current_Tx_Ch_Ptr = Tx_BufferPtr;
  ;

  // Initalize the received numbers s used to store the received numbers
  rxed_Float_Number = 0.0;
  rxed_Long_Number = 0;
  rxed_Number_Type = '\0';

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

//-ATMG8A_USART::~ATMG8A_USART()
//-{
//-}

//_/// @brief Get a float number from the received buffer at the required index
//_/// @param indx : required index of the received float number
//_/// @return
//_float ATMG8A_USART::Read_FNumber_At_Index(uint8_t indx)
//_{
//_  if (indx > -1 && indx < EXT_F_BUFF_SIZE)
//_    return float_Nums_Buffer[indx];
//_  return -1;
//_}
//_
//_/// @brief Get a long number from the received buffer at the required index
//_/// @param indx : required index of the received long number
//_/// @return
//_long ATMG8A_USART::Read_LNumber_At_Index(uint8_t indx)
//_{
//_  if (indx > -1 && indx < EXT_L_BUFF_SIZE)
//_    return long_Nums_Buffer[indx];
//_  return -1;
//_}

/// @brief To Reset the Tx and Rx buffers
void ATMG8A_USART::Reset_Tx_Rx_Buffer()
{
  Rx_Buffer[BUFFER_SIZE] = {'\0'};
  used_Rx_Buff_Size = 0;
  Rx_Buffer_Ptr = Rx_Buffer;
}

#if USE_TX_RX_BUFFERS == 1
/// @brief Reset the read pointer of the required buffer to the begining of the buffer
/// @param pt: pointer type to be reset float,long or string (SOB_F,SOB_I,SOB_S)
void ATMG8A_USART::Reset_Read_Ptr(unsigned char pt)
{
  if (pt == SOB_F)
  {
    read_F_Nums_Buffer_Ptr = float_Nums_Buffer;
    read_F_Ptr_Index = 0;
  }
  else if (pt == SOB_I)
  {
    read_L_Nums_Buffer_Ptr = long_Nums_Buffer;
    read_L_Ptr_Index = 0;
  }
  else if (SOB_S) // in case of SOB_S
  {
    read_String_Buffer_Ptr = string_Buffer;
    curr_RX_Str_Buff_Indx = 0;
  }
}

/// @brief To Get the a stored float number from the float buffer
///@param parameter-fn a variable to store the read float number from the float buffer
/// @return the read number index if the buffer not empty other wise false
int8_t ATMG8A_USART::Get_Rxed_F_Num(float &fn, uint8_t ind)
{
  // bool ret_res = false;
  if (ind > -1 && ind < EXT_F_BUFF_SIZE)
  {
    fn = float_Nums_Buffer[ind];
    return read_F_Ptr_Index;
  }

  else if (read_F_Ptr_Index <= curr_Rx_Float_Buff_Indx)
  {
    fn = *read_F_Nums_Buffer_Ptr++; //-fn = float_Nums_Buffer[curr_Rx_F_Buff_Indx];
    return read_F_Ptr_Index++;
  }

  // in case of the buffer read completely  point to the last number
  else if (read_F_Ptr_Index > curr_Rx_Float_Buff_Indx && curr_Rx_Float_Buff_Indx > -1)
  {
    // Reset_Read_Ptr(SOB_F);
    read_F_Nums_Buffer_Ptr = float_Nums_Buffer + curr_Rx_Float_Buff_Indx;
    read_F_Ptr_Index = curr_Rx_Float_Buff_Indx;
    return read_F_Ptr_Index;
  }

  return -1;
}

/// @brief To Get the a stored long number from the long buffer
///@param parameter-fn a variable to store the read long number from the long buffer
/// @return the read number index if the buffer not empty other wise false
int8_t ATMG8A_USART::Get_Rxed_L_Num(long &ln, uint8_t ind)
{
  if (ind > -1 && ind < EXT_L_BUFF_SIZE)
  {
    ln = long_Nums_Buffer[ind];
    return read_L_Ptr_Index;
  }

  // read the current number from the buffer
  else if (read_L_Ptr_Index <= curr_Rx_Long_Buff_Indx)
  {
    ln = *read_L_Nums_Buffer_Ptr++; //-ln = long_Nums_Buffer[curr_Rx_L_Buff_Indx];
    return read_L_Ptr_Index++;
  }
  // if the long uufer numbers completly read, point to the last number in the buffer
  else if (read_L_Ptr_Index > curr_Rx_Long_Buff_Indx && curr_Rx_Long_Buff_Indx > -1)
  {
    read_L_Ptr_Index = curr_Rx_Long_Buff_Indx;
    read_L_Nums_Buffer_Ptr = long_Nums_Buffer + curr_Rx_Long_Buff_Indx;
    // Reset_Read_Ptr(SOB_I);
    return read_L_Ptr_Index;
  }
  return -1;
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
    curr_Rx_Float_Buff_Indx = -1;
    read_F_Ptr_Index = 0;
    tmpCnt_F = 0;
  }
  else if (pt == SOB_I)
  {
    long_Nums_Buffer_Ptr = long_Nums_Buffer;
    read_L_Nums_Buffer_Ptr = long_Nums_Buffer;
    rxed_LNums_Count = 0;
    curr_Rx_Long_Buff_Indx = -1;
    read_L_Ptr_Index = 0;
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

/// @brief Transmit a char using polling tecnique (thout interrupt)
/// @param data : the chrarcter that will be transmitted
/// @return
bool ATMG8A_USART::Transmit_Char(unsigned char &data)
{
  //$ Wait for Empty Transmit buffer (UDR)/ UDRE UART Data Transmit Buffer empty flag

  while (!(UCSRA & (1 << UDRE)))
  {
  }; // Do nothing

  UDR = data;
  _delay_us(TX_DELAY);

  return true;
}

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

#if USE_USART_INTERRUPTS == 0
/// @brief
/// @param data : is a refence type to the variable that will store the received char
/// @return
bool ATMG8A_USART::Receive_Char(unsigned char &data)
{
  // wait for data to be received
  while (!(UCSRA & (1 << RXC)))
  { // Do nothing
  };

  data = UDR; //(unsigned char)

  return true;
}

void ATMG8A_USART::Receive_Msg()
{
  Rx_Buffer_Ptr = Rx_Buffer;
  unsigned char rx_chr = ' ';

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
    (*Rx_Buffer_Ptr++) = rx_chr;

  } while (rx_chr != EOB);

  Rx_Buffer_Ptr++;
  (*Rx_Buffer_Ptr) = '\0'; // add null termainating charcter (or any non digit char) to make strtol function work properly
}

#endif
// overloading Rx_Char function

/// @brief   :retrive the received char or byte from the Atmega USART Rx buffer (UDR) and store it in rhe RX buffer or 0 if no data in the UDR rx buffer
/// @return  : the total numbers of the received bytes or char in the rx buffer
uint8_t ATMG8A_USART::Receive_Char()
{

#if (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))
  //@ check if char or a byte was received . if no data then return
  if (!(UCSRA & (1 << RXC)))
    return 0;
#endif //_(USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))

  unsigned char rx_byte = UDR;
  // copy the received byte to the Rx buffer and  set the pointer to the current empty rx buffer location (only if data type was detected)
  // *Rx_Buffer_Ptr = UDR;

  //@ If received data type has been detected
  if ((type_Detected == false) && ((rx_byte == SOB_F) || (rx_byte == SOB_I) || (rx_byte == SOB_S)))
  {

    rxed_Number_Type = rx_byte; //*Rx_Buffer_Ptr;
    type_Detected = true;
    // if the received type was detected it will be re-written in the next reading cause we don't need it any more
    return used_Rx_Buff_Size;
  }

  //- if the end of receiving data was detected
  // if (*Rx_Buffer_Ptr == EOB && (type_Detected == true))
  if (rx_byte == EOB && (type_Detected == true))
  {
    EOB_Flag = true;
    //> replace EOB with end of number sign (null character '\0') cause python will not send it, for success of number conversion
    // add null termainating charcter to and return the total number of received bytes
    return Insert_Byte_To_MBuffer('\0');
  }
  //  remaining_tx_size -=1;
  return Insert_Byte_To_MBuffer(rx_byte);
}
/// @brief  To Add a received float number to the float buffer
/// @param f_value : the rxeived float number that will be stored in the Rx float numbers buffer
/// @return
bool ATMG8A_USART::Insert_Number_To_Float_Buffer(float f_value)
{

  *float_Nums_Buffer_Ptr++ = f_value;
  // update the current index value to the last received number
  curr_Rx_Float_Buff_Indx++;
  // update the total number of received float numbers
  rxed_FNums_Count++;
  // in case of overflow
  if (rxed_FNums_Count > EXT_F_BUFF_SIZE)
  {
    Reset_Main_Rx_Ptr(rxed_Number_Type);
  }
  // Rx_Buffer[BUFFER_SIZE] = {'\0'};
  return true;
}

/// @brief To Add a received long number to the long buffer
/// @param l_value  : the received long number that will be stored in the Rx Long numbers buffer
/// @return true if the number was stored successfully
bool ATMG8A_USART::Insert_Number_To_Long_Buffer(long l_value)
{
  *long_Nums_Buffer_Ptr++ = l_value;
  // update the current index value to the last received number
  curr_Rx_Long_Buff_Indx++;
  rxed_LNums_Count++;
  // in case of overflow
  if (rxed_LNums_Count > EXT_L_BUFF_SIZE)
  {
    Reset_Main_Rx_Ptr(rxed_Number_Type);
  }
  //-for(uint8_t i = 0; i < BUFFER_SIZE; i++)
  //-{
  //-  Rx_Buffer[i] = '\0';
  //-}
  // Rx_Buffer[BUFFER_SIZE] = {'\0'};
  return true;
}

/// @brief
/// @param data : the received byte that will be stored in the Rx buffer
/// @return
uint8_t ATMG8A_USART::Insert_Byte_To_MBuffer(unsigned char data)
{
  // in case of overflow the buffer will be reset and the pointer will be set to the start of the buffer location
  if (used_Rx_Buff_Size > BUFFER_SIZE)
  {
    Reset_Tx_Rx_Buffer();
  }
  // store the received byte in the rx buffer and increment the buffer pointer to the next empty buffer location
  *Rx_Buffer_Ptr = data;
  Rx_Buffer_Ptr++;
  // update current rx used buffer size and return the total number of received bytes
  return used_Buffer_Size++;
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

  mSerPtr->new_Data_Received_Flag = false;
  mSerPtr->Receive_Char();

  if (mSerPtr->EOB_Flag)
  {

    mSerPtr->Data_Received();
    // has been moved to Receive_Char() fn
    //> replace EPF with end of number sign (null character '\0') cause python will not send it for success to number conversion
    mSerPtr->new_Data_Received_Flag = true;
    // reset the data type or the next data received
    mSerPtr->type_Detected = false;
    mSerPtr->EOB_Flag = false;

    // has been moved to Data_Received()function
    //>mSerPtr->used_Rx_Buff_Size = 0;
    // mSerPtr -> Data_Received();

    // reset the Rx buffer pointer and the toral received bytes count
    mSerPtr->Rx_Buffer_Ptr = mSerPtr->Rx_Buffer;
  }
  //  #endif
}
#endif //@ USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 1

//====================================================================================================================================//

#if USE_TX_FUNCTION_TEMPLATE == 0

bool ATMG8A_USART::Transmit_Number(unsigned char num_type, const char *fmt, float f_val, long l_value)
{
  used_Tx_Buffer_Size = 0;
  Tx_Buffer[BUFFER_SIZE] = {'\0'};
  //@for (uint8_t i = 0; i < BUFFER_SIZE; i++)
  //@{
  //@  Tx_Buffer[i] = '\0';
  //@}
  // char tx_Num_Buffer[Tx_NUM_SIZE] = {'\0'};
  // Tx_BufferPtr = Tx_Buffer;
  //  char tx_Num_Buffer[Tx_NUM_SIZE] = {'\0'};

  //  ptrLCD -> LCD_Show_Float(fval,1,1,4,Line_2);
  if (num_type == SOB_I)
  {
    snprintf((char *)Tx_Buffer, BUFFER_SIZE, fmt, l_value);
    // snprintf_P((char *)Tx_Buffer, BUFFER_SIZE, fmt, l_value);
  }

  else // num_type == SOB_F
  {
    snprintf((char *)Tx_Buffer, BUFFER_SIZE, fmt, f_val); //
    // snprintf_P((char *)Tx_Buffer, BUFFER_SIZE, fmt, f_val); //
  }

  // essetial initialisations
  transmitted_Chars_Count = 0;
  Tx_BufferPtr = Tx_Buffer;
  // reset end of sending a tx block flag
  end_of_Tx_Block = false;

  //@start transmiion by sending rhe number type
  Transmit_Char(num_type);
  //@start transmitting of the number
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
  //@ End of transmission by sending the end of block sign
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
  bool number_created = false;
  volatile unsigned char *tmpCharPtr = Rx_Buffer;
  uint8_t rd_StrSize = used_Rx_Buff_Size;
  // if no buffer used hto store the received numbers
#if (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))

  do
  {
    if (Receive_Char() == 0)
    {
      return false;
    }

  }

  // continoue to get chars until eob is received which mean a complete number  or data was received
  while (!EOB_Flag && type_Detected);

#endif          //@ (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))
  if (EOB_Flag) // new_Data_Received_Flag
  {
    switch (rxed_Number_Type)
    {
    case SOB_F:

      rxed_Float_Number = (float)(strtod((const char *)Rx_Buffer, NULL));
      //  #if (USE_TX_RX_BUFFERS == 1)

      Insert_Number_To_Float_Buffer(rxed_Float_Number);
      //__ for debug____
      // LCD_Ptr->LCD_Clear();
      // g_Pos = 1;
      // g_Line = Line_1;
      //
      // printf("Foo= %f", (double)rxed_Float_Number);
      // g_Pos =15;
      // printf("%d", rxed_FNums_Count);

      //__ end debug____
      // the received data has been converted to a number and stored in the buffer
      number_created = true;
      // #endif

      break;

    case SOB_I:
      rxed_Long_Number = strtol((const char *)Rx_Buffer, NULL, 10);

      // store the received number value and update the current index value
      Insert_Number_To_Long_Buffer(rxed_Long_Number);
      // the received data has been converted to a number and stored in the buffer
      number_created = true;
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
      number_created = true;
      //  #endif
      break;
    }
    // if no interrupts was used to receive data
#if (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))
    // Reset the important parameters to receive a new data
    {
      new_Data_Received_Flag = true;
      // reset the data type for the next data received
      type_Detected = false;
      EOB_Flag = false;

      // reset the Rx buffer pointer and the toral received bytes count
      Rx_BufferPtr = Rx_Buffer;
    }
#endif //- (USE_USART_INTERRUPTS == 0 || (USE_USART_INTERRUPTS == 1 && USE_RX_INTERRUPT == 0))
    //@ to run the action function defined in onDataReceived function on the new received data
    Rx_Buffer[BUFFER_SIZE] = {'\0'};
    used_Rx_Buff_Size = 0;
    action_Executed = false;
  }

  return number_created;
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

//*      //>max number of digits that can be received as integer number correctly is 9 digits,
//*      //>max valu can be received correctly as integer number is 999999999
//*      //>you can receive 10 digit int number ith a maximum value 1999999999
//*      rxed_Long_Number = strtol(result_num_ptr, NULL, 10);
//*      //>max number of digits that can be received as integer number correctly is 7 digits,
//*      //>max valu can be received correctly as integer number is 9999999
//*

//?      //>max number of digits that can be received as integer number correctly is 9 digits,
//?      //>max valu can be received correctly as integer number is 999999999
//?      //>you can receive 10 digit int number ith a maximum value 1999999999
//?      rxed_Long_Number = strtol(result_num_ptr, NULL, 10);
//?      //>max number of digits that can be received as integer number correctly is 7 digits,
//?      //>max valu can be received correctly as integer number is 9999999

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

#if EXECUTE_AN_ACTION == 1
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
#endif

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