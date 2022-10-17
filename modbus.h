/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
unsigned char checkCRC(void);
void clearResponse(void);
void readReg(void);         /* I/O and Peripheral Initialization */
void readInputReg(void);
void writeReg(void);
void readCoil(void);
void readInputCoil(void);
void writeCoil(void);
void writeMultiCoil(void);
void modbusDelay(void);              /* Writes to Timer0 for 1.04ms delay*/
void decodeIt(void);
unsigned int generateCRC(unsigned char);
