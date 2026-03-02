#include "MTP.h"
void MTPWrite(unsigned char ID, unsigned char WrAddr, unsigned char Length, unsigned char  *WrBuf)
{
	unsigned char i;
	for(i=0; i<Length; i++)
	{
		I2CWrite(ID, WrAddr,1, WrBuf);	
		WrAddr++;
		WrBuf++;
	}	
}



unsigned char MTPRead(unsigned char ID, unsigned char RdAddr, unsigned char Length, unsigned char  *RdBuf)
{
	unsigned char result;
	result = I2CRead(ID, RdAddr, Length, RdBuf);
	if(!result)
	{
		result = I2CRead(ID, RdAddr, Length, RdBuf);
	}
	
	return result;
}



/*******************************************************************************
Function:ResetAFE() 
Description:  Reset SH367309 IC, Send Data:0xEA, 0xC0, 0xA5
Input:	 NULL
Output: NULL
Others:
*******************************************************************************/
void ResetAFE(void)//注意和CRC的不一样
{
	unsigned char  WrBuf[2];	
	WrBuf[0] = 0xC0;
	WrBuf[1] = 0xA5;
	I2CWrite(MTP_ID, 0xEA, 1, WrBuf);
}

