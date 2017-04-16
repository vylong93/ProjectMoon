/**
 ****************************************************************************
 * @file        audio.c
 * @author      Long Dang
 * @version     V0.1
 * @date        16-April-2017
 * @copyright   LGPLv3
 * @brief       This file implement the simple audio library.
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
/** @addtogroup LIB_AUDIO
 * @{
 */
/** @defgroup LIB_AUDIO_PRIVATE Audio (Private)
 * @{
 */
/* Includes ------------------------------------------------------------------*/
#include "audio.h"
#include "ff.h"
#include "text.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define REPORT_ON_SCREEN
#define AUDIO_BUFFER_SIZE	(32) /*!< Number of byte in the local audio buffer */
#define IMA_ADPCM_BLOCK_SIZE	(256) /*!< Record block size 128-word with 16-bit/words */
#define WAV_HEADER_SIZE			(512) /*!< Record file header size in byte unit */
#define FILE_BUFFER_SIZE		(512) /*!< Record file buffer size in byte unit */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t g_pui8AudioBuffer[AUDIO_BUFFER_SIZE];
static bool g_bNeedData = true;

static const uint8_t g_pui8RIFFHeader0[] = /* 52 bytes */
{ 'R', 'I', 'F', 'F', /* Chunk ID (RIFF) */
0x70, 0x70, 0x70, 0x70, /* Chunk payload size (calculate after rec!) */
'W', 'A', 'V', 'E', /* RIFF resource format type */
'f', 'm', 't', ' ', /* Chunk ID (fmt ) */
0x14, 0x00, 0x00, 0x00, /* Chunk payload size (0x14 = 20 bytes) */
0x11, 0x00, /* Format Tag (IMA ADPCM) */
0x01, 0x00, /* Channels (1) */
0x40, 0x1f, 0x00, 0x00, /* [24, 25] Sample Rate, 0x3e80 = 16.0kHz, 0x1f40 = 8.0kHz */
0xd7, 0x0f, 0x00, 0x00, /* [28, 29] Average Bytes Per Second, 0x1fae for 16.0kHz, 0x0fd7 for 8.0kHz */
0x00, 0x01, /* Data Block Size (256 bytes) */
0x04, 0x00, /* ADPCM encoded bits per sample (4 bits) */
0x02, 0x00, /* Extra data (2 bytes) */
0xf9, 0x01, /* Samples per Block (505 samples) */
'f', 'a', 'c', 't', /* Chunk ID (fact) */
0xc8, 0x01, 0x00, 0x00, /* Chunk payload size (512-56=456 bytes (zeropad!)) */
0xff, 0xff, 0xff, 0xff /* Number of Samples (calculate after rec!) */
}; /* Insert 452 zeroes here! */

static const uint8_t g_pui8RIFFHeader504[] = /* 8 bytes */
{ 'd', 'a', 't', 'a', /* Chunk ID (data) */
0x70, 0x70, 0x70, 0x70 /* Chunk payload size (calculate after rec!) */
};

/* Private functions declaration ---------------------------------------------*/
static bool audio_readSongData(FIL *pFile, uint8_t *pui8DestBuffer,
		uint32_t ui32ReadSize);
#ifdef REPORT_ON_SCREEN
static void audio_renderStatus(uint32_t ui32CurrentPos,
		uint32_t *pui32NextReportPos);
#endif

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Read a block data of the file to RAM.
 * @param  pFile: File pointer to current opened song.
 * @param  pui8DestBuffer: Destination buffer data pointer.
 * @param  ui32ReadSize: Size of the data block need to read.
 * @retval bool: process status
 *			@arg true: read completed
 *			@arg false: failed to read - EOF
 */
static bool audio_readSongData(FIL *pFile, uint8_t *pui8DestBuffer,
		uint32_t ui32ReadSize)
{
	FRESULT res;
	uint32_t ui32BytesRead = 0;
	res = f_read(pFile, pui8DestBuffer, ui32ReadSize, (UINT*) &ui32BytesRead);
	if ((0 == ui32BytesRead) || (FR_OK != res))
	{
		return false;
	}
	return true;
}

#ifdef REPORT_ON_SCREEN
/**
 * @brief  Render current audio format, sample rate, channel, decoded time, processed data
 * @param  ui32CurrentPos: current processed data position in byte unit.
 * @param  pui32NextReportPos: next processing data position storage pointer.
 * @retval None
 */
static void audio_renderStatus(uint32_t ui32CurrentPos,
		uint32_t *pui32NextReportPos)
{
#define REPORT_INTERVAL 		(4096)
#define REPORT_INTERVAL_MIDI	(512)
	const char *psNameTable[] =
	{ "unknown", "RIFF", "MP3", "MIDI", };
	uint16_t ui16DecodedTime = 0;
	uint16_t ui16SampleRate = 0;
	bool bIsStereoMode = false;
	audio_format_t audioFormat = afUnknown;

	if (ui32CurrentPos >= *pui32NextReportPos)
	{
		/* Update report on screen
		 2nd line: Audio format
		 3rd line: sample rate - mono/stereo
		 4th line: decode time
		 */
		acodec_getFormat(&audioFormat);
		acodec_getSamplerate(&ui16SampleRate, &bIsStereoMode);
		acodec_getDecodingTime(&ui16DecodedTime);

		text_setCursor(0, 8);
		text_printString("Format: ");
		text_printString(psNameTable[audioFormat]);
		text_printString("                   \n");

		text_printNumber(ui16SampleRate);
		text_printString("Hz - ");
		text_printString((bIsStereoMode) ? "stereo\n" : "mono\n");

		int ss = ui16DecodedTime % 60;
		int mm = ui16DecodedTime / 60;
		text_printNumber(mm / 10);
		text_printNumber(mm % 10);
		text_printString(":");
		text_printNumber(ss / 10);
		text_printNumber(ss % 10);
		text_printString(" ");

		text_printNumber(ui32CurrentPos / 1024);
		text_printString("KB");
		text_printString("             \n");

		graphic_render();
		*pui32NextReportPos +=
				(audioFormat == afMidi || audioFormat == afUnknown) ?
						REPORT_INTERVAL_MIDI : REPORT_INTERVAL;
	}
}
#endif

/* Exported functions prototype ----------------------------------------------*/
/**
 * @brief  Initialize the audio CODEC driver.
 * @retval bool: Status of initialization
 *			@arg true: succeeded
 *			@arg false: failed
 */
bool audio_init(void)
{
	/* Enable the audio CODEC driver */
	if (!acodec_init())
	{
		return false;
	}
	return true;
}

/**
 * @brief  Play the audio file in file system. Current supported *.mp3 and *.wav.
 * @param  pcFileName: string of the audio file to play.
 * @retval bool: process status
 *			@arg true: play file completed
 *			@arg false: failed to play
 */
bool audio_playFileBlocking(const char *pcFileName)
{
#ifdef REPORT_ON_SCREEN
	uint32_t ui32CurrentPos = 0;
	uint32_t ui32NextReportPos = 0;

	graphic_clearRenderBuffer();
	text_setCursor(0, 0);
	text_setWrapText(false);
	text_putLine(pcFileName, FAST);
#endif

	FIL file;
	if (f_open(&file, pcFileName, FA_READ) != FR_OK)
	{
		return false;
	}

	acodec_initPlaying();
	while (true)
	{
		while (acodec_isDeviceBusy())
		{
			/* DREQ is low while the receive buffer is full.
			 You can do something else here, the buffer of the MP3 is full and happy.
			 Maybe set the volume or test to see how much we can delay before we hear audible glitches */

			/* If the MP3 IC is happy, but we need to read new data from the file system,
			 now is a great time to do so */
			if (true == g_bNeedData)
			{
				if (!audio_readSongData(&file, g_pui8AudioBuffer,
				AUDIO_BUFFER_SIZE))
				{
					/* Time to exit ! There is no data left to read! */
					text_putLine("EOF", FAST);
					break;
				}
				g_bNeedData = false;
#ifdef REPORT_ON_SCREEN
				ui32CurrentPos += AUDIO_BUFFER_SIZE;
				audio_renderStatus(ui32CurrentPos, &ui32NextReportPos);
#endif
			}
			/* Test to see just how much we can do before the audio starts to glitch */
//			bsp_acodec_delay_ms(150); /* audible glitches */
//			bsp_acodec_delay_ms(135); /* audible glitches */
//			bsp_acodec_delay_ms(120); /* barely audible glitches */
//			bsp_acodec_delay_ms(100); /* sound fine ~ 128kbps */
//			bsp_acodec_delay_ms(20); /* sound fine ~ 320kbps */
		}

		/* This is here in case we haven't had any free time to load new data */
		if (true == g_bNeedData)
		{
			/* Goto current file system and try reading 32 new bytes of the song */
			if (!audio_readSongData(&file, g_pui8AudioBuffer,
			AUDIO_BUFFER_SIZE))
			{
				/* Time to exit ! There is no data left to read! */
				text_putString("EOF-", FAST);
				break;
			}
			g_bNeedData = false;
#ifdef REPORT_ON_SCREEN
			ui32CurrentPos += AUDIO_BUFFER_SIZE;
			audio_renderStatus(ui32CurrentPos, &ui32NextReportPos);
#endif
		}

		/* Once DREQ is released (high) we now feed 32 bytes of data to the VS1003 from our SD read buffer */
		acodec_sendData(g_pui8AudioBuffer, AUDIO_BUFFER_SIZE);

		/* We've just dumped 32 bytes into VS1003 so our SD read buffer is empty.
		 Set flag so we go get more data */
		g_bNeedData = true;
	}

	return true;
}

/**
 * @brief  Record audio and save to the file system.
 * @param  pcFileName: string of the output audio file.
 * @param  ui32PeriodSecond: the target record period.
 * @param  recordRate: the target sample rate for recording
 *			@arg REC_8KHz: sample rate 8KHz
 *			@arg REC_16KHz: sample rate 16KHz
 * @retval bool: process status
 *			@arg true: record completed
 *			@arg false: record failed
 */
bool audio_recordFileBlocking(const char *pcFileName, uint32_t ui32PeriodSecond,
		record_rate_t recordRate)
{
#ifdef REPORT_ON_SCREEN
	graphic_clearRenderBuffer();
	text_setCursor(0, 0);
	text_setWrapText(false);
	text_putString("Open: ", FAST);
	text_putLine(pcFileName, FAST);
#endif

	FIL file;
	if (FR_OK != f_open(&file, pcFileName, FA_CREATE_ALWAYS | FA_WRITE))
	{
		return false;
	}

	acodec_initRecordAPCM((REC_16KHz == recordRate) ? (true) : (false));

	/* Construct WAV header - 512 bytes */
	uint8_t pui8WAVHeaderBuffer[WAV_HEADER_SIZE];
	uint8_t pui8FileBuffer[FILE_BUFFER_SIZE]; /* data buffer for saving to disk */
	uint32_t i;
	for (i = 0; i < 52; i++)
	{
		pui8WAVHeaderBuffer[i] = g_pui8RIFFHeader0[i];
	}
	for (i = 52; i < 504; i++)
	{
		pui8WAVHeaderBuffer[i] = 0x00;
	}
	for (i = 504; i < 512; i++)
	{
		pui8WAVHeaderBuffer[i] = g_pui8RIFFHeader504[i - 504];
	}
	if (REC_16KHz == recordRate)
	{
		/* [24, 25] Sample Rate, 0x3e80 = 16.0kHz, 0x1f40 = 8.0kHz */
		pui8WAVHeaderBuffer[24] = 0x80;
		pui8WAVHeaderBuffer[25] = 0x3e;
		/* [28, 29] Average Bytes Per Second, 0x1fae for 16.0kHz, 0x0fd7 for 8.0kHz */
		pui8WAVHeaderBuffer[28] = 0xae;
		pui8WAVHeaderBuffer[29] = 0x1f;
	}

	/* Write WAV file header to disk storage */
	UINT ui32WritenByte; /* This variable is not used */
	if (FR_OK
			!= f_write(&file, pui8WAVHeaderBuffer, WAV_HEADER_SIZE,
					&ui32WritenByte))
	{
		/* Can not write file header */
		f_close(&file);
#ifdef REPORT_ON_SCREEN
		text_putLine("Failed to write WAV header", FAST);
#endif
		return false;
	}

	/* Sync to incoming audio frame...
	 lots of data in buffer, wait until buffer level restarts from 0
	 */
	acodec_syncToIncomingAudioFrame();

#ifdef REPORT_ON_SCREEN
	text_putLine("Recording...", FAST);
#endif

	uint8_t *pui8FileBufferPointer = 0;
	uint32_t ui32BlockNumber = 0;
	uint32_t ui32SectorCount = 1;
	uint32_t ui32LimitedSector;

	if (REC_16KHz == recordRate)
	{
		/* For 16kHz sample rate, 505 samples/block,
		 8 disk sectors are exactly 505 milliseconds. */
		ui32LimitedSector = (ui32PeriodSecond * 1000 * 8) / 505;
	}
	else
	{
		/* For 8kHz sample rate, 505 samples/block,
		 4 disk sectors are exactly 505 milliseconds. */
		ui32LimitedSector = (ui32PeriodSecond * 1000 * 4) / 505;
	}

	pui8FileBufferPointer = pui8FileBuffer;
	bool bIsRecording = true;
	while (bIsRecording)
	{
		acodec_readRecordBlock(&pui8FileBufferPointer);
		ui32BlockNumber++;

		/* Wait until 512 bytes available */
		if (2 == ui32BlockNumber)
		{
#ifdef REPORT_ON_SCREEN
			int32_t i32RecordedTime =
					(REC_16KHz == recordRate) ?
							((ui32SectorCount / 8) * 505) :
							((ui32SectorCount / 4) * 505);
			i32RecordedTime /= 1000;
			int32_t i32Minute = i32RecordedTime / 60;
			int32_t i32Second = i32RecordedTime % 60;

			text_setCursor(0, 16);
			text_printNumber(i32Minute / 10);
			text_printNumber(i32Minute % 10);
			text_printString(":");
			text_printNumber(i32Second / 10);
			text_printNumber(i32Second % 10);
			text_printString(" ");
			graphic_render();
#endif
			ui32BlockNumber = 0;
			pui8FileBufferPointer = pui8FileBuffer;

			/* Data block check: the forth byte of each block should always be zero */
			if ((0 == pui8FileBuffer[3]) && (0x00 == pui8FileBuffer[256 + 3]))
			{
				ui32SectorCount++;
				if (FR_OK
						!= f_write(&file, pui8FileBuffer, FILE_BUFFER_SIZE,
								&ui32WritenByte))
				{
					/* Can not write file data */
					f_close(&file);
#ifdef REPORT_ON_SCREEN
					text_putString("Sector ", FAST);
					text_printNumber(ui32SectorCount);
					text_putLine(" failed", FAST);
#endif
					return false;
				}
			}
			else
			{
#ifdef REPORT_ON_SCREEN
				text_putString("Byte check error", FAST);
#endif
			}
		}

		/* Only check the recording limit if timeSec greater than zero */
		if ((ui32LimitedSector > 0) && (ui32SectorCount > ui32LimitedSector))
		{
			bIsRecording = false;
		}
	}

	uint32_t ui32FileSize = ui32SectorCount * 512;
#ifdef REPORT_ON_SCREEN
	text_setCursor(0, 24);
	text_putString("Size: ", FAST);
	text_printNumber(ui32FileSize);
	text_putLine(" bytes", FAST);
#endif

	/* Update WAV header */
	uint32_t ui32SectorCalculatingVaraiable;
	/* ChunkSize (after RIFF): WAV file size - 8 */
	ui32SectorCalculatingVaraiable = ui32FileSize - 8;
	pui8WAVHeaderBuffer[4] = (ui32SectorCalculatingVaraiable & 0xff);
	pui8WAVHeaderBuffer[5] = ((ui32SectorCalculatingVaraiable >> 8) & 0xff);
	pui8WAVHeaderBuffer[6] = ((ui32SectorCalculatingVaraiable >> 16) & 0xff);
	pui8WAVHeaderBuffer[7] = ((ui32SectorCalculatingVaraiable >> 24) & 0xff);

	/* Number of sample */
	ui32SectorCalculatingVaraiable = (ui32SectorCount - 1) * 1010;
	pui8WAVHeaderBuffer[48] = (ui32SectorCalculatingVaraiable & 0xff);
	pui8WAVHeaderBuffer[49] = ((ui32SectorCalculatingVaraiable >> 8) & 0xff);
	pui8WAVHeaderBuffer[50] = ((ui32SectorCalculatingVaraiable >> 16) & 0xff);
	pui8WAVHeaderBuffer[51] = ((ui32SectorCalculatingVaraiable >> 24) & 0xff);

	/* Data size (file size - 512 header) */
	ui32SectorCalculatingVaraiable = ui32FileSize - 512;
	pui8WAVHeaderBuffer[508] = (ui32SectorCalculatingVaraiable & 0xff);
	pui8WAVHeaderBuffer[509] = ((ui32SectorCalculatingVaraiable >> 8) & 0xff);
	pui8WAVHeaderBuffer[510] = ((ui32SectorCalculatingVaraiable >> 16) & 0xff);
	pui8WAVHeaderBuffer[511] = ((ui32SectorCalculatingVaraiable >> 24) & 0xff);

	/* Write new WAV header to disk storage */
	if (FR_OK == f_lseek(&file, 0))
	{
		if (FR_OK
				!= f_write(&file, pui8WAVHeaderBuffer, WAV_HEADER_SIZE,
						&ui32WritenByte))
		{
			/* Can not write file header */
			f_close(&file);
#ifdef REPORT_ON_SCREEN
			text_putLine("Header failed", FAST);
#endif
			return false;
		}
	}
	else
	{
		/* Can not seek file to beginning */
		f_close(&file);
#ifdef REPORT_ON_SCREEN
		text_putLine("Seek failed", FAST);
#endif
		return false;
	}

	/* Clean up */
	f_close(&file);

	/* Finally, reset the VS10xx software, including re-uploading the
	 patches package, to make sure everything is set up properly. */
	acodec_deInitRecordAPCM();

#ifdef REPORT_ON_SCREEN
	text_putLine("Done", FAST);
#endif
	return true;
}

/**@}LIB_AUDIO_PRIVATE*/
/**@}LIB_AUDIO*/
/********************** (TM) PnL - Programming and Leverage ****END OF FILE****/
