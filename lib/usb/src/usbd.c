/**
 ****************************************************************************
 * @file        usbd.c
 * @author      Long Dang
 * @version     V0.1
 * @date        07-May-2017
 * @copyright   LGPLv3
 * @brief       This file implement the USB Mass Storage Device module.
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
/** @addtogroup LIB_USBD
 * @{
 */
/** @defgroup LIB_USBD_PRIVATE USB Mass Storage Device (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "usbd.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef hUsbDeviceFS; /*!< USB Device Core handle declaration */

/* Private functions declaration ---------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Start the USBD in MSC mode
 * @retval None
 */
void usbd_startMassStorageDeviceMode(void)
{
	/* Init Device Library,Add Supported Class and Start the library*/
	USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);

	USBD_RegisterClass(&hUsbDeviceFS, &USBD_MSC);

	USBD_MSC_RegisterStorage(&hUsbDeviceFS, &USBD_Storage_Interface_fops_FS);

	USBD_Start(&hUsbDeviceFS);
}

/**@}LIB_USBD_PRIVATE*/
/**@}LIB_USBD*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
