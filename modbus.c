/******************************************************************************/
/* Files to Include                                                           */
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

#include "modbus.h"
#include <usart.h>
#include "system.h"

/******************************************************************************/
/* Macro                                                                      */
/******************************************************************************/
#define MB_EXC_FUNC			  0x01
#define MB_EXC_ADDR_RANGE	0x02
#define MB_EXC_REG_QTY	  0x03
#define MB_EXC_EXECUTE		0x04

/******************************************************************************/
/* Global variables                                                           */
/******************************************************************************/
extern volatile unsigned int  holdingReg[50];
extern volatile unsigned char coils[50];
extern volatile unsigned char response[125]; //Enough to return all holding-r's
extern volatile unsigned char received[125]; //Enough to write all holding-r's
extern volatile char modbusMessage,messageLength;
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/
void modbusDelay(void)
{
  /* Writes to Timer0 for 1.04ms delay*/
  TMR0H = Timer0High;
  TMR0L = Timer0Low;
}

void clearResponse(void)
{
    unsigned char i;
    for(i=0;i<125;i++){ //response is 125 long
      response[i] = 0;
    }
}

void decodeIt(void)
{
  if(received[0] == SlaveAddress){
    if(checkCRC()){
      if(received[1] == 0x01){
        readCoil();
      }
      else if(received[1] == 0x02){
        readInputCoil();
      }
      else if(received[1] == 0x03){
        readReg();
      }
      else if(received[1] == 0x04){
        readInputReg();
      }
      else if(received[1] == 0x05){
        writeCoil();
      }	  
      else if(received[1] == 0x06){
        writeReg();
      }
      else if(received[1] == 0x0F){
        writeMultiCoil();
      }
      else{
        response[0] = 0; //error this does nothing though..
      }
    }
  }
  modbusMessage = 0;
}

void readReg(void)
{
  unsigned int rr_Address = 0;
  unsigned int rr_numRegs = 0;
  unsigned char j = 3;
  unsigned int crc = 0;
  unsigned int i = 0;

  //Combine address bytes
  rr_Address = received[2];
  rr_Address <<= 8;
  rr_Address |= received[3];

  //Combine number of regs bytes
  rr_numRegs = received[4];
  rr_numRegs <<= 8;
  rr_numRegs |= received[5];

  response[0] = SlaveAddress;
  response[1] = 0x03;
  response[2] = rr_numRegs*2; //2 bytes per reg

  for(i=rr_Address;i<(rr_Address + rr_numRegs);i++){
    if(holdingReg[i] > 255){
      //Need to split it up into 2 bytes
      response[j] = holdingReg[i] >> 8;
      j++;
      response[j] = holdingReg[i];
      j++;
    }
    else{
      response[j] = 0x00;
      j++;
      response[j] = holdingReg[i];
      j++;
    }
  }
  crc = generateCRC(j+2);
  response[j] = crc >> 8;
  j++;
  response[j] = crc;
  j+=2;

  writeEnable = 1;
  for(i=0;i!=j;i++){
   while(busyUsart); //Change this to Busy1USART for double USART PIC's
     TransmitBuffer = response[i];
  }
  writeEnable = 0;
  j=0;

  clearResponse();
}

void readInputReg(void)
{
  unsigned int rr_Address = 0;
  unsigned int rr_numRegs = 0;
  unsigned char j = 3;
  unsigned int crc = 0;
  unsigned int i = 0;

  //Combine address bytes
  rr_Address = received[2];
  rr_Address <<= 8;
  rr_Address |= received[3];

  //Combine number of regs bytes
  rr_numRegs = received[4];
  rr_numRegs <<= 8;
  rr_numRegs |= received[5];

  response[0] = SlaveAddress;
  response[1] = 0x04;
  response[2] = rr_numRegs*2; //2 bytes per reg

  for(i=rr_Address;i<(rr_Address + rr_numRegs);i++){
    if(holdingReg[i] > 255){
      //Need to split it up into 2 bytes
      response[j] = holdingReg[i] >> 8;
      j++;
      response[j] = holdingReg[i];
      j++;
    }
    else{
      response[j] = 0x00;
      j++;
      response[j] = holdingReg[i];
      j++;
    }
  }
  crc = generateCRC(j+2);
  response[j] = crc >> 8;
  j++;
  response[j] = crc;
  j+=2;

  writeEnable = 1;
  for(i=0;i!=j;i++){
   while(busyUsart); //Change this to Busy1USART for double USART PIC's
     TransmitBuffer = response[i];
  }
  writeEnable = 0;
  j=0;

  clearResponse();
}

void writeReg(void)
{
/******************************************************************************/
/* Works out which reg to write and then responds                             */
/******************************************************************************/
  unsigned int wr_AddressLow = 0;
  unsigned int wr_AddressHigh = 0;
  unsigned int wr_Address = 0;

  unsigned int wr_valToWrite = 0;
  unsigned int wr_valToWriteLow = 0;
  unsigned int wr_valToWriteHigh = 0;

  unsigned int crc = 0;
  unsigned int i = 0;

  //Combine address bytes
  wr_Address = received[2];
  wr_Address <<= 8;
  wr_Address |= received[3];

  wr_AddressLow = received[3]; //useful to store
  wr_AddressHigh = received[2];

  //Combine value to write regs
  wr_valToWrite = received[4];
  wr_valToWrite <<= 8;
  wr_valToWrite |= received[5];

  wr_valToWriteLow = received[5];
  wr_valToWriteHigh = received[4];

  holdingReg[wr_Address] = wr_valToWrite;

  response[0] = SlaveAddress;
  response[1] = 0x06;
  response[3] = wr_AddressLow; //2 bytes per reg
  response[2] = wr_AddressHigh;

//TO DO CHECK VALUE IS ACTUALLY WRITTEN//
  response[4] = wr_valToWriteHigh;
  response[5] = wr_valToWriteLow;

  crc = generateCRC(8);

  response[6] = crc >> 8;
  response[7] = crc;

  writeEnable = 1;
  for(i=0;i!=9;i++){
   while(busyUsart);//Change this to Busy1USART for double USART PIC's
     TransmitBuffer = response[i];
  }
  writeEnable = 0;
  clearResponse();
}

void readCoil(void)
{
/******************************************************************************/
/* Reads a coil and then responds                                             */
/******************************************************************************/
  unsigned int rc_Address = 0;
  unsigned int rc_numCoils = 0;
  unsigned int crc = 0;

  unsigned char howManyBytes = 0;
  unsigned char remainder = 0;
  unsigned char lsb = 0;
  unsigned char i,j,k,l = 0;

  //Combine address bytes
  rc_Address = received[2];
  rc_Address <<=8;
  rc_Address |= received[3];

  //Combine number of coils bytes
  rc_numCoils = received[4];
  rc_numCoils <<= 8;
  rc_numCoils |= received[5];

  response[0] = SlaveAddress;
  response[1] = 0x01;

  howManyBytes = rc_numCoils/8;
  remainder = rc_numCoils % 8;

  if(remainder){
    howManyBytes += 1;
  }
  response[2] = howManyBytes;

  l = rc_Address;
  k = 3; //start at response 3

  for(i=howManyBytes; i!=0; i--){
		if(i>1){
      for(j=0;j!=8;j++){
				if(coils[l]){
					lsb = 1;
				}
				else{
          lsb = 0;
				}
				response[k] ^= (lsb << j);
				l++;
	    }
			k++;
	  }
		else{
			for(j=0;j!=remainder;j++){
				if(coils[l]){
					lsb = 1;
				}
				else{
          lsb = 0;
				}
        response[k] ^= (lsb << j);
				l++;
			}
			k++;
		}
  }
  crc = generateCRC(k+2);

  response[k] = crc >> 8;
  response[k+1] = crc;

  writeEnable = 1;
  for(i=0;i!=(k+3);i++){
   while(busyUsart);//Change this to Busy1USART for double USART PIC's
     TransmitBuffer = response[i];
  }
  writeEnable = 0;
  clearResponse();
}

void readInputCoil(void)
{
/******************************************************************************/
/* Reads a coil and then responds                                             */
/******************************************************************************/
  unsigned int rc_Address = 0;
  unsigned int rc_numCoils = 0;
  unsigned int crc = 0;

  unsigned char howManyBytes = 0;
  unsigned char remainder = 0;
  unsigned char lsb = 0;
  unsigned char i,j,k,l = 0;

  //Combine address bytes
  rc_Address = received[2];
  rc_Address <<=8;
  rc_Address |= received[3];

  //Combine number of coils bytes
  rc_numCoils = received[4];
  rc_numCoils <<= 8;
  rc_numCoils |= received[5];

  response[0] = SlaveAddress;
  response[1] = 0x02;

  howManyBytes = rc_numCoils/8;
  remainder = rc_numCoils % 8;

  if(remainder){
    howManyBytes += 1;
  }
  response[2] = howManyBytes;

  l = rc_Address;
  k = 3; //start at response 3

  for(i=howManyBytes; i!=0; i--){
		if(i>1){
      for(j=0;j!=8;j++){
				if(coils[l]){
					lsb = 1;
				}
				else{
          lsb = 0;
				}
				response[k] ^= (lsb << j);
				l++;
	    }
			k++;
	  }
		else{
			for(j=0;j!=remainder;j++){
				if(coils[l]){
					lsb = 1;
				}
				else{
          lsb = 0;
				}
        response[k] ^= (lsb << j);
				l++;
			}
			k++;
		}
  }
  crc = generateCRC(k+2);

  response[k] = crc >> 8;
  response[k+1] = crc;

  writeEnable = 1;
  for(i=0;i!=(k+3);i++){
   while(busyUsart);//Change this to Busy1USART for double USART PIC's
     TransmitBuffer = response[i];
  }
  writeEnable = 0;
  clearResponse();
}

void writeCoil(void)
{
/******************************************************************************/
/* Writes to a coil and then responds                                         */
/******************************************************************************/
  unsigned int wc_AddressLow = 0;
  unsigned int wc_AddressHigh = 0;
  unsigned int wc_Address = 0;

  unsigned int wc_valToWrite = 0;
  unsigned int wc_valToWriteLow = 0;
  unsigned int wc_valToWriteHigh = 0;
  int i = 0;
  unsigned int crc = 0;

  //Combine address bytes
  wc_Address = received[2];
  wc_Address <<= 8;
  wc_Address |= received[3];

  wc_AddressLow = received[3]; //useful to store
  wc_AddressHigh = received[2];

  //Combine value to write regs
  wc_valToWrite = received[4];
  wc_valToWrite <<= 8;
  wc_valToWrite |= received[5];

  wc_valToWriteLow = received[5];
  wc_valToWriteHigh = received[4];

  if(wc_valToWrite){
    coils[wc_Address] = 0xFF;
  }
  else{
    coils[wc_Address] = 0x00;
  }

  response[0] = SlaveAddress;
  response[1] = 0x02;
  response[3] = wc_AddressLow; //2 bytes per reg
  response[2] = wc_AddressHigh;

//TO DO CHECK VALUE IS ACTUALLY WRITTEN//
  response[4] = wc_valToWriteHigh;
  response[5] = wc_valToWriteLow;

  crc = generateCRC(8);

  response[6] = crc >> 8;
  response[7] = crc;

  writeEnable = 1;
  for(i=0;i!=9;i++){
   while(busyUsart);//Change this to Busy1USART for double USART PIC's
     TransmitBuffer = response[i];
  }
  writeEnable = 0;
  clearResponse();
}

void writeMultiCoil(void)
{
  unsigned short addr;
  unsigned short ncoil;
  // unsigned char ncoil;
  unsigned char nbyte;
  unsigned char err;
  unsigned char len;
  unsigned char i;
  unsigned char *data;
  unsigned char ibyte;
  unsigned char ibit;
  unsigned int crc = 0;

  // get start address
  addr = received[2];
  addr = (addr << 8) | received[3];

  // Get number of coil
  ncoil = received[4];
  ncoil = (ncoil << 8) | received[5];

  nbyte = ncoil / 8;
  if (ncoil % 8)
  {
    nbyte += 1;
  }

  /**
   * +----+----+------+-----+-----+------+-----+
   * | ID | FC | ADDR | QTY | CNT | DATA | CRC |
   * +----+----+------+-----+-----+------+-----+
   * | 1  | 1  |   2  |  2  |  1  |   N  |  2  |
   * +----+----+------+-----+-----+------+-----+
   * len_min = 10
   */
  err = 0;
  if ((ncoil < 1) || (ncoil >= sizeof(coils)) || (nbyte != received[6]))
  {
    err = MB_EXC_REG_QTY;
  }
  else if (addr >= sizeof(coils) || (addr + ncoil) >= sizeof(coils))
  {
    err = MB_EXC_ADDR_RANGE;
  }
  else if ((messageLength < 10) || ((nbyte + 9) != messageLength))
  {
    err = MB_EXC_EXECUTE;
  }

  response[0] = SlaveAddress;
  if (err)  // Receive message error
  {
    // build exc
    /**
     * Response
     * +----+----+----------+-----+
     * | ID | FC | EXC_CODE | CRC |
     * +----+----+----------+-----+
     * | 1  | 1  |   1      |  2  |
     * +----+----+----------+-----+
     * len = 5
     */
    response[1] = 0x0F + 0x80;
    response[2] = err;
    len = 5;
  }
  else  // Received message success
  {
    // Set coil
    data = &received[7];
    for (i = 0; i < ncoil; i++)
    {
      ibyte = i / 8;
      ibit = i % 8;
      coils[addr + i] = (data[ibyte] >> ibit) & 0x01;
    }

    // response data
    /**
     * Response
     * +----+----+------+-----+-----+
     * | ID | FC | ADDR | QTY | CRC |
     * +----+----+------+-----+-----+
     * | 1  | 1  |   2  |  2  |  2  |
     * +----+----+------+-----+-----+
     * len_min = 8
     */
    response[0] = received[0];
    received[1] = received[1];
    received[2] = received[2];
    received[3] = received[3];
    received[4] = received[4];
    received[5] = received[5];
    len = 8;
  }

  crc = generateCRC(len);
  response(len - 2) = (unsigned char)(crc >> 8);
  response(len - 1) = (unsigned char)crc;

  writeEnable = 1;
  for (i = 0; i != len; i++)
  {
    while (busyUsart); // Change this to Busy1USART for double USART PIC's
    TransmitBuffer = response[i];
  }
  writeEnable = 0;
}

unsigned int generateCRC(unsigned char messageLength)
{
unsigned int crc = 0xFFFF;
unsigned int crcHigh = 0;
unsigned int crcLow = 0;
int i,j = 0;

  for(i=0;i<messageLength-2;i++){
    crc ^= response[i];
    for(j=8; j!=0; j--){
      if((crc & 0x0001) != 0){
        crc >>= 1;
        crc ^= 0xA001;
      }
      else{
        crc >>= 1;
      }
    }
  }
//bytes are wrong way round so doing a swap here..
crcHigh = (crc & 0x00FF) <<8;
crcLow = (crc & 0xFF00) >>8;
crcHigh |= crcLow;
crc = crcHigh;
return crc;
}

unsigned char checkCRC(void)
{
  unsigned int crc = 0xFFFF;
  unsigned int crcHigh = 0;
  unsigned int crcLow = 0;
  int i,j = 0;

    for(i=0;i<messageLength-2;i++){
      crc ^= received[i];
      for(j=8; j!=0; j--){
        if((crc & 0x0001) != 0){
          crc >>= 1;
          crc ^= 0xA001;
        }
        else{
          crc >>= 1;
        }
      }
    }
  //bytes are wrong way round so doing a swap here..
  crcHigh = (crc & 0x00FF);
  crcLow = (crc & 0xFF00) >>8;
  if((crcHigh == received[i])&&(crcLow == received[i+1]))
  {
    return 1;
  }
  else{
    return 0;
  }


}