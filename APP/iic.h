#ifndef		_IIC_
#define		_IIC_

#include "stm32f0xx_hal.h"
#define I2C_DAT_OUT			{IOtempstruct.Pin = GPIO_PIN_0;IOtempstruct.Mode = GPIO_MODE_OUTPUT_PP;  IOtempstruct.Pull = GPIO_PULLUP;IOtempstruct.Speed = GPIO_SPEED_FREQ_LOW;HAL_GPIO_Init(GPIOA, &IOtempstruct);}	
#define I2C_DAT_HIGH  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET)
#define I2C_DAT_LOW     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET)
#define I2C_CLK_OUT		  {IOtempstruct.Pin = GPIO_PIN_1;IOtempstruct.Mode = GPIO_MODE_OUTPUT_PP;  IOtempstruct.Pull = GPIO_PULLUP;IOtempstruct.Speed = GPIO_SPEED_FREQ_LOW;HAL_GPIO_Init(GPIOA, &IOtempstruct);}	
#define I2C_CLK_HIGH    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
#define I2C_CLK_LOW     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)
#define I2C_DAT_IN     {IOtempstruct.Pin = GPIO_PIN_0;IOtempstruct.Mode = GPIO_MODE_INPUT;  IOtempstruct.Pull = GPIO_PULLUP;IOtempstruct.Speed = GPIO_SPEED_FREQ_LOW;HAL_GPIO_Init(GPIOA, &IOtempstruct);}
#define I2C_CLK_IN    	{IOtempstruct.Pin = GPIO_PIN_1;IOtempstruct.Mode = GPIO_MODE_INPUT;  IOtempstruct.Pull = GPIO_PULLUP;IOtempstruct.Speed = GPIO_SPEED_FREQ_LOW;HAL_GPIO_Init(GPIOA, &IOtempstruct);}
#define I2C_RD_DAT		HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
#define I2C_RD_CLK		HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)
//void Delay_System(unsigned short tt);
//void I2C_Init(void);
//unsigned char I2CReadNByte(unsigned char Addr,unsigned char Com,unsigned char *pRDat);
extern uint8_t I2CRead(uint8_t SlaveID, uint8_t RdAddr, uint8_t Length, uint8_t  *RdBuf);
extern uint8_t I2CWrite(uint8_t SlaveID, uint8_t WrAddr, uint8_t Length, uint8_t  *WrBuf);
#endif
