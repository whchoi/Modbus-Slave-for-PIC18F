/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* TODO Change all these values for your project */

/* Microcontroller MIPs (FCY) */
#define p18f8722         //only other option is p18f8722 atm..
#define SYS_FREQ         10000000L
#define FCY              SYS_FREQ/4
#define Timer0High       0xF5; // for 8MHz 0xF7
#define Timer0Low        0xD7; // for 8MHz 0xDF
#define BAUD_RATE        9600;
#define SlaveAddress     1 // Change this to whatever you want..

//CHANGE THESE FOR EACH PROCESSOR
#ifdef p18f4520
  #define busyUsart        BusyUSART() //For 18F8722 this is Busy1USART()
  #define TransmitBuffer   TXREG //For 18F8722 this is TXREG1
  #define ReceiveBuffer    RCREG //For 18F8722 this is RXREG1
  #define ReceiveFlag1     PIR1bits.RCIF
  #define TransmitFlag1    PIR1bits.TXIF
  #define Timer0Flag       INTCONbits.TMR0IF
#endif
#ifdef p18f8722
  #define busyUsart        Busy1USART() //For 18F8722 this is Busy1USART()
  #define TransmitBuffer   TXREG1 //For 18F8722 this is TXREG1
  #define ReceiveBuffer    RCREG1 //For 18F8722 this is RXREG1
  #define ReceiveFlag1     PIR1bits.RCIF
  #define TransmitFlag1    PIR1bits.TXIF
  #define Timer0Flag       INTCONbits.TMR0IF
#endif
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

void ConfigInterrupts(void);

void isr(void);

void ClsUSART(void);
void OpnUSART(void);

void OpenTmr0(void);

