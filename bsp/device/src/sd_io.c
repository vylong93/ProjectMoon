/**
 ****************************************************************************
 * @file        sd_io.c
 * @author      Long Dang
 * @version     V0.1
 * @date        25-March-2017
 * @copyright   LGPLv3
 * @brief       This file implement the SD I/O controller APIs.
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
/** @addtogroup BSP_DEVICE_SDIO
 * @{
 */
/** @defgroup BSP_DEVICE_SDIO_PRIVATE SDIO (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "sd_io.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/** @defgroup BSP_SPI_PERIPHERALS SPI2 Hardware communication interface
 * @{
 */
/* Definition for SPIx clock resources */
#define SPIx                               SPI2
#define SPIx_CLK_ENABLE()                  __HAL_RCC_SPI2_CLK_ENABLE()

#define SPIx_SCK_GPIO_PORT                 GPIOB             /* PB.13 */
#define SPIx_SCK_PIN                       GPIO_PIN_13
#define SPIx_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()

#define SPIx_MISO_MOSI_GPIO_PORT           GPIOB
#define SPIx_MISO_MOSI_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_MOSI_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOB_CLK_DISABLE()
#define SPIx_MISO_PIN                      GPIO_PIN_14       /* PB.14 */
#define SPIx_MOSI_PIN                      GPIO_PIN_15       /* PB.15 */

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
 on accurate values, they just guarantee that the application will not remain
 stuck if the SPI communication is corrupted.
 You may modify these timeout values depending on CPU frequency and application
 conditions (interrupts routines ...). */
#define SPIx_TIMEOUT_MAX                   (1000)
/**@}BSP_SPI_PERIPHERALS*/

/* SD Control Interface pins */
#define SD_CSn_PIN                               GPIO_PIN_8        /* PA.08*/
#define SD_CSn_GPIO_PORT                         GPIOA
#define SD_CSn_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOA_CLK_ENABLE()
#define SD_CSn_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOA_CLK_DISABLE()
#define SD_CSn_ACTIVE()       					HAL_GPIO_WritePin(SD_CSn_GPIO_PORT, SD_CSn_PIN, GPIO_PIN_RESET)
#define SD_CSn_DEACTIVE()      					HAL_GPIO_WritePin(SD_CSn_GPIO_PORT, SD_CSn_PIN, GPIO_PIN_SET)

/* SD Detect Interface pins */
#define SD_DETECT_PIN                           GPIO_PIN_9 /* PA.09 */
#define SD_DETECT_GPIO_PORT                     GPIOA
#define SD_DETECT_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define SD_DETECT_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()
#define SD_DETECT_EXTI_IRQn                     EXTI0_IRQn

/* Constant value for the SD device */
#define SD_COMMAND_PACKET_SIZE		(6) /*!< SPI command packet size for the SD device: 1 byte CMD, 4 byte ARG, 1 byte CRC */
#define SD_COMMAND_PREFIX			(0x40) /*!< SPI command prefix: 0b01xx.xxxx */
#define SD_COMMAND_MASK				(0x3F) /*!< SPI command mask: 0b00xx.xxxx */
#define SD_DUMMY_BYTE				(0xFF) /*!< Dummy byte of SPI data */
#define SD_NO_RESPONSE_EXPECTED  	(0x80) /*!< Indicate sending non-response SD command */

/** @addtogroup BSP_SPI_PERIPHERALS
 * @{
 */
/* Private variables ---------------------------------------------------------*/
static SPI_HandleTypeDef spihandle_sd; /*!< SPI handler for SD declaration. */

/* Private functions declaration ---------------------------------------------*/
static void SPI_MspInit(SPI_HandleTypeDef *hspi);
static void SPI_Error(void);

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief SPI MSP Initialization.
 *        This function configures the hardware resources used in this project:
 *           @arg Peripheral's clock enable
 *           @arg Peripheral's GPIO Configuration
 * @param hspi: SPI handle pointer
 * @retval None
 */
static void SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };

	/*** Configure the GPIOs ***/
	/* Enable GPIO clock */
	SPIx_SCK_GPIO_CLK_ENABLE()
	;
	SPIx_MISO_MOSI_GPIO_CLK_ENABLE()
	;

	/* configure SPI SCK */
	GPIO_InitStruct.Pin = SPIx_SCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

	/* configure SPI MISO and MOSI */
	GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
	HAL_GPIO_Init(SPIx_MISO_MOSI_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SPIx_MISO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(SPIx_MISO_MOSI_GPIO_PORT, &GPIO_InitStruct);

	/*** Configure the SPI peripheral ***/
	/* Enable SPI clock */
	SPIx_CLK_ENABLE()
	;
}

/**
 * @brief SPI error treatment function
 * @retval None
 */
static void SPI_Error(void)
{
	/* De-initialize the SPI communication BUS */
	HAL_SPI_DeInit(&spihandle_sd);

	/* Re-Initialize the SPI communication BUS */
	bsp_sdio_init();
}
/**@}BSP_SPI_PERIPHERALS*/

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Configures peripherals to control the SD device.
 * @retval bool: Status of initialization
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_sdio_init(void)
{
	/* SPI baudrate is set to 8 MHz maximum (PCLK2/SPI_BaudRatePrescaler = 64/8 = 8 MHz)
	 to verify these constraints:
	 - SD card SPI interface max baudrate is 25MHz for write/read
	 - PCLK2 max frequency is 32 MHz
	 */
	spihandle_sd.Instance = SPIx;
	spihandle_sd.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	spihandle_sd.Init.Direction = SPI_DIRECTION_2LINES;
	spihandle_sd.Init.CLKPhase = SPI_PHASE_1EDGE;
	spihandle_sd.Init.CLKPolarity = SPI_POLARITY_LOW;
	spihandle_sd.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spihandle_sd.Init.CRCPolynomial = 7;
	spihandle_sd.Init.DataSize = SPI_DATASIZE_8BIT;
	spihandle_sd.Init.FirstBit = SPI_FIRSTBIT_MSB;
	spihandle_sd.Init.NSS = SPI_NSS_SOFT;
	spihandle_sd.Init.TIMode = SPI_TIMODE_DISABLE;
	spihandle_sd.Init.Mode = SPI_MODE_MASTER;

	SPI_MspInit(&spihandle_sd);

	if (HAL_OK != HAL_SPI_Init(&spihandle_sd))
	{
		return false;
	}

	/* Configure the SD CSn and Detect pins */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* SD_CS_GPIO and SD_DETECT_GPIO Periph clock enable */
	SD_CSn_GPIO_CLK_ENABLE()
	;
	SD_DETECT_GPIO_CLK_ENABLE()
	;

	/* Configure SD_CS_PIN pin: SD Card CS pin */
	GPIO_InitStruct.Pin = SD_CSn_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SD_CSn_GPIO_PORT, &GPIO_InitStruct);

	/* Configure SD_DETECT_PIN pin: SD Card detect pin */
	GPIO_InitStruct.Pin = SD_DETECT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStruct);

	/* Enable and set SD EXTI Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(SD_DETECT_EXTI_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(SD_DETECT_EXTI_IRQn);

	/* Put the SD device into SPI mode */
	/* Send dummy byte 0xFF, 10 times with CS high */
	/* Rise CS and MOSI for 80 clocks cycles */
	SD_CSn_DEACTIVE();
	int i = 10;
	do
	{
		bsp_sdio_sendDummy();
	} while (--i);

	return true;
}

/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @retval bool: Returns if SD is detected or not
 *			@arg true: SD detected
 *			@arg false: No SD found
 */
bool bsp_sdio_isDetected(void)
{
	/* Check SD card detect pin */
	if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) != GPIO_PIN_RESET)
	{
		return false;
	}
	return true;
}

/**
 * @brief  Write a sequence of bytes to the SD device.
 * @param  pui8Buffer: Pointer to the buffer data need to be send.
 * @param  ui16Size: Data buffer size in byte.
 * @retval bool: Status of transmission
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_sdio_sendData(uint8_t * pui8Buffer, uint16_t ui16Size)
{
	HAL_StatusTypeDef status = HAL_SPI_Transmit(&spihandle_sd, pui8Buffer,
			ui16Size, SPIx_TIMEOUT_MAX);

	/* Check the communication status */
	if (HAL_OK != status)
	{
		/* Execute user timeout callback */
		SPI_Error();
		return false;
	}
	return true;
}

/**
 * @brief  Read a sequence of bytes from the SD device.
 * @param  pui8Buffer: Pointer to the buffer to store the received data.
 * @param  ui16Size: Requested data size in byte.
 * @note   Each byte in the buffer will be send and replace by the received data.
 * 			So that the initialize value of all the data in the buffer should equal to 0xFF for safe.
 * @retval bool: Status of transmission
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_sdio_readData(uint8_t * pui8Buffer, uint16_t ui16Size)
{
	/* NOTE: HAL_SPI_TransmitReceive require transmit data pointer same size with received data.
	 * We should prepare the same data size with fill by 0xFF to use this HAL function.
	 * This is memory wasting before each transaction size could up to 512-byte or higher.
	 * TODO: implement new API: HAL_SPI_Recieve without Transmit data required */
	HAL_StatusTypeDef status = HAL_OK;
	uint32_t ui32TransmisionDummyValue = 0xFFFFFFFF;
	uint32_t ui32DataCounter = 0;
	while (ui16Size--)
	{
		/* Read single byte at a time */
		status = HAL_SPI_TransmitReceive(&spihandle_sd, (uint8_t*) &ui32TransmisionDummyValue,
				(uint8_t*) (pui8Buffer + ui32DataCounter), 1, SPIx_TIMEOUT_MAX);
		++ui32DataCounter;
		/* Check the communication status */
		if (HAL_OK != status)
		{
			/* Execute user timeout callback */
			SPI_Error();
			return false;
		}
	}
	return true;
}

/**
 * @brief  Send 5 bytes command to the SD card and get the R3 response.
 * @param  ui8Cmd: The user expected command to send to SD device.
 * @param  ui32Arg: The command argument.
 * @param  ui8CRC: The CRC value of the command packet.
 * @param  ui8ExpectedResponse: Expected response from the SD device.
 * @retval bool: Status of transmission
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_sdio_sendCommand(uint8_t ui8Cmd, uint32_t ui32Arg, uint8_t ui8CRC,
		uint8_t ui8ExpectedResponse)
{
	uint8_t pui8CommandPacket[SD_COMMAND_PACKET_SIZE] =
	{ (SD_COMMAND_PREFIX | (ui8Cmd & SD_COMMAND_MASK)),
			(uint8_t) (ui32Arg >> 24), (uint8_t) (ui32Arg >> 16),
			(uint8_t) (ui32Arg >> 8), (uint8_t) (ui32Arg), ui8CRC };

	if (bsp_sdio_sendData(pui8CommandPacket, SD_COMMAND_PACKET_SIZE))
	{
		if (SD_NO_RESPONSE_EXPECTED != ui8ExpectedResponse)
		{
			return bsp_sdio_waitResponse(ui8ExpectedResponse);
		}
		return true;
	}
	return false;
}

/**
 * @brief  Send 5 bytes command to the SD card and get the R7 response.
 * @param  ui8Cmd: The user expected command to send to SD device.
 * @param  ui32Arg: The command argument.
 * @param  ui8CRC: The CRC value of the command packet.
 * @param  ui8ExpectedResponse: Expected response from the SD device.
 * @param  pui32TrailingResponse: Additional 4 trailing byte buffer pointer of R7 response.
 * @retval bool: Status of transmission
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_sdio_sendSpecialCommand(uint8_t ui8Cmd, uint32_t ui32Arg, uint8_t ui8CRC,
		uint8_t ui8ExpectedResponse, uint32_t *pui32TrailingResponse)
{
	if (bsp_sdio_sendCommand(ui8Cmd, ui32Arg, ui8CRC, ui8ExpectedResponse))
	{
		if (SD_NO_RESPONSE_EXPECTED != ui8ExpectedResponse)
		{
			uint8_t *pui8TrailingByte = (uint8_t *) pui32TrailingResponse;
			/* Read more 4 trailing byte of R7 response */
			*(pui8TrailingByte + 3) = SD_DUMMY_BYTE;
			*(pui8TrailingByte + 2) = SD_DUMMY_BYTE;
			*(pui8TrailingByte + 1) = SD_DUMMY_BYTE;
			*pui8TrailingByte = SD_DUMMY_BYTE;

			bsp_sdio_readData((uint8_t *) (pui8TrailingByte + 3), 1);
			bsp_sdio_readData((uint8_t *) (pui8TrailingByte + 2), 1);
			bsp_sdio_readData((uint8_t *) (pui8TrailingByte + 1), 1);
			bsp_sdio_readData((uint8_t *) (pui8TrailingByte), 1);
		}
		return true;
	}
	return false;
}

/**
 * @brief  Wait response from the SD card
 * @param  ui8ExpectedResponse: Expected response from the SD card.
 * @retval bool: Status of the process
 *			@arg true: received expected response.
 *			@arg false: timeout
 */
bool bsp_sdio_waitResponse(const uint8_t ui8ExpectedResponse)
{
	uint8_t ui8Response;
	uint32_t ui32Timeout = 0xFFFF;

	/* Try to get the expected response */
	do
	{
		ui8Response = SD_DUMMY_BYTE;
		bsp_sdio_readData(&ui8Response, 1);
		--ui32Timeout;
	} while ((ui8Response != ui8ExpectedResponse) && ui32Timeout);

	/* Check if response is got or a timeout is happen */
	if (0 == ui32Timeout)
	{
		return false;
	}
	return true;
}

/**
 * @brief  Send a dummy byte to SD device.
 * @retval None
 */
void bsp_sdio_sendDummy(void)
{
	uint8_t ui8Dummy = SD_DUMMY_BYTE;
	bsp_sdio_sendData(&ui8Dummy, 1);
}

/**
 * @brief  Activate the SD communication
 * @retval None
 */
void bsp_sdio_activate(void)
{
	SD_CSn_ACTIVE();
}

/**
 * @brief  Send a dummy byte to SD device.
 * @retval None
 */
void bsp_sdio_deactivate(void)
{
	SD_CSn_DEACTIVE();
}

/**@}BSP_DEVICE_SDIO_PRIVATE*/
/**@}BSP_DEVICE_SDIO*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
