/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY0_Pin GPIO_PIN_0
#define KEY0_GPIO_Port GPIOA
#define KEY0_EXTI_IRQn EXTI0_IRQn
#define RN8302B_INIT2_Pin GPIO_PIN_1
#define RN8302B_INIT2_GPIO_Port GPIOA
#define UART2_PBUS_TX_Pin GPIO_PIN_2
#define UART2_PBUS_TX_GPIO_Port GPIOA
#define UART2_PBUS_RX_Pin GPIO_PIN_3
#define UART2_PBUS_RX_GPIO_Port GPIOA
#define RN8302_RST_Pin GPIO_PIN_4
#define RN8302_RST_GPIO_Port GPIOA
#define SPI1_CS1_Pin GPIO_PIN_0
#define SPI1_CS1_GPIO_Port GPIOB
#define SPI1_CS2_Pin GPIO_PIN_1
#define SPI1_CS2_GPIO_Port GPIOB
#define SPI1_CS3_Pin GPIO_PIN_10
#define SPI1_CS3_GPIO_Port GPIOB
#define SPI1_CS4_Pin GPIO_PIN_11
#define SPI1_CS4_GPIO_Port GPIOB
#define DS18B20_IO_Pin GPIO_PIN_12
#define DS18B20_IO_GPIO_Port GPIOB
#define HC595_CLR_Pin GPIO_PIN_13
#define HC595_CLR_GPIO_Port GPIOB
#define HC595_LOCK_Pin GPIO_PIN_14
#define HC595_LOCK_GPIO_Port GPIOB
#define HC595_DO_Pin GPIO_PIN_15
#define HC595_DO_GPIO_Port GPIOB
#define HC595_CLK_Pin GPIO_PIN_8
#define HC595_CLK_GPIO_Port GPIOA
#define UART1_USB_TX_Pin GPIO_PIN_9
#define UART1_USB_TX_GPIO_Port GPIOA
#define UART1_USB_RX_Pin GPIO_PIN_10
#define UART1_USB_RX_GPIO_Port GPIOA
#define HC595_OE_Pin GPIO_PIN_11
#define HC595_OE_GPIO_Port GPIOA
#define RN8302B_INIT1_Pin GPIO_PIN_12
#define RN8302B_INIT1_GPIO_Port GPIOA
#define RN8302B_INIT4_Pin GPIO_PIN_15
#define RN8302B_INIT4_GPIO_Port GPIOA
#define RN8302B_INIT3_Pin GPIO_PIN_4
#define RN8302B_INIT3_GPIO_Port GPIOB
#define DI_Pin GPIO_PIN_5
#define DI_GPIO_Port GPIOB
#define DI_EXTI_IRQn EXTI9_5_IRQn
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_7
#define LED0_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
uint8_t LED(uint8_t temp1,uint8_t temp2,uint8_t temp3);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
