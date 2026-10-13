// Driver for communicating with the DS18B20 temperature sensor using the 1-Wire communication protocol.

#include <LPC21xx.h>  
#include "delay.h"
#include "types.h"

// P0.19 is connected to the DS18B20 1-Wire data line.
// 1 << 19 creates a mask for bit 19 of Port 0.
#define D (1 << 19)

// If P0.19 = HIGH, R gives a non-zero value.
// If P0.19 = LOW, R gives 0.
#define R (IOPIN0 & (1 << 19))

/****************************************
        DS18B20 RESET FUNCTION
****************************************/

// Sends a reset pulse to the DS18B20 and checks
// whether the sensor gives a presence response.
unsigned char ResetDS18b20(void)
{
    // Stores the logic level read from the 1-Wire
    // data line during the presence-response period.
    unsigned int presence;


    // Configure P0.19 as an output.
    // The microcontroller must control the data line
    // while generating the reset pulse.
    IODIR0 |= D;


    // Keep the 1-Wire line HIGH before starting reset.
    IOPIN0 |= D;


    // Short delay before pulling the line LOW.
    delay_us(1);


    // Pull P0.19 LOW.
    // This starts the DS18B20 reset pulse.
    IOPIN0 &= ~D;


    // Keep the data line LOW for approximately 480 us.
    delay_us(478);


    // Release the data line by making it HIGH.
    IOPIN0 |= D;


    // Wait for the DS18B20 to respond with its presence pulse.
    delay_us(54);


    // Read the complete Port 0 input value.
    // The state of P0.19 is included in this value.
    presence = IOPIN0;


    // Wait for the remaining reset timing period.
    delay_us(423);
   
  // Check the state of the DS18B20 data line.
    // presence = value captured earlier.
    // R        = current state of P0.19.
    // In this program, a non-zero result returns 1;
    // otherwise the function returns 0.
    if(presence & R)
        return 1;
    else
        return 0;
}

/****************************************
        READ ONE BIT FROM DS18B20
****************************************/

// Reads one bit from the DS18B20 using 1-Wire timing.
unsigned char ReadBit(void)
{
    // Stores the logic level read from the DS18B20
    // during the read time slot.
    unsigned int B;
   
  // Pull the 1-Wire line LOW to start a read slot.
    IOPIN0 &= ~D;
  // Keep the line LOW briefly.
    delay_us(1);
  // Release the data line HIGH.
    IOPIN0 |= D;
  // Change P0.19 to an INPUT.
    // This allows the DS18B20 to control the data line.
    IODIR0 &= ~D;
 // Wait for the DS18B20 to place its bit value
    // on the 1-Wire data line.
    delay_us(10);
  // Read Port 0.
    // B now contains the logic state of P0.19
    // at the sampling moment.
    B = IOPIN0;
  // Change P0.19 back to an OUTPUT
    // for the next 1-Wire operation.
    IODIR0 |= D;
  // Check whether the sampled P0.19 bit is HIGH.
    // B = value captured from IOPIN0.
    // R = P0.19 bit mask/current data-line value.
    // HIGH -> received bit is 1
    // LOW  -> received bit is 0
    if(B & R)
        return 1;
    else
        return 0;
}

/****************************************
        WRITE ONE BIT TO DS18B20
****************************************/

// Sends one bit to the DS18B20 using 1-Wire timing.
// Dbit contains the bit that must be transmitted:
// Dbit = 0 -> write logic 0
// Dbit = 1 -> write logic 1
void WriteBit(unsigned char Dbit)
{
    // Pull the 1-Wire data line LOW
    // to start the write time slot.
    IOPIN0 &= ~D;
  // Short starting delay.
    delay_us(1);
  // If Dbit is 1, release the data line HIGH.
    // If Dbit is 0, the line remains LOW
    // for the required write timing.
    if(Dbit)
        IOPIN0 |= D;
 // Keep the write slot active for the required duration.
    delay_us(58);
  // Release the 1-Wire data line HIGH.
    IOPIN0 |= D;
  // Recovery time before the next bit.
    delay_us(1);
}

/****************************************
        READ ONE BYTE FROM DS18B20
****************************************/

// Reads 8 bits from the DS18B20 and returns them
// as one complete byte.
unsigned char ReadByte(void)
{
    // Loop counter used to read 8 individual bits.
    unsigned char i;
  // Stores the complete byte received from the sensor.
    // Initially all bits are 0.
    unsigned char Din = 0;
  // Read all 8 bits.
    for(i = 0; i < 8; i++)
    {
        // Read one bit from the sensor.
        // If the received bit is 1:
        //     place 1 at bit position i
        // If the received bit is 0:
        //     place 0 at bit position i.
        Din |= ReadBit() ? (0x01 << i) : 0;
      // Wait before reading the next bit.
        delay_us(45);
    }
  // Return the complete 8-bit value.
    return(Din);
}


/****************************************
        WRITE ONE BYTE TO DS18B20
****************************************/

// Sends one complete 8-bit byte to the DS18B20.
void WriteByte(unsigned char Dout)
{
    // Loop counter for transmitting 8 bits.
    unsigned char i;
  // Transmit all 8 bits.
    for(i = 0; i < 8; i++)
    {
        // Send the least significant bit of Dout.
        // Dout & 0x01 extracts bit 0.
        // That bit is passed to WriteBit().
        WriteBit(Dout & 0x01);
      // Shift Dout right by one position.
        // This moves the next bit into bit 0
        // so it can be transmitted next.
        Dout = Dout >> 1;
      // Small delay between bits.
        delay_us(1);
    }
  // Wait for the byte-write operation to complete.
    delay_us(98);
}

/****************************************
        READ TEMPERATURE
****************************************/

// Reads the temperature data from the DS18B20.
// Return value:
//     -1     -> sensor communication failure
//     other  -> temperature data returned by the sensor
int ReadTemp(void)
{
    // Loop counter used for reading the two
    // temperature bytes.
    unsigned char n;
  // Stores the two bytes received from the sensor
    // buff[0] = lower temperature byte
    // buff[1] = upper temperature byte
    unsigned char buff[2];
 // Stores the final combined 16-bit temperature value.
    int temp;
  // Counts the number of attempts while waiting
    // for temperature conversion to complete.
    u32 timeout = 0;
   
  /******** RESET SENSOR ********/

    // Reset the DS18B20 before sending a command.
    // ResetDS18b20() generates the 1-Wire reset pulse
    // and checks the sensor response.
    if(ResetDS18b20())
    {
        // Report communication failure to the caller.
        return -1;
    }
  // 0xCC = SKIP ROM command.
    // This tells the DS18B20 that the microcontroller
    // wants to communicate with the available sensor
    // without sending a specific ROM address.
    // This is normally used when only one sensor
    // is present on the 1-Wire bus.
    WriteByte(0xCC);
  // 0x44 = CONVERT T command.
    // This tells the DS18B20 to measure the temperature
    // and perform the temperature conversion.
    WriteByte(0x44);

  /******** WAIT FOR TEMPERATURE CONVERSION ********/

    // Keep checking the sensor until conversion completes.
    // ReadByte() reads a byte from the sensor.
    // In this program, 0xFF is treated as the waiting state.
    while(ReadByte() == 0xFF)
    {
        // Increase the timeout counter every time
        // the sensor still appears to be busy.
        timeout++;
      // Prevent the program from waiting forever
        // if the sensor communication fails.
        if(timeout > 5000)
        {
            return -1;
        }
    }

  /******** RESET SENSOR AGAIN ********/

    // A second reset is required before requesting
    // the converted temperature data.
    if(ResetDS18b20())
    {
        // Report sensor communication failure.
        return -1;
    }


    // 0xCC = SKIP ROM command.
    // Selects the single DS18B20 on the 1-Wire bus.
    WriteByte(0xCC);
  // 0xBE = READ SCRATCHPAD command.
    // This tells the DS18B20 to send the stored
    // temperature data from its scratchpad.
    WriteByte(0xBE);


    /******** READ TEMPERATURE BYTES ********/

    // Read two bytes containing the temperature value.
    for(n = 0; n < 2; n++)
    {
        // Read one byte from the DS18B20.
        // n = 0 -> store lower temperature byte in buff[0]
        // n = 1 -> store upper temperature byte in buff[1]
        buff[n] = ReadByte();
    }
  // Take the upper temperature byte.
    temp = buff[1];
  // Shift the upper byte 8 positions to the left.
    // Example:
    // 00000000 00110010
    // becomes
    // 00110010 00000000
    temp = temp << 8;
  // Combine the upper byte and lower byte
    // temp currently contains buff[1] in the upper 8 bits.
    // buff[0] contains the lower 8 bits.
    temp = temp | buff[0];
  // Return the complete 16-bit temperature data.
    return temp;
}
