/*
* Copyright (c) 2018,Company
* All rights reserved.
*
* File Name : 
* Description : 
*
* Version&Data :V1.0 2018/01/02
* Author : Vichan 
* Eidt : initial version
*/

#ifndef __USER_IAP_H
#define __USER_IAP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f0xx_hal.h"
#include "User_Flash.h"
#include "User_Usart2.h"

#define ApplicationAddress      0x08003000      
  

void JumpToAPP(void);
uint8_t JudgeIAPFlag(void);
void IAPProrame(void);
void ClearIAPFlag(void);
void SetIAPFlag(void);

#endif
