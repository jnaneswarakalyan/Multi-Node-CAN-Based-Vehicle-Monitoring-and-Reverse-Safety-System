#include <lpc21xx.h>          // LPC21xx microcontroller register definitions
#include "buzzer.h"     // Buzzer function declarations

#define BUZZER 20             // Buzzer is connected to Port 0, Pin 20

void Buzzer_init(void)
{
    IODIR0 |= (1 << BUZZER);  // Configure P0.20 as an output pin
    IOCLR0 |= (1 << BUZZER);  // Drive P0.20 LOW to keep the buzzer initially OFF/ON according to the active-low circuit
}

void Buzzer_ON(void)
{
    IOCLR0 |= (1 << BUZZER);  // Drive P0.20 LOW to activate the buzzer
}

void Buzzer_OFF(void)
{
    IOSET0 |= (1 << BUZZER);  // Drive P0.20 HIGH to deactivate the buzzer
}
