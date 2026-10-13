// Function declarations used to control the LCD display.

#ifndef LCD_H
#define LCD_H

// Send a command instruction to the LCD.
void WriteLCD(unsigned char data);

// Select LCD command mode and send a command.
void CmdLCD(unsigned char cmd);

// Display one character on the LCD.
void CharLCD(unsigned char ascii);

// Initialize the LCD and configure it for operation.
void InitLCD(void);

// Display a string of characters on the LCD.
void StrLCD(char *p);

// Display an unsigned 32-bit integer on the LCD.
void U32LCD(unsigned int n);

// Display a floating-point number with the specified decimal places.
void F32LCD(float fnum, unsigned char ndp);

// Display a signed integer on the LCD.
void S32LCD(int n);

// Display an integer value in hexadecimal format.
void HexLCD(unsigned int n);

// Display an integer value in binary format.
void BinLCD(unsigned int n, unsigned char nbd);

// Display an integer value in octal format.
void OctLCD(unsigned int n);

// Store custom characters in the LCD CGRAM.
void BuildCGRAM(unsigned char *p, unsigned char nb);

#endif
