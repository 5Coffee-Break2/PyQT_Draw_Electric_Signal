#ifndef INITIALIZATIONS
#define INITIALIZATIONS
// 0 Dissabled , 1 Enabled

// **************************************************(  Customized definitions )***********************************************//
//-//@ this flag option is essential in compiler flags to enable displaying float number with printf function "-Wl,-u,vfprintf -lprintf_flt -lm"
//-//@ in platform.ini file add the below line in the .ini file
//-//@ build_flags = -I Include - Wl, -u, vfprintf - lprintf_flt - lm
//-#define USE_PRINTF 0  // Enable or dissable the use of printf and fprintf functions to display numbers on the LCD
//-                      //.#define SHOW_INT 1
//-                      //.#define SHOW_FLOAT 1
//-                      //.#define SHOW_DOUBLE 0
//-#define USE_SPRINTF 1 // This the default value to use the default method to dispaly numbers on the LCD using sprintf function
#define EXTEND_FUNCTION 0
//***************************************************( End of Customized definitions )****************************************//

//@ LCD Pins configuration (you can change the ports and their pins values according to your LCD wiring configuration)
#define DP4 PC0             //@ port Pin which is connected to the lcd data line4
#define DP5 PC1             //@ port Pin which is connected to the lcd data line5
#define DP6 PC2             //@ port Pin which is connected to the lcd data line6
#define DP7 PC3             //@ port Pin which is connected to the lcd data line7
#define DPP4 PORT_C_        //@ Port for the lcd data line4
#define DPP5 PORT_C_        //@ Port for the lcd data line5
#define DPP6 PORT_C_        //@ Port for the lcd data line6
#define DPP7 PORT_C_        //@ Port for the lcd data line7
#define DE PD5              //@ Display Enable pin
#define DRS PD6             //@ Display Registers select pin
#define DRW PD0             //@ Display Read/Write pin or connect it to the ground pin

#ifndef USE_PRINTF //-#ifndef USE_SPRINTF
    #define USE_SPRINTF 0
    #define USE_PRINTF 1
#else
    #define USE_SPRINTF 1
    #define USE_PRINTF 0
#endif

//-#ifndef USE_PRINTF
//-#define USE_PRINTF 0
//-#endif

//-#if (USE_PRINTF == 1)
//-#define USE_SPRINTF 0
//-//.#define SHOW_INT 0
//-//.#define SHOW_DOUBLE 0
//-//.#define SHOW_FLOAT 0
//-#endif

#if USE_PRINTF == 1 && USE_SPRINTF == 1
#error "error Both USE_PRINTF and USE_SPRINTF = 1"
#endif

#if USE_PRINTF == 0 && USE_SPRINTF == 0
#error "error Both USE_PRINTF and USE_SPRINTF = 0"
#endif
//. #ifndef SHOW_INT
//.   #define SHOW_INT 1
//. #endif

//.#ifndef SHOW_DOUBLE
//.  #define SHOW_DOUBLE 1
//.#endif

//.#ifndef SHOW_FLOAT
//.  #define SHOW_FLOAT 1
//.#endif

//.#ifndef SHOW_HEX
//.  #define SHOW_HEX 0
//.#endif

//.#ifndef DOUBLE_PRECISSION
//.  #define DOUBLE_PRECISSION 4 // define the Precision for the double Function number of digits after the period
//.#endif

//.#ifndef INT_WIDTH
//.  #define INT_WIDTH "%-6ld" // "%-6d"  "%-12ld"// define the minimum number of digits to be diplayed if the value < this width a space character will be added to compensate the difference
//.                    //d after 6 is a flag for sighned decimal
//.#endif

// #ifndef FLOAT_PRECISSION
//   #define FLOAT_PRECISSION 1 // define the the number of the digits after the period
// #endif
//
// #ifndef FLOAT_WIDTH
//   #define FLOAT_WIDTH -6 // define the minimum number of digitl of the number including .(period) and + sighn, the negative means the number is left justified
// #endif

#endif