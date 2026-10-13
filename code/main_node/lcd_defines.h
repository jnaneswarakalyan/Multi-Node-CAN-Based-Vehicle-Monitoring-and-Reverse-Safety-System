// LCD pin connections and command definitions.
// Used by the LCD driver to control the 16x2/20x4 character LCD.


// LCD PIN CONFIGURATION

#define LCD_DATA 8
// LCD data lines start from P0.8.
// P0.8 to P0.15 are used as the 8-bit LCD data bus.

#define RW 18
// LCD Read/Write control pin is connected to P0.18.
// RW = 0 -> Write operation
// RW = 1 -> Read operation

#define RS 16
// LCD Register Select pin is connected to P0.16.
// RS = 0 -> Command register selected
// RS = 1 -> Data register selected

#define EN 17
// LCD Enable pin is connected to P0.17.
// A pulse on EN tells the LCD to accept the command/data.


// LCD COMMANDS

#define CLEAR_LCD 0x01
// Clear the entire LCD display
// and move the cursor to the starting position.

#define RET_CUR_HOME 0X02
// Return the LCD cursor to the home position.

#define DISP_OFF 0X08
// Turn OFF the LCD display.

#define DISP_ON 0X0C
// Turn ON the LCD display with the cursor hidden.

#define DISP_ON_CUR_ON 0X0E
// Turn ON the LCD display and show the cursor.

#define DISP_ON_CUR_ON_BLINK 0X0F
// Turn ON the LCD display,
// show the cursor and make the cursor blink.


// LCD DISPLAY MODE CONFIGURATION

#define MODE_8BIT_1LINE 0X30
// Configure the LCD to use 8-bit data mode
// with one display line.

#define MODE_8BIT_2LINE 0X38
// Configure the LCD to use 8-bit data mode
// with two display lines.

#define MODE_4BIT_1LINE 0X20
// Configure the LCD to use 4-bit data mode
// with one display line.

#define MODE_4BIT_2LINE 0X28
// Configure the LCD to use 4-bit data mode
// with two display lines.


// LCD CURSOR POSITION COMMANDS

#define GOTO_LINE1_POS0 0X80
// Move the LCD cursor to Line 1, Position 0.

#define GOTO_LINE2_POS0 0XC0
// Move the LCD cursor to Line 2, Position 0.

#define GOTO_LINE3_POS0 0X94
// Move the LCD cursor to Line 3, Position 0.

#define GOTO_LINE4_POS0 0XD4
// Move the LCD cursor to Line 4, Position 0.


// LCD CURSOR / DISPLAY SHIFT COMMANDS

#define SHIFT_CUR_RIGHT 0X06
// Move the cursor position to the right
// after writing a character.

#define SHIFT_DISP_LEFT 0X10
// Shift the displayed contents of the LCD to the left.

#define SHIFT_DISP_RIGHT 0X14
// Shift the displayed contents of the LCD to the right.


// LCD CUSTOM CHARACTER MEMORY

#define GOTO_CGRAM 0X40
// Select LCD CGRAM starting address.
// CGRAM is used to store custom characters,
// such as the left and right indicator symbols.
