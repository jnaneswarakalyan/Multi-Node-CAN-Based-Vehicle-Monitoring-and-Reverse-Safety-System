
// interrupt_MAJOR.c
// Handles external interrupts for the Main Node.
// EINT0 -> Mode switch
// EINT1 -> Left indicator switch
// EINT2 -> Right indicator switch

#include <LPC21xx.h>             // LPC21xx microcontroller register definitions
#include "types.h"           // Project-specific data type definitions

extern volatile u8 mode;          // Stores vehicle mode: 0 = Forward, 1 = Reverse
extern volatile u8 left_flag;     // Stores left indicator ON/OFF status
extern volatile u8 right_flag;    // Stores right indicator ON/OFF status
extern volatile u8 tx_flag;       // Requests CAN transmission from the main program
extern volatile u8 indicator_cmd; // Stores indicator command: 'L', 'R' or 'S'


// Interrupt service routine declarations.
// __irq tells the compiler these functions are interrupt handlers.
void eint0_isr(void)__irq;         // EINT0 interrupt handler for Mode switch
void eint1_isr(void)__irq;         // EINT1 interrupt handler for Left indicator
void eint2_isr(void)__irq;         // EINT2 interrupt handler for Right indicator


void interrupt_enable(void)
{
        // Configure P0.1, P0.3 and P0.7 for external interrupts.
        PINSEL0 &= ~(
                        (3<<2)  |
                        (3<<6)  |
                        (3<<14)
                    );

        PINSEL0 |= (
                        (3<<2)  |     // P0.1 configured as EINT0
                        (3<<6)  |     // P0.3 configured as EINT1
                        (3<<14)       // P0.7 configured as EINT2
                    );

        EXTINT = 0x07;              // Clear any pending EINT0, EINT1 and EINT2 flags

        EXTMODE = 0x07;             // Configure EINT0, EINT1 and EINT2 as edge-triggered

        EXTPOLAR = 0x00;            // Select falling-edge triggering for all three interrupts

        VICIntSelect = 0x00;        // Configure these interrupts as IRQ interrupts

        VICIntEnable =
                        (1<<14) |    // Enable EINT0
                        (1<<15) |    // Enable EINT1
                        (1<<16);     // Enable EINT2

        VICVectCntl0 = (1<<5) | 14; // Enable VIC slot 0 for EINT0

        VICVectAddr0 = (u32)eint0_isr; // Store EINT0 ISR address in VIC slot 0

        VICVectCntl1 = (1<<5) | 15; // Enable VIC slot 1 for EINT1

        VICVectAddr1 = (u32)eint1_isr; // Store EINT1 ISR address in VIC slot 1

        VICVectCntl2 = (1<<5) | 16; // Enable VIC slot 2 for EINT2

        VICVectAddr2 = (u32)eint2_isr; // Store EINT2 ISR address in VIC slot 2
}


/****************************************
        MODE SWITCH - EINT0
****************************************/

void eint0_isr(void)__irq
{
        // Toggle the vehicle mode.
        // 0 = Forward, 1 = Reverse.
        mode = !mode;

        // Request the main program to send the new mode through CAN.
        tx_flag = 2;

        // Clear the EINT0 interrupt flag.
        EXTINT = 1<<0;

        // Inform the VIC that the interrupt has been serviced.
        VICVectAddr = 0;
}


/****************************************
        LEFT SWITCH - EINT1
****************************************/

void eint1_isr(void)__irq
{
        // Check whether the left indicator is currently OFF.
        if(left_flag == 0)
        {
                // Turn the left indicator ON.
                left_flag = 1;

                // Turn the right indicator OFF.
                right_flag = 0;

                // Set the command to LEFT indicator.
                indicator_cmd = 'L';
        }
        else
        {
                // Turn the left indicator OFF.
                left_flag = 0;

                // Set the command to turn indicators OFF.
                indicator_cmd = 'S';
        }

        // Request the main program to send the indicator command through CAN.
        tx_flag = 1;

        // Clear the EINT1 interrupt flag.
        EXTINT = 1<<1;

        // Inform the VIC that the interrupt has been serviced.
        VICVectAddr = 0;
}


/****************************************
        RIGHT SWITCH - EINT2
****************************************/

void eint2_isr(void)__irq
{
        // Check whether the right indicator is currently OFF.
        if(right_flag == 0)
        {
                // Turn the right indicator ON.
                right_flag = 1;

                // Turn the left indicator OFF.
                left_flag = 0;

                // Set the command to RIGHT indicator.
                indicator_cmd = 'R';
        }
        else
        {
                // Turn the right indicator OFF.
                right_flag = 0;

                // Set the command to turn indicators OFF.
                indicator_cmd = 'S';
        }

        // Request the main program to send the indicator command through CAN.
        tx_flag = 1;

        // Clear the EINT2 interrupt flag.
        EXTINT = 1<<2;

        // Inform the VIC that the interrupt has been serviced.
        VICVectAddr = 0;
}
