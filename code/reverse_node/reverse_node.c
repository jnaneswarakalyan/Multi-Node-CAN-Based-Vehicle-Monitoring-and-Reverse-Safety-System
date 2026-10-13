// Receives the vehicle mode through CAN, measures obstacle distance
// using an ultrasonic sensor, determines the reverse safety status,
// and sends the distance and alert information through CAN.

#include <LPC21xx.h>

#include "types.h"
#include "delay.h"
#include "can.h"
#include "can_defines.h"


/*GLOBAL VARIABLES*/

// Stores the current vehicle mode.
// 0 = Forward mode
// 1 = Reverse mode
u8 mode = 0;


// Stores the measured distance from the
// vehicle to the detected obstacle.
u32 distance;


// Stores the reverse safety status.
// 0 = No alert
// 1 = Obstacle is too close
u8 alert;

// Function that measures the distance using
// the ultrasonic sensor and returns the result.
u32 dist_cal(void);

// Function that configures the microcontroller
// pins required for the ultrasonic sensor.
void INIT_Ultrasonic(void);



/* MAIN FUNCTION*/

int main()
{
    // tx = CAN frame used to transmit information.
    // rx = CAN frame used to receive information.
    struct CAN_Frame tx, rx;
    // Initialize CAN Controller 1
    // so this node can communicate through CAN.
    Init_CAN1();
  
  // Initialize the ultrasonic sensor interface.
    INIT_Ultrasonic();

  /*MAIN PROGRAM LOOP*/

    while(1)
    {
        /* RECEIVE VEHICLE MODE*/

        // Check whether a CAN message has arrived
        // in the CAN1 receive buffer.
        if(C1GSR & RBS_BIT_READ)
        {
            // Read the received CAN frame into rx.
            CAN1_Rx(&rx);
          // CAN ID 0x102 is used for the
            // vehicle mode command.
            if(rx.ID == 0x102)
            {
                // Data1 contains the current mode.
                // 0 = Forward
                // 1 = Reverse
                mode = rx.Data1;
            }
        }
        /*REVERSE MODE OPERATIO*/

        // Perform ultrasonic measurement only
        // when the vehicle is in Reverse mode.
        if(mode)
        {
            /* AVERAGE THREE DISTANCE READINGS */

            // Take three ultrasonic distance measurements
            // and calculate their average.
            // Averaging helps reduce sudden variations
            // in individual sensor readings.
            distance =
            (
                dist_cal() +
                dist_cal() +
                dist_cal()
            ) / 3;


            /* CHECK SAFETY DISTANCE */
          // If the obstacle is closer than 20 cm,
            // activate the reverse safety alert.
            if(distance < 20)
            {
                alert = 1;
            }
            else
            {
                // Distance is 20 cm or more,
                // so no reverse alert is required.
                alert = 0;
            }


            /* TRANSMIT DISTANCE AND ALERT */

            // CAN ID 0x201 identifies the message
            // containing reverse sensor information.
            tx.ID = 0x201;
          // RTR = 0 means this is a CAN Data Frame.
            tx.vbf.RTR = 0;
          // Two data values are transmitted:
            // Data1 = distance
            // Data2 = alert
            tx.vbf.DLC = 2;
          // Store measured obstacle distance
            // in the first CAN data field.
            tx.Data1 = distance;
          // Store reverse safety alert status
            // in the second CAN data field.
            tx.Data2 = alert;
          // Transmit the distance and alert information
            // to the other CAN node.
            CAN1_Tx(tx);
        }


        // Wait approximately 100 ms before
        // repeating the measurement/communication cycle.
        delay_ms(100);
    }
}
