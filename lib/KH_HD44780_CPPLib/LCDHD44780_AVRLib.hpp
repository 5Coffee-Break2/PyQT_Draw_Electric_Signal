/************************************************************************/
/*				LCD HD44780 Generic Driver Library 4 Bit Mode    		 */
/*                            19/11/2020  V1.0				   			 */
/************************************************************************/
/************************************************************************/
/*						Author: Eng.Khalid Sobhy                         */
/************************************************************************/
/*
The instruction to write a character at a certain location on the display in case of 16x2 display
by writing  the location  to DDRAM address cross ponding to the required location

1st line
Digit position on LCD   	    1	 2	 3	 4	 5	 6	 7	 8	 9	 10 11 12 13 14 15 16   17 18 19 20 21 22 23 24 25 26 27 28 29 30 31   32 .........40
DDRAM address in HEX				 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F   10                                        1F   20..........27

2nd Line
Digit position on LCD 		41  42  43  44  45  46  47  48  49  50  51  52  53  54 55 56    57 58 59 60 61 26 63 64 65 66 67 68 69 70 71   72 73...80
DDRAM address in HEX			40  41  42  43  44  45  46  47  48  49  4A  4B  4C  4D 4E 4F    50 51 52 53 54 55 56 57 59 5A 5B 5C 5D 5E 5F   60 61...67

The Instruction to Write to DDRAM
            RS	RW	D7	D6	D5	D6	D5	D4	D3	D2	D1	 D0
            0	0	1	....the address from tables above..
*/

#ifndef HD44780_KH_CPP
#define HD44780_KH_CPP

enum LineNumber
{
  Line_1 = 1,
  Line_2 = 2
};
enum Page
{
  Page_1 = 0,
  Page_2 = 16,
  Page_3 = 32
};
enum num_Type
{
  I_type,
  F_type,
  D_type
};

/****************************/
#define CONVERSION_BUFFER_LENGTH 18
#define COMMAND_DELAY 2 // delay time between successive commands in millisecond								(Don't Change it)
#define PULSE_DELAY 1   // delay for the Enable pulse of the enable pin of the LCD in microsecond			(Don't Change it)

#ifndef F_CPU            // if the frequency not explicitly defined
#define F_CPU 16000000UL // then define it with the mentioned value 16MHZ
#endif

#ifndef _UTIL_DELAY_H_  // if the delay header is not included
#include <util/delay.h> //then include it
#include <stdio.h>
#endif

#include "LCD_Setting.hpp"
// #include "string.h"

//______Global Variables (an external global variable to the lCD is declared at the end of this file)_______
extern LineNumber g_Line;
extern uint8_t g_Pos;
extern Page g_Page;

class LCD_HD44780
{
private:
  static char x_location;
  static LineNumber currentLine;
  //-volatile unsigned char *ptrLine_One;
  //-volatile unsigned char *ptrLine_Tow;
  volatile unsigned char *DataLinesPrts[4]; // an array of pointers
  char dataLinespins[4];
  volatile unsigned char *pReigisterSelPort; // AVR port to which Register select of the LCD connected
  volatile unsigned char *pEnableLinePort;   // AVR port to which Enable line for the LCD is connected
  volatile unsigned char *pReadWritePort;    // AVR port to which Read Write control line for the LCD is connected

  // char conversion_Buffer[CONVERSION_BUFFER_LENGTH];
  char RegisterSelpin; // Pin number to which lcd Register select line connected to
  char EnablePin;      // Pin number to which lcd Enable line connected to
  char ReadWritePin;   // Pin number to which lcd Read Write control line connected to
  void EN_Pulse();
  void WriteData(char data);
  void Write_Byte_LCD(char cmd);
  void LCD_initialize();

#if USE_SPRINTF == 1
  struct nums_Values
  {
    long Ln_value = 0;
    float Fn_value = 0.0;
    double Dn_value = 0e0;
  };
#endif
public:
  //!!!!!!!!!!!! for test /////////////////
  uint8_t LCD_SHOW_String(const char strArr[], uint8_t x = 1, LineNumber ln = Line_1, Page pn = Page_1);
  // Structure to represent a position on the LCD.
  struct Pos_Status
  {
    uint8_t row_num;
    bool row_1_equpied; // uint8_t col;
    bool row_2_equpied;
    Pos_Status() : row_num(0),row_1_equpied(false), row_2_equpied(false) {} // Default constructor to initialize to (0,0)
    //-- Position(int c, int r) : col(c), row(r) {}
    //-- Position(const Position &other) : col(other.col), row(other.row) {} // Copy constructor
  };
  // struct Position
  // {
  //    bool col;//uint8_t col;
  //   uint8_t row;

  //   Position() : col(false), row(0) {} // Default constructor to initialize to (0,0)
  //                                  //-- Position(int c, int r) : col(c), row(r) {}
  //                                  //-- Position(const Position &other) : col(other.col), row(other.row) {} // Copy constructor

  //   // Overload the == operator for comparing positions.  Important for using in the set.
  //   bool
  //   operator==(const Position &other) const
  //   {
  //     return (col == other.col && row == other.row);
  //   }

  //   // Overload the < operator.  This is required for using Position as key in std::set.
  //   bool operator<(const Position &other) const
  //   {
  //     if (row != other.row)
  //     {
  //       return row < other.row;
  //     }
  //     else
  //     {
  //       return col < other.col;
  //     }
  //   }
  // };
  //!The index of the array+1 represent the position on the display
  Pos_Status Display_buffer[16];
  void ManageDisplay(uint8_t, uint8_t, uint8_t);
  //!!!!!!!!!!!! for test /////////////////
  //~LCD_HD44780();
#if USE_SPRINTF == 1
  struct nums_Values num_Value;
  num_Type num_Typ;
  void LCD_Show_Number(num_Type nty, const char *fmt, uint8_t loc = 1, LineNumber line = Line_1, Page page = Page_1);

  /// @brief Display a constant string on the LCD
  /// @param strArr a char array to holde the string requied string to be displayed on the LCD or constat string (between qutes "")
  /// @param x :Char Postion
  /// @param ln :Line # (1 or 2)
  /// @param pn :Page # (1,2 or 3)
  /// @return the string length (number of characters)

  uint8_t LCD_SHOW_String(const char strArr[], uint8_t x = 1, LineNumber ln = Line_1, Page pn = Page_1);
#endif
  // #if USE_SPRINTF == 1

  // #endif
  uint8_t dataRoom; // Store the actual data buffer  length
  Page currentPage;
  // to hold the float number parameters minimum number width and the number precession
  //> uint8_t f_Width,
  //>         f_Precesion;
  /**
   * @brief a function to send a byte or character directly to the LCD hardware
   * @attention Low leve hardware Function
   * @param arguments: unsigned char type
   */
  void WriteCommand(volatile unsigned char arguments);

  /**
   * @brief Initialize and new LCDobject
   *
   * @param dline4pin: Pin number on the microco controller connected to data line pin 4 on the LCD module
   * @param dline5pin: Pin number on the microco controller connected to data line pin 5 on the LCD module
   * @param dline6pin: Pin number on the microco controller connected to data line pin 6 on the LCD module
   * @param dline7pin: Pin number on the microco controller connected to data line pin 7 on the LCD module
   * @param Dline4: Pointer to the the pin 4 Data line port
   * @param Dline5: Pointer to the the pin 5 Data line port
   * @param Dline6: Pointer to the the pin 6 Data line port
   * @param Dline7: Pointer to the the pin 7 Data line port
   * @param registerSelpin:  Register selection pin of LCD lines
   * @param enablePin: Enable pin of LCD lines
   * @param ReWrPin: Read Write pin selection on the LCD
   * @param ReigisterSel: Port of AVR connected to LCD register select pin
   * @param EnableTick: Port of AVR connected to LCD Enable pin
   * @param RdWrPrt: Port of AVR connected to LCD Read/Write pin
   */
  LCD_HD44780(
      unsigned char dline4pin,
      unsigned char dline5pin,
      unsigned char dline6pin,
      unsigned char dline7pin,
      volatile unsigned char *Dline4,
      volatile unsigned char *Dline5,
      volatile unsigned char *Dline6,
      volatile unsigned char *Dline7,
      unsigned char registerSelpin,
      unsigned char enablePin,
      unsigned char ReWrPin,
      volatile unsigned char *ReigisterSel,
      volatile unsigned char *EnableTick,
      volatile unsigned char *RdWrPrt);

  void LCD_Clear();
  void LCD_ShowPage(Page);
  void LCD_Display_Character(char ch, uint8_t pos = 1, LineNumber line = Line_1, Page page = Page_1);

  /// @brief Move the LCD curssor to a defined position
  /// @param charPosition The required new X position of the cursor (1:16)
  /// @param charlineNumber The required new Y position (Line number 1:2)
  void LCD_SetPosition(uint8_t charPosition, LineNumber charlineNumber); // void LCD_SetPosition(char charPosition, char lineNumber);

  //.  #if (SHOW_FLOAT == 1)
  //.        /// @brief Function to Display a float number on the LCD
  //.          /// @attention Float number Width defined in class variable f_width   : The minimum number width which Define the number of digitl of the number including .(period) and + sighn, the negative means the number is left justified
  //.          /// @attention Float number Precession defined in the class variable f_Precession : The number of digits afer the period
  //.          /// @param val  : The value to display
  //.          /// @param xpos : The starting postion on the line
  //.          /// @param ypos : The line number on the dispaly to show up the float number
  //.          /// @param pnum : The prefered page number to display the floating number
  //.        uint8_t LCD_Show_Float(float val,uint8_t xpos = 1, LineNumber ypos= Line_1,Page pnum = Page_1 );//LCD_Show_Float(float val,int8_t,uint8_t,Page,uint8_t, LineNumber );
  //.  #endif
  //.
  //.  #if(USE_PRINTF == 0)
  //.        #if(SHOW_DOUBLE == 1)
  //.           ///@return The size of the displayed number
  //.           uint8_t LCD_Show_Double(double,uint8_t = 1 , LineNumber = Line_1,Page = Page_1 );
  //.        #endif

  //.        #if(SHOW_INT == 1)
  //.        /// @brief Convert long int (10 digit min value -999999999 max value 999999999) to string
  //.        /// @param pnum LCD page 1,2, or 3 (not recomended)
  //.        /// @param xpos start column value for displaying the number
  //.        /// @param ypos The line value 1 or 2 for displaying the number
  //.        /// @param value The required number to be dispalyed
  //.        /// @param strPtr  pointer to a temporary buffer for converting
  //.        /// @return The size of the displayed number
  //.        uint8_t  LCD_Show_Int(long int value,uint8_t xpos = 1,LineNumber ypos = Line_1,Page pnum =Page_1);
  //.        #endif

  //.        #if(SHOW_HEX == 1)
  //.         void LCD_ShowHex(Page ,char , LineNumber , int);
  //.        #endif
  //.     // //void LCD_ShowFloat(float val,uint8_t xpos,uint8_t ypos);
  //.  #endif

#if (EXTEND_FUNCTION == 1)
  void LCD_DonConBOff();
  void LCD_DOnCoffBon();
  void LCD_DonCoffBoff();
  // void MoveCursor();
  void BlinCharacter(char x, LineNumber y);

  inline void LCD_ReadMode();
  inline void LCD_WriteMode();
#endif

  // under test Fn's
  // char GetDataByte(char,char);
  //    void WaitForBussyFlag();

  // void Writing_LCD_Bussy(char argu);
  //  void LCD_Display(Page);
  //    void LCD_ShowInt(Page,char , LineNumber, int );
  // void MyDelay(double);
  // void LCD_Show();
  // void LCD_ShowNextPage();
  // void LCD_Shift(char d);
};
extern LCD_HD44780 *LCD_Ptr;
// bool ReadPortPin(volatile unsigned char *, char );

#endif