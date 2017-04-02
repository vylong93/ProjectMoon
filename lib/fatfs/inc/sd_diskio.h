/**
 ****************************************************************************
 * @file        sd_diskio.h
 * @author      Long Dang
 * @version     V0.1
 * @date        01-April-2017
 * @copyright   LGPLv3
 * @brief       This is the header of the SD Disk IO Driver for FatFS.
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
#ifndef SD_DISKIO_H_
#define SD_DISKIO_H_

/** @addtogroup LIB_FATFS_DISKIO_SD
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "diskio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
DSTATUS diskio_sd_initialize(BYTE lun);
DSTATUS diskio_sd_status(BYTE lun);
DRESULT diskio_sd_read(BYTE lun, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
DRESULT diskio_sd_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
DRESULT diskio_sd_ioctl(BYTE lun, BYTE cmd, void *buff);
#endif  /* _USE_IOCTL == 1 */

/**@}LIB_FATFS_DISKIO_SD*/
#endif /* SD_DISKIO_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
