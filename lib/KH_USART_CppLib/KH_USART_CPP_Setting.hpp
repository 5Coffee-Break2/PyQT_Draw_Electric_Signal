#pragma once
//@ required delay between series transmission of bytes in micro seconds
#define SERIES_TX_DELAY 100
//@ may be deleted
#define USE_TX_RX_BUFFERS 1
//@ To Enable/Disable using function template 1/0
#define USE_TX_FUNCTION_TEMPLATE 0
//@ To Enable/Disable using General interrupt 1/0
#define USE_USART_INTERRUPTS 0
//@ To Enable/Disable using Rx Interrupt 1/0
#define USE_RX_INTERRUPT 0
//@ To Enable/Disable using Tx Interrupt 1/0
# define USE_TX_INTERRUPT 0

//********************************************** */
//@Use extra buffers to store the received data a definite buffer for each data type
#ifndef USE_TX_RX_BUFFERS
        #define USE_TX_RX_BUFFERS 1
#endif
//@Setup buffers sizes used to store the received data
#if USE_TX_RX_BUFFERS == 1
    #ifndef EXT_F_BUFF_SIZE
        //@The extra buffer float type size
        # define EXT_F_BUFF_SIZE 17
    #endif
    
    #ifndef EXT_L_BUFF_SIZE
        //@The extra long type buffer size
        # define EXT_L_BUFF_SIZE 17
    #endif
    
    #ifndef EXT_S_BUFF_SIZE
        //@The extra buffer string type  size
        # define EXT_S_BUFF_SIZE 17
    #endif
    
   
#endif

//@To use Tx function template
#ifndef USE_TX_FUNCTION_TEMPLATE
    #define USE_TX_FUNCTION_TEMPLATE 0
#endif

#ifndef F_CPU
        #define  F_CPU 16000000UL
#endif

 //@Enable interrupt in general
#ifndef USE_USART_INTERRUPTS
    #define USE_USART_INTERRUPTS 0
 #endif    
 //@Enable Rx interrupt and dissable Tx interrupt by default 
  #if USE_USART_INTERRUPTS == 1
        //@Use RXC interrupt for receiving data
        #ifndef USE_RX_INTERRUPT
            #define USE_RX_INTERRUPT 1
        #endif
        
        //@ don't use interrupts routines for transmitting data
            #ifndef USE_TX_INTERRUPT
                #define USE_TX_INTERRUPT 0
            #endif       
    #endif


//@Define the required Baud rate
 #ifndef BAUD_RATE
    #define  BAUD_RATE   19200      
 #endif                              
 
 //@ set baud rate value for UBRR  
 //@Define the right value that must be written to UBRR register to get the required Baud rate which depends on the main clock frequency
 //@#define MYUBRR  ((F_CPU/(BAUD_RATE * 16UL))-1)        
                                    

 #if USE_USART_INTERRUPTS == 1
    #include <avr/interrupt.h>
 #endif
  
  #define EOB    0x3                 //@* end of transmition Block                  //@'X'//@0x17       
  #define SOB_S  0x2                 //@* start of transmition Block (string)       //@'S'//@0x0C      
  #define SOB_I 'I'//@0x0C            //@* start of transmition Block (Int)
  #define SOB_F 'A'//@0x0C            //@* start of transmition Block (float)
  //@#define START_OF_MSG '~'           //@ indicate the beginning of a new message in the Rx String buffer

  #define BUFFER_SIZE 20             //@The main buffers Tx/Rx  sizes used to store the received numbers or string
  #define Tx_NUM_SIZE 20             //@the maximum sent number length or size
  #define INTRPT_DEALY 200//@190      //@minimum working value is    190
  #define TX_DELAY 230                  //@(usec )minimum working
  