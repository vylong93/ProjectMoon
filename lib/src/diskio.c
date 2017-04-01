/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*                                                                       */
/*   Portions Copy-left 2017 PnL                                         */
/*   Portions Copyright (C) 2014, ChaN, all right reserved               */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various existing       */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
/**
 ****************************************************************************
 * @file        diskio.c
 * @author      Long Dang
 * @version     V0.1
 * @date        01-April-2017
 * @copyright   LGPLv3
 * @brief       This file implement the DiskIO module for FAT file system.
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
/** @defgroup LIB_DISKIO Disk IO Driver for FatFS
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "sd_diskio.h"
#include "ff.h"

/* Private typedef -----------------------------------------------------------*/
/**
 * @struct _generic_diskio_driver_t
 * This type define the Generic Disk IO Driver structure.
 */
typedef struct _generic_diskio_driver_t
{
	DSTATUS (*disk_initialize)(BYTE); /*!< Initialize Disk Drive */
	DSTATUS (*disk_status)(BYTE); /*!< Get Disk Status */
	DRESULT (*disk_read)(BYTE, BYTE*, DWORD, UINT); /*!< Read Sector(s) */
#if _USE_WRITE == 1
	DRESULT (*disk_write)(BYTE, const BYTE*, DWORD, UINT); /*!< Write Sector(s) when _USE_WRITE = 1 */
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
	DRESULT (*disk_ioctl)(BYTE, BYTE, void*); /*!< I/O control operation when _USE_IOCTL = 1 */
#endif /* _USE_IOCTL == 1 */
} generic_diskio_driver_t;

/**
 * @struct _diskio_drivers_t
 * This type define the Global Disk IO Drivers structure.
 * The number of Disk IO Driver depend on the macro _VOLUMES defined in FatFs module (ff.h).
 */
typedef struct _diskio_drivers_t
{
	uint32_t pui32IsInitialized[_VOLUMES]; /*!< Initializes status of the driver */
	generic_diskio_driver_t *pDiskDriver[_VOLUMES]; /*!< Disk IO driver pointer */
	uint8_t pui8Lun[_VOLUMES]; /*!< multi-lun management. Only used for USB Key Disk */
} diskio_drivers_t;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
diskio_drivers_t g_diskIODrivers =
{
{ 0 },
{ 0 },
{ 0 } }; /*!< Global Disk IO Drivers */

/* Private functions declaration ---------------------------------------------*/
static void mapDiskIODriverOfSD(generic_diskio_driver_t *pDiskDriver);

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Map the SD Disk IO Driver.
 * @param  pDiskDriver: Disk IO driver pointer
 * @retval None
 */
void mapDiskIODriverOfSD(generic_diskio_driver_t *pDiskDriver)
{
	pDiskDriver->disk_initialize = sd_diskio_initialize;
	pDiskDriver->disk_status = sd_diskio_status;
	pDiskDriver->disk_read = sd_diskio_read;
#if _USE_WRITE == 1
	pDiskDriver->disk_write = sd_diskio_write;
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
	pDiskDriver->disk_ioctl = sd_diskio_ioctl;
#endif /* _USE_IOCTL == 1 */
}

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Initializes a Drive.
 * @param  pdrv: Physical drive number (0..) to identify the drive.
 * @retval DSTATUS: Operation status
 * 			@arg RES_OK: succeeded
 * 			@arg RES_ERROR: failed
 */
DSTATUS disk_initialize(BYTE pdrv)
{
	if (0 == g_diskIODrivers.pui32IsInitialized[pdrv])
	{
		/* Link the corresponding DiskIO driver base on the Physical driver number */
		if (0 == pdrv)
		{
			/* Disk "0:/" is the SD device -> Mapping to the SD DiskIO Driver */
			mapDiskIODriverOfSD(g_diskIODrivers.pDiskDriver[pdrv]);
		}
		else
		{
			/* Current system have only one SD device at "0:/" */
			return RES_ERROR;
		}

		/* Initialize the Disk IO device */
		if (RES_OK
				== g_diskIODrivers.pDiskDriver[pdrv]->disk_initialize(
						g_diskIODrivers.pui8Lun[pdrv]))
		{
			g_diskIODrivers.pui32IsInitialized[pdrv] = 1;
			return RES_OK;
		}
		else
		{
			/* Driver fails */
			return RES_ERROR;
		}
	}
	else
	{
		/* Driver already initialized */
		return RES_OK;
	}
}

/**
 * @brief  Gets Disk Status
 * @param  pdrv: Physical drive number (0..) to identify the drive.
 * @retval DSTATUS: Operation status
 * 			@arg RES_OK: succeeded
 * 			@arg RES_ERROR: failed
 */
DSTATUS disk_status(BYTE pdrv)
{
	if (0 == g_diskIODrivers.pui32IsInitialized[pdrv])
	{
		return RES_ERROR;
	}

	DSTATUS diskStatusReturn;
	diskStatusReturn = g_diskIODrivers.pDiskDriver[pdrv]->disk_status(
			g_diskIODrivers.pui8Lun[pdrv]);
	return diskStatusReturn;
}

/**
 * @brief  Reads Sector(s)
 * @param  pdrv: Physical drive number (0..)
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: Operation result
 * 			@arg RES_OK: Successful
 * 			@arg RES_ERROR: R/W Error
 * 			@arg RES_WRPRT: Write Protected
 * 			@arg RES_NOTRDY: Not Ready
 * 			@arg RES_PARERR: Invalid Parameter
 */
DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
	if (0 == g_diskIODrivers.pui32IsInitialized[pdrv])
	{
		return RES_ERROR;
	}

	DRESULT diskResultReturn;
	diskResultReturn = g_diskIODrivers.pDiskDriver[pdrv]->disk_read(
			g_diskIODrivers.pui8Lun[pdrv], buff, sector, count);
	return diskResultReturn;
}

#if _USE_WRITE == 1
/**
 * @brief  Writes Sector(s)
 * @param  pdrv: Physical drive number (0..)
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: Operation result
 * 			@arg RES_OK: Successful
 * 			@arg RES_ERROR: R/W Error
 * 			@arg RES_WRPRT: Write Protected
 * 			@arg RES_NOTRDY: Not Ready
 * 			@arg RES_PARERR: Invalid Parameter
 */
DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
	if (0 == g_diskIODrivers.pui32IsInitialized[pdrv])
	{
		return RES_ERROR;
	}

	DRESULT diskResultReturn;
	diskResultReturn = g_diskIODrivers.pDiskDriver[pdrv]->disk_write(
			g_diskIODrivers.pui8Lun[pdrv], buff, sector, count);
	return diskResultReturn;
}
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
/**
 * @brief  I/O control operation
 * @param  pdrv: Physical drive number (0..)
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: Operation result
 * 			@arg RES_OK: Successful
 * 			@arg RES_ERROR: R/W Error
 * 			@arg RES_WRPRT: Write Protected
 * 			@arg RES_NOTRDY: Not Ready
 * 			@arg RES_PARERR: Invalid Parameter
 */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
	if (0 == g_diskIODrivers.pui32IsInitialized[pdrv])
	{
		return RES_ERROR;
	}

	DRESULT diskResultReturn;
	diskResultReturn = g_diskIODrivers.pDiskDriver[pdrv]->disk_ioctl(
			g_diskIODrivers.pui8Lun[pdrv], cmd, buff);
	return diskResultReturn;
}
#endif /* _USE_IOCTL == 1 */

/**
 * @brief  Gets Time from RTC
 * @retval Time in DWORD
 */
__weak DWORD get_fattime(void)
{
	return 0;
}

/**
 * @} LIB_DISKIO
 */
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
