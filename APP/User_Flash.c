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

#include"User_Flash.h"

uint32_t FlashReadWord(uint32_t ReadAddr);
uint16_t FlashReadHalfWord(uint32_t ReadAddr);
FLASH_EraseInitTypeDef FLASH_EraseInitType;
User_FlashTypeDef UserFlashTypeDef;

uint32_t PageSize;
uint32_t TotalPage;

void Flash_Init(void)
{
    PageSize = 1024;
    TotalPage = 64;
}

uint8_t FlashWritenHalfWord(uint32_t WriteAddr, uint8_t WriteHalfWordSize, uint16_t *pWDat)
{
    static uint32_t tmpaddr;
    static uint32_t i;
    static uint32_t pagebaseaddr;
    static uint16_t pageoffsetaddr;
    static uint32_t pageremainhalfword;
    static uint16_t pagereadhalfworddata;  
    if( WriteAddr<(STM32MainFlashBaseAddr+BootLoadSize) )           /***Bootload ban itself***///uboot ¿Õ¼äÊÇ8k  ÆôÊ¼µØÖ·å+8k  Èç¹ûÒª±à³ÌµÄµØÖ·Õ¼ÓÃuboot µÄµØÖ··´»Ø1
    {
        return 0x01;
    }
 
    tmpaddr = (PageSize*TotalPage-1) + STM32MainFlashBaseAddr;      //Flash Max addr
    if( tmpaddr<(WriteHalfWordSize*2+WriteAddr-1) )                 //overflow  //Òª±à³ÌµÄµØÖ·´óÓÚ×î´óflashµØÖ· ·´»Ø2
    {
        return 0x02;
    }
    
    tmpaddr = WriteAddr - STM32MainFlashBaseAddr;
    if( (tmpaddr%2)!=0 )                                            /***addr***///²»ÊÇÒ»¸öÍêÕûµÄwordµØÖ··´»Ø3
    {
        return 0x03;
    }
    
    tmpaddr = (WriteAddr-STM32MainFlashBaseAddr)/PageSize;          //×ª»»³ÉÒ³
    pagebaseaddr = tmpaddr*PageSize + STM32MainFlashBaseAddr;       //×ª³ÉµØÖ·
    pageoffsetaddr = WriteAddr - pagebaseaddr;      //ss                       //Æ«ÒÆµØÖ·
    pageremainhalfword = (PageSize-pageoffsetaddr)/2;               
    for(i=0; i<pageremainhalfword; i++ )                            /***check remain space***/
    {
        tmpaddr = pagebaseaddr + pageoffsetaddr + i*2;
        pagereadhalfworddata = FlashReadHalfWord(tmpaddr);
        if(pagereadhalfworddata!=0xffff)                            //erase the page
        {
            FLASH_EraseInitType.TypeErase = 0X00;
            FLASH_EraseInitType.PageAddress = pagebaseaddr;
            FLASH_EraseInitType.NbPages = 1;
            HAL_FLASH_Unlock();
            HAL_FLASHEx_Erase(&FLASH_EraseInitType,&(UserFlashTypeDef.FlashEraseReturn));
            HAL_FLASH_Lock();
            break;
        }
    }
    tmpaddr = pageoffsetaddr + WriteHalfWordSize*2; //            ´óÓÚÒ»Ò³1024  Ôò¶à²Á³öÒ»Ò³
    if(tmpaddr>PageSize)                                            /***overflow the page***/
    {
        for(i=0; i<(PageSize/2); i++)
        {
            tmpaddr = (pagebaseaddr+PageSize) + i*2;
            pagereadhalfworddata = FlashReadHalfWord(tmpaddr);
            if( pagereadhalfworddata!=0xffff )                      //erase next page
            {
                FLASH_EraseInitType.TypeErase = 0X00;
                FLASH_EraseInitType.PageAddress = (pagebaseaddr+PageSize);
                FLASH_EraseInitType.NbPages = 1;
                HAL_FLASH_Unlock();
                HAL_FLASHEx_Erase(&FLASH_EraseInitType,&(UserFlashTypeDef.FlashEraseReturn));
                HAL_FLASH_Lock();
                break;
            }
        }
    }
    if( WriteHalfWordSize<=pageremainhalfword )                     /******/
    {
        for(i=0; i<WriteHalfWordSize; i++)
        {
            tmpaddr = WriteAddr + i*2;
            HAL_FLASH_Unlock();
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,tmpaddr,pWDat[i]);
            HAL_FLASH_Lock();
        }
    }
    else                                                            /***overflow the page***/
    {   
        for(i=0; i<WriteHalfWordSize; i++)
        {
            tmpaddr = WriteAddr + i*2;
            HAL_FLASH_Unlock();
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,tmpaddr,pWDat[i]);
            HAL_FLASH_Lock();
        }
        for(i=0; i<(WriteHalfWordSize-pageremainhalfword); i++)
        {
            tmpaddr = (pagebaseaddr+PageSize) + i*2;
            HAL_FLASH_Unlock();
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,tmpaddr,pWDat[i]);
            HAL_FLASH_Lock();
        }
    }
    return 0x00;
}

uint32_t FlashReadWord(uint32_t ReadAddr)
{
	return *(volatile uint32_t*)ReadAddr; 
}

uint16_t FlashReadHalfWord(uint32_t ReadAddr)
{
	return *(volatile uint16_t*)ReadAddr; 
}
