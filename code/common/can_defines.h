// CAN1 pin, bit timing, control and register bit definitions.
// Used by the CAN driver for LPC2129 CAN Controller 1.

// CAN1 RX PIN CONFIGURATION

#define RD1_PIN 0x00040000       // Configure P0.25 as CAN1 receive pin (RD1)


// CAN1 BIT TIMING CONFIGURATION

#define PCLK       60000000      // Peripheral clock frequency = 60 MHz
#define BIT_RATE   125000        // CAN communication bit rate = 125 kbps
#define QUANTA     16            // Number of time quanta in one CAN bit

#define BRP        (PCLK/(BIT_RATE*QUANTA))
// Calculate the Baud Rate Prescaler for the CAN bit timing.

#define SAMPLE_POINT  (0.7 * QUANTA)
// Set the CAN sample point at approximately 70% of the CAN bit.

#define TSEG1        ((int)SAMPLE_POINT-1)
// TSEG1 = Propagation Segment + Phase Segment 1.

#define TSEG2        (QUANTA-(1+TSEG1))
// TSEG2 = Phase Segment 2.

#define SJW          ((TSEG2 >= 5) ? 4 : (TSEG2-1))
// Set the Synchronization Jump Width used for CAN resynchronization.

#define SAM          0
// SAM = 0 -> Sample the CAN bus once per bit.
// SAM = 1 -> Sample the CAN bus three times per bit.

#define BTR_LVAL    (SAM<<23|(TSEG2-1)<<20|(TSEG1-1)<<16|(SJW-1)<<14|(BRP-1))
// Create the complete CAN Bit Timing Register value for C1BTR.


// C1CMR BIT DEFINITIONS
// Used to control CAN1 transmission and reception.

#define TR_BIT_SET   1<<0       // Request CAN transmission
#define RRB_BIT_SET  1<<2       // Release the CAN receive buffer
#define STB1_BIT_SET 1<<5       // Select CAN1 Transmit Buffer 1


// C1GSR STATUS BIT DEFINITIONS
// Used to check the current CAN1 controller status.

#define RBS_BIT_READ  1<<0      // Receive Buffer Status: received message available
#define TBS1_BIT_READ 1<<2      // Transmit Buffer 1 Status: buffer is available
#define TCS1_BIT_READ 1<<3      // Transmit Complete Status: transmission completed


// CxTFI1 / CxRFS BIT DEFINITIONS
// Used to access specific fields of the CAN frame information registers.

#define RTR_BIT  30             // Bit position of the Remote Transmission Request field
#define DLC_BITS 16             // Starting bit position of the Data Length Code field


// CxMOD BIT DEFINITIONS
// Used for CAN controller mode configuration.

#define RM_BIT  0               // Bit position of the CAN Reset Mode field


// AFMR BIT DEFINITIONS
// Used for CAN acceptance filter configuration.

#define ACCOFF_BIT 0            // Bit position for Acceptance Filter Off control
#define ACCBP_BIT  1            // Bit position for Acceptance Filter Bypass control
