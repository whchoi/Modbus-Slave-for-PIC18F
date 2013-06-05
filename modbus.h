/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void clearResponse(void);
void readReg(void);         /* I/O and Peripheral Initialization */
void writeReg(void);
void readCoil(void);
void writeCoil(void);
void modbusDelay(void);              /* Writes to Timer0 for 1.04ms delay*/
void decodeIt(void);
unsigned int generateCRC(unsigned char);
