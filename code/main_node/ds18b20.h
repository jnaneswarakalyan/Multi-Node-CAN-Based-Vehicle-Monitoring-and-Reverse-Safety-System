// Function declarations used to communicate with the DS18B20
// temperature sensor using the 1-Wire communication protocol.

#ifndef DS18B20_H
#define DS18B20_H

// Reset the DS18B20 and check for sensor presence.
unsigned char ResetDS18b20(void);

// Read one bit of data from the DS18B20.
unsigned char ReadBit(void);

// Write one bit of data to the DS18B20.
void WriteBit(unsigned char);

// Read one byte of data from the DS18B20.
unsigned char ReadByte(void);

// Write one byte of data to the DS18B20.
void WriteByte(unsigned char);

// Start temperature conversion and read the temperature value.
int ReadTemp(void);

#endif
