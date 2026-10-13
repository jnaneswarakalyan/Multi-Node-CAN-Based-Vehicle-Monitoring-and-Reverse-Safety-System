// major_lcd.c
// LCD driver functions for the 16x4 LCD.

#include <LPC21xx.h>            
#include "lcd_defines.h"    
#include "types.h"           
#include "DEFINES_MAJOR.h"       
#include "delay.h"        
#include "MAJOR_LCD.h"           

// Send one byte of data/command to the LCD.
void WriteLCD(u8 data)
{
    // RW = 0 selects LCD write operation.
    SCLRBIT(IOCLR0, RW);
  // Place the 8-bit data on the LCD data pins P0.8-P0.15.
    WBYTE(IOPIN0, LCD_DATA, data);
 // EN = 1 enables the LCD to read the data.
    SSETBIT(IOSET0, EN);
 // Keep Enable high for the required short pulse duration.
    delay_us(1);
 // EN = 0 completes the LCD write operation.
    SCLRBIT(IOCLR0, EN);
 // Give the LCD time to internally process the command/data.
    delay_ms(2);
}

// Send a command instruction to the LCD.
void CmdLCD(u8 cmd)
{
    // RS = 0 selects the LCD command register.
    SCLRBIT(IOCLR0, RS);
  // Send the command byte to the LCD.
    WriteLCD(cmd);
}

// Send one character to be displayed on the LCD.
void CharLCD(u8 ascii)
{
    // RS = 1 selects the LCD data register.
    SSETBIT(IOSET0, RS);
   // Send the character to the LCD.
    WriteLCD(ascii);
}

// Initialize the LCD in 8-bit, 2-line mode.
void InitLCD(void)
{
    // Configure LCD data pins P0.8 to P0.15 as output pins.
    WBYTE(IODIR0, LCD_DATA, 255);
  // Configure RS pin P0.16 as an output.
    SETBIT(IODIR0, RS);
 // Configure RW pin P0.17 as an output.
    SETBIT(IODIR0, RW);
  // Configure EN pin P0.18 as an output.
    SETBIT(IODIR0, EN);
 // Wait for the LCD power-up initialization time.
    delay_ms(15);
  // Send the initial 8-bit LCD initialization command.
    CmdLCD(MODE_8BIT_1LINE);
  // Wait for the LCD to process the command.
    delay_ms(5);
// Send the initialization command again as required
    // during LCD power-up initialization.
  CmdLCD(MODE_8BIT_1LINE);
  // Short delay before the next initialization command.
    delay_us(100);
 // Send the initialization command for the third time.
    CmdLCD(MODE_8BIT_1LINE);
 // Configure the LCD for 8-bit, 2-line operation.
    CmdLCD(MODE_8BIT_2LINE);
  // Turn the LCD display ON.
    CmdLCD(DISP_ON);
 // Clear the complete LCD display.
    CmdLCD(CLEAR_LCD);
  // Set the cursor to move to the right after displaying data.
    CmdLCD(SHIFT_CUR_RIGHT);
}

// Display a null-terminated string on the LCD.
void StrLCD(s8* p)
{
    // Continue displaying characters until the null character '\0'
    // indicating the end of the string is reached.
    while(*p)
        CharLCD(*p++);
}

// Display an unsigned 32-bit decimal number on the LCD.
void U32LCD(u32 n)
{
    // Array stores individual decimal digits.
    u8 a[10];
  // Index used while storing digits in the array.
    s32 i = 0;
 // Special case for displaying zero.
    if(n == 0)
    {
        CharLCD('0');
    }
    else
    {
        // Extract digits from right to left.
        while(n)
        {
            // Store the current digit as its ASCII value.
            a[i++] = (n % 10) + 48;
          // Remove the last digit from the number.
            n /= 10;
        }

      // Display the stored digits in reverse order
        // to obtain the correct decimal number.
        for(--i; i >= 0; i--)
            CharLCD(a[i]);
    }
}


// Display a signed 32-bit decimal number.
void S32LCD(s32 n)
{
    // Check whether the number is negative.
    if(n < 0)
    {
        // Display the negative sign.
        CharLCD('-');
      // Convert the negative number to its positive value
        // before displaying its digits.
        n = -n;
      // Display the positive part of the number.
        U32LCD(n);
    }
}


// Display a floating-point number with the requested
// number of digits after the decimal point.
void F32LCD(f32 fnum, u8 nDP)
{
    u32 n;
    s32 i;
  // Check whether the floating-point value is negative.
    if(fnum < 0)
    {
        // Display the negative sign.
        CharLCD('-');
    }
  // Extract the integer part of the floating-point value.
    n = fnum;
 // Display the integer part.
    U32LCD(n);
  // Display the decimal point.
    CharLCD('.');
// Generate and display the required number
    // of digits after the decimal point.
    for(i = 0; i < nDP; i++)
    {
        // Remove the integer part and multiply by 10
        // to obtain the next fractional digit.
        fnum = (fnum - n) * 10;
      // Extract the next integer digit.
        n = fnum;
      // Convert the digit to ASCII and display it.
        CharLCD(n + 48);
    }
}


// Display an unsigned 32-bit number in hexadecimal format.
void HexLCD(u32 n)
{
    // Array stores hexadecimal digits.
    u8 a[8], rem;
  // Index used while storing hexadecimal digits.
    s32 i = 0;
 // Special case for displaying zero.
    if(n == 0)
    {
        CharLCD('0');
    }
    else
    {
        // Extract hexadecimal digits from right to left.
        while(n)
        {
            // Get the remainder after division by 16.
            rem = n % 16;
          // Convert the hexadecimal digit into ASCII.
            // 0-9  -> ASCII '0'-'9'
            // 10-15 -> ASCII 'A'-'F'
            (rem < 10) ? (rem += 48) : (rem += 55);
          // Store the converted hexadecimal character.
            a[i++] = rem;
          // Remove the processed hexadecimal digit.
            n /= 16;
        }
      // Display the stored hexadecimal digits
        // in the correct order.
        for(--i; i >= 0; i--)
        {
            CharLCD(a[i]);
        }
    }
}


// Display a number in binary format.
void BinLCD(u32 n, u8 nbd)
{
    s32 i;
  // Start from the most significant bit requested
    // and move toward the least significant bit.
    for(i = (nbd - 1); i >= 0; i--)
    {
        // Extract one bit and convert it into
        // ASCII '0' or '1' before displaying it.
        CharLCD(((n >> i) & 1) + 48);
    }
}


// Store custom character patterns into the LCD CGRAM.
void BuildCGRAM(u8* p, u8 nb)
{
    s32 i;
  // Move the LCD address pointer to CGRAM.
    // CGRAM is used to store user-defined LCD characters.
    CmdLCD(GOTO_CGRAM);
  // Write the custom-character pattern bytes
    // into the LCD CGRAM.
    for(i = 0; i <= nb; i++)
    {
        CharLCD(p[i]);
    }
  // Return the LCD cursor to the beginning of Line 2
    // after completing the CGRAM operation.
    CmdLCD(GOTO_LINE2_POS0);
}
