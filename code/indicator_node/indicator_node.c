// Controls the vehicle's left and right indicators
// based on commands received through CAN communication.

#include <LPC21xx.h>

#include "types.h"
#include "delay.h"
#include "can.h"
#include "can_defines.h"


/* Stores the indicator command received through CAN.
   'L' = Left indicator
   'R' = Right indicator
   'S' = Indicators OFF */
u8 indicator_cmd = 'S';


/* Loop variable used for LED sequence control */
s32 i;


int main()
{
    /* CAN receive frame used to receive
       indicator commands from the Main Node */
    struct CAN_Frame rx;


    /* Initialize CAN Controller 1 */
    Init_CAN1();


    /* Configure P0.0 to P0.7 as LED output pins */
    IO0DIR |= 0xFF;


    /* LEDs are active-low, so setting the pins HIGH
       keeps all LEDs turned OFF initially */
    IO0SET = 0xFF;


    while(1)
    {
        /* Check whether a CAN message has been received */
        if(C1GSR & RBS_BIT_READ)
        {
            /* Read the received CAN frame */
            CAN1_Rx(&rx);


            /* CAN ID 0x101 carries the indicator command */
            if(rx.ID == 0x101)
            {
                /* Data1 contains 'L', 'R' or 'S' */
                indicator_cmd = rx.Data1;
            }
        }


        /* LEFT INDICATOR */
        if(indicator_cmd == 'L')
        {
            /* Turn ON LEDs one by one from left to right */
            for(i = 0; i <= 7; i++)
            {
                /* Turn OFF all LEDs before selecting
                   the next LED in the sequence */
                IO0SET = 0xFF;

                /* Turn ON the current LED.
                   LEDs are active-low. */
                IO0CLR = (1 << i);

                /* Control the LED animation speed */
                delay_ms(70);
            }
        }


        /* RIGHT INDICATOR */
        else if(indicator_cmd == 'R')
        {
            /* Turn ON LEDs one by one from right to left */
            for(i = 7; i >= 0; i--)
            {
                /* Turn OFF all LEDs before selecting
                   the next LED in the sequence */
                IO0SET = 0xFF;

                /* Turn ON the current LED */
                IO0CLR = (1 << i);

                /* Control the LED animation speed */
                delay_ms(70);
            }
        }


        /* INDICATORS OFF */
        else
        {
            /* Turn OFF all indicator LEDs */
            IO0SET = 0xFF;
        }
    }
}
