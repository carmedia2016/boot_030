/*
* Copyright (c) 2018,Company
* All rights reserved.
*
* File Name : User_IAP.c
* Description : 
*
* Version&Data :V1.0 2018/01/02
* Author : Vichan 
* Eidt : initial version
*/

#include"User_IAP.h"

typedef  void (*pFunction)(void); 
pFunction JumpToApplication;
uint32_t JumpAddress;

void JumpToAPP(void)
{
    if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
    { 
      JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
      JumpToApplication = (pFunction) JumpAddress;
      __set_MSP(*(__IO uint32_t*) ApplicationAddress);
      JumpToApplication();
    }
}

uint8_t JudgeIAPFlag(void)
{
    uint16_t tmp;
    tmp = FlashReadHalfWord(IAPFLAGADDR);
    if(tmp==0X5555)             //ISP
    {
        return 0x01;
    }
    else                        //Jump to app
    {
        return 0x00;
    }
}

void ClearIAPFlag(void)
{
    FLASH_EraseInitType.TypeErase = 0X00;
    FLASH_EraseInitType.PageAddress = IAPFLAGADDR;
    FLASH_EraseInitType.NbPages = 1;
    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&FLASH_EraseInitType,&(UserFlashTypeDef.FlashEraseReturn));
    HAL_FLASH_Lock();    
}

void IAPProrame(void)
{
    Usart2ReceFrameDataParser();
    if( (Usart2Frame.UsartIAPRST==0x01)&&(Usart2Frame.UsartResetCount>=4000) )
    {
				Usart2Frame.UsartResetCount = 0;
        Usart2Frame.UsartIAPRST = 0;
        if(Usart2Frame.UsartIAPOK==0x01)
        {
            Usart2Frame.UsartIAPOK = 0;
            ClearIAPFlag();
            HAL_Delay(10);
        }
        NVIC_SystemReset();
        HAL_Delay(10);
    }
}

void SetIAPFlag(void)       //only for debug;
{
    Usart2Frame.UsartReceMsgFlashAddr = IAPFLAGADDR;
    Usart2Frame.UsartFlashDataCount = 1;
    Usart2Frame.UsartFlashDataBuff[0] = 0X5555;
    FlashWritenHalfWord(Usart2Frame.UsartReceMsgFlashAddr,Usart2Frame.UsartFlashDataCount,Usart2Frame.UsartFlashDataBuff);
}

