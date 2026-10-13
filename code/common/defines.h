// Common macros used for setting, clearing, reading and modifying
// individual bits, nibbles and bytes in LPC21xx registers.

#ifndef DEFINE_H
#define DEFINE_H


// SET A BIT

#define SETBIT(WORD,BITPOS) (WORD |= 1<<BITPOS)
// Set the selected bit position to 1.
// Used when a particular bit in a register must be enabled or set.


/* CLEAR A BIT */

#define CLRBIT(WORD,BITPOS) (WORD &= ~(1<<BITPOS))
// Clear the selected bit position to 0.
// Used when a particular bit in a register must be disabled or cleared.


/* WRITE LOGIC 1 */

#define SSETBIT(WORD,BITPOS) (WORD = 1<<BITPOS)
// Write a 1 to the selected bit position.
// Used with registers such as IOSET where writing 1 sets the corresponding pin.


/* WRITE LOGIC 0 */

#define SCLRBIT(WORD,BITPOS) (WORD = 1<<BITPOS)
// Write a value to the selected bit position.
// Used with registers such as IOCLR for clearing an output pin.


/* WRITE A BYTE */

#define WBYTE(WORD,SBITPOS,BYTE) \
        WORD = (WORD & ~(0xff<<SBITPOS)) | (BYTE<<SBITPOS)
// Clear 8 bits starting from SBITPOS and write the new byte value.
// Used to update an 8-bit field inside a register.


/* WRITE A NIBBLE */

#define WNIBBLE(WORD,SBITPOS,NIBBLE) \
        WORD = (WORD & ~(0x0F<<SBITPOS)) | (NIBBLE<<SBITPOS)
// Clear 4 bits starting from SBITPOS and write the new nibble value.


/* WRITE A SINGLE BIT */

#define WBIT(WORD,SBITPOS,BIT) \
        WORD = (WORD & ~(1<<SBITPOS)) | (BIT<<SBITPOS)
// Clear the selected bit and write the required 0 or 1 value.


/* READ A SINGLE BIT */

#define RBIT(WORD,SBITPOS) ((WORD>>SBITPOS)&1)
// Read one bit from the selected position.
// The result is either 0 or 1.


/* READ A NIBBLE */

#define RNIBBLE(WORD,SBITPOS) ((WORD>>SBITPOS)&15)
// Read 4 bits starting from SBITPOS.
// The result contains the selected nibble value.


/* READ A BYTE */

#define RBYTE(WORD,SBITPOS) ((WORD>>SBITPOS)&255)
// Read 8 bits starting from SBITPOS.
// The result contains the selected byte value.


/* COPY A BIT BETWEEN TWO REGISTERS */

#define READWRITERBIT2(DWORD,DBIT,SWORD,SBIT) \
        DWORD = (DWORD & ~(1<<DBIT)) | (((SWORD>>SBIT)&1)<<DBIT)
// Read one bit from SWORD and copy it to the selected bit position in DWORD.
// The destination bit is cleared before the new value is written.


/* COPY A BIT WITHIN THE SAME REGISTER */

#define READWRITERBIT(DWORD,DBIT,SBIT) \
        DWORD = (DWORD & ~(1<<DBIT)) | (((DWORD>>SBIT)&1)<<DBIT)
// Read one bit from SBIT and copy its value to DBIT
// within the same register.


/* TOGGLE A BIT */

#define CPLBIT(WORD,BITPOS) (WORD ^= 1<<BITPOS)
// Toggle the selected bit.
// If the bit is 0, it becomes 1.
// If the bit is 1, it becomes 0.


#endif
