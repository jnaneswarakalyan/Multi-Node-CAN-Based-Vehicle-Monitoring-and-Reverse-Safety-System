// main_node.c
// Main Node of the CAN-Based Vehicle Safety & Monitoring System.
// Handles engine temperature monitoring, indicator control,
// CAN communication, vehicle mode display and reverse safety alert.

#include <LPC21xx.h>              // LPC21xx microcontroller register definitions
#include "types.h"           // Project-specific data type definitions such as u8, u32, f32
#include "lcd.h"            // LCD function declarations
#include "lcd_defines.h"    // LCD commands, positions and control definitions
#include "delay.h"          // Delay function declarations
#include "ds18b20.h"         // DS18B20 temperature sensor functions
#include "can.h"             // CAN initialization, transmission and reception functions
#include "can_defines.h"     // CAN register and bit definitions
#include "buzzer.h"          // Buzzer control functions


// Vehicle operating mode.
// 0 = Forward mode
// 1 = Reverse mode
volatile u8 mode = 0;


// Indicator switch status updated by the interrupt routines.
volatile u8 left_flag = 0;
volatile u8 right_flag = 0;


// Indicates which CAN message needs to be transmitted.
// 0 = No transmission
// 1 = Indicator command
// 2 = Vehicle mode
volatile u8 tx_flag = 0;


// Stores the reverse-alert information received from the Reverse Node.
volatile u8 reverse_alert = 0;


// Stores the current indicator command.
// 'L' = Left indicator
// 'R' = Right indicator
// 'S' = Indicators OFF
volatile u8 indicator_cmd = 'S';


// Used to create the blinking effect of the indicator symbols on the LCD.
u8 blink = 0;


// Stores the temperature value read from the DS18B20 sensor.
f32 temp;


// Stores the integer part of the temperature for LCD display.
s32 tp;


// Stores the fractional temperature digit.
// The program displays either .0 or .5.
u8 tpd;


// Stores the obstacle distance received from the Reverse Node through CAN.
u32 dist = 0;


// Custom LCD character patterns used for the left and right indicator symbols.
// First 8 bytes  = first custom indicator character.
// Next 8 bytes   = second custom indicator character.
u8 indicator_lut[] =
{
    0x01,
    0x03,
    0x07,
    0x0F,
    0x1F,
    0x0F,
    0x07,
    0x03,

    0x10,
    0x18,
    0x1C,
    0x1E,
    0x1F,
    0x1E,
    0x1C,
    0x18
};


// Function declaration for enabling the external interrupts.
void interrupt_enable(void);


int main()
{
    // tx = CAN frame used for transmitting data.
    // rx = CAN frame used for receiving data.
    struct CAN_Frame tx, rx;


    // Initialize the LCD and configure its pins.
    InitLCD();


    // Initialize CAN Controller 1 for communication
    // between the Main Node and other CAN nodes.
    Init_CAN1();


    // Enable the external interrupts used for
    // mode, left-indicator and right-indicator switches.
    interrupt_enable();


    // Initialize the buzzer GPIO pin.
    Buzzer_init();


    // Store the two custom indicator characters in LCD CGRAM.
    // Character 0 = first indicator symbol.
    // Character 1 = second indicator symbol.
    BuildCGRAM(indicator_lut, 15);


    /****************************************
            SYSTEM STARTUP DISPLAY
    ****************************************/

    // Move the LCD cursor to Line 1, Position 0.
    CmdLCD(GOTO_LINE1_POS0);


    // Display the first line of the startup message.
    StrLCD("Vehicle Safety And");


    // Move the LCD cursor to Line 2, Position 0.
    CmdLCD(GOTO_LINE2_POS0);


    // Display the second line of the startup message.
    StrLCD("Monitoring System ");


    // Keep the startup message visible for 2 seconds.
    delay_ms(2000);


    // Clear the LCD after displaying the startup message.
    CmdLCD(0x01);


    /****************************************
            MAIN MONITORING LOOP
    ****************************************/

    // Continuously monitor sensors, switches and CAN messages.
    while(1)
    {
        /****************************************
                LINE1 : TEMPERATURE
        ****************************************/

        // Move the LCD cursor to the beginning of Line 1.
        CmdLCD(0x80);


        // Clear the previous temperature information.
        StrLCD("               ");


        // Return the cursor to the beginning of Line 1.
        CmdLCD(0x80);


        // Read the engine temperature from the DS18B20 sensor.
        temp = ReadTemp();


        // Check whether the temperature sensor returned an error.
        if(temp == -1)
        {
            // Display sensor failure information.
            StrLCD("sensor fail");
        }
        else
        {
            // Extract the integer part of the temperature.
            // DS18B20 temperature data is shifted right by 4 bits
            // to obtain the integer portion.
            tp = ((int)temp) >> 4;


            // Determine the fractional part of the temperature.
            // If bit 3 is set, display .5; otherwise display .0.
            tpd = (((int)temp) & 0x08) ? '5' : '0';


            // Display the temperature label.
            StrLCD("ENG TEMP:");


            // Display the integer temperature value.
            U32LCD(tp);


            // Display the decimal point.
            CharLCD('.');


            // Display the fractional digit.
            CharLCD(tpd);


            // Display the degree symbol.
            CharLCD(223);


            // Display the Celsius unit.
            CharLCD('C');


            // Add a space after the temperature.
            StrLCD(" ");
        }


        /****************************************
                LINE2 : INDICATOR STATUS
        ****************************************/

        // Toggle blink between 0 and 1.
        // This creates the ON/OFF blinking effect.
        blink = !blink;


        // Move the LCD cursor to Line 2.
        CmdLCD(0xc0);


        // Clear the previous indicator information.
        StrLCD("             ");


        // Return the cursor to the beginning of Line 2.
        CmdLCD(0xc0);


        // Check whether the left indicator is active.
        if(indicator_cmd == 'L')
        {
            // Display the left indicator symbol only
            // during the ON phase of the blink.
            if(blink)
                CharLCD(0);
            else
                CharLCD(' ');


            // Display the second custom indicator symbol.
            CharLCD(1);


            // Display left indicator status.
            StrLCD(" LEFT IND");
        }


        // Check whether the right indicator is active.
        else if(indicator_cmd == 'R')
        {
            // Display the first custom indicator symbol.
            CharLCD(0);


            // Display the second indicator symbol
            // only during the ON phase of the blink.
            if(blink)
                CharLCD(1);
            else
                CharLCD(' ');


            // Display right indicator status.
            StrLCD(" RIGHT IND");
        }


        // If neither left nor right indicator is active.
        else
        {
            // Display both indicator symbols.
            CharLCD(0);
            CharLCD(1);


            // Display indicator OFF status.
            StrLCD(" IND OFF");
        }


        /****************************************
                CAN TRANSMISSION
        ****************************************/

        // tx_flag = 1 means an indicator command
        // needs to be transmitted to the Indicator Node.
        if(tx_flag == 1)
        {
            // CAN ID 0x101 is assigned to indicator commands.
            tx.ID = 0x101;


            // RTR = 0 means this is a normal CAN data frame.
            tx.vbf.RTR = 0;


            // DLC = 1 means the CAN frame contains one data byte.
            tx.vbf.DLC = 1;


            // Store the indicator command in the first data byte.
            // 'L' = Left
            // 'R' = Right
            // 'S' = OFF
            tx.Data1 = indicator_cmd;


            // Transmit the indicator command through CAN1.
            CAN1_Tx(tx);


            // Clear the transmission request after sending.
            tx_flag = 0;
        }


        // tx_flag = 2 means the vehicle mode
        // needs to be transmitted to the Reverse Node.
        else if(tx_flag == 2)
        {
            // CAN ID 0x102 is assigned to vehicle mode information.
            tx.ID = 0x102;


            // RTR = 0 means this is a normal CAN data frame.
            tx.vbf.RTR = 0;


            // DLC = 1 means one data byte is being transmitted.
            tx.vbf.DLC = 1;


            // Send the current vehicle mode.
            // 0 = Forward
            // 1 = Reverse
            tx.Data1 = mode;


            // Transmit the vehicle mode through CAN1.
            CAN1_Tx(tx);


            // Clear the transmission request after sending.
            tx_flag = 0;
        }


        /****************************************
                RECEIVE DISTANCE + ALERT
        ****************************************/

        // Check whether a CAN message has arrived
        // in the CAN1 receive buffer.
        if(C1GSR & RBS_BIT_READ)
        {
            // Read the received CAN frame.
            CAN1_Rx(&rx);


            // CAN ID 0x201 is used by the Reverse Node
            // to send distance and alert information.
            if(rx.ID == 0x201)
            {
                // Data1 contains the measured obstacle distance.
                dist = rx.Data1;


                // Data2 contains the alert status.
                reverse_alert = rx.Data2;
            }
        }


        /****************************************
                LINE3 : MODE DISPLAY
        ****************************************/

        // Move the LCD cursor to Line 3, Position 0.
        CmdLCD(0x94);


        // Check the current vehicle mode.
        if(mode)
        {
            // mode = 1 means Reverse mode.
            StrLCD("MODE: REVERSE ");
        }
        else
        {
            // mode = 0 means Forward mode.
            StrLCD("MODE: FORWARD ");
        }


        /****************************************
                LINE4 : STATUS / DISTANCE
        ****************************************/

        // Move the LCD cursor to Line 4, Position 0.
        CmdLCD(0xd4);


        // Display obstacle information only in Reverse mode.
        if(mode)
        {
            // Display the distance label.
            StrLCD("DIST:");


            // Display the obstacle distance.
            U32LCD(dist);


            // Display the distance unit.
            StrLCD("cm ");


            // Check whether the obstacle is closer than 20 cm.
            if(dist < 20)
            {
                // Display the danger warning.
                StrLCD("ALERT   ");


                // Turn ON the buzzer to warn the driver.
                Buzzer_ON();
            }
            else
            {
                // Display the safe condition.
                StrLCD("SAFE   ");


                // Turn OFF the buzzer.
                Buzzer_OFF();
            }
        }
        else
        {
            // Display normal forward-driving status.
            StrLCD("FORWARD DRIVE MODE");


            // Make sure the reverse warning buzzer is OFF.
            Buzzer_OFF();
        }


        // Wait 300 ms before starting the next monitoring cycle.
        delay_ms(300);
    }
}
