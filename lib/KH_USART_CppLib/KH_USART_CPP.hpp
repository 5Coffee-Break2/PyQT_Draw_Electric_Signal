#ifndef KH_USART_CPP_LIB
#define KH_USART_CPP_LIB

//________________Temp Include for debug________________________________/
//-#include "LCDHD44780_AVRLib.hpp"
//-extern LCD_HD44780 *LCD_Ptr;

//__________End of Tem Include for debug________________________________/
//-#define USE_TX_FUNCTION_TEMPLATE 1

#include <KH_USART_CPP_Setting.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//-#include <stdarg.h>

// end char pointer used with strtod function
//.extern char* endPtr ;

//** just for try **//
/*
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);
int uart_putchar(char c, FILE *stream);*/
// FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
//** end of just for try **//

/// @brief UART contstructor
/// @param ubrr UBR value correspnding to the required baud rate
/// @param en_tx    To enable UART Tx mode
/// @param en_rx    To enable UART Rx mode

class ATMG8A_USART
{
private:
  //> Transmit String
  // bool Transmit_String();   //using interrupt tecnique
  //. uint8_t Detect_Sign(char,unsigned char ** );
  // uint8_t Detect_Sign(unsigned char** ptr);
  // if enabled using buffers to store the received data
  uint8_t prev_MsgLength ;
#if (USE_TX_RX_BUFFERS == 1)
#if ENABLE_FLOAT_RX == 1
      float float_Nums_Buffer[EXT_F_BUFF_SIZE];
  volatile float *float_Nums_Buffer_Ptr;
  // These variables to store the count of the received bytes or characters
  uint8_t rxed_FNums_Count;
  // indexes to access the current value of the receiver buffers
  int8_t curr_Rx_Float_Buff_Indx,
      current_read_Index_FNum;

#endif
#if ENABLE_LONG_RX == 1
  long long_Nums_Buffer[EXT_L_BUFF_SIZE];
  volatile long *long_Nums_Buffer_Ptr;
  // These variables to store the count of the received bytes or characters
  uint8_t rxed_LNums_Count;
  /// @brief indexes to access the current value of the receiver buffers (location of the last received number)
  int8_t curr_Rx_Long_Buff_Indx,
      current_Read_Index_LNum;
#endif

#if ENABLE_STRING_RX == 1
  //unsigned char message_Buffer[EXT_S_BUFF_SIZE];
  volatile unsigned char *message_Buffer_Ptr;
  // indexes to access the current value of the receiver buffers
  int8_t curr_RX_Msg_Buff_Indx;
#endif
#endif
  //! rxed_Msg_Length;

  //  tmp_Str_Indx;
  // used for calculating if new numbers or string were received and added to the buffer
  uint8_t tmpCnt_F, tmpCnt_L,
      tmpCnt_S;
// a flage to indicate if an action executed or not yet
#if EXECUTE_AN_ACTION == 1
  bool action_Executed; //, new_Data_Received_Flag;
#endif
  void Reset_Tx_Rx_Buffer();
  uint8_t Insert_Byte_To_MBuffer(unsigned char);
#if ENABLE_FLOAT_RX == 1
  bool Insert_Number_To_Float_Buffer(float);
#endif
#if ENABLE_LONG_RX == 1
  bool Insert_Number_To_Long_Buffer(long);
#endif
  // bool Insert_Char_To_String_Buffer(unsigned char);

public:
  // int8_t curr_RX_Str_Buff_Indx ;
  // volatile bool EOB_Flag;
  // if enabled using buffers to store the received data
#if ENABLE_STRING_RX == 1
  unsigned char message_Buffer[EXT_S_BUFF_SIZE];
  #endif

  bool new_Data_Received_Flag;
#if EXECUTE_AN_ACTION == 1
  bool Is_New_Data();
#endif
#if (USE_TX_RX_BUFFERS == 1)
// read only pointers for refrencing the float,long And string buffers for reading only
#if ENABLE_FLOAT_RX == 1
  // varibles used to store the recevied numbers and chars
  volatile float rxed_Float_Number;
  volatile const float *read_F_Nums_Buffer_Ptr;
  int8_t Get_Rxed_F_Num(float &fn, uint8_t ind = -1);
#endif
#if ENABLE_LONG_RX == 1
  /// @brief To Get a long number from the received buffer at the required index (point to the current unread location)
  volatile const long *read_L_Nums_Buffer_Ptr;
  int8_t Get_Rxed_L_Num(long &ln, uint8_t ind = -1);
  // varibles used to store the recevied numbers and chars
  volatile long rxed_Long_Number;
#endif
#if ENABLE_STRING_RX == 1
  uint8_t rxed_Msg_Length;
  volatile const unsigned char *read_Message_Buffer_Ptr;
  // varibles used to store the recevied numbers and chars
  volatile unsigned char rxed_Char;
#endif

#if ENABLE_FLOAT_RX == 1 || ENABLE_LONG_RX == 1 || ENABLE_STRING_RX == 1
  uint8_t Get_Rxed_Nums_Count(unsigned char);
  void Reset_Read_Ptr(unsigned char);
  void Reset_Main_Rx_Ptr(unsigned char);
  /// @brief The type of the received data frame (float,long,string)
  volatile unsigned char rxed_Data_Type;
  volatile bool type_Detected, // a flag for detected data type
      EOB_Flag,                // a flag for receiving a complete block of data numbers or message string
      end_of_Rx_Block,
      end_of_Tx_Block;
//_float Read_FNumber_At_Index(uint8_t );
//_long Read_LNumber_At_Index(uint8_t indx);
#endif

  // variable to hold the type ofthe received number

  volatile int8_t used_Buffer_Size;

  // struct status_Flags
  // {
  //    volatile bool type_Detected:1;  // a flag for detected data type
  //    volatile bool EOB_Flag :1;      // a flag for receiving a complete block of data numbers or message string
  //    volatile bool end_of_Rx_Block:1;
  //    volatile bool end_of_Tx_Block:1 ;
  // } Tx_Rx_Flags;

  // volatile int8_t remaining_tx_size;
  volatile uint8_t used_Rx_Buff_Size,
      // also indicate to the index of the next byte that will be send
      transmitted_Chars_Count,
      // also indicate to the index of the next empty buffer location that is ready to store a charcater
      used_Tx_Buffer_Size;
#if ENABLE_FLOAT_RX == 1 || ENABLE_LONG_RX == 1 || ENABLE_STRING_RX == 1
  // this buffer used for receiving single data block only (float number,long number,string msge)
  unsigned char Rx_Buffer[BUFFER_SIZE];
  bool Is_Receiveed_Msg_Equal(const char *);
#endif

#if ENABLE_FLOAT_TX == 1 || ENABLE_LONG_TX == 1 || ENABLE_STRING_TX == 1
  unsigned char Tx_Buffer[BUFFER_SIZE];
#endif

  /// @brief  Buffer for Transmitted Data
  // void Clear_TX_Buffer();
  volatile unsigned char *Rx_Buffer_Ptr;
  unsigned char *Tx_BufferPtr;
  // const char * Receive_Message()   ;
  ATMG8A_USART(unsigned long ubrr, bool en_tx, bool en_rx);
  //-~ATMG8A_USART();

#if EXECUTE_AN_ACTION == 1
  // this function will accept a function pointer as an action to be taken when receiving a ew data
  // Function pointer syntax  returnType (*pointerName)(parameterTypes);
  /**
   * @brief Function that will be called on reciving a number
   *
   * @param ntyp the type of the received number
   * @param FnPtr function pointer that wiil be called when the mentioned type is received
   */
  void On_Data_Received(void (*FnPtr)(void));
#endif
  int8_t Add_Tx_Character(unsigned char);
  // void ResetPtr();

  // void Fill_Tx_Buffer(unsigned char *);                 //used with Tx technique
  //- bool Transmit_Number(const char *,uint8_t,...);

#if USE_TX_FUNCTION_TEMPLATE == 1
  //! important note to use a function template and work correcly it must be declared and defined inside the header file
  template <typename T>
  bool Send_Nums(const char *, T);
#endif
#if USE_TX_FUNCTION_TEMPLATE == 0
  bool Transmit_Number(unsigned char, const char *, float, long);
#endif
  bool Transmit_Char(unsigned char &);
  void Transmit_Msg(unsigned char *);
  //- void Send_Msg(const char *frmt = "%s",uint8_t arg_cnt = 1,...);
  // int8_t Transmit_Char();                 //using interrupt technique

  // bool Receive_Number();
#if USE_USART_INTERRUPTS == 0
  bool Receive_Char(unsigned char &);
  void Receive_Msg();
#endif
  uint8_t Receive_Char();
  const unsigned char *current_Tx_Ch_Ptr;

#if USE_TX_RX_BUFFERS == 1
  // void Reset_Buffer();
  bool Data_Received();
#endif

#if USE_USART_INTERRUPTS == 1
  /// @brief  USART Interrupts structure
  struct USART_Intrrupts
  {
#if USE_TX_INTERRUPT == 1
    char U_Transmit_Completed_INTR : 1;
    char U_DataRegisterEmpty_INTR : 1;
#endif
#if USE_RX_INTERRUPT == 1
    char U_Receive_Complete_INTR : 1;
#endif
  } Rx_Tx_Intrerrupts;
  // void USART_Setup_Interrupt(bool DataREmpty, bool Tx_intr,bool Rx_Intr);
  void USART_Setup_Interrupt(bool rxc = false, bool dre = false, bool txc = false);
#endif
};

#if USE_TX_FUNCTION_TEMPLATE == 1
// Template functions definiotions
template <typename T>
bool ATMG8A_USART::Send_Nums(const char *fmt, T value)
{
  used_Tx_Buffer_Size = 0;
  Tx_Buffer[BUFFER_SIZE] = {'\0'};
  // Tx_BufferPtr = Tx_Buffer;

  unsigned char num_type;
  const char *fmt_twin = fmt;
  // skip % in the string formatter fmt
  fmt_twin++;
  while (*fmt_twin != '\0')
  {

    if (*fmt_twin == 'f')
    {
      num_type = SOB_F;
      break;
    }
    else if (*fmt_twin == 'l')
    {
      num_type = SOB_I;
      break;
    }
    fmt_twin++;
  }

  snprintf((char *)Tx_Buffer, BUFFER_SIZE, fmt, value);

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

#endif

#endif
