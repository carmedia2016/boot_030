/*
* Copyright (c) 2018,Company
* All rights reserved.
*
* File Name : 
* Description : 
*
*
* Version&Data :V1.0 2018/01/02
* Author : Vichan 
* Eidt : initial version
*/

#ifndef __USER_FLASH_H
#define __USER_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f0xx_hal.h"
     
#define STM32MainFlashBaseAddr          0X08000000
#define BootLoadSize                    (8*1024)

extern uint32_t PageSize;
extern uint32_t TotalPage;
typedef struct  
{
    uint32_t FlashEraseReturn;
    uint32_t FlashProgramReturn;
} User_FlashTypeDef;      

extern User_FlashTypeDef UserFlashTypeDef;
extern FLASH_EraseInitTypeDef FLASH_EraseInitType;
uint8_t FlashWritenHalfWord(uint32_t WriteAddr, uint8_t WriteHalfWordSize, uint16_t *pWDat);
uint16_t FlashReadHalfWord(uint32_t ReadAddr);
void Flash_Init(void);

#endif
