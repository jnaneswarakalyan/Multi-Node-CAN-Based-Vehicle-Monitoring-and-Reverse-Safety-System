// Ultrasonic sensor driver used by the Reverse Node.
// Generates a trigger pulse, measures the echo pulse width,
// and converts the measured time into obstacle distance.

#include <LPC21xx.h>

#include "delay.h"
#include "types.h"


/* ULTRASONIC PIN DEFINITIONS */

// P0.20 is connected to the ultrasonic
// sensor Trigger pin.
#define trr_pin 20

// P0.19 is connected to the ultrasonic
// sensor Echo pin.
#define echo_pin 19

/* ULTRASONIC INITIALIZATION */

// Configures the Trigger and Echo pins
// and initializes Timer0 for distance measurement.
void INIT_Ultrasonic(void)
{
    // Configure P0.20 as an OUTPUT.
    // The microcontroller sends the trigger pulse
    // to the ultrasonic sensor through this pin.
    IODIR0 |= (1 << trr_pin);
  
  // Configure P0.19 as an INPUT.
    // The ultrasonic sensor sends the Echo signal
    // back to the microcontroller through this pin.
    IODIR0 &= ~(1 << echo_pin);
  
  /* TIMER0 INITIALIZATION */

    // Configure Timer0 prescaler.
    // According to this project's configuration,
    // this is used to obtain approximately 1 us
    // timer resolution with a 15 MHz PCLK.
    T0PR = 15;
  // Keep Timer0 stopped initially.
    T0TCR = 0x00;
}



/* SEND ULTRASONIC TRIGGER */

// Generates the trigger pulse required
// to start an ultrasonic measurement.
void send_pulse(void)
{
    // Make the Trigger pin LOW before
    // generating the trigger pulse.
    IOCLR0 = (1 << trr_pin);
    // Keep Trigger LOW for approximately 2 us.
    delay_us(2);
    // Make the Trigger pin HIGH.
    // This starts the ultrasonic measurement.
    IOSET0 = (1 << trr_pin);
    // Keep Trigger HIGH for approximately 10 us.
    // This satisfies the trigger pulse requirement
    // used by the sensor in this project.
    delay_us(10);
    // Return Trigger pin to LOW.
    // The sensor now sends the ultrasonic signal
    // and produces an Echo pulse.
    IOCLR0 = (1 << trr_pin);
}



/* READ ULTRASONIC ECHO */

// Measures the duration of the Echo signal
// Returns:
//     Echo pulse duration from Timer0.
//     0 if the sensor does not respond
//     within the timeout period.
unsigned int read_echo(void)
{
    // Software counter used to prevent the program
    // from waiting forever if the Echo signal
    // never arrives.
    u32 timeout = 0;
  
    /* WAIT FOR ECHO HIGH */

    // Wait until the Echo pin becomes HIGH.
    // Echo LOW -> sensor has not returned the signal yet.
    // Echo HIGH -> returned ultrasonic signal detected.
    while(((IOPIN0 >> echo_pin) & 1) == 0)
    {
        // Increase timeout counter while waiting.
        timeout++;
      
         // If Echo does not become HIGH within
        // the allowed waiting period, consider
        // the sensor reading unsuccessful.
        if(timeout > 30000)
        {
            return 0;
        }
    }


    /* START TIMER */

    // Reset Timer0 counter to zero.
    T0TCR = 0x02;
    // Start Timer0.
    T0TCR = 0x01;
  
    // Reset timeout counter for the next waiting loop.
    timeout = 0;


    /* WAIT FOR ECHO LOW */

    // Continue waiting while the Echo signal
    // remains HIGH.
    // The duration of this HIGH pulse represents
    // the time taken by the ultrasonic signal
    // to travel to the obstacle and return.
    while((IOPIN0 >> echo_pin) & 1)
    {
        // Increase timeout counter while waiting.
        timeout++;
      
        // If Echo remains HIGH for too long,
        // stop the timer and report sensor failure.
        if(timeout > 30000)
        {
            T0TCR = 0x00;
            return 0;
        }
    }


    /* STOP TIMER */

    // Stop Timer0 after the Echo pulse becomes LOW.
    T0TCR = 0x00;
  
    // Return the measured Echo pulse duration.
    return T0TC;
}



/* CALCULATE DISTANCE */

// Measures the ultrasonic Echo pulse and
// converts the measured time into distance.
// Returns:
//     Distance in centimeters.
//     999 when the sensor reading fails.
unsigned int dist_cal(void)
{
    // Stores the Echo pulse duration.
    u32 pulse;
  
    // Stores the calculated obstacle distance.
    u32 distance;
  
    // Send the trigger pulse to the ultrasonic sensor.
    send_pulse();
  
    // Measure the duration of the Echo pulse.
    pulse = read_echo();


    /* SENSOR FAILURE CHECK */

    // A pulse value of 0 indicates that
    // the Echo signal was not received properly.
    if(pulse == 0)
    {
        // 999 is used by this project as a special
        // value indicating that a valid distance
        // measurement was not obtained.
        return 999;
    }


    // Convert Echo pulse duration into distance
    // in centimeters.
    // For the timing used in this project,
    // dividing by 58 gives approximately
    // the distance in centimeters.
    distance = pulse / 58;


    /* LIMIT SENSOR RANGE */

    // Limit the maximum reported distance
    // to 400 cm.
    if(distance > 400)
    {
        distance = 400;
    }
  
    // Return the calculated obstacle distance
    // in centimeters.
    return distance;
}
