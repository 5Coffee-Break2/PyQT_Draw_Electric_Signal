#ifndef GLOBAL_VARS
#define GLOBAL_VARS

//#include <stdio.h>
//#include <stdlib.h>
//#include "LCDHD44780_AVRLib.hpp"
//---------------=========== Timer 2 Global variables ===========-------------//
volatile float current_Time; //= 0.0f;
volatile float ticks;        // = 0.0;
//_the time required for the counter to count 256 digit (over flow) in seconds
volatile float ticks_time_256; //= (PRE_SCALE / F_CPU) * 256; // (PRE_SCALE / 16000000L) * 256
volatile float tick_time;      // = (PRE_SCALE / F_CPU);

///---------------=========== LCD Global variables ===========-------------//
//FILE PrintF_WR_Stream;
//// a glopal LCD pointer and neceeay parameters to be used any where in the project
//LCD_HD44780 *LCD_Ptr; // don't remove it
//LineNumber g_Line;
//uint8_t g_Pos;
//Page g_Page;
#endif
