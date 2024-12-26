#ifndef KH_USART_CPP_LIB
#define KH_USART_CPP_LIB

//________________Temp Include for debug________________________________/
//-#include "LCDHD44780_AVRLib.hpp"
//-extern LCD_HD44780 *LCD_Ptr;

//__________End of Tem Include for debug________________________________/
//-#define USE_TX_FUNCTION_TEMPLATE 1

#include <KH_USART_CPP_Setting.hpp> 
#include <stdio.h>

//-#include <stdarg.h>

//end char pointer used with strtod function
//.extern char* endPtr ; 

//** just for try **//
 /*
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);
int uart_putchar(char c, FILE *stream);*/
//FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
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
        //uint8_t Detect_Sign(unsigned char** ptr);
        // if enabled using buffers to store the received data
        #if (USE_TX_RX_BUFFERS == 1)
            float float_Nums_Buffer[EXT_F_BUFF_SIZE];
            long  long_Nums_Buffer[EXT_L_BUFF_SIZE];
            unsigned char string_Buffer [EXT_S_BUFF_SIZE];
            volatile float * float_Nums_Buffer_Ptr;
            volatile long * long_Nums_Buffer_Ptr;
            volatile unsigned char * string_Buffer_Ptr;
           
            //These variables to store the count of the received bytes or characters
            uint8_t rxed_FNums_Count ,
                    rxed_LNums_Count ,
                    rxed_String_Count;
            //indexes to access the current value of the receiver buffers
            int8_t curr_Rx_F_Buff_Indx,
                curr_Rx_L_Buff_Indx,
                curr_RX_Str_Buff_Indx,
                read_F_Ptr_Index,
                read_L_Ptr_Index;
            //  tmp_Str_Indx;
            // used for calculating if new numbers or string were received and added to the buffer
            uint8_t tmpCnt_F
                   ,tmpCnt_L,
                    tmpCnt_S ;
            // a flage to indicate if an action executed or not yet
            bool action_Executed;//, new_Data_Received_Flag;
          #endif
    public:
        //int8_t curr_RX_Str_Buff_Indx ;
       // volatile bool EOB_Flag;
       // if enabled using buffers to store the received data
      bool  new_Data_Received_Flag;
      bool Is_New_Data();
        #if (USE_TX_RX_BUFFERS == 1)
            //read only pointers for refrencing the float,long And string buffers for reading only
            volatile const float * read_F_Nums_Buffer_Ptr;
            volatile const long  * read_L_Nums_Buffer_Ptr;
            volatile const unsigned  char* read_String_Buffer_Ptr;
            void Reset_Main_Rx_Ptr(unsigned char );
            int8_t Get_Rxed_F_Num(float &);
            int8_t Get_Rxed_L_Num(long &);
            uint8_t Get_Rxed_Nums_Count(unsigned char);
        #endif

         //varibles used to store the recevied numbers and chars
         volatile unsigned char rxed_Char;   
         volatile float rxed_Float_Number;
         volatile long rxed_Long_Number;

         //variable to hold the type ofthe received number
         volatile unsigned char rxed_Number_Type;
         
         volatile int8_t used_Buffer_Size;
         volatile bool type_Detected,  // a flag for detected data type
                        EOB_Flag ,      // a flag for receiving a complete block of data numbers or message string
                        end_of_Rx_Block,
                        end_of_Tx_Block ;
       // struct status_Flags
       // {
       //    volatile bool type_Detected:1;  // a flag for detected data type
       //    volatile bool EOB_Flag :1;      // a flag for receiving a complete block of data numbers or message string
       //    volatile bool end_of_Rx_Block:1;
       //    volatile bool end_of_Tx_Block:1 ;
       // } Tx_Rx_Flags;
                        
        // volatile int8_t remaining_tx_size;
         volatile uint8_t used_Rx_Buff_Size,
                          //also indicate to the index of the next byte that will be send
                          transmitted_Chars_Count,
                          //also indicate to the index of the next empty buffer location that is ready to store a charcater
                          used_Tx_Buffer_Size;
         //this buffer used for receiving single data block only (float number,long number,string msge)
         unsigned char Rx_Buffer[BUFFER_SIZE];  
         unsigned char Tx_Buffer[BUFFER_SIZE];
        
        /// @brief  Buffer for Transmitted Data
        //void Clear_TX_Buffer();
        volatile unsigned char * Rx_BufferPtr;
        unsigned char * Tx_BufferPtr;
       // const char * Receive_Message()   ;     
        ATMG8A_USART (unsigned long ubrr,bool en_tx,bool en_rx);
        ~ATMG8A_USART();
         
          
          //this function will accept a function pointer as an action to be taken when receiving a ew data
             //Function pointer syntax  returnType (*pointerName)(parameterTypes);
          /**
          * @brief Function that will be called on reciving a number
          * 
          * @param ntyp the type of the received number
          * @param FnPtr function pointer that wiil be called when the mentioned type is received
          */
         void On_Data_Received(void (*FnPtr)(void))  ; 
         int8_t Add_Tx_Character(unsigned char );
       // void ResetPtr();
       
       // void Fill_Tx_Buffer(unsigned char *);                 //used with Tx technique
       //- bool Transmit_Number(const char *,uint8_t,...);
   
    #if USE_TX_FUNCTION_TEMPLATE == 1
         //! important note to use a function template and work correcly it must be declared and defined inside the header file
        template <typename T> bool Send_Nums(const char *,T);
    #endif
    #if USE_TX_FUNCTION_TEMPLATE == 0
        bool Transmit_Number(unsigned char,const char * ,float,long);
    #endif
        bool Transmit_Char (unsigned char & );
        void Transmit_Msg(unsigned char *);
       //- void Send_Msg(const char *frmt = "%s",uint8_t arg_cnt = 1,...);
     // int8_t Transmit_Char();                 //using interrupt technique

        //bool Receive_Number();
     #if USE_USART_INTERRUPTS == 0
        bool Receive_Char (unsigned char  & );
        void Receive_Msg();
     #endif
     uint8_t Receive_Char();
    const unsigned char *current_Tx_Ch_Ptr ;
   
    #if USE_TX_RX_BUFFERS == 1
        // void Reset_Buffer();
        bool Data_Received();
    #endif
     
     #if USE_USART_INTERRUPTS == 1
     /// @brief  USART Interrupts structure
        struct USART_Intrrupts
            {   
                #if USE_TX_INTERRUPT == 1
                    char U_Transmit_Completed_INTR :1;
                    char U_DataRegisterEmpty_INTR : 1;
                #endif
                #if USE_RX_INTERRUPT == 1
                    char U_Receive_Complete_INTR :1;
                #endif  
            } Rx_Tx_Intrerrupts ;
        //void USART_Setup_Interrupt(bool DataREmpty, bool Tx_intr,bool Rx_Intr);
        void USART_Setup_Interrupt(bool rxc = false,bool dre =false,bool txc = false);
    #endif

        //> Transmit sighned long int
        //!void Transmit_long(long value);
        //> Transmit float number
        //!void Transmit_Float(float fval,int8_t nw ,uint8_t np);
        //> Transmit a string message
    
        //!void Transmit_Message(unsigned char *);
        //> Receive a float number (under setup)
        //float Receive_Flaot_Num( );
         //> Receive a float number (under setup)
        //!long Receive_Int_Num( );
        //!double Receive_Float_Num( );// receive float number
        //!double Receive_Float_Num_1( );
        
        
      //  float CustomStrtoF(const char *str);
      //  char * dtoa(char *s, double n);
      //  char* floatToString(float num);
      //  bool Send_Big_Float(float );
        // under setup
       
        //! void Reset_Main_Rx_Ptr(unsigned char );
        //!     bool Get_Rxed_F_Num(float &);
        //!     bool Get_Rxed_L_Num(long &);
//!uint8_t Get_Rxed_Nums_Count(unsigned char );
              //read only Tx buffer pointer
     
        
        //Under test
    //    bool Transmit_Byte (uint8_t & );
    //    bool Receive_Byte (uint8_t &);
    //    char Data_Received();
    //    
    //    
    //    
//
    //    ///@brief This Function is used to send Unsigned int value on serial communication
    //    ///@param Unsigned Int value to be send
    //      void Send_Int(int );  //Is used to send unsigned int value
    //      bool Receive_Int(int & rx_int);
    //  //  int GetInt();
    //  
    //  // void Send_Float(float& );  //Is used to send unsigned int value
    // //   bool Receive_float(float & rx_int);
    // //  void breakDown (float member);// breakDown(int index, unsigned char outbox[], float member);
    // //   float buildUp();
    //    void Send_Float_Num(float Float_Num,uint8_t FNum_Width ,uint8_t FNum_Precission);
    //    bool Receive_Float_Num(float &);
    //    void Receive_Int_num(int& );
    //    void Send_Int_Num(int& txn_int);
    //    //uint8_t Transmit_String(const char strArr[]);
    //    bool Receive_String();
  };

 #if USE_TX_FUNCTION_TEMPLATE == 1
//Template functions definiotions
        template <typename T>  bool ATMG8A_USART::Send_Nums(const char * fmt,T value)
         {
            used_Tx_Buffer_Size = 0;
           Tx_Buffer[BUFFER_SIZE]={'\0'};
           //Tx_BufferPtr = Tx_Buffer;

           unsigned char num_type;
          const char * fmt_twin = fmt;
          //skip % in the string formatter fmt
          fmt_twin++;
          while (*fmt_twin != '\0')
          {

            if(*fmt_twin == 'f')
              {
                num_type = SOB_F;
                break;
              }
              else if(*fmt_twin == 'l')
                {
                  num_type = SOB_I;
                  break ;
                }
                fmt_twin++;
        }
        
        snprintf((char *)Tx_Buffer, BUFFER_SIZE, fmt, value);

            //essetial initialisations
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

        num_type = EOB ;
        Transmit_Char(num_type);
        end_of_Tx_Block =true;
        return end_of_Tx_Block;

         }
        #endif

#endif  
