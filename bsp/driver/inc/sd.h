/**
 ****************************************************************************
 * @file        sd.h
 * @author      Long Dang
 * @version     V0.1
 * @date        19-March-2017
 * @copyright   LGPLv3
 * @brief       This is the header of the SD Driver.
 * 				Reference to the stm3210c_eval_sd.h driver of STMicroelectronics.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SD_H_
#define SD_H_

/** @defgroup DRV Board driver
 * @{
 */
/** @defgroup DRV_SD SD
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "sd_io.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @struct _sd_csd_t
 * This type define the Card Specific Data: CSD Register
 */
typedef struct _sd_csd_t
{
	uint8_t CSDStruct; /*!< CSD structure */
	uint8_t SysSpecVersion; /*!< System specification version */
	uint8_t Reserved1; /*!< Reserved */
	uint8_t TAAC; /*!< Data read access-time 1 */
	uint8_t NSAC; /*!< Data read access-time 2 in CLK cycles */
	uint8_t MaxBusClkFrec; /*!< Max. bus clock frequency */
	uint16_t CardComdClasses; /*!< Card command classes */
	uint8_t RdBlockLen; /*!< Max. read data block length */
	uint8_t PartBlockRead; /*!< Partial blocks for read allowed */
	uint8_t WrBlockMisalign; /*!< Write block miss alignment */
	uint8_t RdBlockMisalign; /*!< Read block miss alignment */
	uint8_t DSRImpl; /*!< DSR implemented */
	uint8_t Reserved2; /*!< Reserved */
	uint32_t DeviceSize; /*!< Device Size */
	uint8_t MaxRdCurrentVDDMin; /*!< Max. read current @ VDD min */
	uint8_t MaxRdCurrentVDDMax; /*!< Max. read current @ VDD max */
	uint8_t MaxWrCurrentVDDMin; /*!< Max. write current @ VDD min */
	uint8_t MaxWrCurrentVDDMax; /*!< Max. write current @ VDD max */
	uint8_t DeviceSizeMul; /*!< Device size multiplier */
	uint8_t EraseGrSize; /*!< Erase group size */
	uint8_t EraseGrMul; /*!< Erase group size multiplier */
	uint8_t WrProtectGrSize; /*!< Write protect group size */
	uint8_t WrProtectGrEnable; /*!< Write protect group enable */
	uint8_t ManDeflECC; /*!< Manufacturer default ECC */
	uint8_t WrSpeedFact; /*!< Write speed factor */
	uint8_t MaxWrBlockLen; /*!< Max. write data block length */
	uint8_t WriteBlockPaPartial; /*!< Partial blocks for write allowed */
	uint8_t Reserved3; /*!< Reserved */
	uint8_t ContentProtectAppli; /*!< Content protection application */
	uint8_t FileFormatGrouop; /*!< File format group */
	uint8_t CopyFlag; /*!< Copy flag (OTP) */
	uint8_t PermWrProtect; /*!< Permanent write protection */
	uint8_t TempWrProtect; /*!< Temporary write protection */
	uint8_t FileFormat; /*!< File Format */
	uint8_t ECC; /*!< ECC code */
	uint8_t CSD_CRC; /*!< CSD CRC */
	uint8_t Reserved4; /*!< always 1*/
} sd_csd_t;

/**
 * @struct _sd_cid_t
 * This type define the Card Identification Data: CID Register
 */
typedef struct _sd_cid_t
{
	uint8_t ManufacturerID; /*!< ManufacturerID */
	uint16_t OEM_AppliID; /*!< OEM/Application ID */
	uint32_t ProdName1; /*!< Product Name part1 */
	uint8_t ProdName2; /*!< Product Name part2*/
	uint8_t ProdRev; /*!< Product Revision */
	uint32_t ProdSN; /*!< Product Serial Number */
	uint8_t Reserved1; /*!< Reserved1 */
	uint16_t ManufactDate; /*!< Manufacturing Date */
	uint8_t CID_CRC; /*!< CID CRC */
	uint8_t Reserved2; /*!< always 1 */
} sd_cid_t;

/**
 * @struct _sd_card_info_t
 * This type define the SD Card information.
 */
typedef struct _sd_card_info_t
{
	sd_csd_t Csd;
	sd_cid_t Cid;
	uint32_t CardCapacity; /*!< Card Capacity */
	uint32_t CardBlockSize; /*!< Card Block Size */
} sd_card_info_t;

/**
 * @typedef sd_hardware_status_t
 * This type define the SD detection on its memory slot.
 */
typedef enum
{
	SD_NOT_PRESENT = 0, /*!< SD not insert yet */
	SD_PRESENT = 1 /*!< SD inserted */
} sd_hardware_status_t;

/* Exported constants --------------------------------------------------------*/
#define SD_BLOCK_SIZE				(0x200) /*!< Block size 512 bytes work with FatFS */

/* Exported macro ------------------------------------------------------------*/
#define sd_isDetected()	bsp_sd_isDetected() /*!< Wrapper SDIO API */

/* Exported functions --------------------------------------------------------*/
bool sd_init(void);
bool sd_getStatus(void);
bool sd_getCardInfo(sd_card_info_t *pCardInfo);
bool sd_readBlocks(uint32_t* pui32Data, uint64_t ui64ReadAddr,
		uint16_t ui16BlockSize, uint32_t ui32NumberOfBlocks);
bool sd_writeBlocks(uint32_t* pui32Data, uint64_t ui64WriteAddr,
		uint16_t ui16BlockSize, uint32_t ui32NumberOfBlocks);

/**
 * @} DRV_SD
 */
/**
 * @} DRV
 */
#endif /* SD_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
