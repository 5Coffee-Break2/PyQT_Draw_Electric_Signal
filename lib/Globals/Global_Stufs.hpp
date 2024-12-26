#ifndef _GLOBAL_STUFFES_H
#define _GLOBAL_STUFFES_H

#define LED_1 DDRB 
#define LED_2 DDRB
#define LED_3 DDRD

#define LED_1_Outp PORTB 
#define LED_2_Outp PORTB
#define LED_3_Outp PORTD

#define LED_1_Pin 0
#define LED_2_Pin 1
#define LED_3_Pin 6

#define WINK_DELAY 100000

#include "KH_Atmega8.hpp"
//#include <util/delay.h>

void Set_Leds();
void my_Delay_Fn(long dt=80000);
void Wink_Both(long ld = WINK_DELAY);
void Wink_Led2(long ld = WINK_DELAY);
void Wink_Led1(long ld = WINK_DELAY);
void Wink_Led3(long ld = WINK_DELAY);

#endif 

