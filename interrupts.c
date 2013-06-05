/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#include <p18cxxx.h>    /* C18 General Include File */
#include "user.h"
#include "modbus.h"
#include "system.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
volatile char endOfMessage,newMessage = 1;
volatile char timerCount,messageLength,modbusMessage,z = 0;
volatile unsigned char response[125]; //Enough to return all holding-r's
volatile unsigned char received[125]; //Enough to write all holding-r's 

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

// start ISR code
#pragma code isr = 0x08 // store the below code at address 0x08
#pragma interrupt isr   // let the compiler know that the function isr() is int

void isr(void)
{
  if(ReceiveFlag1){ // USART receive interrupt flag has been set
    if((!endOfMessage)&&(!newMessage)){
      if(TransmitFlag1){     // check if the TXREG is empty
        received[z] = ReceiveBuffer;
        z++;
        timerCount = 0;
      }
    }
    if(newMessage){
      OpenTmr0();
      if(TransmitFlag1){     // check if the TXREG is empty
        received[z] = ReceiveBuffer;
        z++;
        newMessage = 0;
        endOfMessage = 0;
        messageLength = 0;
        modbusMessage = 0;
        timerCount = 0;
        return;
      }
    }
  }
  else if(Timer0Flag){ //TMR0 interrupt flag
    modbusDelay();  //Resets timer for 1.04ms
    timerCount++;
    if(timerCount > 4){
      endOfMessage = 1;
      newMessage = 1;
      messageLength = z;
      modbusMessage = 1;
      for(z=(messageLength);z!=125;z++){ //Clear rest of message
        received[z] = 0;
      }
      z=0;
      T0CONbits.TMR0ON = 0; //Close timer0
      timerCount = 0;
    }
   Timer0Flag = 0;  // Clear flag
  }
}
#pragma code // return to the default code section
