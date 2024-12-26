#ifndef KH_ATMEGA8A
#define KH_ATMEGA8A
	
#ifndef __AVR_ATmega8__
#define __AVR_ATmega8__
#endif 

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

	//From ATmega8a Data Sheet
	#define PIN_D_ (volatile unsigned char *)	0x30
	#define DDR_D_ (volatile unsigned char *)	0x31
	#define PORT_D_ (volatile unsigned char *)	0x32
	#define PIN_C_ (volatile unsigned char *)	0x33
	#define DDR_C_ (volatile unsigned char *)	0x34
	#define PORT_C_ (volatile unsigned char *)	0x35
	#define PIN_B_ (volatile unsigned char *)	0x36
	#define DDR_B_ (volatile unsigned char *)	0x37
	#define PORT_B_ (volatile unsigned char *)	0x38

	//counters
	#define TCCR0_ (volatile unsigned char *)	0x53
	#define TCNT0_ (volatile unsigned char *)	0x52
	#define TCCR1A_ (volatile unsigned char *)	0x4F
	#define TCCR1B_ (volatile unsigned char *)	0x4E
	#define TCNT1H_ (volatile unsigned char *)	0x4D
	#define TCNT1L_ (volatile unsigned char *)	0x4C
	#define OCR1AH_ (volatile unsigned char *)	0x4B
	#define OCR1AL_ (volatile unsigned char *)	0x4A
	#define OCR1BH_ (volatile unsigned char *)	0x49
	#define OCR1BL_ (volatile unsigned char *)	0x48
	#define ICR1H_ (volatile unsigned char *)	0x47
	#define ICR1L_ (volatile unsigned char *)	0x46
	#define TCCR2_ (volatile unsigned char *)	0x45
	#define TCNT2_ (volatile unsigned char *)	0x44
	#define OCR2_ (volatile unsigned char *)	0x45

	//Flag registers
	#define GICR_ (volatile unsigned char *)	0x5B
	#define GIFR_ (volatile unsigned char *)	0x5A
	#define TIMSK_ (volatile unsigned char *)	0x59
	#define TIFR_ (volatile unsigned char *)	0x58
	//Status register
	#define SREG_ (volatile unsigned char *)	0x3F

#include <avr\io.h>
#ifdef USE_DELAY_LIB
#include <util/delay.h>
#endif
#endif