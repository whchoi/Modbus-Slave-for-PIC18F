Modbus Slave project for PIC18F Microcontrollers
===============================================================


This project contains the code to get ANY PIC18F series Microcontroller
with a working USART to act as a Modbus Slave device.

The project is ready to go for either the PIC18F8722 or the PIC18F4520
but with some simple changes can work with any other device.

IF USING 18F4520 or 18F8722
-----------------------------------------------------------------------
  To get started make the relevant changes to "System.h"
  All you need to do is define your processor, frequency, baudrate etc
  in here and you are ready to go.

  From here you can start coding away in "main.c" and start adding your
  own functions to "user.c".. all you need to do is equate a holdinReg
  or a coil to something.

  For example

    void main(void)
    {
      OpnUSART();
      ConfigInterrupts();

      while(1){
        if(modbusMessage){
          decodeIt();}

          //Start code here...

          TRISD,TRISC = 0;
          LATD = holdingReg[0];
          LATCbits.LATC1 = coils[0];
        }
    }

  This code would output whatever is in holdingReg[0] to LATD and whatever
  is in coils[0] to LATC1.. it is that easy..

  Obviously you can take it further than this and start making cool functions
  but that is up to you.. all of the communication code is taken care for you.

IF USING ANOTHER PROCESSOR
---------------------------------------------------------------------------------
  There are some things you are going to need to change.. as you can see in
  system.h at the bottom there are some #ifdef's defined for the 2 choices
  of processor.

  You will need to make your own ifdef for your processor.. containing the
  relevant configurations and options.

  There are 2 more places that the ifdefs will be needed to be changed and that
  is in all of the functions inside "system.c" and in "configurationbits.c"

  If you look inside the ifdefs in these 2 files it should be clear what they
  do and what you will need to change to get it working on your processor..

  So for example if I wanted to get this going on an PIC18F4550 I would first
  go to system.h and define it at the top as p18f4550..

  Then I would go to the bottom of system.h and make an ifdef use case for the
  p18f4550, leaving the other 2 ifdefs intact..

  I would then go into system.c and add my own ifdef to all the functions and
  finally create an ifdef in the configuration bits file.. that is all.

  Thanks and enjoy :)

