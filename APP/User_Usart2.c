/*
* Copyright (c) 2018,Company
* All rights reserved.
*
* File Name : 
* Description :

* Version&Data :V1.2 2018/01/24
* Author : Vichan 
* Eidt : prevent iap erase the iapflag addr(IAPFLAGADDR) except special com 
* add iap timeout function. if iap do not have rece valid com in limit time it will be rst
*
* Version&Data :V1.1 2018/01/09
* Author : Vichan 
* Eidt : initial version
*
* Version&Data :V1.0 2018/01/03
* Author : Vichan 
* Eidt : initial version
*/
#include "User_Usart2.h"
#include "gpio.h"
UsartFrameTypeDef Usart2Frame;
uint32_t IAPTimeOut;        //add 2018.1.24---if iap timeout,it will be rst
static void Usart2SendAckCom(void);

/******************************************************************************
* Function Name : Init_UserUsart2()
* Description   :
* Input         : 
* Output        :
* Return        :
* Remarks       : 
******************************************************************************/
void Init_UserUsart2(void)
{
    Usart2Frame.UsartReceProtocolTyte = 0X00;
    Usart2Frame.UsartReceFrameEnd = 0X16;
    Usart2Frame.UsartReceFrameHead = 0X68;
    USART2->CR1 |= 1<<6; 
    USART2->CR1 |= 1<<5;     
}

/******************************************************************************
* Function Name : UsartSendFrame()
* Description   :
* Input         : 
* Output        :
* Return        :
* Remarks       : 
******************************************************************************/
uint8_t UsartSendFrame(UsartFrameTypeDef *UsartFrameStruct,uint8_t *pWdat,uint8_t WSize,uint8_t AutoEnd)
{
    uint8_t i;
    if( (UsartFrameStruct->UsartSendFrameState==0x00)&&(UsartFrameStruct->UsartSendFrameOK==0) )
    {
        UsartFrameStruct->UsartSendFrameState = 0X01;
        UsartFrameStruct->UsartSendSize = WSize;
        for(i=0;i<UsartFrameStruct->UsartSendSize; i++)
        {
            if(AutoEnd==1)
            {
                if(pWdat[i]==0)
                {
                    break;
                }
            }
            UsartFrameStruct->UsartSendBuff[i] = pWdat[i];
        }
        if(AutoEnd==1)
        {
            UsartFrameStruct->UsartSendSize = i;
        }
    }
    if(UsartFrameStruct->UsartSendFrameState==0x01)
    {
        UsartFrameStruct->UsartSendCount = 1;
        UsartFrameStruct->UsartSendFrameOK = 0;
        UsartFrameStruct->UsartSendFrameState = 0x02;
        UsartFrameStruct->UsartSendFrameTimeCount = 0x00;
        USART2->TDR = UsartFrameStruct->UsartSendBuff[UsartFrameStruct->UsartSendCount-1];  //STM32F0_CUBE 
    }  
    return UsartFrameStruct->UsartSendFrameOK;
}

/******************************************************************************
* Function Name : Usart1SendFrameTimeControl()
* Description   : 
* Input         :
* Output        :
* Return        :
* Remarks       : 
******************************************************************************/
void UsartSendFrameTimeControl(UsartFrameTypeDef *UsartFrameStruct,uint8_t TimeOut)
{
    if( (UsartFrameStruct->UsartSendFrameState!=0)&&(UsartFrameStruct->UsartSendFrameOK==0) )
    {
        UsartFrameStruct->UsartSendFrameTimeCount ++;
    }
    else
    {
        UsartFrameStruct->UsartSendFrameTimeCount = 0;
    }
    if( UsartFrameStruct->UsartSendFrameTimeCount>=TimeOut )
    {
        UsartFrameStruct->UsartSendFrameTimeCount = 0;
        UsartFrameStruct->UsartSendFrameState = 0;
        UsartFrameStruct->UsartSendFrameOK = 2;
    }
}

/******************************************************************************
* Function Name : Usart1SendDataHandler()
* Description   : 
* Input         :
* Output        :
* Return        :
* Remarks       : 
******************************************************************************/
void UsartSendDataHandler(UsartFrameTypeDef *UsartFrameStruct)
{
    if(UsartFrameStruct->UsartSendFrameState==0x02)
    {
        UsartFrameStruct->UsartSendCount ++;
        USART2->TDR = UsartFrameStruct->UsartSendBuff[UsartFrameStruct->UsartSendCount-1];  //STM32F0_CUBE
        if(UsartFrameStruct->UsartSendCount>=UsartFrameStruct->UsartSendSize)
        {
            UsartFrameStruct->UsartSendCount = 0;
            UsartFrameStruct->UsartSendFrameState = 0;
            UsartFrameStruct->UsartSendFrameOK = 1;
        }
    }
}

/******************************************************************************
* Function Name : Usart1ReceFrameTimeControl()
* Description   : 
* Input         :
* Output        :
* Return        :
* Remarks       :
******************************************************************************/
void UsartReceFrameTimeControl(UsartFrameTypeDef *UsartFrameStruct,uint8_t TimeOut,uint8_t IdleTimeOut)
{
    if( (UsartFrameStruct->UsartReceFrameState!=0)&&(UsartFrameStruct->UsartReceFrameOK==0) )
    {
        UsartFrameStruct->UsartFrameTimeCount ++;
    }
    if( UsartFrameStruct->UsartFrameTimeCount>=TimeOut )
    {
        UsartFrameStruct->UsartFrameTimeCount = 0;
        UsartFrameStruct->UsartReceFrameState = 0;
        UsartFrameStruct->UsartReceFrameOK = 0;
    }
    if(IdleTimeOut!=0)
    {
        if(UsartFrameStruct->UsartFrameTimeCount>=IdleTimeOut)
        {
            UsartFrameStruct->UsartFrameTimeCount = 0;
            UsartFrameStruct->UsartReceFrameState = 0;
            UsartFrameStruct->UsartReceCount = 0;
            UsartFrameStruct->UsartReceFrameOK = 1;
        }
    }
}

/******************************************************************************
* Function Name : Usart1ReceDataHandler()
* Description   : 
* Input         :
* Output        :
* Return        :
* Remarks       : 
******************************************************************************/
void UsartReceDataHandler(UsartFrameTypeDef *UsartFrameStruct)
{
    uint8_t cksum;
    uint16_t i,tempvalue;
    UsartFrameStruct->UsartReceByte = USART2->RDR;                                      //STM32F0_CUBE
    UsartFrameStruct->UsartFrameTimeCount = 0;
    if(UsartFrameStruct->UsartReceProtocolTyte==0)                                      //Protocol-0 IAP
    {
        if( (UsartFrameStruct->UsartReceFrameState==0)&&(UsartFrameStruct->UsartReceFrameOK==0) )
        {
            if(UsartFrameStruct->UsartReceByte==UsartFrameStruct->UsartReceFrameHead)   //0X68
            {
                for(i=0; i<25; i++)
                {
                    UsartFrameStruct->UsartReceBuff[i] = 0x00;
                }
                UsartFrameStruct->UsartReceFrameState = 0x01;
                UsartFrameStruct->UsartReceCount = 1;
                UsartFrameStruct->UsartReceBuff[UsartFrameStruct->UsartReceCount-1] = UsartFrameStruct->UsartReceByte;
                UsartFrameStruct->UsartReceMsgCom = 0x00;
                UsartFrameStruct->UsartReceMsgSubCom = 0x00;
                UsartFrameStruct->UsartReceMsgFlashAddr = 0x00;
                UsartFrameStruct->UsartReceDevID[0] = 0x00;
                UsartFrameStruct->UsartReceDevID[1] = 0x00;
                UsartFrameStruct->UsartReceDevID[2] = 0x00;
                UsartFrameStruct->UsartReceDevID[3] = 0x00;
                UsartFrameStruct->UsartReceDevID[4] = 0x00;
            }
        }
        else if(UsartFrameStruct->UsartReceFrameState==0x01)                            //Frame Len
        {
            UsartFrameStruct->UsartReceFrameState = 0x02;
            UsartFrameStruct->UsartReceCount = 2;
            UsartFrameStruct->UsartReceFrameLen = UsartFrameStruct->UsartReceByte;
            UsartFrameStruct->UsartReceBuff[UsartFrameStruct->UsartReceCount-1] = UsartFrameStruct->UsartReceByte;
        }
        else if(UsartFrameStruct->UsartReceFrameState==0x02)                            //check Frame len
        {
            UsartFrameStruct->UsartReceCount ++;
            UsartFrameStruct->UsartReceFrameLenCheck = UsartFrameStruct->UsartReceByte;
            UsartFrameStruct->UsartReceBuff[UsartFrameStruct->UsartReceCount-1] = UsartFrameStruct->UsartReceByte;

            if( (UsartFrameStruct->UsartReceFrameLenCheck+UsartFrameStruct->UsartReceFrameLen)==0xFF )
            {
                UsartFrameStruct->UsartReceFrameState = 0x03;
            }
            else
            {
                UsartFrameStruct->UsartReceFrameState = 0x00;
                UsartFrameStruct->UsartReceCount = 0;
            }
            
        }
        else if(UsartFrameStruct->UsartReceFrameState==0x03)                               
        {
            UsartFrameStruct->UsartReceCount ++;
            UsartFrameStruct->UsartReceBuff[UsartFrameStruct->UsartReceCount-1] = UsartFrameStruct->UsartReceByte;
            UsartFrameStruct->UsartFrameTimeCount = 0;
            if(UsartFrameStruct->UsartReceCount>=UsartFrameStruct->UsartReceBuff[1])
            {
                if( (UsartFrameStruct->UsartReceBuff[UsartFrameStruct->UsartReceCount-1])==UsartFrameStruct->UsartReceFrameEnd )
                {
                    UsartFrameStruct->UsartReceFrameCheckData = UsartFrameStruct->UsartReceBuff[UsartFrameStruct->UsartReceCount-2];
                    UsartFrameStruct->UsartReceMsgCom = UsartFrameStruct->UsartReceBuff[MsgComAddr];                    //Com code
                    cksum = Usart2Frame.UsartReceBuff[0];
                    for(i=1; i<(Usart2Frame.UsartReceFrameLen-2); i++ )
                    {
                        cksum ^= Usart2Frame.UsartReceBuff[i];
                    }
                    if( (cksum==UsartFrameStruct->UsartReceFrameCheckData)&&(UsartFrameStruct->UsartReceMsgCom==0x0c) )
                    {
                        UsartFrameStruct->UsartReceFrameOrder = UsartFrameStruct->UsartReceBuff[FrameOrderAddr];        //Frame Order//串口接收帧序号
                        UsartFrameStruct->UsartReceDevID[0] = UsartFrameStruct->UsartReceBuff[DevIDBaseAddr];
                        UsartFrameStruct->UsartReceDevID[1] = UsartFrameStruct->UsartReceBuff[DevIDBaseAddr+1];
                        UsartFrameStruct->UsartReceDevID[2] = UsartFrameStruct->UsartReceBuff[DevIDBaseAddr+2];
                        UsartFrameStruct->UsartReceDevID[3] = UsartFrameStruct->UsartReceBuff[DevIDBaseAddr+3];
                        UsartFrameStruct->UsartReceDevID[4] = UsartFrameStruct->UsartReceBuff[DevIDBaseAddr+4];
                        UsartFrameStruct->UsartReceMsgLen = UsartFrameStruct->UsartReceBuff[MsgLenAddr];                //Msg len
                        UsartFrameStruct->UsartReceMsgSubCom = UsartFrameStruct->UsartReceBuff[SubComAddr];             //com sub 
                        UsartFrameStruct->UsartReceMsgFlashAddr = UsartFrameStruct->UsartReceBuff[MsgFlashBaseAddr];    //Flash addr
                        UsartFrameStruct->UsartReceMsgFlashAddr <<= 8;
                        UsartFrameStruct->UsartReceMsgFlashAddr += UsartFrameStruct->UsartReceBuff[MsgFlashBaseAddr+1];
                        UsartFrameStruct->UsartReceMsgFlashAddr <<= 8;
                        UsartFrameStruct->UsartReceMsgFlashAddr += UsartFrameStruct->UsartReceBuff[MsgFlashBaseAddr+2];
                        UsartFrameStruct->UsartReceMsgFlashAddr <<= 8; 
                        UsartFrameStruct->UsartReceMsgFlashAddr += UsartFrameStruct->UsartReceBuff[MsgFlashBaseAddr+3];
                        UsartFrameStruct->UsartFlashDataCount = 0;
                        if(UsartFrameStruct->UsartReceMsgSubCom==0x03)                                  //write Flash
                        {
                            for(i=0; i<(UsartFrameStruct->UsartReceMsgLen-6); (i=i+2) )
                            {
                                tempvalue = Usart2Frame.UsartReceBuff[i+1+MsgFlashDataBaseAddr];
                                tempvalue <<= 8;
                                tempvalue += Usart2Frame.UsartReceBuff[i+MsgFlashDataBaseAddr];
                                UsartFrameStruct->UsartFlashDataBuff[UsartFrameStruct->UsartFlashDataCount] = tempvalue;
                                UsartFrameStruct->UsartFlashDataCount++;
                            }
                        }
                        else if(UsartFrameStruct->UsartReceMsgSubCom==0x02)
                        {
                        
                        }
                        UsartFrameStruct->UsartReceFrameOK = 1;
                        UsartFrameStruct->UsartReceFrameState = 0x00;
                        UsartFrameStruct->UsartFrameTimeCount = 0;
                    }
                    else 
                    {
                        if( (UsartFrameStruct->UsartReceCount==10)&&(UsartFrameStruct->UsartReceBuff[6]==0x02) )
                        {
                            if(cksum==UsartFrameStruct->UsartReceFrameCheckData)
                            {
                                UsartFrameStruct->UsartReceFrameOK = 1;
                                UsartFrameStruct->UsartReceFrameState = 0x00;
                                UsartFrameStruct->UsartFrameTimeCount = 0;
                                UsartFrameStruct->UsartReceMsgSubCom = 0XF1;        //特殊指令
                            }
                            else
                            {
                                UsartFrameStruct->UsartReceFrameState = 0x00;
                                UsartFrameStruct->UsartFrameTimeCount = 0; 
                            }
                        }
                        else
                        {
                            UsartFrameStruct->UsartReceFrameState = 0x00;
                            UsartFrameStruct->UsartFrameTimeCount = 0;                             
                        }
               
                    }
                }
                else
                {
                    UsartFrameStruct->UsartReceFrameState = 0x00;
                    UsartFrameStruct->UsartFrameTimeCount = 0;
                }
            }
        }
    }
}

/******************************************************************************
* Function Name : USART2_IRQHandler()
* Description   : 
* Input         :
* Output        :
* Return        :
* Remarks       : 
******************************************************************************/
void USART2_IRQHandler(void)                                    //interrupt fun
{
    if(USART2->ISR&(USART_ISR_RXNE))                            //STM32F0_Cube                        
    {
        UsartReceDataHandler(&Usart2Frame);
    }
    if(USART2->ISR&(USART_ISR_TC))
    {
        USART2->ICR |= USART_ICR_TCCF;
        UsartSendDataHandler(&Usart2Frame);
    }
    USART2->ICR |= 0x1b5f;                                      //clear all interrupt flags
}

/******************************************************************************
* Function Name : Usart2ReceFrameDataParser()
* Description   : 
* Input         :
* Output        :
* Return        :
* Remarks       : 
******************************************************************************/
uint8_t Usart2ReceFrameDataParser(void)
{
    uint32_t i;
    uint8_t ErrorNum;
    static uint32_t writeflashaddrbak=0; 
    static uint8_t innerstatus = 0;
    uint32_t allflashchecksum=0,offset,checksize;
    uint16_t allflashchecksum1,temp;
    uint32_t flashromSize;
    uint32_t readromSize;
    if(innerstatus==0)
    {
        if(Usart2Frame.UsartReceFrameOK==1)
        {
            HAL_Delay(2);                                       //delay 1ms use for 485 enable delay
            if(Usart2Frame.UsartReceMsgSubCom==0x01)            //handshake protocol 
            {
                Usart2Frame.UsartIAPReturnCode = 0x5555;
                Usart2SendAckCom();  
                innerstatus = 1;
            }
            else if((Usart2Frame.UsartReceMsgSubCom == 0x02) &&(Usart2Frame.UsartIAPRST == 0))   //iap start 
            {
                HAL_Delay(2); 
                Usart2Frame.UsartIAPReturnCode = 0x5555;
                Usart2SendAckCom();
                innerstatus = 1;
                writeflashaddrbak = Usart2Frame.UsartReceMsgFlashAddr;
                Usart2Frame.UsartIAPOK = 0X00;
            }
            else if((Usart2Frame.UsartReceMsgSubCom == 0x03)&&(Usart2Frame.UsartIAPRST == 0) )                  //write flash
            {
                if(writeflashaddrbak == Usart2Frame.UsartReceMsgFlashAddr)
                {
                    if(Usart2Frame.UsartReceMsgFlashAddr>=IAPFLAGADDR)      //add 2018.1.24 prevent erase iap flag
                    {
                        Usart2Frame.UsartIAPReturnCode = 0x0000;
                    }
                    else
                    {
                        writeflashaddrbak += Usart2Frame.UsartFlashDataCount*2;
                        ErrorNum = FlashWritenHalfWord(Usart2Frame.UsartReceMsgFlashAddr,Usart2Frame.UsartFlashDataCount,Usart2Frame.UsartFlashDataBuff);
                        if(ErrorNum==0)
                        {
                            Usart2Frame.UsartIAPReturnCode = 0x5555;
                        }
                        else
                        {
                            Usart2Frame.UsartIAPReturnCode = 0x0000;
                        }
                    }
                }
                else
                {
                    Usart2Frame.UsartIAPReturnCode = 0x0000;
                }
                Usart2SendAckCom();
                innerstatus = 1;
            }
            else if(Usart2Frame.UsartReceMsgSubCom==0x04)       //check app program  appbaseaddr and app size(halfword)
            {
                HAL_Delay(2); 
                offset = 0;
                allflashchecksum = 0;
                allflashchecksum1 = 0;
                checksize = Usart2Frame.UsartReceBuff[16];      
                checksize <<= 8;
                checksize += Usart2Frame.UsartReceBuff[17];
							
                flashromSize = TotalPage*PageSize;
                flashromSize += 0x08000000;
                readromSize = (Usart2Frame.UsartReceMsgFlashAddr + checksize*2);
                if( (flashromSize<=readromSize)||(Usart2Frame.UsartReceMsgFlashAddr<0x08000000) )
                {
                    Usart2Frame.UsartIAPReturnCode = 0x0000;
                    Usart2SendAckCom();
                    innerstatus = 1;
                    return 0xff;
                }
                
                for(i=0; i<checksize; i++)
                {
                    temp = FlashReadHalfWord(Usart2Frame.UsartReceMsgFlashAddr+offset);
                    offset = offset + 2;
                    allflashchecksum += temp;
                    allflashchecksum1 ^= temp;
                }
                checksize = Usart2Frame.UsartReceBuff[18];   //acc check
                checksize <<= 8;
                checksize += Usart2Frame.UsartReceBuff[19];
                checksize <<= 8;
                checksize += Usart2Frame.UsartReceBuff[20];
                checksize <<= 8; 
                checksize += Usart2Frame.UsartReceBuff[21];

                temp = Usart2Frame.UsartReceBuff[22];       //^check
                temp <<= 8;
                temp += Usart2Frame.UsartReceBuff[23];
                if( (checksize==allflashchecksum)&&(temp==allflashchecksum1) )
                {
                    Usart2Frame.UsartIAPReturnCode = 0x5555;
                    Usart2Frame.UsartIAPOK = 0X01;
                }
                else
                {
                    Usart2Frame.UsartIAPReturnCode = 0x0000;
                    Usart2Frame.UsartIAPOK = 0;
                }
                Usart2SendAckCom();
                innerstatus = 1;
            }
            else if(Usart2Frame.UsartReceMsgSubCom==0x05)       //Master reset the device  //复位
            {
                HAL_Delay(2); 
                Usart2Frame.UsartIAPRST = 0x01;
                Usart2Frame.UsartResetCount = 0;
                Usart2Frame.UsartIAPReturnCode = 0x5555;
                Usart2SendAckCom();
                innerstatus = 1;
            }
            else if(Usart2Frame.UsartReceMsgSubCom==0x80)       //reserve com   set PageSize&TotalPage //预留com设置页面大小总页面
            {
                HAL_Delay(2); 
                if( (Usart2Frame.UsartReceBuff[16]==0xAA)&&(Usart2Frame.UsartReceBuff[17]==0xAA) )
                {
                    if( (Usart2Frame.UsartReceBuff[18]==0xBB)&&(Usart2Frame.UsartReceBuff[19]==0xBB) )
                    {
                        PageSize = Usart2Frame.UsartReceBuff[20];
                        PageSize <<= 8;
                        PageSize += Usart2Frame.UsartReceBuff[21];
                        
                        TotalPage = Usart2Frame.UsartReceBuff[22];
                        TotalPage <<= 8;
                        TotalPage += Usart2Frame.UsartReceBuff[23];
                        if( (PageSize==1024)||(PageSize==2048) )
                        {
                            if( (TotalPage>=16)&&(TotalPage<=256) )
                            {
                                Usart2Frame.UsartIAPReturnCode = 0x5555;
                            }
                            else
                            {
                                Usart2Frame.UsartIAPReturnCode = 0x0000;
                            }
                        }
                        else
                        {
                            Usart2Frame.UsartIAPReturnCode = 0x0000;
                        }
                        Usart2SendAckCom();
                        innerstatus = 1;
                    }
                    else
                    {
                        innerstatus = 0;
                        Usart2Frame.UsartReceFrameOK = 0;
                    }
                }
                else
                {
                    innerstatus = 0;
                    Usart2Frame.UsartReceFrameOK = 0;
                }
            }
            else if(Usart2Frame.UsartReceMsgSubCom==0x81)   //reserve com   May be used  for AFE cfg data  cali data and so on//可用于AFE cfg数据、cali数据等
            {
                HAL_Delay(2); 
                if( (Usart2Frame.UsartReceDevID[0]==0XFF)&&(Usart2Frame.UsartReceDevID[1]==0XEE) 
                     &&(Usart2Frame.UsartReceDevID[2]==0XDD)&&(Usart2Frame.UsartReceDevID[3]==0XCC) )
                {

                        ErrorNum = FlashWritenHalfWord(Usart2Frame.UsartReceMsgFlashAddr,Usart2Frame.UsartFlashDataCount,Usart2Frame.UsartFlashDataBuff);
                        if(ErrorNum==0)
                        {
                                Usart2Frame.UsartIAPReturnCode = 0x5555;
                        }
                        else
                        {
                                Usart2Frame.UsartIAPReturnCode = 0x0000;
                        }
                        Usart2SendAckCom();
                        innerstatus = 1;									
                }
                else
                {
                    innerstatus = 0;
                    Usart2Frame.UsartReceFrameOK = 0;
                }
            }
            else if(Usart2Frame.UsartReceMsgSubCom==0xF1)            //特殊指令
            {
                Usart2Frame.UsartSendBuff[9] = 8;
                Usart2Frame.UsartReceMsgCom = 0X0C;
                Usart2Frame.UsartReceMsgSubCom = 0X02;
                Usart2Frame.UsartReceMsgFlashAddr = 0X08003000;
                Usart2Frame.UsartIAPReturnCode = 0X55AA;
                Usart2SendAckCom();
                innerstatus = 1;
            }
            else
            {
                innerstatus = 0;
                Usart2Frame.UsartReceFrameOK = 0;
            }
        }
    }
    else if(innerstatus==1)//发出等待应答模式
    {
        //if use rs485 comm
        //Enable Send GPIO(disable receive)
        //Delay1ms
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
        HAL_Delay(1);

        UsartSendFrame(&Usart2Frame,Usart2Frame.UsartSendBuff,Usart2Frame.UsartSendBuff[1],0);//数据 发出
        if(Usart2Frame.UsartSendFrameOK==1)     //收到应答
        {
            IAPTimeOut = 0;
            Usart2Frame.UsartSendFrameOK = 0;
            innerstatus = 0;
            Usart2Frame.UsartReceFrameOK = 0;

            //Delay1ms
            //Disable Send GPIO(enable receive)
            HAL_Delay(1);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
        }
        else if(Usart2Frame.UsartSendFrameOK==2)//没接收到 超时
        {
            Usart2Frame.UsartSendFrameOK = 0;
            innerstatus = 0;
            Usart2Frame.UsartReceFrameOK = 0;

            //Delay1ms
            //Disable Send GPIO(enable receive)
            HAL_Delay(1);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
        }
    }	
    return 0xff;
}

static void Usart2SendAckCom(void)//应答数据更新
{
    uint8_t cksum,i;
    Usart2Frame.UsartSendBuff[0] = Usart2Frame.UsartReceFrameHead;
    Usart2Frame.UsartSendBuff[1] = 20;
    Usart2Frame.UsartSendBuff[2] = 0xff - Usart2Frame.UsartSendBuff[1];
    Usart2Frame.UsartSendBuff[3] = Usart2Frame.UsartReceFrameOrder;
    Usart2Frame.UsartSendBuff[4] = Usart2Frame.UsartReceDevID[0];
    Usart2Frame.UsartSendBuff[5] = Usart2Frame.UsartReceDevID[1];
    Usart2Frame.UsartSendBuff[6] = Usart2Frame.UsartReceDevID[2];
    Usart2Frame.UsartSendBuff[7] = Usart2Frame.UsartReceDevID[3];
    Usart2Frame.UsartSendBuff[8] = Usart2Frame.UsartReceDevID[4];
    Usart2Frame.UsartSendBuff[9] = 8;
    Usart2Frame.UsartSendBuff[10] = Usart2Frame.UsartReceMsgCom;
    Usart2Frame.UsartSendBuff[11] = Usart2Frame.UsartReceMsgSubCom;
    Usart2Frame.UsartSendBuff[12] = Usart2Frame.UsartReceMsgFlashAddr>>24;
    Usart2Frame.UsartSendBuff[13] = Usart2Frame.UsartReceMsgFlashAddr>>16;
    Usart2Frame.UsartSendBuff[14] = Usart2Frame.UsartReceMsgFlashAddr>>8;
    Usart2Frame.UsartSendBuff[15] = Usart2Frame.UsartReceMsgFlashAddr>>0;
    Usart2Frame.UsartSendBuff[16] = Usart2Frame.UsartIAPReturnCode>>8;
    Usart2Frame.UsartSendBuff[17] = Usart2Frame.UsartIAPReturnCode>>0;
    
    cksum = Usart2Frame.UsartSendBuff[0];
    for(i=1; i<(Usart2Frame.UsartSendBuff[1]-2); i++ )
    {
        cksum ^= Usart2Frame.UsartSendBuff[i];
    }
    Usart2Frame.UsartSendBuff[18] = cksum;
    Usart2Frame.UsartSendBuff[19] = Usart2Frame.UsartReceFrameEnd;
}


