#ifndef HD44780Commands
#define HD44780Commands
															/*--------------------------HD44780 Commands -----------------------------------------*/
																				
			//Legand------->         D=Display		B=Blink Character		C=Cursor
			
#define LCD_RETURN_HOME 			0x02	//need 1.52ms to execute, Sets DDRAM address 0 in address counter. Also returns display from being shifted to original position.DDRAM contents remain unchanged.
#define LCD_CLEAR					0x01	//Clear the display and set DDRam address to 0 in the address counter AC
// Function set with font 5x8 point
#define LCD_4Bit2Line				0x28	//Set the data length = 4 bit and number of display lines = 2 , font size 5x8 dot
#define LCD_4Bit1Line				0x20	//Set the data length = 4 bit and number of display lines = 1 , font size 5x8 dot
#define LCD_8Bit1Line 				0x30	//Set the data length = 8 bit and number of display lines = 1 , font size 5x8 dot			
#define LCD_8Bit2Line 				0x38	//Set the data length = 8 bit and number of display lines = 2 , font size 5x8 dot
//Entry mode set
#define LCD_CURDEC_DISShiftOff	    0x04	//Entry mode set : Sets cursor move direction to Left (decrement) and specifies display shift Off
#define LCD_CURDEC_DISShiftOn		0x05	//Entry mode set : Sets cursor move direction to Left (decrement) and specifies display shift On
#define LCD_CURInc_DISShiftOff	    0x06    //Entry mode set : Sets cursor move direction to right (incrementing) and specifies display shift to non (Display is not shifted).These operations are performed during data write and read.
#define LCD_CURInc_DISShiftOn		0x07	//Entry mode set : Sets cursor move direction to right (incrementing) and specifies display shift on
//Display control
#define LCD_DOn_COff_BOff			0x0C	//Turn On the display, Cursor Off and blinking character off
#define LCD_DOn_COn_BOn		    	0x0F	//Turn On the display, Cursor On  and blinking character On
#define LCD_DOn_COff_BOn			0x0D	//Turn On the display, Cursor Off and blinking character On
#define LCD_DOn_COn_BOff			0x0E	//Turn On the display, Cursor On  and blinking character Off
//////////////
#define LCD_DOff_COff_BOff			0x08	//Turn Off the display, Cursor Off and blinking character off
#define LCD_DOff_COn_BOn		  	0x0B	//Turn Off the display, Cursor On  and blinking character On
#define LCD_DOff_COff_BOn			0x09	//Turn Off the display, Cursor Off and blinking character On
#define LCD_DOff_COn_BOff			0x0A	//Turn Off the display, Cursor On  and blinking character Off
//Cursor or Display Shift
#define LCD_SHLFTCUR				0x10	//Shift Left Cursor	,Moves cursor and shifts display without changing DDRAM contents.
#define LCD_SHRGTCUR				0x12 //4	//shift Right Cursor ,Moves cursor and shifts display without changing DDRAM contents
#define LCD_SHLFTDSP				0x18	//Shift Left Entire Display ,Moves cursor and shifts display without changing DDRAM contents
#define LCD_SHRGTDSP				0x1C    //Shift Right Entire Display,Moves cursor and shifts display without changing DDRAM contents
#endif