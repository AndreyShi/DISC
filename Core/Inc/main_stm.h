/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f4xx_hal.h"

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
#define DBG1_pin GPIO_PIN_2
#define DBG2_pin GPIO_PIN_4
#define DBG1_PORT GPIOE_STM
#define DBG2_PORT GPIOE_STM

// отладка
#define DBG1_SET DBG1_PORT->BSRR |= (DBG1_pin);
#define DBG1_RST DBG1_PORT->BSRR |= (DBG1_pin << 16);

#define DBG2_SET DBG2_PORT->BSRR |= (DBG2_pin);
#define DBG2_RST DBG2_PORT->BSRR |= (DBG2_pin << 16);

#define DBG1_TOG DBG1_SET DBG1_RST  // фиолетовый
#define DBG2_TOG DBG2_SET DBG2_RST  // синий
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC_STM
#define PC15_OSC32_OUT_Pin GPIO_PIN_15
#define PC15_OSC32_OUT_GPIO_Port GPIOC_STM
#define A0_Pin GPIO_PIN_0
#define A0_GPIO_Port GPIOF_STM
#define A1_Pin GPIO_PIN_1
#define A1_GPIO_Port GPIOF_STM
#define A2_Pin GPIO_PIN_2
#define A2_GPIO_Port GPIOF_STM
#define A3_Pin GPIO_PIN_3
#define A3_GPIO_Port GPIOF_STM
#define A4_Pin GPIO_PIN_4
#define A4_GPIO_Port GPIOF_STM
#define A5_Pin GPIO_PIN_5
#define A5_GPIO_Port GPIOF_STM
#define SPI5_SCK_Pin GPIO_PIN_7
#define SPI5_SCK_GPIO_Port GPIOF_STM
#define SPI5_MISO_Pin GPIO_PIN_8
#define SPI5_MISO_GPIO_Port GPIOF_STM
#define SPI5_MOSI_Pin GPIO_PIN_9
#define SPI5_MOSI_GPIO_Port GPIOF_STM
#define ENABLE_Pin GPIO_PIN_10
#define ENABLE_GPIO_Port GPIOF_STM
#define PH0_OSC_IN_Pin GPIO_PIN_0
#define PH0_OSC_IN_GPIO_Port GPIOH
#define PH1_OSC_OUT_Pin GPIO_PIN_1
#define PH1_OSC_OUT_GPIO_Port GPIOH
#define SDNWE_Pin GPIO_PIN_0
#define SDNWE_GPIO_Port GPIOC_STM
#define NCS_MEMS_SPI_Pin GPIO_PIN_1
#define NCS_MEMS_SPI_GPIO_Port GPIOC_STM
#define CSX_Pin GPIO_PIN_2
#define CSX_GPIO_Port GPIOC_STM
#define B1_Pin GPIO_PIN_0
#define B1_GPIO_Port GPIOA_STM
#define MEMS_INT1_Pin GPIO_PIN_1
#define MEMS_INT1_GPIO_Port GPIOA_STM
#define MEMS_INT2_Pin GPIO_PIN_2
#define MEMS_INT2_GPIO_Port GPIOA_STM
#define B5_Pin GPIO_PIN_3
#define B5_GPIO_Port GPIOA_STM
#define VSYNC_Pin GPIO_PIN_4
#define VSYNC_GPIO_Port GPIOA_STM
#define G2_Pin GPIO_PIN_6
#define G2_GPIO_Port GPIOA_STM
#define ACP_RST_Pin GPIO_PIN_7
#define ACP_RST_GPIO_Port GPIOA_STM
#define OTG_FS_PSO_Pin GPIO_PIN_4
#define OTG_FS_PSO_GPIO_Port GPIOC_STM
#define OTG_FS_OC_Pin GPIO_PIN_5
#define OTG_FS_OC_GPIO_Port GPIOC_STM
#define R3_Pin GPIO_PIN_0
#define R3_GPIO_Port GPIOB_STM
#define R6_Pin GPIO_PIN_1
#define R6_GPIO_Port GPIOB_STM
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB_STM
#define SDNRAS_Pin GPIO_PIN_11
#define SDNRAS_GPIO_Port GPIOF_STM
#define A6_Pin GPIO_PIN_12
#define A6_GPIO_Port GPIOF_STM
#define A7_Pin GPIO_PIN_13
#define A7_GPIO_Port GPIOF_STM
#define A8_Pin GPIO_PIN_14
#define A8_GPIO_Port GPIOF_STM
#define A9_Pin GPIO_PIN_15
#define A9_GPIO_Port GPIOF_STM
#define A10_Pin GPIO_PIN_0
#define A10_GPIO_Port GPIOG
#define A11_Pin GPIO_PIN_1
#define A11_GPIO_Port GPIOG
#define D4_Pin GPIO_PIN_7
#define D4_GPIO_Port GPIOE_STM
#define D5_Pin GPIO_PIN_8
#define D5_GPIO_Port GPIOE_STM
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOE_STM
#define D7_Pin GPIO_PIN_10
#define D7_GPIO_Port GPIOE_STM
#define D8_Pin GPIO_PIN_11
#define D8_GPIO_Port GPIOE_STM
#define D9_Pin GPIO_PIN_12
#define D9_GPIO_Port GPIOE_STM
#define D10_Pin GPIO_PIN_13
#define D10_GPIO_Port GPIOE_STM
#define D11_Pin GPIO_PIN_14
#define D11_GPIO_Port GPIOE_STM
#define D12_Pin GPIO_PIN_15
#define D12_GPIO_Port GPIOE_STM
#define G4_Pin GPIO_PIN_10
#define G4_GPIO_Port GPIOB_STM
#define G5_Pin GPIO_PIN_11
#define G5_GPIO_Port GPIOB_STM
#define OTG_HS_ID_Pin GPIO_PIN_12
#define OTG_HS_ID_GPIO_Port GPIOB_STM
#define VBUS_HS_Pin GPIO_PIN_13
#define VBUS_HS_GPIO_Port GPIOB_STM
#define OTG_HS_DM_Pin GPIO_PIN_14
#define OTG_HS_DM_GPIO_Port GPIOB_STM
#define OTG_HS_DP_Pin GPIO_PIN_15
#define OTG_HS_DP_GPIO_Port GPIOB_STM
#define D13_Pin GPIO_PIN_8
#define D13_GPIO_Port GPIOD_STM
#define D14_Pin GPIO_PIN_9
#define D14_GPIO_Port GPIOD_STM
#define D15_Pin GPIO_PIN_10
#define D15_GPIO_Port GPIOD_STM
#define TE_Pin GPIO_PIN_11
#define TE_GPIO_Port GPIOD_STM
#define RDX_Pin GPIO_PIN_12
#define RDX_GPIO_Port GPIOD_STM
#define WRX_DCX_Pin GPIO_PIN_13
#define WRX_DCX_GPIO_Port GPIOD_STM
#define D0_Pin GPIO_PIN_14
#define D0_GPIO_Port GPIOD_STM
#define D1_Pin GPIO_PIN_15
#define D1_GPIO_Port GPIOD_STM
#define BA0_Pin GPIO_PIN_4
#define BA0_GPIO_Port GPIOG
#define BA1_Pin GPIO_PIN_5
#define BA1_GPIO_Port GPIOG
#define R7_Pin GPIO_PIN_6
#define R7_GPIO_Port GPIOG
#define DOTCLK_Pin GPIO_PIN_7
#define DOTCLK_GPIO_Port GPIOG
#define SDCLK_Pin GPIO_PIN_8
#define SDCLK_GPIO_Port GPIOG
#define HSYNC_Pin GPIO_PIN_6
#define HSYNC_GPIO_Port GPIOC_STM
#define G6_Pin GPIO_PIN_7
#define G6_GPIO_Port GPIOC_STM
#define I2C3_SDA_Pin GPIO_PIN_9
#define I2C3_SDA_GPIO_Port GPIOC_STM
#define I2C3_SCL_Pin GPIO_PIN_8
#define I2C3_SCL_GPIO_Port GPIOA_STM
#define STLINK_RX_Pin GPIO_PIN_9
#define STLINK_RX_GPIO_Port GPIOA_STM
#define STLINK_TX_Pin GPIO_PIN_10
#define STLINK_TX_GPIO_Port GPIOA_STM
#define R4_Pin GPIO_PIN_11
#define R4_GPIO_Port GPIOA_STM
#define R5_Pin GPIO_PIN_12
#define R5_GPIO_Port GPIOA_STM
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA_STM
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA_STM
#define TP_INT1_Pin GPIO_PIN_15
#define TP_INT1_GPIO_Port GPIOA_STM
#define R2_Pin GPIO_PIN_10
#define R2_GPIO_Port GPIOC_STM
#define D2_Pin GPIO_PIN_0
#define D2_GPIO_Port GPIOD_STM
#define D3_Pin GPIO_PIN_1
#define D3_GPIO_Port GPIOD_STM
#define G7_Pin GPIO_PIN_3
#define G7_GPIO_Port GPIOD_STM
#define B2_Pin GPIO_PIN_6
#define B2_GPIO_Port GPIOD_STM
#define G3_Pin GPIO_PIN_10
#define G3_GPIO_Port GPIOG
#define B3_Pin GPIO_PIN_11
#define B3_GPIO_Port GPIOG
#define B4_Pin GPIO_PIN_12
#define B4_GPIO_Port GPIOG
#define LD3_Pin GPIO_PIN_13
#define LD3_GPIO_Port GPIOG
#define LD4_Pin GPIO_PIN_14
#define LD4_GPIO_Port GPIOG
#define SDNCAS_Pin GPIO_PIN_15
#define SDNCAS_GPIO_Port GPIOG
#define SDCKE1_Pin GPIO_PIN_5
#define SDCKE1_GPIO_Port GPIOB_STM
#define SDNE1_Pin GPIO_PIN_6
#define SDNE1_GPIO_Port GPIOB_STM
#define B6_Pin GPIO_PIN_8
#define B6_GPIO_Port GPIOB_STM
#define B7_Pin GPIO_PIN_9
#define B7_GPIO_Port GPIOB_STM
#define NBL0_Pin GPIO_PIN_0
#define NBL0_GPIO_Port GPIOE_STM
#define NBL1_Pin GPIO_PIN_1
#define NBL1_GPIO_Port GPIOE_STM
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
