/*
 * rn8302.c
 *
 *  Created on: 2020年3月26日
 *      Author: Administrator
 */

#include "include.h"


uint8_t reg[2]={0x0,0x0};


#define  GETWRITECMDADDR(wReg)    ((((uint8_t)(wReg >> 4)) & 0xf0) + 0x80)
#define  GETREADCMDADDR(wReg)			(((uint8_t)(wReg >> 4)) & 0xf0)
#define  GETREGADDR(wReg)			    (wReg & 0x00ff)





void Rn8302Read(uint16_t temp,uint32_t* val,uint8_t num)
{
	uint8_t Address[2];
	uint8_t Rx[10];
	uint8_t *pBuf = (uint8_t*)val;

    HAL_GPIO_WritePin(SPI1_CS1_GPIO_Port, SPI1_CS1_Pin, GPIO_PIN_RESET);

    Address[0] = (uint8_t)GETREGADDR(temp);
    Address[1] = GETREADCMDADDR(temp);

	HAL_SPI_Transmit(&hspi1, Address, 2, 0XFF);
    HAL_SPI_Receive(&hspi1, Rx, num, 0XFF);

//    HAL_UART_Transmit(&huart1, Rx, num, 1000);

//    for (int i = 0; i < num; i++)
//	{
//		pBuf[num-1-i] = Rx[i];//数据高位对齐
//	}

    for (int i = 0; i < num; i++)
	{
		pBuf[num-1-i] = Rx[i];//数据高位对齐
	}

    HAL_GPIO_WritePin(SPI1_CS1_GPIO_Port, SPI1_CS1_Pin, GPIO_PIN_SET);

}

void Rn8302Write(uint16_t wReg, uint8_t *pBuf, uint8_t DatLen)
{
	uint8_t i, chksum = 0, Repeat;
	uint8_t DatP[10];

	for (Repeat = 3; Repeat != 0; Repeat--)
	{
		DatP[0] = (uint8_t)GETREGADDR(wReg);
		DatP[1] = GETWRITECMDADDR(wReg);
		HAL_GPIO_WritePin(SPI1_CS1_GPIO_Port, SPI1_CS1_Pin, GPIO_PIN_RESET);
		for (i = 0; i < DatLen; i++)/*高字节在前，低字节在后*/
		{
			DatP[2+i] = pBuf[DatLen - 1 - i];
		}
		for (i = 0; i < DatLen + 2; i++)/*求校验*/
		{
			chksum += DatP[i];
		}
		DatP[DatLen + 2] = ~chksum;
		HAL_SPI_Transmit(&hspi1, DatP, DatLen + 1 + 2, 0XFFF);
		HAL_GPIO_WritePin(SPI1_CS1_GPIO_Port, SPI1_CS1_Pin, GPIO_PIN_SET);
	}
}
