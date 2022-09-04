#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"

#include "ff.h"
#include "diskio.h"

#include "TF.h"

static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject;   /* File object */

/*! @brief SDMMC host detect card configuration */
static const sdmmchost_detect_card_t s_sdCardDetect = {
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
	  //.cdTimeOut_ms = 3000U,
    .cdTimeOut_ms = (~0U),
};

void BOARD_USDHCClockConfiguration(void)
{
    /*configure system pll PFD2 fractional divider to 18*/
    CLOCK_InitSysPfd(kCLOCK_Pfd0, 0x12U);
    /* Configure USDHC clock source and divider */
    CLOCK_SetDiv(kCLOCK_Usdhc1Div, 0U);
    CLOCK_SetMux(kCLOCK_Usdhc1Mux, 1U);
}

status_t sdcardWaitCardInsert(void)
{
    /* Save host information. */
    g_sd.host.base = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd = &s_sdCardDetect; /* card detect type */
	
		//FreeRTOS下必须打开次中断
	  NVIC_SetPriority(SD_HOST_IRQ, 5U);
		
    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        PRINTF("SD host init fail\r\n");
        return kStatus_Fail;
    }
    /* power off card */
    SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);
    /* wait card insert */
    if (SD_WaitCardDetectStatus(SD_HOST_BASEADDR, &s_sdCardDetect, true) == kStatus_Success)
    {
        PRINTF("Card inserted.\r\n");
        /* power on the card */
        SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
    }
    else
    {
        PRINTF("Card detect fail.\r\n");
        return kStatus_Fail;
    }

    return kStatus_Success;
}

int SD_MOUNT(void)
{
    FRESULT rssult;
    if (sdcardWaitCardInsert() != kStatus_Success)
    {
			PRINTF("sdcard Wait Card Insert  outtime .\r\n");
			return -1;
    }
		rssult = f_mount(&g_fileSystem,"2:/",1U);
		PRINTF("f_mount result=%d  \r\n",rssult);

		//更改驱动器
		//rssult = f_chdrive(driverNumberBuffer);PRINTF("f_chdrive =%d \r\n",rssult);
		return 0;
}


