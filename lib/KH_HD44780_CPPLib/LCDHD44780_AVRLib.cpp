
// #define BUFFER_LENGTH 17 // buffer size for double and float Functions
// char str[BUFFER_LENGTH];
//  char *gcvt(double number, int ndigit, char *buf);
// #include <util\delay.h>
//#include "global_vars.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "LCDHD44780_AVRLib.hpp"
#include "LCD-HD44780Commands.hpp"
// include <AVR_ExternalInterrupts.hpp>
#include "KH_Atmega8.hpp"


// redirect the standard output stream with a custom user defined stream
// For definig an output stream to be used by printf function
int WriteByteToStdout(char u8Data, FILE *stream);
 FILE PrintF_WR_Stream;
 // a glopal LCD pointer and neceeay parameters to be used any where in the project
 LCD_HD44780 *LCD_Ptr; // don't remove it
 LineNumber g_Line;
 uint8_t g_Pos;
 Page g_Page;

/**
 * @brief Initialize and new LCDobject
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
LCD_HD44780::LCD_HD44780(
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
    unsigned char RedWrPin,
    volatile unsigned char *ReigisterSel,
    volatile unsigned char *EnableTick,
    volatile unsigned char *ReWrPrt)
{
  // #if USE_SPRINTF == 1
  strStream[17] = {'\0'};
  // #endif
  // f_Width =1;
  // f_Precesion = 4;
  //-ptrLine_One = (volatile unsigned char *)0x0;
  //-ptrLine_Tow = (volatile unsigned char *)0x40;
  currentPage = Page_1;
  // pDataLines = DataLinesPrts;
  //  pDataLines=& DataLinesPrts;       // this is the way to assign a pointer of array to the adddres of the array
  DataLinesPrts[0] = Dline7; // (*pDataLines)[0] =  Dline7;      // this the way we can access the elements of array of pointers
  DataLinesPrts[1] = Dline6;
  DataLinesPrts[2] = Dline5;
  DataLinesPrts[3] = Dline4;
  // pDataLines = DataLinesPrts;     // reset the pointer to the array once again
  dataLinespins[0] = dline7pin;
  dataLinespins[1] = dline6pin;
  dataLinespins[2] = dline5pin;
  dataLinespins[3] = dline4pin;

  pReigisterSelPort = ReigisterSel;
  pEnableLinePort = EnableTick;
  pReadWritePort = ReWrPrt;

  RegisterSelpin = registerSelpin;
  EnablePin = enablePin;
  ReadWritePin = RedWrPin;

  *(pReadWritePort - 1) |= 1 << ReadWritePin; /// make ReadWritePin Pin  of pReadWritePort as output  connected to pin R/W of the LCD (Read = 1, Write =0)
  *pReadWritePort &= ~(1 << ReadWritePin);    /// Let ReadWritePin Pin   of pReadWritePort output 0-->write mode fo the LCD , 1 --> write mode for the LCD
  LCD_initialize();
  WriteCommand(LCD_DOn_COff_BOff);
  dataRoom = 0;
  WriteCommand(LCD_SHRGTCUR);

//_************************************* Setup  output file stream for using printf fn ***********************************_//
//! FILE USART_0_stream = FDEV_SETUP_STREAM(WriteByteToStdout, NULL, _FDEV_SETUP_WRITE);  //doesn't compiled
#if USE_PRINTF == 1
  PrintF_WR_Stream.put = WriteByteToStdout;
  PrintF_WR_Stream.flags = _FDEV_SETUP_WRITE;
  PrintF_WR_Stream.buf = strStream;
  stdout = &PrintF_WR_Stream;
#endif
  // Initialize the required parametters for displaying data on the LCD
  //! These are global variabled declared on the top of the page
  // global pointer to the lcd object to use it any where
  LCD_Ptr = this;
  g_Line = Line_1;
  g_Pos = 1;
  g_Page = Page_1;
  //_***************************************************** End of setup  globals********************************************************_/
}

#if USE_SPRINTF == 1
void LCD_HD44780::LCD_Show_Number(num_Type nty, const char *fmt, uint8_t loc, LineNumber ln, Page page)
{
  switch (nty)
  {
  case I_type:
    snprintf(strStream, sizeof(strStream), fmt, num_Value.Ln_value);
    //LCD_SHOW_String(strStream, loc, ln,page ); //(strStream, g_Pos, g_Line, g_Page);
    // return;
    break;

  case F_type:
    snprintf(strStream, sizeof(strStream), fmt, num_Value.Fn_value); //
    //LCD_SHOW_String(strStream, g_Pos, g_Line, g_Page);
    //return;
    break;
  case D_type:
    snprintf(strStream, sizeof(strStream), fmt, num_Value.Dn_value);
    //LCD_SHOW_String(strStream, g_Pos, g_Line, g_Page);
    //return;
    break;
   // default:
    //  LCD_SHOW_String(strStream,g_Pos,g_Line,g_Page);
    //  break;
  }
  LCD_SHOW_String(strStream, loc, ln, page); //(strStream, g_Pos, g_Line, g_Page);
  //  if (num_Typ == I_type)
  //  {
  //    snprintf(strStream, sizeof(strStream), fmt, num_Value.Ln_value);
  //
  //  }
  //  if (num_Typ == F_type)
  //  {
  //    snprintf(strStream, sizeof(strStream), fmt,num_Value.Fn_value);//
  //
  //  }
  //  if (num_Typ == D_type)
  //  {
  //    snprintf(strStream, sizeof(strStream), fmt, num_Value.Dn_value);
  //
  //  }
  //        LCD_SHOW_String(strStream,g_Pos,g_Line,g_Page);
}

uint8_t LCD_HD44780::LCD_SHOW_String(const char strArr[], uint8_t x, LineNumber ln, Page pn)
{
  dataRoom = 0;
  const char *strPtr = strArr;
  LCD_SetPosition((x + pn), ln);
  do
  {
    WriteData(*strPtr++);
    dataRoom += 1;
  }

  while (*strPtr != '\0');

  if (pn != currentPage)
  {
    LCD_ShowPage(pn);
    //   currentPage= pn;
  }
  //      str[0]='\0';  //* clear the buffer for the next use
  return dataRoom;
}

#endif
/*void LCD_HD44780::ClearBufer()
{
  for (char j = 0; j < BUFFER_LENGTH; j++)
  {
    str[j] = '\0';
  }
}*/

void LCD_HD44780::LCD_initialize()
{

  // set the ports pins which the LCD connected to as output pins
  for (uint8_t x = 0; x < 4; x++)
  {
    // set the ports pins as output
    // set the pointer to DDR instead of PORT register
    (*(DataLinesPrts[x] - 1)) |= (1 << dataLinespins[x]); // this the way to access the content of pointer to array of pointers
  }

  // set the control lines pins as output

  *(pReigisterSelPort - 1) |= (1 << RegisterSelpin);
  *(pEnableLinePort - 1) |= (1 << EnablePin);
  // MyDelay(100); //10000
  //! _delay_ms(20);
  // wait until auto initialization of the LCD complete (more than 15ms)

  WriteCommand(0x30);
  //!_delay_ms(5); // wait more than 4.1ms
  // MyDelay(2000);

  WriteCommand(0x30);
  //!_delay_us(105); // wait more than 100us
  // MyDelay(1500);

  WriteCommand(0x30);
  //!_delay_ms(COMMAND_DELAY);
  // MyDelay(1500);

  WriteCommand(LCD_4Bit1Line); // Set Data length to 4 Bit with 1 Line and 5x8 font

  //!_delay_ms(COMMAND_DELAY);
  // MyDelay(1500);
  WriteCommand(LCD_4Bit2Line); // Set Data length to 4 Bit with 2 Line and 5x8 font

  //! _delay_ms(COMMAND_DELAY);
  // WriteCommand(LCD_DOff_COff_BOff); // Display Off

  //!_delay_ms(COMMAND_DELAY);
  // MyDelay(1500);
  WriteCommand(LCD_CLEAR); // Display Clear

  //!_delay_ms(COMMAND_DELAY);
  // MyDelay(1500);
  WriteCommand(LCD_CURInc_DISShiftOff); // set cursor to increment with no display shift

  //!_delay_ms(COMMAND_DELAY);
  // MyDelay(1000);

  WriteCommand(LCD_DOn_COff_BOn);
  // MyDelay (1500);
  //!_delay_ms(5);
}

void LCD_HD44780::WriteCommand(volatile unsigned char cmd)
{
  // WaitForBussyFlag();
  *pReigisterSelPort &= ~(1 << RegisterSelpin); // make RS =0 to write to command register of the LCD
  Writing_LCD(cmd);
  // for (char x =0;x<=3;x++)
  _delay_us(500); //(4500);//(200); //!(COMMAND_DELAY);*/
}

/// @brief: Write single character to the LCD Module
/// @param data byte to be write
void LCD_HD44780::WriteData(char data)
{
  // 	WaitBussyFlag();
  *pReigisterSelPort |= 1 << RegisterSelpin; // make RS=1  to write to the Data register of the LCD
  Writing_LCD(data);
  // Dissabled for test  in 4-1-2023*/    WaitForBussyFlag();
  _delay_us(300); //(4500);//(100);  //!_delay_ms(COMMAND_DELAY);
}

/// @brief Write single Byte to the LCD module
/// @param argu
void LCD_HD44780::Writing_LCD(char argu)
{

  // The LCD hardware writes the upper nible 1st then Write the lower nibble
  // current bit postion working on
  uint8_t bpos = 0;
  //> check the command bit x if it is 1 or 0 and write it to the required port
  // start with the high nibble
  for (char x = 7; x >= 0; x--)
  {
    (argu & (1 << x)) ? ((*DataLinesPrts[bpos]) |= 1 << dataLinespins[bpos]) : ((*DataLinesPrts[bpos]) &= (~(1 << dataLinespins[bpos])));
    // if (argu & (1 << x))
    //   (*DataLinesPrts[bpos]) |= 1 << dataLinespins[bpos]; // content of pointer to arra pointer
    // else
    //   (*DataLinesPrts[bpos]) &= (~(1 << dataLinespins[bpos]));
    bpos++;
    if (x == 4) // if we finshed checking the upper nible
    {
      EN_Pulse(); // send a pulse for send the upper nibble of the command to the LCD command register
      bpos = 0;
    }
    if (x == 0)
      EN_Pulse(); // send a pulse for send the lower nibble of the command to the LCD command register
  }

  //-uint8_t x_bit = 7;
  // pointer to write data port
  //-volatile unsigned char **pWr_Nibble  =  DataLinesPrts;
  // pointer to write port pin
  //- volatile char * pWr_Pin = dataLinespins;
  //-  while (1)
  //-  {
  //-   //again:
  //-
  //-    switch (argu & (1 << x_bit))
  //-    {
  //-    case true:
  //-          *(*pWr_Nibble++) |= 1 << *pWr_Pin++;
  //-    // (*DataLinesPrts[bpos]) |= 1 << dataLinespins[bpos]; // content of pointer to arra pointer
  //-     break;
  //-
  //-     case false:
  //-           *(*pWr_Nibble++) &= (~(1 << *pWr_Pin++));
  //-     //(*DataLinesPrts[bpos]) &= (~(1 << dataLinespins[bpos]));
  //-     break;
  //-
  //-    }
  //-
  //-   if (x_bit == 4) // if we finshed checking the upper nible
  //-   {
  //-     EN_Pulse(); // send a pulse for send the upper nibble of the command to the LCD command register
  //-     pWr_Nibble = DataLinesPrts;
  //-     pWr_Pin = dataLinespins;
  //-     //bpos = 0;
  //-     //x_bit--;
  //-    //goto again;
  //-     //continue;
  //-
  //-   }
  //-  //point to the next write pin
  //-     //.pWr_Nibble++;   //pWr_Nibble is a pointer to a pointer
  //-     //.pWr_Pin++;
  //-
  //-
  //-    if(x_bit == 0)
  //-    {
  //-      EN_Pulse(); // send a pulse for send the upper nibble of the command to the LCD command register
  //-    //  bpos = 0;
  //-      return;
  //-    }
  //-     // bpos++;
  //-      x_bit--;
  //-  }
}

void LCD_HD44780::EN_Pulse()
{
  *pEnableLinePort |= 1 << EnablePin;
  //! _delay_us(PULSE_DELAY);
  *pEnableLinePort &= (~(1 << EnablePin));
}

/// @brief Set the character location on the LCD module
/// @param charPosition: location number on the display line
/// @param charlineNumber: the line numberon the LCD module
void LCD_HD44780::LCD_SetPosition(uint8_t charPosition, LineNumber charlineNumber)
{
  // line1 StartAddress = 0x00;		//from data sheet for 2 Line 8x2 character
  // line2 StartAddress = 0x40;     //from data sheet for 2 Line 8x2 character
  // the total number of characters is 80 character ---> 40 char x 2 lines
  if (charPosition >= 1 && charPosition <= 40 /*80*/) // i have used 16 character only from the 40 character in each line
  {

    if (charlineNumber == 1)
      // the address of the 1st character inthe 1st line = 0x00
      // To set the DDRAM address you must or logic the required address with 0x80(from the data sheet)
      WriteCommand((charPosition - 1) | 0x80);

    else if (charlineNumber == 2)
      // the address of the 1st character in the 2nd line = 0x40
      // To set the DDRAM address you must or logic the required address with 0x80(from the data sheet)
      WriteCommand((0x3F + charPosition) | 0x80); //((0x40+charPosition-1) | 0x80);			//0x40-1=0x3F
  }
}

void LCD_HD44780::LCD_Clear()
{
  WriteCommand(LCD_CLEAR);
}

/// @brief Show the Required Page 1,2,or 3( This function used to display the required page 1,2, or 3  it works fine on page 1 and 2, but page 3 is in accurate because the sreen is    shared between Page 1 and page 3)
/// @param Page: Page# 1,2 or 3 (page 3 under debugging)
void LCD_HD44780::LCD_ShowPage(Page page_num)
{
  if (page_num == currentPage)
    return;
  unsigned char lim = 16;
  WriteCommand(LCD_DOff_COff_BOff); // turn off the display
  // from page P1 to P2 or P2 to P3 or from P3 to P1
  if (
      (currentPage == Page_1 && page_num == Page_2) ||
      (currentPage == Page_2 && page_num == Page_3) ||
      (currentPage == Page_3 && page_num == Page_1))
  {
    if (currentPage == Page_3 && page_num == Page_1)
      lim = 8;

    for (char zz = 0; zz < lim; zz++) // page_num+1
    {
      WriteCommand(LCD_SHLFTDSP);
    }

    // currentPage = page_num;
    //  return ;
  }

  // from P2 to P1 or from P3 to P2 or from P1 to P3
  else if (
      (currentPage == Page_2 && page_num == Page_1) ||
      (currentPage == Page_3 && page_num == Page_2) ||
      (currentPage == Page_1 && page_num == Page_3))
  {
    if (currentPage == Page_1 && page_num == Page_3)
      lim = 8;
    for (char zz = 0; zz < lim; zz++) // page_num+1
    {
      WriteCommand(LCD_SHRGTDSP);
    }
    // currentPage = page_num;
    // return ;
  }
  WriteCommand(LCD_DOn_COff_BOff); // Turn On Display
  currentPage = page_num;
}

/**
 * @brief Write Single character to the LCD module
 * @param p: Page Number (1,2 or 3)
 * @param x: Character Postion on the line
 * @param l: Line number (1 or 2)
 * @param d: Character to be write
 */
void LCD_HD44780::LCD_Show_Character(char d, uint8_t x, LineNumber l, Page p)
{
  LCD_SetPosition((x + p), l);
  WriteData(d);
  if (p != currentPage)
  {
    LCD_ShowPage(p);
    //     currentPage= p;
  }
}

#if (EXTEND_FUNCTION)
void LCD_HD44780::LCD_DonConBOff()
{
  WriteCommand(LCD_DOn_COn_BOff);
}
void LCD_HD44780::LCD_DOnCoffBon()
{
  WriteCommand(LCD_DOn_COff_BOn);
}

void LCD_HD44780::LCD_DonCoffBoff()
{
  WriteCommand(LCD_DOn_COff_BOff);
}

void LCD_HD44780::BlinCharacter(char x, LineNumber y)
{
  char tc = 0;
  // volatile unsigned char Ptr* = &tc;
  // WriteCommand(LCD_DOn_COn_BOff);
  LCD_SetPosition(17, line_1);
}

void LCD_HD44780::LCD_ReadMode()
{
  *pReadWritePort |= (1 << ReadWritePin); // To enable the Read mode for reading bussy flag write 1 to the read write port
}

void LCD_HD44780::LCD_WriteMode()
{
  *pReadWritePort &= ~(1 << ReadWritePin); // To enable the Read mode for reading bussy flag write 0 to the read write port
}
#endif

// #if(USE_PRINTF == 0)
//>   #if(SHOW_INT == 1)
//>      uint8_t LCD_HD44780::LCD_Show_Int(long int value,uint8_t xpos,LineNumber ypos,Page pnum )
//>       {
//>         //5 is the minimum number of the digits to be displaied (for int type it should be 6) - sign for left justified
//>         sprintf(conversion_Buffer,INT_WIDTH, value);
//>          return LCD_SHOW_String(conversion_Buffer,xpos,ypos,pnum  );
//>       }
//>   #endif
//>
//>   #if (SHOW_HEX == 1)
//>       uint8_t LCD_HD44780::LCD_ShowHex(Page pnum,char xpos, LineNumber ypos, int value,char* strPtr)
//>       {
//>         sprintf(strPtr, "%X ", value);
//>         return LCD_SHOW_String(pnum,xpos, ypos );
//>         //LCD_ShowText(pnum,xpos + 2, ypos, str);
//>       }
//>   #endif

//>   #if (SHOW_FLOAT == 1)
//>       uint8_t LCD_HD44780::LCD_Show_Float(float val,uint8_t xpos , LineNumber ypos,Page pnum  )
//>       {
//>         //	snprintf(str,sizeof(str),"%g",val); //not working will
//>         // sprintf(str,"%0.6f",val); //not working will
//>         // ClearBufer();
//>         //  snprintf(buffer, sizeof(buffer), "%g", myDoubleVar);
//>       //  dtostrf(val, FLOAT_WIDTH, FLOAT_PRECISSION, str);
//>         dtostrf(val, f_Width, f_Precesion, conversion_Buffer);
//>         // gcvt(val,6,str);
//>         return LCD_SHOW_String(conversion_Buffer,xpos,ypos,pnum  );
/*
 Function dtostrf()
 char * dtostrf(
 double __val,
 signed char __width,
 unsigned char __prec,
 char * __s)
 The dtostrf() function converts the double value passed in val into an ASCII representationthat will be stored under s. The caller is responsible for providing sufficient storage in s.

 Conversion is done in the format "[-]d.ddd". The minimum field width of the output string (including the possible '.' and the possible sign for negative values) is given in width, and prec determines the number of digits after the decimal sign. width is signed value, negative for left adjustment.

 The dtostrf() function returns the pointer to the converted string s.
 */
//>       }
//>   #endif

//>    #if(SHOW_DOUBLE == 1)
//>
//>       uint8_t LCD_HD44780::LCD_Show_Double(double val,uint8_t xpos , LineNumber ypos ,Page pnum )
//>       {
//>         // ClearBufer();
//>         // gcvt(val,6,str);
//>         //   sprintf(str, "%f", val);
//>         dtostre(val, conversion_Buffer, DOUBLE_PRECISSION, DTOSTR_ALWAYS_SIGN | DTOSTR_UPPERCASE); // note that precision is defined in the top of this file
//>        // LCD_ShowText(pnum,xpos, ypos, str);
//>          return  LCD_SHOW_String(conversion_Buffer,xpos ,ypos,pnum);
/*
char* dtostre ( double __val, char * __s, unsigned char __prec, unsigned char __flags )

The dtostre() function converts the double value passed in val into an ASCII representation that will be stored under s. The caller is responsible for providing
sufficient storage in s.Conversion is done in the format "[-]d.ddde�dd" where there is one digit before the decimal-point character and the number of digits after
it is equal to the precision prec; if the precision is zero, no decimal-point character appears. If flags has the DTOSTRE_UPPERCASE bit set, the letter 'E'
(rather than 'e' ) will be used to introduce the exponent. The exponent always contains two digits; if the value is zero, the exponent is "00".
  If flags has the DTOSTRE_ALWAYS_SIGN bit set, a space character will be placed into the leading position for positive numbers.
    If flags has the DTOSTRE_PLUS_SIGN bit set, a plus sign will be used instead of a space character in this case.
The dtostre() function returns the pointer to the converted string s.
*/
//>       }
//>   #endif
// #endif
/**
 * @brief Thi sfunction used to display datavia printf function and File stream
 *
 * @param u8Data character wishing to display
 * @param stream pointer to a FILE stream obj
 * @return int  return the count of displayed characters
 */
int WriteByteToStdout(char u8Data, FILE *stream) //(char u8Data, __file *stream)//int  ATMG8A_USART::USART_SendByte(char u8Data, FILE *stream)
{
  uint8_t wz = g_Pos;

  if (g_Pos > 16)
    g_Pos = 1;
  LCD_Ptr->LCD_Show_Character(u8Data, g_Pos, g_Line, g_Page);
  g_Pos += 1;
  return g_Pos - wz;
}

// void writeLCD(FILE *fp, const int numOfChars, ...)
// {
//    va_list ap;
//    va_start(ap, numOfChars);
//    for(int i = 0; i < numOfChars; i++)
//    {
//       const char c = va_arg(ap,char);
//       putc(c, fp);
//    }
//    va_end(ap);
// }

/*LCD_HD44780::~LCD_HD44780()
{
  ;
}*/

//! Last Modifications 4-1-2023
/*
void LCD_HD44780::WaitForBussyFlag()
{
//! bool is_bussy = false;  //Dissabled for test 4-1-2023
// make data pin 7 as input
 *DataLinesPrts[0] &= ~(1 << dataLinespins[0]);
  // make RS =0 to write to command register of the LCD
 *pReigisterSelPort &= ~(1 << RegisterSelpin);
 // To enable the Read mode for reading  write 1 to the read write port
 *pReadWritePort |= (1 << ReadWritePin);
  EN_Pulse(); // read the bussy flag

 while((*(DataLinesPrts[0]-2)) & (1 << dataLinespins[0]))
 //! *pReadWritePort |= (1 << ReadWritePin);   //Dissabled for test 4-1-2023
 EN_Pulse();

 //Return the port to the last mode state befor reading
 // To enable the write mode write 0 to the read write port
 *pReadWritePort &= ~(1 << ReadWritePin);
// make data pin 7 as output
 *DataLinesPrts[0] |= (1 << dataLinespins[0]);

 return ; //!is_bussy;   //Dissabled for test 4-1-2023
}  */

/*void LCD_HD44780::LCD_Shift(char d)
{
 for(char i=0;i<d;i++)
   WriteCommand(LCD_SHLFTDSP);  //LCD_SHLFTDSP

}*/
