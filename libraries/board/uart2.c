#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "fsl_lpuart.h"
#include "pin_mux.h"
#include "fsl_common.h"
#include "clock_config.h"

#include "board.h"
#include "uart2.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t txbuff[] = "Uart2 Init OK\r\n";
uint8_t rxbuff[20] = {0};


status_t BOARD_InitUART2(void)
{
    status_t status;
    lpuart_config_t config;

    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_02_LPUART2_TX,0U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_03_LPUART2_RX,0U);

    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_02_LPUART2_TX,0x10B0u);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_03_LPUART2_RX,0x10B0u);

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = 115200;
    config.enableTx = true;
    config.enableRx = true;

    status=LPUART_Init(LPUART2, &config, BOARD_DebugConsoleSrcFreq());

    return status;
    //LPUART_WriteBlocking(LPUART2, txbuff, sizeof(txbuff) - 1);
}





