/**
 ****************************************************************************
 * @file        lcd.c
 * @author      Long Dang
 * @version     V0.1
 * @date        12-March-2017
 * @copyright   LGPLv3
 * @brief       This file provide implementation of the LCD module interface.
 ****************************************************************************
 * @attention
 *
 * <h2><center>&trade; PnL - Programming and Leverage </center></h2>
 *
 * This file is part of Project Moon.
 *
 *   Project Moon is free embedded software: you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *
 *   Project Moon is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *   See the GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with Project Moon.
 *   If not, see <http://www.gnu.org/licenses>.
 ****************************************************************************
 */
/** @defgroup BSP_LCD LCD
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "lcd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/** @defgroup BSP-LCD-PERIPHERALS I2C1 Hardware communication interface
 * @{
 */
/* Definition for I2Cx clock resources */
#define I2Cx                            I2C1
#define I2Cx_CLK_ENABLE()               __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()

#define I2Cx_FORCE_RESET()              __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()            __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SDA_PIN                    GPIO_PIN_7
#define I2Cx_SDA_GPIO_PORT              GPIOB

/* Definition for I2Cx's DMA */
#define I2Cx_DMA                        DMA1
#define I2Cx_DMA_INSTANCE_TX            DMA1_Channel6
#define I2Cx_DMA_INSTANCE_RX            DMA1_Channel7

/* Definition for I2Cx's DMA NVIC */
#define I2Cx_DMA_TX_IRQn                DMA1_Channel6_IRQn
#define I2Cx_DMA_RX_IRQn                DMA1_Channel7_IRQn
#define I2Cx_DMA_TX_IRQHandler          DMA1_Channel6_IRQHandler
#define I2Cx_DMA_RX_IRQHandler          DMA1_Channel7_IRQHandler
/**
 * @} BSP-LCD-PERIPHERALS
 */

/* Definition for LCD reset Pin */
#define LCD_RESn_PIN                    GPIO_PIN_8
#define LCD_RESn_GPIO_PORT              GPIOB
#define LCD_RESn_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define LCD_RESn_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOB_CLK_DISABLE()
#define LCD_RESn_ACTIVE()				HAL_GPIO_WritePin(LCD_RESn_GPIO_PORT, LCD_RESn_PIN, GPIO_PIN_RESET)
#define LCD_RESn_DEACTIVE()				HAL_GPIO_WritePin(LCD_RESn_GPIO_PORT, LCD_RESn_PIN, GPIO_PIN_SET)

/* Constant value of LCD module */
#define LCD_I2C_DEVICE_ADDRESS		(0x3C)	/*!< 0b011110[SA0][RW] - 0x3C or 0x3D */
#define LCD_COMMAND_PACKET_SIZE		(2)		/*!< Size in byte of LCD command packet. */

/* Private variables ---------------------------------------------------------*/
static uint16_t g_ui16SlaveAddress = (uint16_t) (LCD_I2C_DEVICE_ADDRESS << 1); /*!< I2C Address of LCD device. */

/* Private functions declaration ---------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
I2C_HandleTypeDef i2chandle_lcd; /*!< I2C handler for LCD declaration. */

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Configures peripherals to control the LCD module.
 * @param  None
 * @retval bool: Status of initialization
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_lcd_init(void)
{
	/*##-1- Configure the I2C peripheral ######################################*/
	i2chandle_lcd.Instance = I2Cx;
	i2chandle_lcd.Init.ClockSpeed = 400000; /* Max value: 400 KHz on STM32F1xx */
	i2chandle_lcd.Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2chandle_lcd.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2chandle_lcd.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2chandle_lcd.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2chandle_lcd.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	i2chandle_lcd.Init.OwnAddress1 = 0x00;
	i2chandle_lcd.Init.OwnAddress2 = 0xFF;

	if (HAL_OK != HAL_I2C_Init(&i2chandle_lcd))
	{
		return false;
	}

	/*##-2- Configure the LCD reset pin ######################################*/
	GPIO_InitTypeDef gpioinitstruct;

	/* Enable the LCD_RESn Port Clock */
	LCD_RESn_GPIO_CLK_ENABLE()
	;

	/* Configure the LCD_RESn GPIO pin */
	gpioinitstruct.Pin = LCD_RESn_PIN;
	gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull = GPIO_PULLUP;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(LCD_RESn_GPIO_PORT, &gpioinitstruct);

	return true;
}

/**
 * @brief  Reset the LCD module by initialize sequence on LCD reset pin.
 * @param  None
 * @retval None
 */
void bsp_lcd_reset(void)
{
	LCD_RESn_DEACTIVE();
	bsp_delay_ms(1); /* wait 1ms for VDD of LCD module stable */
	LCD_RESn_ACTIVE(); /* reset LCD module */
	bsp_delay_ms(10); /* wait 10ms */
	LCD_RESn_DEACTIVE(); /* bring the LCD module out of reset */
}

/**
 * @brief  Transmit SSD1306 command to LCD module.
 * @param  ui8Command: 8-bit command.
 * @retval bool: Transmission result
 *			@arg true: completed
 *			@arg false: aborted
 */
bool bsp_lcd_sendCommand(uint8_t ui8Command)
{
	uint8_t pui8CommandPacket[LCD_COMMAND_PACKET_SIZE] =
	{ 0x00, /* Control byte: Co = 0, D/C = 0 'Command', six '0' */
	ui8Command /* Data byte */
	};

	/*  Before starting a new communication transfer, you need to check the current
	 state of the peripheral; if it's busy you need to wait for the end of current
	 transfer before starting a new one. */
	/* FIXME: For simplicity reasons, this example is just waiting till the end of the
	 transfer, but application may perform other tasks while transfer operation
	 is ongoing. */
	while (HAL_I2C_STATE_READY != HAL_I2C_GetState(&i2chandle_lcd));

	while (HAL_OK
			!= HAL_I2C_Master_Transmit_DMA(&i2chandle_lcd, g_ui16SlaveAddress,
					pui8CommandPacket, LCD_COMMAND_PACKET_SIZE))
	{
		/* When Acknowledge failure occurs (Slave don't acknowledge its address)
		 Master restarts communication. Transmission aborted when Timeout error occurs.*/
		if (HAL_I2C_ERROR_AF != HAL_I2C_GetError(&i2chandle_lcd))
		{
			return false;
		}
	}
	return true;
}

/**
 * @brief  Transmit SSD1306 data to LCD module.
 * @param  pui8Buffer: Data buffer pointer.
 * @param  ui32Size: Data buffer size in byte.
 * @retval bool: Transmission result
 *			@arg true: completed
 *			@arg false: aborted
 */
bool bsp_lcd_sendData(uint8_t * pui8Buffer, uint32_t ui32Size)
{
	/*  Before starting a new communication transfer, you need to check the current
	 state of the peripheral; if it's busy you need to wait for the end of current
	 transfer before starting a new one. */
	/* FIXME: For simplicity reasons, this example is just waiting till the end of the
	 transfer, but application may perform other tasks while transfer operation
	 is ongoing. */
	while (HAL_I2C_STATE_READY != HAL_I2C_GetState(&i2chandle_lcd));

	while (HAL_OK
			!= HAL_I2C_Master_Transmit_DMA(&i2chandle_lcd, g_ui16SlaveAddress,
					pui8Buffer, ui32Size))
	{
		/* When Acknowledge failure occurs (Slave don't acknowledge its address)
		 Master restarts communication. Transmission aborted when Timeout error occurs.*/
		if (HAL_I2C_ERROR_AF != HAL_I2C_GetError(&i2chandle_lcd))
		{
			return false;
		}
	}
	return true;
}

/**
 * @} BSP_LCD
 */
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
