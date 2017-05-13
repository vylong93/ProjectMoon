/**
 ****************************************************************************
 * @file        sd.c
 * @author      Long Dang
 * @version     V0.1
 * @date        19-March-2017
 * @copyright   LGPLv3
 * @brief       This file implement the SD Driver.
 * 				Reference to the stm3210c_eval_sd.c driver of STMicroelectronics.
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
/** @addtogroup BSP_DRV_SD
 * @{
 */
/** @defgroup BSP_DRV_SD_PRIVATE SD (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "sd.h"

/* Private typedef -----------------------------------------------------------*/
/**
 * @typedef sd_response_t
 * This type define the SD command response and Data response.
 */
typedef enum
{ /**
 * @brief  SD command response and error flags
 */
	SD_RESPONSE_NO_ERROR = (0x00),
	SD_IN_IDLE_STATE = (0x01),
	SD_ERASE_RESET = (0x02),
	SD_ILLEGAL_COMMAND = (0x04),
	SD_COM_CRC_ERROR = (0x08),
	SD_ERASE_SEQUENCE_ERROR = (0x10),
	SD_ADDRESS_ERROR = (0x20),
	SD_PARAMETER_ERROR = (0x40),
	SD_RESPONSE_FAILURE = (0xFF),

	/**
	 * @brief  Data response error
	 */
	SD_DATA_OK = (0x05),
	SD_DATA_CRC_ERROR = (0x0B),
	SD_DATA_WRITE_ERROR = (0x0D),
	SD_DATA_OTHER_ERROR = (0xFF)
} sd_response_t;

/**
 * @typedef sd_software_status_t
 * This type define the SD mode.
 */
typedef enum
{
	SD_NOT_IN_SPI_IDLE = 0, /*!< SD not in SPI mode */
	SD_IN_SPI_IDLE = 1 /*!< SD already in SPI mode */
} sd_software_status_t;

/* Private define ------------------------------------------------------------*/
/**
 * @brief  SD Commands: CMDxx = CMD-number | 0x40
 */
#define SD_CMD_GO_IDLE_STATE          (0)   /*!< CMD0 = 0x40 */
#define SD_CMD_SEND_OP_COND           (1)   /*!< CMD1 = 0x41 */
#define SD_CMD_SEND_IF_COND			  (8)   /*!< CMD8 = 0x48 */
#define SD_CMD_SEND_CSD               (9)   /*!< CMD9 = 0x49 */
#define SD_CMD_SEND_CID               (10)  /*!< CMD10 = 0x4A */
#define SD_CMD_STOP_TRANSMISSION      (12)  /*!< CMD12 = 0x4C */
#define SD_CMD_SEND_STATUS            (13)  /*!< CMD13 = 0x4D */
#define SD_CMD_SET_BLOCKLEN           (16)  /*!< CMD16 = 0x50 */
#define SD_CMD_READ_SINGLE_BLOCK      (17)  /*!< CMD17 = 0x51 */
#define SD_CMD_READ_MULT_BLOCK        (18)  /*!< CMD18 = 0x52 */
#define SD_CMD_SET_BLOCK_COUNT        (23)  /*!< CMD23 = 0x57 */
#define SD_CMD_WRITE_SINGLE_BLOCK     (24)  /*!< CMD24 = 0x58 */
#define SD_CMD_WRITE_MULT_BLOCK       (25)  /*!< CMD25 = 0x59 */
#define SD_CMD_PROG_CSD               (27)  /*!< CMD27 = 0x5B */
#define SD_CMD_SET_WRITE_PROT         (28)  /*!< CMD28 = 0x5C */
#define SD_CMD_CLR_WRITE_PROT         (29)  /*!< CMD29 = 0x5D */
#define SD_CMD_SEND_WRITE_PROT        (30)  /*!< CMD30 = 0x5E */
#define SD_CMD_SD_ERASE_GRP_START     (32)  /*!< CMD32 = 0x60 */
#define SD_CMD_SD_ERASE_GRP_END       (33)  /*!< CMD33 = 0x61 */
#define SD_CMD_UNTAG_SECTOR           (34)  /*!< CMD34 = 0x62 */
#define SD_CMD_ERASE_GRP_START        (35)  /*!< CMD35 = 0x63 */
#define SD_CMD_ERASE_GRP_END          (36)  /*!< CMD36 = 0x64 */
#define SD_CMD_UNTAG_ERASE_GROUP      (37)  /*!< CMD37 = 0x65 */
#define SD_CMD_ERASE                  (38)  /*!< CMD38 = 0x66 */
#define SD_CMD_APP_SEND_OP_COND	      (41)  /*!< CMD41 = 0x69 */
#define SD_CMD_APP_CMD				  (55)  /*!< CMD55 = 0x77 */
#define SD_CMD_READ_OCR				  (58)  /*!< CMD58 = 0x7A */

/**
 * @brief  Start Data tokens:
 *         Tokens (necessary because at NOP/idle (and CS active) only 0xff is
 *         on the data/command line)
 */
#define SD_START_DATA_SINGLE_BLOCK_READ    (0xFE)  /*!< Data token start byte, Start Single Block Read */
#define SD_START_DATA_MULTIPLE_BLOCK_READ  (0xFE)  /*!< Data token start byte, Start Multiple Block Read */
#define SD_START_DATA_SINGLE_BLOCK_WRITE   (0xFE)  /*!< Data token start byte, Start Single Block Write */
#define SD_START_DATA_MULTIPLE_BLOCK_WRITE (0xFD)  /*!< Data token start byte, Start Multiple Block Write */
#define SD_STOP_DATA_MULTIPLE_BLOCK_WRITE  (0xFD)  /*!< Data token stop byte, Stop Multiple Block Write */

/* Only need to correct the CRC of the first two CMD */
#define SD_CRC_CMD_GO_IDLE_STATE	(0x95)
#define SD_CRC_CMD_SEND_IF_COND		(0x87)
#define SD_CRC_NOT_CARE				(0xFF)

#define SD_NUMBER_OF_CSD_RESPONSE_BYTE	(16)
#define SD_NUMBER_OF_CID_RESPONSE_BYTE	(16)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
sd_hardware_status_t g_sdStatus = SD_NOT_PRESENT;
sd_software_status_t g_sdSoftareStatus = SD_NOT_IN_SPI_IDLE;

/* Private functions declaration ---------------------------------------------*/
static bool sd_goIdleState(void);
static bool sd_getCSDRegister(sd_csd_t* pCsd);
static bool sd_getCIDRegister(sd_cid_t* pCid);
static sd_response_t sd_getDataResponse(void);

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Put SD in Idle state.
 * @retval bool: The SD Response
 *			@arg true: Sequence succeed
 *			@arg false: Sequence failed
 */
static bool sd_goIdleState(void)
{
	bool bReturn = false;
	/* Send CMD0 (SD_CMD_GO_IDLE_STATE) to put SD in SPI mode and
	 Wait for In Idle State Response (R1 Format) equal to 0x01 */
	bsp_sdio_activate();
	bReturn = bsp_sdio_sendCommand(SD_CMD_GO_IDLE_STATE, 0,
	SD_CRC_CMD_GO_IDLE_STATE, SD_IN_IDLE_STATE);
	bsp_sdio_deactivate();
	bsp_sdio_sendDummy();
	if (false == bReturn)
	{
		/* No Idle State Response: return response failure */
		return false;
	}

	/*----------Activates the card initialization process-----------*/
	/* Send CMD8 Wait for no error Response (R7 Format) Response 0x01 0x000001AA */
	bsp_sdio_activate();
	uint32_t ui32TrailingResponse = 0;
	bReturn = bsp_sdio_sendSpecialCommand(SD_CMD_SEND_IF_COND, 0x000001AA,
	SD_CRC_CMD_SEND_IF_COND, SD_IN_IDLE_STATE, &ui32TrailingResponse);
	bsp_sdio_deactivate();
	bsp_sdio_sendDummy();
	if ((false == bReturn) || (0x01AA != (ui32TrailingResponse & 0x0FFF)))
	{
		/* No Idle State Response: return response failure
		 or Mismatch trailing response */
		return false;
	}

	/* Send CMD55-ACMD41 until response equal to 0x0 and
	 Wait for no error Response (R1 Format) equal to 0x00 */
	while (true)
	{
		bsp_sdio_activate();
		bsp_sdio_sendCommand(SD_CMD_APP_CMD, 0, SD_CRC_NOT_CARE,
				SD_IN_IDLE_STATE); /* Response 0x01 */
		bsp_sdio_deactivate();
		bsp_sdio_sendDummy();

		bsp_sdio_activate();
		bReturn = bsp_sdio_sendCommand(SD_CMD_APP_SEND_OP_COND, 0x40000000,
		SD_CRC_NOT_CARE, SD_RESPONSE_NO_ERROR);
		bsp_sdio_deactivate();
		bsp_sdio_sendDummy();
		if (true == bReturn)
		{
			break;
		}
	}

	/* Send CMD58 */
	bsp_sdio_activate();
	ui32TrailingResponse = 0;
	bReturn = bsp_sdio_sendSpecialCommand(SD_CMD_READ_OCR, 0, SD_CRC_NOT_CARE,
			SD_RESPONSE_NO_ERROR, &ui32TrailingResponse);
	bsp_sdio_deactivate();
	bsp_sdio_sendDummy();
	if (false == bReturn)
	{
		/* Error occur. Return response failure. Should not be here */
		return false;
	}

	/* Check CCS flag (bit 30) in OCR register */
	if (0 == (ui32TrailingResponse & 0x40000000))
	{
		/* SD Ver.2 Byte address - Force block size to 512 bytes to work with FAT file system */
		bsp_sdio_activate();
		bReturn = bsp_sdio_sendCommand(SD_CMD_SET_BLOCKLEN, SD_BLOCK_SIZE,
		SD_CRC_NOT_CARE, SD_RESPONSE_NO_ERROR);
		bsp_sdio_deactivate();
		bsp_sdio_sendDummy();
		if (false == bReturn)
		{
			/* Error occur. Return response failure. Should not be here */
			return false;
		}
	}
	/* else: SD Ver.2 Block address */
	return true;
}

/**
 * @brief  Read the CSD card register.
 *         Reading the contents of the CSD register in SPI mode is a simple
 *         read-block transaction.
 * @param  pCsd: pointer on an SCD register structure
 * @retval bool: The SD Response
 *			@arg true: Read succeed
 *			@arg false: Read failed
 */
static bool sd_getCSDRegister(sd_csd_t* pCsd)
{
	bool bReturn = false;
	uint8_t pui8CsdResponseArray[SD_NUMBER_OF_CSD_RESPONSE_BYTE];

	/* Send CMD9 (CSD register) or CMD10(CSD register) and
	 * Wait for response in the R1 format (0x00 is no errors) */
	bsp_sdio_activate();
	if (bsp_sdio_sendCommand(SD_CMD_SEND_CSD, 0, SD_CRC_NOT_CARE,
			SD_RESPONSE_NO_ERROR))
	{
		if (bsp_sdio_waitResponse(SD_START_DATA_SINGLE_BLOCK_READ))
		{
			/* Store CSD register value on CSD_Tab */
			bsp_sdio_readData(pui8CsdResponseArray,
			SD_NUMBER_OF_CSD_RESPONSE_BYTE);

			/* Get CRC bytes (not really needed by us, but required by SD) */
			bsp_sdio_sendDummy();
			bsp_sdio_sendDummy();

			bReturn = true;
		}
	}
	bsp_sdio_deactivate();

	/* Send dummy byte: 8 Clock pulses of delay */
	bsp_sdio_sendDummy();

	if (true == bReturn)
	{
		/* Byte 0 */
		pCsd->CSDStruct = (pui8CsdResponseArray[0] & 0xC0) >> 6;
		pCsd->SysSpecVersion = (pui8CsdResponseArray[0] & 0x3C) >> 2;
		pCsd->Reserved1 = pui8CsdResponseArray[0] & 0x03;

		/* Byte 1 */
		pCsd->TAAC = pui8CsdResponseArray[1];

		/* Byte 2 */
		pCsd->NSAC = pui8CsdResponseArray[2];

		/* Byte 3 */
		pCsd->MaxBusClkFrec = pui8CsdResponseArray[3];

		/* Byte 4 */
		pCsd->CardComdClasses = pui8CsdResponseArray[4] << 4;

		/* Byte 5 */
		pCsd->CardComdClasses |= (pui8CsdResponseArray[5] & 0xF0) >> 4;
		pCsd->RdBlockLen = pui8CsdResponseArray[5] & 0x0F;

		/* Byte 6 */
		pCsd->PartBlockRead = (pui8CsdResponseArray[6] & 0x80) >> 7;
		pCsd->WrBlockMisalign = (pui8CsdResponseArray[6] & 0x40) >> 6;
		pCsd->RdBlockMisalign = (pui8CsdResponseArray[6] & 0x20) >> 5;
		pCsd->DSRImpl = (pui8CsdResponseArray[6] & 0x10) >> 4;
		pCsd->Reserved2 = 0; /*!< Reserved */

		pCsd->DeviceSize = (pui8CsdResponseArray[6] & 0x03) << 10;

		/* Byte 7 */
		pCsd->DeviceSize |= (pui8CsdResponseArray[7]) << 2;

		/* Byte 8 */
		pCsd->DeviceSize |= (pui8CsdResponseArray[8] & 0xC0) >> 6;

		pCsd->MaxRdCurrentVDDMin = (pui8CsdResponseArray[8] & 0x38) >> 3;
		pCsd->MaxRdCurrentVDDMax = (pui8CsdResponseArray[8] & 0x07);

		/* Byte 9 */
		pCsd->MaxWrCurrentVDDMin = (pui8CsdResponseArray[9] & 0xE0) >> 5;
		pCsd->MaxWrCurrentVDDMax = (pui8CsdResponseArray[9] & 0x1C) >> 2;
		pCsd->DeviceSizeMul = (pui8CsdResponseArray[9] & 0x03) << 1;
		/* Byte 10 */
		pCsd->DeviceSizeMul |= (pui8CsdResponseArray[10] & 0x80) >> 7;

		pCsd->EraseGrSize = (pui8CsdResponseArray[10] & 0x40) >> 6;
		pCsd->EraseGrMul = (pui8CsdResponseArray[10] & 0x3F) << 1;

		/* Byte 11 */
		pCsd->EraseGrMul |= (pui8CsdResponseArray[11] & 0x80) >> 7;
		pCsd->WrProtectGrSize = (pui8CsdResponseArray[11] & 0x7F);

		/* Byte 12 */
		pCsd->WrProtectGrEnable = (pui8CsdResponseArray[12] & 0x80) >> 7;
		pCsd->ManDeflECC = (pui8CsdResponseArray[12] & 0x60) >> 5;
		pCsd->WrSpeedFact = (pui8CsdResponseArray[12] & 0x1C) >> 2;
		pCsd->MaxWrBlockLen = (pui8CsdResponseArray[12] & 0x03) << 2;

		/* Byte 13 */
		pCsd->MaxWrBlockLen |= (pui8CsdResponseArray[13] & 0xC0) >> 6;
		pCsd->WriteBlockPaPartial = (pui8CsdResponseArray[13] & 0x20) >> 5;
		pCsd->Reserved3 = 0;
		pCsd->ContentProtectAppli = (pui8CsdResponseArray[13] & 0x01);

		/* Byte 14 */
		pCsd->FileFormatGrouop = (pui8CsdResponseArray[14] & 0x80) >> 7;
		pCsd->CopyFlag = (pui8CsdResponseArray[14] & 0x40) >> 6;
		pCsd->PermWrProtect = (pui8CsdResponseArray[14] & 0x20) >> 5;
		pCsd->TempWrProtect = (pui8CsdResponseArray[14] & 0x10) >> 4;
		pCsd->FileFormat = (pui8CsdResponseArray[14] & 0x0C) >> 2;
		pCsd->ECC = (pui8CsdResponseArray[14] & 0x03);

		/* Byte 15 */
		pCsd->CSD_CRC = (pui8CsdResponseArray[15] & 0xFE) >> 1;
		pCsd->Reserved4 = 1;
	}
	return bReturn;
}

/**
 * @brief  Read the CID card register.
 *         Reading the contents of the CID register in SPI mode is a simple
 *         read-block transaction.
 * @param  pCid: pointer on an CID register structure
 * @retval bool: The SD Response
 *			@arg true: Read succeed
 *			@arg false: Read failed
 */
static bool sd_getCIDRegister(sd_cid_t* pCid)
{
	bool bReturn = false;
	uint8_t pui8CidResponseArray[SD_NUMBER_OF_CID_RESPONSE_BYTE];

	/* Send CMD10 (CID register) and Wait for response in the R1 format (0x00 is no errors) */
	bsp_sdio_activate();
	if (bsp_sdio_sendCommand(SD_CMD_SEND_CID, 0, SD_CRC_NOT_CARE,
			SD_RESPONSE_NO_ERROR))
	{
		if (bsp_sdio_waitResponse(SD_START_DATA_SINGLE_BLOCK_READ))
		{
			/* Store CID register value on CID_Tab */
			bsp_sdio_readData(pui8CidResponseArray,
			SD_NUMBER_OF_CSD_RESPONSE_BYTE);

			/* Get CRC bytes (not really needed by us, but required by SD) */
			bsp_sdio_sendDummy();
			bsp_sdio_sendDummy();

			bReturn = true;
		}
	}
	bsp_sdio_deactivate();

	/* Send dummy byte: 8 Clock pulses of delay */
	bsp_sdio_sendDummy();

	if (true == bReturn)
	{
		/* Byte 0 */
		pCid->ManufacturerID = pui8CidResponseArray[0];

		/* Byte 1 */
		pCid->OEM_AppliID = pui8CidResponseArray[1] << 8;

		/* Byte 2 */
		pCid->OEM_AppliID |= pui8CidResponseArray[2];

		/* Byte 3 */
		pCid->ProdName1 = pui8CidResponseArray[3] << 24;

		/* Byte 4 */
		pCid->ProdName1 |= pui8CidResponseArray[4] << 16;

		/* Byte 5 */
		pCid->ProdName1 |= pui8CidResponseArray[5] << 8;

		/* Byte 6 */
		pCid->ProdName1 |= pui8CidResponseArray[6];

		/* Byte 7 */
		pCid->ProdName2 = pui8CidResponseArray[7];

		/* Byte 8 */
		pCid->ProdRev = pui8CidResponseArray[8];

		/* Byte 9 */
		pCid->ProdSN = pui8CidResponseArray[9] << 24;

		/* Byte 10 */
		pCid->ProdSN |= pui8CidResponseArray[10] << 16;

		/* Byte 11 */
		pCid->ProdSN |= pui8CidResponseArray[11] << 8;

		/* Byte 12 */
		pCid->ProdSN |= pui8CidResponseArray[12];

		/* Byte 13 */
		pCid->Reserved1 |= (pui8CidResponseArray[13] & 0xF0) >> 4;
		pCid->ManufactDate = (pui8CidResponseArray[13] & 0x0F) << 8;

		/* Byte 14 */
		pCid->ManufactDate |= pui8CidResponseArray[14];

		/* Byte 15 */
		pCid->CID_CRC = (pui8CidResponseArray[15] & 0xFE) >> 1;
		pCid->Reserved2 = 1;
	}
	return bReturn;
}

/**
 * @brief  Get SD card data response.
 * @retval The SD status: Read data response xxx0<status>1
 *         @arg status 010: Data accepted.
 *         @arg status 101: Data rejected due to a CRC error.
 *         @arg status 110: Data rejected due to a Write error.
 *         @arg status 111: Data rejected due to other error.
 */
static sd_response_t sd_getDataResponse(void)
{
	uint32_t ui32Timeout = 64;
	sd_response_t sdResponse, sdResponeReturnvalue;

	while (ui32Timeout--)
	{
		/* Read response */
		bsp_sdio_readData(&sdResponse, 1);

		/* Mask unused bits */
		sdResponse &= 0x1F;
		switch (sdResponse)
		{
		case SD_DATA_OK:
			sdResponeReturnvalue = SD_DATA_OK;
			break;

		case SD_DATA_CRC_ERROR:
			return SD_DATA_CRC_ERROR;

		case SD_DATA_WRITE_ERROR:
			return SD_DATA_WRITE_ERROR;

		default:
			sdResponeReturnvalue = SD_DATA_OTHER_ERROR;
			break;
		}

		/* Exit loop in case of data ok */
		if (SD_DATA_OK == sdResponeReturnvalue)
		{
			break;
		}
	}

	/* Wait for null data */
	do
	{
		bsp_sdio_readData(&sdResponse, 1);
	} while (0 == sdResponse);

	return sdResponeReturnvalue;
}

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Initializes the SD/SD communication.
 * @retval bool: The SD Response
 *			@arg true: Sequence succeed
 *			@arg false: Sequence failed
 */
bool sd_init(void)
{
	if (SD_IN_SPI_IDLE == g_sdSoftareStatus)
	{
		return true;
	}
	else
	{
		/* Configure IO functionalities for SD pin */
		bsp_sdio_init();

		/* Check SD card  pin */
		if (bsp_sdio_isDetected())
		{
			g_sdStatus = SD_PRESENT;
		}
		else
		{
			g_sdStatus = SD_NOT_PRESENT;
			g_sdSoftareStatus = SD_NOT_IN_SPI_IDLE;
			return false;
		}

		/* SD initialized and set to SPI mode properly */
		if (sd_goIdleState())
		{
			g_sdSoftareStatus = SD_IN_SPI_IDLE;
			return true;
		}
		else
		{
			g_sdSoftareStatus = SD_NOT_IN_SPI_IDLE;
			return false;
		}
	}
}

/**
 * @brief  Returns the SD status.
 * @retval bool: The SD Status
 *			@arg true: OK
 *			@arg false: FAILS
 */
bool sd_getStatus(void)
{
	return ((SD_PRESENT == g_sdStatus) ? (true) : (false));
}

/**
 * @brief  Returns the SD status.
 * @param  pCardInfo: pointer to the SD Card Info structure.
 * @retval bool: The SD Status
 *			@arg true: Sequence succeed
 *			@arg false: Sequence failed
 */
bool sd_getCardInfo(sd_card_info_t *pCardInfo)
{
	if (sd_getCSDRegister(&(pCardInfo->Csd)))
	{
		pCardInfo->CardCapacity = (pCardInfo->Csd.DeviceSize + 1);
		pCardInfo->CardCapacity *= (1 << (pCardInfo->Csd.DeviceSizeMul + 2));
		pCardInfo->CardBlockSize = 1 << (pCardInfo->Csd.RdBlockLen);
		pCardInfo->CardCapacity *= pCardInfo->CardBlockSize;
	}
	else
	{
		return false;
	}

	return sd_getCIDRegister(&(pCardInfo->Cid));
}

/**
 * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
 * @param  pui32Data: Pointer to the buffer that will contain the data to transmit
 * @param  ui64ReadAddr: Address from where data is to be read
 * @param  ui16BlockSize: SD card data block size, that should be 512
 * @param  ui32NumberOfBlocks: Number of SD blocks to read
 * @retval bool: The SD Status
 *			@arg true: Sequence succeed
 *			@arg false: Sequence failed
 */
bool sd_readBlocks(uint32_t* pui32Data, uint64_t ui64ReadAddr,
		uint16_t ui16BlockSize, uint32_t ui32NumberOfBlocks)
{
	bool bReturn = false;
	uint32_t ui32Offset = 0;
	uint8_t *pui8Data = (uint8_t *) pui32Data;

	/* Send CMD16 (SD_CMD_SET_BLOCKLEN) to set the size of the block and
	 Check if the SD acknowledged the set block length command: R1 response (0x00: no errors) */
	bsp_sdio_activate();
	bReturn = bsp_sdio_sendCommand(SD_CMD_SET_BLOCKLEN, (uint32_t) ui16BlockSize,
	SD_CRC_NOT_CARE, SD_RESPONSE_NO_ERROR);
	if (false == bReturn)
	{
		bsp_sdio_deactivate();
		bsp_sdio_sendDummy();
		return false;
	}

	/* Data transfer */
	while (ui32NumberOfBlocks--)
	{
		bsp_sdio_deactivate();
		/* Send dummy byte: 8 Clock pulses of delay */
		bsp_sdio_sendDummy();

		/* Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
		/* Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
		bsp_sdio_activate();
		bReturn = bsp_sdio_sendCommand(SD_CMD_READ_SINGLE_BLOCK,
				ui64ReadAddr + ui32Offset,
				SD_CRC_NOT_CARE, SD_RESPONSE_NO_ERROR);
		if (false == bReturn)
		{
			bsp_sdio_deactivate();
			bsp_sdio_sendDummy();
			return false;
		}

		/* Now look for the data token to signify the start of the data */
		if (bsp_sdio_waitResponse(SD_START_DATA_SINGLE_BLOCK_READ))
		{
			/* Read the SD block data : read NumByteToRead data */
			bsp_sdio_readData((uint8_t *) (pui8Data + ui32Offset), ui16BlockSize);

			/* Set next read address*/
			ui32Offset += ui16BlockSize;

			/* get CRC bytes (not really needed by us, but required by SD) */
			bsp_sdio_sendDummy();
			bsp_sdio_sendDummy();

			/* Set response value to success */
			bReturn = true;
		}
		else
		{
			/* Set response value to failure */
			bReturn = false;
		}
	}

	bsp_sdio_deactivate();
	/* Send dummy byte: 8 Clock pulses of delay */
	bsp_sdio_sendDummy();

	return bReturn;
}

/**
 * @brief  Writes block(s) to a specified address in an SD card, in polling mode.
 * @param  pui32Data: Pointer to the buffer that will contain the data to transmit
 * @param  ui64WriteAddr: Address from where data is to be written
 * @param  ui16BlockSize: SD card data block size, that should be 512
 * @param  ui32NumberOfBlocks: Number of SD blocks to write
 * @retval bool: The SD Status
 *			@arg true: Sequence succeed
 *			@arg false: Sequence failed
 */
bool sd_writeBlocks(uint32_t* pui32Data, uint64_t ui64WriteAddr,
		uint16_t ui16BlockSize, uint32_t ui32NumberOfBlocks)
{
	uint8_t bReturn = false;
	uint32_t ui32Offset = 0;
	uint8_t *pui8Data = (uint8_t *) pui32Data;

	uint8_t ui8StartData = SD_START_DATA_SINGLE_BLOCK_WRITE;

	/* Data transfer */
	bsp_sdio_activate();
	while (ui32NumberOfBlocks--)
	{
		/* Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write blocks  and
		 Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
		if (!bsp_sdio_sendCommand(SD_CMD_WRITE_SINGLE_BLOCK,
				(uint32_t) (ui64WriteAddr + ui32Offset), SD_CRC_NOT_CARE,
				SD_RESPONSE_NO_ERROR))
		{
			bsp_sdio_deactivate();
			bsp_sdio_sendDummy();
			return false;
		}

		/* Send dummy byte */
		bsp_sdio_sendDummy();

		/* Send the data token to signify the start of the data */
		bsp_sdio_sendData(&ui8StartData, 1);

		/* Write the block data to SD : write count data by block */
		bsp_sdio_sendData((uint8_t *) (pui8Data + ui32Offset), ui16BlockSize);

		/* Set next write address */
		ui32Offset += ui16BlockSize;

		/* Put CRC bytes (not really needed by us, but required by SD) */
		uint8_t pui8CRCResponse[2];
		bsp_sdio_readData(pui8CRCResponse, 2);

		/* Read data response */
		if (SD_DATA_OK == sd_getDataResponse())
		{
			/* Set response value to success */
			bReturn = true;
		}
		else
		{
			/* Set response value to failure */
			bReturn = false;
		}
	}
	bsp_sdio_deactivate();

	/* Send dummy byte: 8 Clock pulses of delay to initiate internal write */
	bsp_sdio_sendDummy();

	return bReturn;
}

/**@}BSP_DRV_SD_PRIVATE*/
/**@}BSP_DRV_SD*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
