#include "74HC595.h"
#include "main.h"




#define JDON_1 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET)
#define JDON_0 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)
#define JDONRCK_1 HAL_GPIO_WritePin(HC595_LOCK_GPIO_Port,HC595_LOCK_Pin,GPIO_PIN_SET)
#define JDONRCK_0 HAL_GPIO_WritePin(HC595_LOCK_GPIO_Port,HC595_LOCK_Pin,GPIO_PIN_RESET)





#define SCK_1 HAL_GPIO_WritePin(HC595_CLK_GPIO_Port,HC595_CLK_Pin,GPIO_PIN_SET)
#define SCK_0 HAL_GPIO_WritePin(HC595_CLK_GPIO_Port,HC595_CLK_Pin,GPIO_PIN_RESET)





	void 	JD_control(unsigned int SendVal)
{
		JDONRCK_0;
		SCK_0;
		HAL_Delay(1);
		unsigned char i;
		for(i=0;i<32;i++)
		{
	   	if((SendVal<<i)&0x80000000)
			{
				JDON_1;
				HAL_Delay(1);
			}
			else
			{
				JDON_0;
				HAL_Delay(1);
			}
			SCK_1;
			HAL_Delay(1);
			SCK_0;
			HAL_Delay(1);
		}
		JDONRCK_1;
		HAL_Delay(1);
		JDONRCK_0;
		HAL_Delay(1);
}
