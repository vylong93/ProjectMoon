/**
 ****************************************************************************
 * @file        main.h
 * @author      Long Dang
 * @version     V0.1
 * @date        18-December-2016
 * @copyright   LGPLv3
 * @brief       Header for main.c module.
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
#ifndef MAIN_H_
#define MAIN_H_

/** @defgroup MAIN Application
 * @{
 */
/** @defgroup LIB Library
 * @{
 */
	/** @defgroup LIB_AUDIO Audio */

	/** @defgroup LIB_GFX Graphic
	 * @{
	 */
		/** @defgroup LIB_GFX_TXT Text */
	/**@}LIB_GFX*/

	/** @defgroup LIB_FATFS FAT file system
	 * @{
	 */
		/** @defgroup LIB_FATFS_DISKIO Disk I/O
		 * @{
		 */
			/** @defgroup LIB_FATFS_DISKIO_SD SD Disk I/O */
		/**@}LIB_FATFS_DISKIO*/
	/**@}LIB_FATFS*/
/**@}LIB*/

/** @defgroup BSP Board Support Package
 * @{
 */
	/** @defgroup BSP_DRV Driver
	 * @{
	 */
		/** @defgroup BSP_DRV_DISP Display */
		/** @defgroup BSP_DRV_SD SD */
		/** @defgroup BSP_DRV_ACODEC Audio CODEC */
	/**@}BSP_DRV*/

	/** @defgroup BSP_DEVICE Device-Peripheral
	 * @{
	 */
		/** @defgroup BSP_DEVICE_LED LED */
		/** @defgroup BSP_DEVICE_LCD LCD */
		/** @defgroup BSP_DEVICE_SDIO SD I/O */
		/** @defgroup BSP_DEVICE_ACODEC Audio CODEC I/O */
		/** @defgroup BSP_DEVICE_BUTTON Button */
	/**@}BSP_DEVICE*/

	/** @defgroup BSP_INT_PRIORITY Interrup priority */
/**@}BSP*/

/**
 * @mainpage Welcome to Project Moon
 *
 * \section intro_sec Introduction
 *
 * Project Moon is an open source project that build up an MP3 Player with
 * Recording function from scratch. All the sources code are opened under GNU LGPLv3 license.
 * The hardware also freely open. The purpose of this project is to demonstrate in highly
 * details guideline that show you how to build your own electronic device
 * (ex. an MP3 Player with customizable feature).
 *
 * Additionally, this project is non-profit project and powered by PnL team which stand for
 * Programming and Leverage. Currently, I'm the only member of this team - Long Dang.
 *
 * My credo:
 * > "Let finding 'lý tưởng' and passion. Build your own tech, your style!"
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: TODO
 *
 * etc...
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "led.h"			/* BSP-LED APIs */
#include "text.h"			/* LIB_TEXT APIs */
#include "graphic.h"		/* LIB_GRAPHIC APIs */
#include "ff.h"				/* FatFS APIs */
#include "audio_codec.h"	/* BSP_DRV_ACODEC APIs */
#include "audio.h"			/* LIB_AUDIO APIs */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**@}MAIN*/
#endif /* MAIN_H_ */

/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
