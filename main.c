/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "modbus.h"
#include <usart.h>

/******************************************************************************/
/* Modbus Global Variable Declaration                                         */
/******************************************************************************/
volatile unsigned int  holdingReg[50] = {0};
volatile unsigned char coils[50]= {0};
extern volatile char modbusMessage;

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

//SAM NOT RECEIVING ANYTHING FROM TONYS GADGET..

void main(void)
{
  OpnUSART();
  ConfigInterrupts();

  while(1){
    if(modbusMessage){
      decodeIt();}

      //Start code here... below is just an example
    }

}

