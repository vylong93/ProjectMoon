/**
 ****************************************************************************
 * @file        sd_diskio.c
 * @author      Long Dang
 * @version     V0.1
 * @date        01-April-2017
 * @copyright   LGPLv3
 * @brief       This file implement the SD Disk IO Driver for FatFS.
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
/** @addtogroup LIB_FATFS_DISKIO_SD
 * @{
 */
/** @defgroup LIB_FATFS_DISKIO_SD_PRIVATE SD Disk I/O (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "sd_diskio.h"
#include "sd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static volatile DSTATUS g_diskStatus = STA_NOINIT; /*!< SD disk status */

/* Private functions declaration ---------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Initializes a Drive
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 * 		@arg RES_OK: succeeded
 * 		@arg STA_NOINIT: failed
 */
DSTATUS sd_diskio_initialize(BYTE lun)
{
	UNUSED(lun);
	g_diskStatus = STA_NOINIT;
	/* Configure the uSD device */
	if (sd_init())
	{
		g_diskStatus &= ~STA_NOINIT;
	}
	return g_diskStatus;
}

/**
 * @brief  Gets Disk Status
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 * 		@arg RES_OK: succeeded
 * 		@arg STA_NOINIT: failed
 */
DSTATUS sd_diskio_status(BYTE lun)
{
	UNUSED(lun);
	g_diskStatus = STA_NOINIT;
	if (sd_getStatus())
	{
		g_diskStatus &= ~STA_NOINIT;
	}
	return g_diskStatus;
}

/**
 * @brief  Reads Sector(s)
 * @param  lun : not used
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: Operation result
 * 		@arg RES_OK: succeeded
 * 		@arg RES_ERROR: failed
 */
DRESULT sd_diskio_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
	UNUSED(lun);
	if (!sd_readBlocks((uint32_t*) buff, (uint64_t) (sector * SD_BLOCK_SIZE),
	SD_BLOCK_SIZE, count))
	{
		return RES_ERROR;
	}
	return RES_OK;
}

#if _USE_WRITE == 1
/**
 * @brief  Writes Sector(s)
 * @param  lun : not used
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: Operation result
 * 		@arg RES_OK: succeeded
 * 		@arg RES_ERROR: failed
 */
DRESULT sd_diskio_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
	UNUSED(lun);
	if (!sd_writeBlocks((uint32_t*) buff, (uint64_t) (sector * SD_BLOCK_SIZE),
	SD_BLOCK_SIZE, count))
	{
		return RES_ERROR;
	}
	return RES_OK;
}
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
/**
 * @brief  I/O control operation
 * @param  lun : not used
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: Operation result
 * 		@arg RES_OK: succeeded
 * 		@arg RES_ERROR: failed
 */
DRESULT sd_diskio_ioctl(BYTE lun, BYTE cmd, void *buff)
{
	UNUSED(lun);
	DRESULT diskResult = RES_ERROR;
	sd_card_info_t CardInfo;

	if (STA_NOINIT & g_diskStatus)
	{
		return RES_NOTRDY;
	}

	switch (cmd)
	{
	/* Make sure that no pending write process */
	case CTRL_SYNC:
		diskResult = RES_OK;
		break;

		/* Get number of sectors on the disk (DWORD) */
	case GET_SECTOR_COUNT:
		if (sd_getCardInfo(&CardInfo))
		{
			*(DWORD*) buff = CardInfo.CardCapacity / SD_BLOCK_SIZE;
			diskResult = RES_OK;
		}
		break;

		/* Get R/W sector size (WORD) */
	case GET_SECTOR_SIZE:
		*(WORD*) buff = SD_BLOCK_SIZE;
		diskResult = RES_OK;
		break;

		/* Get erase block size in unit of sector (DWORD) */
	case GET_BLOCK_SIZE:
		*(DWORD*) buff = SD_BLOCK_SIZE;
		break;

	default:
		diskResult = RES_PARERR;
	}

	return diskResult;
}
#endif  /* _USE_IOCTL == 1 */

/**@}LIB_FATFS_DISKIO_SD_PRIVATE*/
/**@}LIB_FATFS_DISKIO_SD*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
