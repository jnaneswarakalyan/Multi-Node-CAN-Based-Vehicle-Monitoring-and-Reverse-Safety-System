// CAN Controller 1 driver for the LPC2129.
// Handles CAN1 initialization, transmission and reception.

#include <LPC21xx.h>
#include "types.h"
#include "can.h"
#include "can_defines.h"


/****************************************
        CAN1 INITIALIZATION
****************************************/

// Initializes CAN Controller 1 and configure
void Init_CAN1(void)
{
    // Configure P0.25 as the CAN1 receive pin (RD1).
    // RD1_PIN contains the required PINSEL1 configuration.
    PINSEL1 |= RD1_PIN;
  // Put CAN1 into reset/configuration mode.
    // C1MOD = 1 means CAN1 is temporarily disabled
    // so that its configuration can be changed.
    C1MOD = 1;
  // Accept all received CAN messages.
    // AFMR = 2 configures the acceptance filter
    // so that received CAN messages are accepted.
    AFMR = 2;
  // Configure the CAN1 bit timing and baud rate.
    // BTR_LVAL is defined in can_defines_major.h
    // and contains the required CAN timing configuration.
    C1BTR = BTR_LVAL;
  // Leave reset/configuration mode and enable CAN1.
    C1MOD = 0;
}



/****************************************
        CAN1 TRANSMISSION
****************************************/

// Transmits one CAN frame through CAN1.
// txFrame contains:
// ID    -> CAN message identifier
// RTR   -> Data frame or Remote frame
// DLC   -> Number of data bytes
// Data1 -> CAN data bytes 1-4
// Data2 -> CAN data bytes 5-8
void CAN1_Tx(struct CAN_Frame txFrame)
{
    // Wait until CAN1 Transmit Buffer 1 becomes available.
    // TBS1_BIT_READ indicates whether Transmit Buffer 1
    // is ready to accept a new CAN message.
    while((C1GSR & TBS1_BIT_READ) == 0);
  // Copy the CAN message identifier into
    // the CAN1 Transmit Identifier register.
    C1TID1 = txFrame.ID;


    // Configure the CAN frame:
    // RTR -> selects Data Frame or Remote Frame.
    // DLC -> specifies the number of data bytes
    // RTR is placed at bits 30 and 31.
    // DLC is placed at bits 16 to 19.
    C1TFI1 =
            (txFrame.vbf.RTR << 30) |
            (txFrame.vbf.DLC << 16);


    // Check whether this is a Data Frame.
    // RTR = 0 -> Data Frame
    // RTR = 1 -> Remote Frame
    if(txFrame.vbf.RTR != 1)
    {
        // Store CAN data bytes 1-4
        // in the first transmit data register.
        C1TDA1 = txFrame.Data1;
      // Store CAN data bytes 5-8
        // in the second transmit data register.
        C1TDB1 = txFrame.Data2;
    }
  // Select Transmit Buffer 1 and start CAN transmission.
    // STB1_BIT_SET -> select Transmit Buffer 1.
    // TR_BIT_SET   -> request transmission.
    C1CMR = STB1_BIT_SET | TR_BIT_SET;
  // Wait until CAN1 reports that the transmission
    // has completed successfully.
    while((C1GSR & TCS1_BIT_READ) == 0);
}

/****************************************
        CAN1 RECEPTION
****************************************/

// Receives one CAN frame through CAN1.
// rxFrame is a pointer to a CAN_Frame structure.
// The received CAN information is written directly
// into the structure supplied by the caller.
void CAN1_Rx(struct CAN_Frame *rxFrame)
{
    // Wait until CAN1 has received a CAN message.
    // RBS_BIT_READ indicates that the Receive Buffer
    // contains a received message.
    while((C1GSR & RBS_BIT_READ) == 0);


    // Read the 11-bit CAN identifier from the
    // CAN1 Receive Identifier register.
    // The received ID is stored in rxFrame->ID.
    rxFrame->ID = C1RID;


    // Extract the RTR bit from the received frame.
    // Bit 30 of C1RFS indicates whether the received
    // message is a Remote Frame or Data Frame.
    // RTR = 0 -> Data Frame
    // RTR = 1 -> Remote Frame
    rxFrame->vbf.RTR = (C1RFS >> 30) & 1;


    // Extract the Data Length Code (DLC).
    // DLC tells how many data bytes are present
    // in the received CAN message.
    rxFrame->vbf.DLC = (C1RFS >> 16) & 0x0F;
 // Only read the data registers when the received
    // message is a Data Frame.
    if(rxFrame->vbf.RTR == 0)
    {
        // Read received CAN data bytes 1-4.
        rxFrame->Data1 = C1RDA;
      // Read received CAN data bytes 5-8.
        rxFrame->Data2 = C1RDB;
    }
  // Release the CAN receive buffer.
    // RRB_BIT_SET tells CAN1 that the received message
    // has been read and the receive buffer can be reused.
    C1CMR = RRB_BIT_SET;
}
