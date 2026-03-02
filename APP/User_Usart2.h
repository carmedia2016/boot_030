/*
* Copyright (c) 2018,Company
* All rights reserved.
*
* File Name : 
* Description :
*
* Version&Data :V1.1 2018/01/09
* Author : Vichan 
*
* Eidt : initial version
* Version&Data :V1.0 2018/01/03
* Author : Vichan 
* Eidt : initial version
*/

#ifndef __USERUSART2_H
#define __USERUSART2_H

#ifdef __cplusplus
 extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#include "stm32f0xx_hal.h"
#include "User_Flash.h"


#define FrameOrderAddr          3
#define DevIDBaseAddr           4
#define MsgLenAddr              9
#define MsgComAddr              10
#define SubComAddr              11
#define MsgFlashBaseAddr        12
#define MsgFlashDataBaseAddr    16    

#define Usart2ReceBuffSize      512    
#define Usart2SendBuffSize      512    

#define IAPFLAGADDR             0X0800F400  
extern uint32_t IAPTimeOut;
   
typedef struct  
{
    uint8_t  UsartReceProtocolTyte;
    uint8_t  UsartReceFrameHead;                                    //串口接收帧头 
    uint8_t  UsartReceFrameLen;                                     //串口帧接收长度
    uint8_t  UsartReceFrameLenCheck;                                //串口帧接收长度校验
    uint8_t  UsartReceFrameOrder;                                   //串口接收帧序号
    uint8_t  UsartReceMsgLen;                                       //串口接收消息体长
    uint8_t  UsartReceMsgCom;                                       //串口接收消息体命令
    uint8_t  UsartReceMsgSubCom;                                    //子命令码1 是握手  2  是起始  3 写flash
    uint32_t UsartReceMsgFlashAddr;                                 //串口接收消息体Flash地址
    uint8_t  UsartReceFrameCheckData;                               //串口接收校验
    uint8_t  UsartReceFrameEnd;                                     //串口接收帧尾
    uint8_t  UsartReceDevID[5];
    uint16_t UsartFlashDataBuff[256];
    uint16_t UsartFlashDataCount;
    uint16_t UsartIAPReturnCode;
    uint8_t  UsartIAPOK;
    uint8_t  UsartIAPRST;
	  uint32_t UsartResetCount;
	  
    
    uint8_t  UsartReceByte;                                         //
    uint8_t  UsartReceBuff[Usart2ReceBuffSize];                     //
    uint8_t  UsartReceCount;                                        //
    uint8_t  UsartReceFrameState;                                   //
    uint8_t  UsartFrameTimeCount;                                   //
    uint8_t  UsartReceFrameOK;                                      //

    uint8_t  UsartSendFrameTimeCount;                               //
    uint8_t  UsartSendBuff[Usart2SendBuffSize];                     //
    uint8_t  UsartSendFrameState;                                   //
    uint8_t  UsartSendFrameOK;                                      //
    uint8_t  UsartSendSize;                                         //
    uint8_t  UsartSendCount;                                        //
} UsartFrameTypeDef;    
extern UsartFrameTypeDef Usart2Frame;  

void Init_UserUsart2(void);
void UsartReceDataHandler(UsartFrameTypeDef *UsartFrameStruct);
void UsartSendDataHandler(UsartFrameTypeDef *UsartFrameStruct);
uint8_t UsartSendFrame(UsartFrameTypeDef *UsartFrameStruct,uint8_t *pWdat,uint8_t WSize,uint8_t AutoEnd);
uint8_t Usart2ReceFrameDataParser(void);
void UsartReceFrameTimeControl(UsartFrameTypeDef *UsartFrameStruct,uint8_t TimeOut,uint8_t IdleTimeOut);
void UsartSendFrameTimeControl(UsartFrameTypeDef *UsartFrameStruct,uint8_t TimeOut);

#endif 




