/**
 ****************************************************************************
 * @file        audio_codec_io.c
 * @author      Long Dang
 * @version     V0.1
 * @date        02-April-2017
 * @copyright   LGPLv3
 * @brief       This file implement the VS1003 device IO controller.
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
/** @addtogroup BSP_DEVICE_ACODEC
 * @{
 */
/** @defgroup BSP_DEVICE_ACODEC_PRIVATE Audio CODEC I/O (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "audio_codec_io.h"

/* Private typedef -----------------------------------------------------------*/
/** @defgroup BSP_SPI1_PERIPHERALS SPI1 Hardware communication interface
 * @{
 */
/**
 * @typedef spi_clockspeed_t
 * This type define two SPI clock speed mode for VS1003 device.
 */
typedef enum
{
	CLOCK_SLOW = SPI_BAUDRATEPRESCALER_64, /*!< 1MHz */
	CLOCK_FAST = SPI_BAUDRATEPRESCALER_16 /*!< 4MHz */
} spi_clockspeed_t;
/**@}BSP_SPI1_PERIPHERALS*/

/* Private define ------------------------------------------------------------*/
/** @addtogroup BSP_SPI1_PERIPHERALS
 * @{
 */
#define SPI2x                               SPI1
#define SPI2x_CLK_ENABLE()                  __HAL_RCC_SPI1_CLK_ENABLE()

#define SPI2x_SCK_GPIO_PORT                 GPIOA             /* PA.5 */
#define SPI2x_SCK_PIN                       GPIO_PIN_5
#define SPI2x_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPI2x_SCK_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()

#define SPI2x_MISO_MOSI_GPIO_PORT           GPIOA
#define SPI2x_MISO_MOSI_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPI2x_MISO_MOSI_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOA_CLK_DISABLE()
#define SPI2x_MISO_PIN                      GPIO_PIN_6       /* PA.6 */
#define SPI2x_MOSI_PIN                      GPIO_PIN_7       /* PA.7 */

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
 on accurate values, they just guarantee that the application will not remain
 stuck if the SPI communication is corrupted.
 You may modify these timeout values depending on CPU frequency and application
 conditions (interrupts routines ...). */
#define SPI2x_TIMEOUT_MAX                   1000
/**@}BSP_SPI1_PERIPHERALS*/

/* Constant value for the VS10xx device */
/**
 * @brief Serial Control Interface Opcode definition
 */
#define SCI_READ_OPCODE		(0x03) /*!< Operation code for Write command */
#define SCI_WRITE_OPCODDE	(0x02) /*!< Operation code for Read command */
#define VS10xx_CONTROL_PACKET_SIZE	(4) /*!< SPI control packet size for the VS10xx device: 1 byte Opcode, 1 byte Reg Address, 2 bytes Reg value */
#define VS10xx_DUMMY_BYTE			(0xFF) /*!< Dummy byte of SPI data */
#define VS10xx_CHUNK_SIZE	(32) /*!< VS1003 can take at least 32 bytes of SDI data when DREQ is high. The transmission should stop during DREQ low. */

/**
 * @brief VS10xx Control Interface pins
 */
#define VS10xx_CS_PIN                             GPIO_PIN_4        /* PA.4 */
#define VS10xx_CS_GPIO_PORT                       GPIOA
#define VS10xx_CS_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define VS10xx_CS_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOA_CLK_DISABLE()

/**
 * @brief VS10xx Hardware Reset pins
 */
#define VS10xx_RST_PIN                            GPIO_PIN_3        /* PA.3 */
#define VS10xx_RST_GPIO_PORT                      GPIOA
#define VS10xx_RST_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define VS10xx_RST_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

/**
 * @brief VS10xx Data Request Interface pins: 0 mean STOP transfer SCI or SDI
 */
#define VS10xx_DREQ_PIN                           GPIO_PIN_2        /* PA.2 */
#define VS10xx_DREQ_GPIO_PORT                     GPIOA
#define VS10xx_DREQ_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define VS10xx_DREQ_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()

/* Private macro -------------------------------------------------------------*/
/**
 * @brief Chip Select macro definition
 */
#define VS10xx_SCI_ACTIVATE()		HAL_GPIO_WritePin(VS10xx_CS_GPIO_PORT, VS10xx_CS_PIN, GPIO_PIN_RESET)
#define VS10xx_SCI_DEACTIVATE()		HAL_GPIO_WritePin(VS10xx_CS_GPIO_PORT, VS10xx_CS_PIN, GPIO_PIN_SET)

/**
 * @brief Data Chip Select macro definition - [SM_SDINEW = 1 && SM_SDI_SHARE = 1]
 */
#define VS10xx_SDI_ACTIVATE()		VS10xx_SCI_DEACTIVATE()
#define VS10xx_SDI_DEACTIVATE()		VS10xx_SCI_ACTIVATE()

/**
 * @brief Chip Reset macro definition: t_active: 2 XTALI; t_inactive: 16600-50000 XTALI or DREQ rises
 */
#define VS10xx_RST_ACTIVATE()		HAL_GPIO_WritePin(VS10xx_RST_GPIO_PORT, VS10xx_RST_PIN, GPIO_PIN_RESET)
#define VS10xx_RST_DEACTIVATE()		HAL_GPIO_WritePin(VS10xx_RST_GPIO_PORT, VS10xx_RST_PIN, GPIO_PIN_SET)

/**
 * @brief Chip Reset macro definition: t_active: 2 XTALI; t_inactive: 16600-50000 XTALI or DREQ rises
 */
#define VS10xx_AWAIT_DATA_REQUEST()	while(HAL_GPIO_ReadPin(VS10xx_DREQ_GPIO_PORT, VS10xx_DREQ_PIN) == GPIO_PIN_RESET)
#define VS10xx_READ_DATA_REQUEST() HAL_GPIO_ReadPin(VS10xx_DREQ_GPIO_PORT, VS10xx_DREQ_PIN)

/** @addtogroup BSP_SPI1_PERIPHERALS
 * @{
 */
/* Private variables ---------------------------------------------------------*/
static SPI_HandleTypeDef spihandle_vs10xx; /*!< SPI handler for VS1003 declaration. */

/* Private functions declaration ---------------------------------------------*/
static bool SPI2x_Init(spi_clockspeed_t clockSpeed);
static void SPI2x_MspInit(SPI_HandleTypeDef *hspi);
static void SPI2x_Error(void);

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Initializes SPI HAL.
 * @param  clockSpeed: Configure the SPI communication clock speed.
 * 			@arg CLOCK_SLOW
 * 			@arg CLOCK_FAST
 * @retval None
 */
static bool SPI2x_Init(spi_clockspeed_t clockSpeed)
{
	/* DeInitializes the SPI peripheral */
	spihandle_vs10xx.Instance = SPI2x;
	HAL_SPI_DeInit(&spihandle_vs10xx);

	/* SPI baudrate is set to 1 MHz or 4MHz depend on the input argument,
	 to verify these constraints:
	 - VS1003 SPI interface max baudrate is CLKI/7 for SCI read and CLKI/4 for SCI and SDI write
	 - CLKI = 12.288 MHz (XTAL) x Clock Multiplier (1.0x -> 4.5x) SC_MULT bits in SCI_CLOCKF reg
	 */
	spihandle_vs10xx.Init.BaudRatePrescaler = clockSpeed;
	spihandle_vs10xx.Init.Direction = SPI_DIRECTION_2LINES;
	spihandle_vs10xx.Init.CLKPhase = SPI_PHASE_2EDGE;
	spihandle_vs10xx.Init.CLKPolarity = SPI_POLARITY_HIGH;
	spihandle_vs10xx.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spihandle_vs10xx.Init.CRCPolynomial = 7;
	spihandle_vs10xx.Init.DataSize = SPI_DATASIZE_8BIT;
	spihandle_vs10xx.Init.FirstBit = SPI_FIRSTBIT_MSB;
	spihandle_vs10xx.Init.NSS = SPI_NSS_SOFT;
	spihandle_vs10xx.Init.TIMode = SPI_TIMODE_DISABLE;
	spihandle_vs10xx.Init.Mode = SPI_MODE_MASTER;

	SPI2x_MspInit(&spihandle_vs10xx);

	if (HAL_OK != HAL_SPI_Init(&spihandle_vs10xx))
	{
		return false;
	}
	return true;
}

/**
 * @brief SPI MSP Initialization.
 *        This function configures the hardware resources used in this project:
 *           @arg Peripheral's clock enable
 *           @arg Peripheral's GPIO Configuration
 * @param hspi: SPI handle pointer
 * @retval None
 */
static void SPI2x_MspInit(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };

	/*** Configure the GPIOs ***/
	/* Enable GPIO clock */
	SPI2x_SCK_GPIO_CLK_ENABLE()
	;
	SPI2x_MISO_MOSI_GPIO_CLK_ENABLE()
	;

	/* configure SPI SCK */
	GPIO_InitStruct.Pin = SPI2x_SCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SPI2x_SCK_GPIO_PORT, &GPIO_InitStruct);

	/* configure SPI MISO and MOSI */
	GPIO_InitStruct.Pin = SPI2x_MOSI_PIN;
	HAL_GPIO_Init(SPI2x_MISO_MOSI_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SPI2x_MISO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(SPI2x_MISO_MOSI_GPIO_PORT, &GPIO_InitStruct);

	/*** Configure the SPI peripheral ***/
	/* Enable SPI clock */
	SPI2x_CLK_ENABLE()
	;
}

/**
 * @brief SPI error treatment function
 * @retval None
 */
static void SPI2x_Error(void)
{
	/* De-initialize the SPI communication BUS */
	HAL_SPI_DeInit(&spihandle_vs10xx);

	/* Re- Initiaize the SPI communication BUS */
	SPI2x_Init(CLOCK_SLOW);
}
/**@}BSP_SPI1_PERIPHERALS*/

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Configures peripherals to control the VS1003 device.
 * @retval bool: Status of initialization
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_acodec_init(void)
{
	/* 1. Init SPI peripheral in slow speed mode */
	if (!SPI2x_Init(CLOCK_SLOW))
	{
		return false;
	}

	/* 2. Init VS1003 specific control pin */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* VS10xx_CS_GPI, VS10xx_RST_GPIO and VS10xx_DREQ_GPIO Periph clock enable */
	VS10xx_CS_GPIO_CLK_ENABLE()
	;
	VS10xx_RST_GPIO_CLK_ENABLE()
	;
	VS10xx_DREQ_GPIO_CLK_ENABLE()
	;

	/* Configure VS10xx_CS_PIN pin: VS10xx CS pin */
	GPIO_InitStruct.Pin = VS10xx_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(VS10xx_CS_GPIO_PORT, &GPIO_InitStruct);

	/* Configure VS10xx_RST_PIN pin: VS10xx Reset pin */
	GPIO_InitStruct.Pin = VS10xx_RST_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(VS10xx_RST_GPIO_PORT, &GPIO_InitStruct);

	/* Configure VS10xx_DREQ_PIN pin: VS10xx Data Request pin */
	GPIO_InitStruct.Pin = VS10xx_DREQ_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(VS10xx_DREQ_GPIO_PORT, &GPIO_InitStruct);

	return true;
}

/**
 * @brief  Generate hardware reset sequence to the VS1003 device.
 * @retval None
 */
void bsp_acodec_reset(void)
{
	/* Write here your MCU code which puts VS10xx in hardware
	 reset and back (set xRESET to 0 for at least a few clock cycles,
	 then to 1). */

	/* VS10xx HW reset - asynchronous = have effect immediately  */
	VS10xx_RST_ACTIVATE();

	/* Must delay in 2 XTALI (12.288MHz) */
	bsp_delay_ms(1);

	/* Release Reset */
	VS10xx_RST_DEACTIVATE();

	/* DREQ will stay down for at least 16600 clock cycles ~ 1.35ms with CLKI = 12.288MHz.
	 After this the user should set such basic software registers as SCI_MODE, SCI_BASS,
	 SCI_CLOCKF, and SCI_VOL before starting decoding. */
	bsp_delay_ms(2); /* Must delay in 2 XTALI (12.288MHz) */
}

/**
 * @brief  Check if the data request pin is low state (busy).
 * 			The DREQ pin/signal is used to signal if VS1003's FIFO is capable of receiving data.
 * 			If DREQ is high, VS1003 can take at least 32 bytes of SDI data or one SCI command.
 * 			When these criteria are not met, DREQ is turned low, and the sender should stop transferring new data.
 * @retval bool: Busy status of the FIFO of the device
 *			@arg true: busy
 *			@arg false: available for transmission
 */
bool bsp_acodec_isDeviceBusy(void)
{
	return (GPIO_PIN_RESET
			== HAL_GPIO_ReadPin(VS10xx_DREQ_GPIO_PORT, VS10xx_DREQ_PIN)) ?
			(true) : (false);
}

/**
 * @brief  Write to VS1003's register.
 * @param  ui8Address: Register's address.
 * @param  ui16Value: Target register's value.
 * @retval bool: Status of transmission
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_acodec_writeRegsiter(uint8_t ui8Address, uint16_t ui16Value)
{
	uint8_t pui8ControlPacket[VS10xx_CONTROL_PACKET_SIZE] =
	{ SCI_WRITE_OPCODDE, /* Write operation */
	ui8Address, /* Which register */
	(uint8_t) (ui16Value >> 8), /* High byte */
	(uint8_t) (ui16Value) /* Low byte */
	};

	VS10xx_AWAIT_DATA_REQUEST();

	/* SCI select low */
	VS10xx_SCI_ACTIVATE();

	HAL_StatusTypeDef status = HAL_SPI_Transmit(&spihandle_vs10xx,
			pui8ControlPacket, VS10xx_CONTROL_PACKET_SIZE, SPI2x_TIMEOUT_MAX);
	/* Check the communication status */
	if (HAL_OK != status)
	{
		/* Execute user timeout callback */
		SPI2x_Error();
		VS10xx_SCI_DEACTIVATE();
		return false;
	}
	VS10xx_AWAIT_DATA_REQUEST();

	/* SCI select high */
	VS10xx_SCI_DEACTIVATE();
	return true;
}

/**
 * @brief  Read VS1003's register.
 * @param  ui8Address: Register's address.
 * @param  pui16Value: Register's value pointer.
 * @retval bool: Status of transmission
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_acodec_readRegsiter(uint8_t ui8Address, uint16_t *pui16Value)
{
	/* Note: If SPI speed low is 1MHz and speed high is 4MHz,
	 then no need to wrap this function by
	 SPI2x_Init(SPI_SPEED_LOW);
	 ReadSci
	 SPI2x_Init(SPI_SPEED_HIGH);
	 Because after increase VS10xx speed to 36.864MHz
	 SPI Read max: CLKI/7 = 5.2MHz > 4MHz
	 SPI Write max: CLKI/4 = 9.2MHz > 4MHz
	 */
	uint8_t pui8ControlPacketReceive[VS10xx_CONTROL_PACKET_SIZE];
	uint8_t pui8ControlPacketTransmit[VS10xx_CONTROL_PACKET_SIZE] =
	{ SCI_READ_OPCODE, /* Read operation */
	ui8Address, /* Which register */
	VS10xx_DUMMY_BYTE, VS10xx_DUMMY_BYTE };

	VS10xx_AWAIT_DATA_REQUEST();

	/* SCI select low */
	VS10xx_SCI_ACTIVATE();

	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&spihandle_vs10xx,
			pui8ControlPacketTransmit, pui8ControlPacketReceive,
			VS10xx_CONTROL_PACKET_SIZE, SPI2x_TIMEOUT_MAX);
	/* Check the communication status */
	if (status != HAL_OK)
	{
		/* Execute user timeout callback */
		SPI2x_Error();
		VS10xx_SCI_DEACTIVATE();
		return false;
	}

	/* Note: the first and second received bytes are dummy data */
	*pui16Value = (pui8ControlPacketReceive[2] << 8) /* Read high byte */
	| (pui8ControlPacketReceive[3]); /* Read low byte */

	VS10xx_AWAIT_DATA_REQUEST();

	/* SCI select high */
	VS10xx_SCI_DEACTIVATE();
	return true;
}

/**
 * @brief  Send a bunk of data to VS1003 device.
 * @param  pui8Buffer: Pointer to the buffer contain transmission data.
 * @param  ui32Size: Transmit data size in byte.
 * @retval bool: Status of transmission
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_acodec_sendData(const uint8_t *pui8Buffer, uint32_t ui32Size)
{
	VS10xx_AWAIT_DATA_REQUEST();

	/* SDI select low */
	VS10xx_SDI_ACTIVATE();

	uint32_t ui32ChunkLength;
	uint32_t ui32Offset = 0;
	while (ui32Size)
	{
		ui32ChunkLength =
				(ui32Size < VS10xx_CHUNK_SIZE) ?
						(ui32Size) : (VS10xx_CHUNK_SIZE);

		/* Sync every VS1003_CHUNK_SIZE */
		VS10xx_AWAIT_DATA_REQUEST();

		/* Transfer data */
		HAL_StatusTypeDef status = HAL_SPI_Transmit(&spihandle_vs10xx,
				(uint8_t *) (pui8Buffer + ui32Offset), ui32ChunkLength,
				SPI2x_TIMEOUT_MAX);
		/* Check the communication status */
		if (HAL_OK != status)
		{
			/* Execute user timeout callback */
			SPI2x_Error();
			VS10xx_SCI_DEACTIVATE();
			return false;
		}

		ui32Offset += ui32ChunkLength;
		ui32Size -= ui32ChunkLength;
	}

	/* SDI select high */
	VS10xx_SDI_DEACTIVATE();
	return true;
}

/**
 * @brief  Send a bunk of repeated data byte to VS1003 device.
 * @param  ui8DataByte: Transmit repeated data value.
 * @param  ui32Size: Transmit data size in byte.
 * @retval bool: Status of transmission
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool bsp_acodec_sendDataRepeatedly(uint8_t ui8DataByte, uint32_t ui32Size)
{
	VS10xx_AWAIT_DATA_REQUEST();

	/* SDI select low */
	VS10xx_SDI_ACTIVATE();

	uint32_t ui32ChunkLength;
	while (ui32Size)
	{
		ui32ChunkLength =
				(ui32Size < VS10xx_CHUNK_SIZE) ?
						(ui32Size) : (VS10xx_CHUNK_SIZE);

		/* Sync every VS1003_CHUNK_SIZE */
		VS10xx_AWAIT_DATA_REQUEST();

		/* Transfer data */
		HAL_StatusTypeDef status = HAL_SPI_Transmit(&spihandle_vs10xx,
				&ui8DataByte, ui32ChunkLength, SPI2x_TIMEOUT_MAX);
		/* Check the communication status */
		if (HAL_OK != status)
		{
			/* Execute user timeout callback */
			SPI2x_Error();
			VS10xx_SCI_DEACTIVATE();
			return false;
		}

		ui32Size -= ui32ChunkLength;
	}

	/* SDI select high */
	VS10xx_SDI_DEACTIVATE();
	return true;
}

/**@}BSP_DEVICE_ACODEC_PRIVATE*/
/**@}BSP_DEVICE_ACODEC*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
