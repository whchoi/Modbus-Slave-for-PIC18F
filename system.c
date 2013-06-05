/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "system.h"

//PLEASE NOTE THESE FUNCTIONS ONLY WORK FOR 18F4520
void OpnUSART(void)
{
/******************************************************************************/
/* This just sets up the relevant parameters for the UART                     */
/******************************************************************************/
  //-----SPBRG needs to be changed depending upon oscillator frequency-------
  unsigned long spbrg = 0;

  spbrg = SYS_FREQ/BAUD_RATE; // define these in system.h
  spbrg /= 64;
  spbrg -= 1;
  ClsUSART();    //Incase USART already opened
  #ifdef p18f4520
      TXSTA = 0;           // Reset USART registers to POR state
      RCSTA = 0;

      TRISCbits.TRISC6 = 0; // Sets transmit pin as output
      TRISCbits.TRISC7 = 1; // Sets recieve pin as input

      TXSTAbits.SYNC = 0;  // Async mode
      TXSTAbits.TX9 = 0;   // 8-bit mode
      TXSTAbits.SENDB = 0; // Sync break option (not sure)
      TXSTAbits.BRGH = 0;  // Low speed

      RCSTAbits.CREN = 1;  //Continuous receive
      RCSTAbits.RX9 = 0;   //8-bit reception

      PIE1bits.RCIE = 1;   // Interrupt on receive
      PIE1bits.TXIE = 0;   // Interrupt on transmit

      BAUDCONbits.BRG16 = 0; // 8-bit Baud register
      SPBRG = spbrg;         // Write baudrate to SPBRG1

      TXSTAbits.TXEN = 1;  // Enable transmitter
      RCSTAbits.SPEN = 1;  // Enable receiver
  #endif
  #ifdef p18f8722
      TXSTA1 = 0;           // Reset USART registers to POR state
      RCSTA1 = 0;

      TRISCbits.TRISC6 = 0; // Sets transmit pin as output
      TRISCbits.TRISC7 = 1; // Sets recieve pin as input

      TXSTA1bits.SYNC = 0;  // Async mode
      TXSTA1bits.TX9 = 0;   // 8-bit mode
      TXSTA1bits.SENDB = 0; // Sync break option (not sure)
      TXSTA1bits.BRGH = 0;  // Low speed

      RCSTA1bits.CREN = 1;  //Continuous receive
      RCSTA1bits.RX9 = 0;   //8-bit reception

      PIE1bits.RCIE = 1;   // Interrupt on receive
      PIE1bits.TXIE = 0;   // Interrupt on transmit

      BAUDCON1bits.BRG16 = 0; // 8-bit Baud register
      SPBRG = spbrg;         // Write baudrate to SPBRG1

      TXSTA1bits.TXEN = 1;  // Enable transmitter
      RCSTA1bits.SPEN = 1;  // Enable receiver
  #endif
}

void ClsUSART(void)
{
  #ifdef p18f4520
    RCSTA &= 0b01001111;  // Disable the receiver
    TXSTAbits.TXEN = 0;   //  and transmitter
    PIE1 &= 0b11001111;   // Disable both interrupts
  #endif
  #ifdef p18f8722
    RCSTA1 &= 0b01001111;  // Disable the receiver
    TXSTA1bits.TXEN = 0;   //  and transmitter
    PIE1 &= 0b11001111;   // Disable both interrupts
  #endif
}

void OpenTmr0(void)
{
    T0CONbits.TMR0ON = 0;   // timer off
    T0CONbits.T08BIT = 0;   // 16-bit operation
    T0CONbits.T0CS = 0;     //Internal clock
    T0CONbits.PSA = 1;      //No prescaler 1:1

    TMR0H = Timer0High;  // Write high byte to Timer0
    TMR0L = Timer0Low;  // Write low byte to Timer0

    T0CONbits.TMR0ON = 1;

    INTCONbits.TMR0IF = 0;  // Clear Timer0 overflow flag
    INTCONbits.TMR0IE = 1;  // Enable Timer0 overflow interrupt

    //WriteTimer0(62935); // Setup for 1.04ms overflow.. 9600baud = 1.04mS/char
    //To work this out - FOSC/4 = TOSC, (1/TOSC)*(65535) = time to overflow
}

void ConfigInterrupts(void)
{
    RCONbits.IPEN   = 0; // disable interrupt priority
    INTCONbits.GIE  = 1; // enable interrupts
    INTCONbits.PEIE = 1; // enable peripheral interrupts
}

