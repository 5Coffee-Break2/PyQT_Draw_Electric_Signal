// #include "KH_Atmega8.hpp"
#include "KH_Atmega8.hpp"
#include "KH_USART_CPP.hpp"
#include "Global_Stufs.hpp"
// #include <avr/interrupt.h>
#include "util/delay.h"

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
  prev_MsgLength = 0;
#if (USE_TX_RX_BUFFERS == 1)
#if ENABLE_FLOAT_RX == 1
      float_Nums_Buffer[EXT_F_BUFF_SIZE] = {'\0'};
  // Private pointers  for refrencing the float and the float buffers
  float_Nums_Buffer_Ptr = float_Nums_Buffer;
  read_F_Nums_Buffer_Ptr = float_Nums_Buffer;
  rxed_FNums_Count = 0;
  current_read_Index_FNum = 0; //= -1;
                               // Initalize the received numbers s used to store the received numbers
  rxed_Float_Number = 0.0;
  curr_Rx_Float_Buff_Indx = -1;
#endif

#if ENABLE_LONG_RX == 1
  long_Nums_Buffer[EXT_L_BUFF_SIZE] = {'\0'};
  long_Nums_Buffer_Ptr = long_Nums_Buffer;
  curr_Rx_Long_Buff_Indx = -1;
  rxed_LNums_Count = 0;
  current_Read_Index_LNum = 0; //= -1;
  read_L_Nums_Buffer_Ptr = long_Nums_Buffer;
  // Initalize the received numbers s used to store the received numbers
  rxed_Long_Number = 0;
#endif

#if ENABLE_STRING_RX == 1
  message_Buffer[EXT_S_BUFF_SIZE] = {'\0'};
  message_Buffer_Ptr = message_Buffer;
  read_Message_Buffer_Ptr = message_Buffer;
  rxed_Msg_Length = 0;
  curr_RX_Msg_Buff_Indx = -1;

  
#endif
  // for read only

  //  tmp_Str_Indx = -1;

#if EXECUTE_AN_ACTION == 1
  tmpCnt_F = 0,
  tmpCnt_L = 0;
  action_Executed = false;
#endif
  new_Data_Received_Flag = false;

#endif
#if ENABLE_FLOAT_RX == 1 || ENABLE_LONG_RX == 1 || ENABLE_STRING_RX == 1
  // initialize the Tx\Rx buffers
  Rx_Buffer[BUFFER_SIZE] = {'\0'};
  rxed_Data_Type = '\0';

#endif
  Tx_Buffer[BUFFER_SIZE] = {'\0'};
  //- new_Data_Received_Flag = false;
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

#if ENABLE_FLOAT_RX == 1 || ENABLE_LONG_RX == 1 || ENABLE_STRING_RX == 1
bool ATMG8A_USART::Is_Receiveed_Msg_Equal(const char *msg)
{
  // check if the received message is equal to the required message
  if (strncmp((const char *)Rx_Buffer, msg, strlen(msg)) == 0)
  {
  return true;
  }
  return false; 
}


#endif
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

#if USE_TX_RX_BUFFERS == 1 && (ENABLE_FLOAT_RX == 1 || ENABLE_LONG_RX == 1 || ENABLE_STRING_RX == 1)
/// @brief Reset the read pointer of the required buffer to the begining of the buffer
/// @param pt: pointer type to be reset float,long or string (SOB_F,SOB_I,SOB_S)
void ATMG8A_USART::Reset_Read_Ptr(unsigned char pt)
{
#if ENABLE_FLOAT_RX == 1
  if (pt == SOB_F)
  {
    read_F_Nums_Buffer_Ptr = float_Nums_Buffer;
    current_read_Index_FNum = 0;
  }
#endif
#if ENABLE_LONG_RX == 1
  if (pt == SOB_I)
  {
    read_L_Nums_Buffer_Ptr = long_Nums_Buffer;
    current_Read_Index_LNum = 0;
  }
#endif
#if ENABLE_STRING_RX == 1
  if (pt == SOB_S) // in case of SOB_S
  {
    read_Message_Buffer_Ptr = message_Buffer;
    curr_RX_Msg_Buff_Indx = 0;
  }
#endif
}
#if ENABLE_FLOAT_RX == 1
/// @brief To Get the a stored float number from the float buffer max value could be stored is 9 digits incluing the decimal point
///@param parameter-fn a variable to store the read float number from the float buffer
/// @return the read number index if the buffer not empty other wise false
int8_t ATMG8A_USART::Get_Rxed_F_Num(float &fn, uint8_t ind)
{
  // bool ret_res = false;
  // read the number from the buffer at the required index
  if (ind > -1 && ind < rxed_FNums_Count)
  {
    fn = float_Nums_Buffer[ind];
    return current_read_Index_FNum;
  }
  // read the current number from the buffer pointed by the read pointer
  else if (current_read_Index_FNum <= rxed_FNums_Count)
  {
    fn = *read_F_Nums_Buffer_Ptr++; //-fn = float_Nums_Buffer[curr_Rx_F_Buff_Indx];
    return current_read_Index_FNum++;
  }

  // in case of the buffer read completely  point to the last number
  else if (current_read_Index_FNum > curr_Rx_Float_Buff_Indx && curr_Rx_Float_Buff_Indx > -1)
  {
    // Reset_Read_Ptr(SOB_F);
    read_F_Nums_Buffer_Ptr = float_Nums_Buffer + curr_Rx_Float_Buff_Indx;
    current_read_Index_FNum = curr_Rx_Float_Buff_Indx;
    return current_read_Index_FNum;
  }

  return -1;
}
#endif
#if ENABLE_LONG_RX == 1
/// @brief To Get the a stored long number from the long buffer max valuc ould be stored 999999999
///@param parameter-fn a variable to store the read long number from the long buffer
/// @return the read number index if the buffer not empty other wise false
int8_t ATMG8A_USART::Get_Rxed_L_Num(long &ln, uint8_t ind)
{
  if (ind > -1 && ind < rxed_LNums_Count)
  {
    ln = long_Nums_Buffer[ind];
    return current_Read_Index_LNum;
  }

  // read the current number from the buffer
  else if (current_Read_Index_LNum <= curr_Rx_Long_Buff_Indx)
  {
    ln = *read_L_Nums_Buffer_Ptr++; //-ln = long_Nums_Buffer[curr_Rx_L_Buff_Indx];
    return current_Read_Index_LNum++;
  }
  // if the long uufer numbers completly read, point to the last number in the buffer
  else if (current_Read_Index_LNum > curr_Rx_Long_Buff_Indx && curr_Rx_Long_Buff_Indx > -1)
  {
    current_Read_Index_LNum = curr_Rx_Long_Buff_Indx;
    read_L_Nums_Buffer_Ptr = long_Nums_Buffer + curr_Rx_Long_Buff_Indx;
    // Reset_Read_Ptr(SOB_I);
    return current_Read_Index_LNum;
  }
  return -1;
}
#endif
#if ENABLE_FLOAT_RX == 1 || ENABLE_LONG_RX == 1
/// @brief
/// @param nty is the type of buffer's numbers (float or long)
/// @return return the current size of the required buffer
uint8_t ATMG8A_USART::Get_Rxed_Nums_Count(unsigned char nty)
{
#if ENABLE_FLOAT_RX == 1
  if (nty == SOB_F)
    return rxed_FNums_Count;
#endif
#if ENABLE_LONG_RX == 1
  if (nty == SOB_I)
    return rxed_LNums_Count;
#endif
  return 0;
}
#endif

#if ENABLE_LONG_RX == 1 || ENABLE_FLOAT_RX == 1 || ENABLE_STRING_RX == 1
/// @brief Reset the main buffers pointers to store numbers from the begining and over write the existing numbers
/// @param pt: pointer type to be reset float or long
void ATMG8A_USART::Reset_Main_Rx_Ptr(unsigned char pt)
{
#if ENABLE_FLOAT_RX == 1
  if (pt == SOB_F)
  {
    float_Nums_Buffer_Ptr = float_Nums_Buffer;
    read_F_Nums_Buffer_Ptr = float_Nums_Buffer;
    rxed_FNums_Count = 0;
    curr_Rx_Float_Buff_Indx = -1;
    current_read_Index_FNum = 0;
    tmpCnt_F = 0;
  }
#endif
#if ENABLE_LONG_RX == 1
  if (pt == SOB_I)
  {
    long_Nums_Buffer_Ptr = long_Nums_Buffer;
    read_L_Nums_Buffer_Ptr = long_Nums_Buffer;
    rxed_LNums_Count = 0;
    curr_Rx_Long_Buff_Indx = -1;
    current_Read_Index_LNum = 0;
    tmpCnt_L = 0;
  }
#endif
#if ENABLE_STRING_RX == 1
  if (pt == SOB_S) // in case of SOB_S
  {
    message_Buffer_Ptr = message_Buffer;
    read_Message_Buffer_Ptr = message_Buffer;
    rxed_Msg_Length = 0;
    curr_RX_Msg_Buff_Indx = -1;
    tmpCnt_S = 0;
  }
#endif
#if EXECUTE_AN_ACTION == 1
  action_Executed = false;
#endif
}
#endif
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

  //- if the end of receiving data was detected
  // if (*Rx_Buffer_Ptr == EOB && (type_Detected == true))
  if (rx_byte == EOB && (type_Detected == true))
  {
    EOB_Flag = true;
    //> replace EOB with end of number sign (null character '\0') cause python will not send it, for success of number conversion
    // add null termainating charcter to and return the total number of received bytes
     Insert_Byte_To_MBuffer('\0');
     used_Buffer_Size = strlen((const char *)Rx_Buffer);
     //_--_!Start New Updating
     //-uint8_t msg_length = strlen((const char *)mSerPtr->Rx_Buffer);
     //-// uint8_t diff = 0;
     //-// uint8_t new_length=msg_length;
     //-// strcpy((char *)rx_ch_buff, (const char *)mSerPtr->Rx_Buffer);
     //-if (prev_MsgLength > msg_length)
     //-{
     //-  // diff = prev_length - strlen((const char *)mSerPtr->Rx_Buffer);
//-
     //-  for (uint8_t i = msg_length; i <= prev_MsgLength; i++)
     //-  {
     //-    Rx_Buffer[i] = '\0'; // Clear the buffer after use
     //-  }
     //-}
     //-prev_MsgLength = msg_length; // strlen((const char *)mSerPtr->Rx_Buffer);

     //_ g_Line = Line_1;
     //_ g_Pos = 1;
     //_ printf("%s", rx_ch_buff);
     //_ g_Line = Line_2;
     //_ g_Pos = 1;
     //_ printf("%s", mSerPtr->Rx_Buffer);
     //_--_!End New Updating
  }

  //-Filter the received data type`
  //@ If received data type has been detected
  if ((type_Detected == false) && ((rx_byte == SOB_F) || (rx_byte == SOB_I) || (rx_byte == SOB_S)))
  {
    type_Detected = true;
    rxed_Data_Type = rx_byte; //*Rx_Buffer_Ptr;
   //-                           // if the received type was detected it will be re-written in the next reading cause we don't need it any more

    return used_Rx_Buff_Size;
  }

  //  remaining_tx_size -=1;
  return Insert_Byte_To_MBuffer(rx_byte);
}

#if ENABLE_FLOAT_RX == 1
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
    Reset_Main_Rx_Ptr(rxed_Data_Type);
  }
  // Rx_Buffer[BUFFER_SIZE] = {'\0'};
  return true;
}
#endif
#if ENABLE_LONG_RX == 1
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
    Reset_Main_Rx_Ptr(rxed_Data_Type);
  }
  //-for(uint8_t i = 0; i < BUFFER_SIZE; i++)
  //-{
  //-  Rx_Buffer[i] = '\0';
  //-}
  // Rx_Buffer[BUFFER_SIZE] = {'\0'};
  return true;
}
#endif
/// @brief
/// @param data : the received byte that will be stored in the Rx buffer
/// @return the length of the received bytes in the Rx buffer(used buffer size or length
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
  return 0; // used_Buffer_Size++;//- used_Buffer_Size will be calculated with strlen dunction  of the string library when a message completly eceived
  //- used_Buffer_Size = strlen((const char *)Rx_Buffer);
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
  #if ENABLE_STRING_RX == 1
     // volatile unsigned char *tmpCharPtr = Rx_Buffer;
   //   int64_t rd_StrSize = used_Rx_Buff_Size;
  #endif
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
    switch (rxed_Data_Type)
    {
#if ENABLE_FLOAT_RX == 1
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
#endif
#if ENABLE_LONG_RX == 1
    case SOB_I:
      rxed_Long_Number = strtol((const char *)Rx_Buffer, NULL, 10);

      // store the received number value and update the current index value
      Insert_Number_To_Long_Buffer(rxed_Long_Number);
      // the received data has been converted to a number and stored in the buffer
      number_created = true;
      // #endif
      break;
#endif
#if ENABLE_STRING_RX == 1
    case SOB_S:
    //  uint8_t msg_length = strlen((const char *)Rx_Buffer);
      // uint8_t diff = 0;
      // uint8_t new_length=msg_length;
      strcpy((char *)message_Buffer, (const char *)mSerPtr->Rx_Buffer);
      read_Message_Buffer_Ptr = message_Buffer;
      curr_RX_Msg_Buff_Indx = -1;
      //  #if (USE_TX_RX_BUFFERS == 1)

      // in case of overflow
      //- if (EXT_S_BUFF_SIZE - curr_RX_Msg_Buff_Indx < used_Rx_Buff_Size)
      //- {
      //-   Reset_Main_Rx_Ptr(rxed_Data_Type);
      //- }
        rxed_Msg_Length = used_Buffer_Size;
      //-----// update the read only received string buffer pointer to the start location of the received message
      //-----read_Message_Buffer_Ptr = message_Buffer;
      //-----message_Buffer_Ptr = message_Buffer;
      //-----// Rx_BufferPtr = Rx_Buffer;
      //-----while (rd_StrSize > -1)
      //-----// for (uint8_t x=0;x < used_Rx_Buff_Size;x++)
      //-----{
      //-----  *message_Buffer_Ptr++ = *tmpCharPtr++;
      //-----  curr_RX_Msg_Buff_Indx++;
      //-----  rd_StrSize--;
      //-----}
      //-----// update the main string buffer pointer to point to the start location of the new received string message
      //-----//-message_Buffer_Ptr++;
      //-----//-rxed_Msg_Length++;
      number_created = true;
      //  #endif
      break;

    default:
      break;
#endif
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
#if EXECUTE_AN_ACTION == 1
    action_Executed = false;
#endif
  }

  return number_created;
}
#endif

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
  switch (rxed_Data_Type)
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

/*void ATMG8A_USART::My_Delay(double t)
{
  double lim= t ;//(1/16);

  while(lim)
{
 lim-=1;
}
}*/