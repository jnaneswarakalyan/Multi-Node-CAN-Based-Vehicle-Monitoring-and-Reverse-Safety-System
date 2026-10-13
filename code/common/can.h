// CAN Controller 1 function declarations and CAN frame structure.

#ifndef _CAN_H_
#define _CAN_H_                    // Define header guard to prevent multiple inclusion

#include "types.h"            // Project-specific data type definitions


// Structure used to store a CAN message.
struct CAN_Frame
{
        u32 ID;                    // Stores the CAN message identifier

        // CAN frame control information.
        struct BitField
        {
                u8 RTR : 1;        // 0 = Data Frame, 1 = Remote Frame
                u8 DLC : 4;        // Number of data bytes in the CAN frame
        }vbf;

        u32 Data1, Data2;          // Stores the 8 CAN data bytes
};


// Initializes CAN Controller 1.
void Init_CAN1(void);


// Transmits a CAN frame through CAN1.
void CAN1_Tx(struct CAN_Frame);


// Receives a CAN frame through CAN1.
void CAN1_Rx(struct CAN_Frame *);


// End of header guard / conditional compilation.
#endif
