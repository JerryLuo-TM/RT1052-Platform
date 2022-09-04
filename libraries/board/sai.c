#include "fsl_sai.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"

#include "fsl_debug_console.h"

#include "clock_config.h"
#include "pin_mux.h"
#include "board.h"

#include "wm8960.h"
#include "i2c.h"
#include "sai.h"

#include "music.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* SAI instance and clock */
#define DEMO_CODEC_WM8960
//#define DEMO_SAI SAI1
#define DEMO_SAI SAI2

//#define DEMO_SAI_IRQ SAI1_IRQn
//#define SAI_TxIRQHandler SAI1_IRQHandler

/* Select Audio/Video PLL (786.48 MHz) as sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_SELECT (2U)
/* Clock pre divider for sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER (0U)
/* Clock divider for sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_DIVIDER (63U)
/* Get frequency of sai1 clock */
#define DEMO_SAI_CLK_FREQ                                                        \
    (CLOCK_GetFreq(kCLOCK_AudioPllClk) / (DEMO_SAI1_CLOCK_SOURCE_DIVIDER + 1U) / \
    (DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER + 1U))



/*******************************************************************************
 * Variables
 ******************************************************************************/
sai_handle_t txHandle = {0};
wm8960_handle_t codecHandle = {0};

static volatile bool isFinished = false;

/*
 * AUDIO PLL setting: Frequency = Fref * (DIV_SELECT + NUM / DENOM)
 *                              = 24 * (32 + 77/100)
 *                              = 786.48 MHz
 */
const clock_audio_pll_config_t audioPllConfig = {
    .loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
    .postDivider = 1,   /* Divider after the PLL, should only be 1, 2, 4, 8, 16. */
    .numerator   = 77,  /* 30 bit numerator of fractional loop divider. */
    .denominator = 100, /* 30 bit denominator of fractional loop divider */
};

void BOARD_EnableSaiMclkOutput(bool enable)
{
    if (enable){
        IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_SAI2_MCLK_DIR_MASK;
    } else {
        IOMUXC_GPR->GPR1 &= (~IOMUXC_GPR_GPR1_SAI2_MCLK_DIR_MASK);
    }
}

static void callback(I2S_Type *base, sai_handle_t *handle, status_t status, void *userData)
{
    isFinished = true;
}


void SAI_Inint(void)
{
    status_t status;
    sai_config_t config;
    uint32_t mclkSourceClockHz = 0U;
    sai_transfer_format_t format;
    sai_transfer_t xfer;

    uint32_t temp = 0;

    //BOARD_InitPins();
    CLOCK_InitAudioPll(&audioPllConfig);

    /*Clock setting for SAI2*/
    CLOCK_SetMux(kCLOCK_Sai2Mux, DEMO_SAI1_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Sai2PreDiv, DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER);
    CLOCK_SetDiv(kCLOCK_Sai2Div, DEMO_SAI1_CLOCK_SOURCE_DIVIDER);

    /*Enable MCLK clock*/
    BOARD_EnableSaiMclkOutput(true);
    PRINTF("SAI example started! \r\n");

    /*
     * config.masterSlave = kSAI_Master;
     * config.mclkSource = kSAI_MclkSourceSysclk;
     * config.protocol = kSAI_BusLeftJustified;
     * config.syncMode = kSAI_ModeAsync;
     * config.mclkOutputEnable = true;
     */
    SAI_TxGetDefaultConfig(&config);
    SAI_TxInit(DEMO_SAI, &config);

    /* Configure the audio format */
    format.bitWidth      = kSAI_WordWidth16bits;
    format.channel       = 0U;
    format.sampleRate_Hz = kSAI_SampleRate22050Hz;
    format.masterClockHz = DEMO_SAI_CLK_FREQ;
    format.protocol      = config.protocol;
    format.stereo        = kSAI_Stereo;
    format.isFrameSyncCompact = false;
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;
#endif
    PRINTF("format masterClock HZ = %d \r\n",  format.masterClockHz);

    /* Configure Codec I2C */
    codecHandle.base = LPI2C1;
    codecHandle.i2cHandle = &i2cHandle;

    // WM8960_Init(NULL);
    WM8960_ConfigDataFormat(&codecHandle, format.masterClockHz, format.sampleRate_Hz, format.bitWidth);

    SAI_TransferTxCreateHandle(DEMO_SAI, &txHandle, callback, NULL);
    mclkSourceClockHz = DEMO_SAI_CLK_FREQ;
    SAI_TransferTxSetFormat(DEMO_SAI, &txHandle, &format, mclkSourceClockHz, format.masterClockHz);

    /*  xfer structure */
    temp = (uint32_t)music;
    xfer.data = (uint8_t *)temp;
    xfer.dataSize = MUSIC_LEN;
    status = SAI_TransferSendNonBlocking(DEMO_SAI, &txHandle, &xfer);

	PRINTF("send status = %d  \r\n", status);

    /* Wait until finished */
    while (isFinished != true);

    while (!(DEMO_SAI->TCSR & I2S_TCSR_FWF_MASK))
    {
    }

    /* Once transfer finish, disable SAI instance. */
    SAI_Deinit(DEMO_SAI);

	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_10_JTAG_TDO,  1U);/* GPIO_AD_B0_10 is configured as SAI2_MCLK */
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_09_JTAG_TDI,  1U);/* GPIO_AD_B0_09 is configured as SAI2_TX_DATA */
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,1U);/* GPIO_AD_B0_05 is configured as SAI2_TX_BCLK */
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_04_GPIO1_IO04,1U);/* GPIO_AD_B0_04 is configured as SAI2_TX_SYNC */

    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_10_JTAG_TDO,0x10B0u);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_09_JTAG_TDI,0x10B0u);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0x10B0u);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_04_GPIO1_IO04,0x10B0u);

    PRINTF("\n\r SAI example finished!\n\r ");

}





