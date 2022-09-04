#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "i2c.h"

#include <stdio.h>
#include <string.h>

#ifndef CODEC_USEGPIO
#if defined(FSL_FEATURE_SOC_LPI2C_COUNT) && (FSL_FEATURE_SOC_LPI2C_COUNT)
lpi2c_master_handle_t i2cHandle = {0};
#else
i2c_master_handle_t i2cHandle = {{0, 0, kI2C_Write, 0, 0, NULL, 0}, 0, 0, NULL, NULL};
#endif
#endif /* CODEC_USEGPIO */

//IIC1 引脚初始化
void BOARD_InitI2C1Pins(void)
{
    CLOCK_EnableClock(kCLOCK_Iomuxc);   /* iomuxc clock (iomuxc_clk_enable): 0x03u */

    /*Clock setting for LPI2C*/
    CLOCK_SetMux(kCLOCK_Lpi2cMux, LPI2C_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, LPI2C_CLOCK_SOURCE_DIVIDER);

    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL,1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA,1U);

    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL,0xD8B0u);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA,0xD8B0u);
}

//IIC裸机初始化
void I2C_Init(void)
{
    lpi2c_master_config_t i2cConfig = {0};
    //引脚初始化
    BOARD_InitI2C1Pins();
    //获取默认配置参数
    LPI2C_MasterGetDefaultConfig(&i2cConfig);
    //更改通讯速率
    i2cConfig.baudRate_Hz = I2C_BAUDRATE;
    //初始化IIC 外设
    LPI2C_MasterInit(LPI2C1, &i2cConfig, I2C_CLOCK_FREQ);
    LPI2C_MasterTransferCreateHandle(LPI2C1, &i2cHandle, NULL, NULL);
}

//IIC裸机写操作
status_t I2C_Write(uint32_t devADD,uint32_t subAdd,uint8_t *dataBuff,uint16_t dataLen)
{
    // lpi2c_master_transfer_t *xfer = &(handle->xfer);
    lpi2c_master_transfer_t xfer;
    status_t status;

    xfer.slaveAddress = devADD;
    xfer.direction = kLPI2C_Write;
    xfer.subaddress = subAdd;
    xfer.subaddressSize = 0x01;
    xfer.data = dataBuff;
    xfer.dataSize = dataLen;
    xfer.flags = kLPI2C_TransferDefaultFlag;

    status = LPI2C_MasterTransferBlocking(LPI2C1, &xfer);

    return status;
}

//IIC裸机读操作
uint32_t I2C_Read(uint32_t devADD,uint32_t subAdd,uint8_t* dataBuffer, uint16_t dataLen)
{
	lpi2c_master_transfer_t masterXfer = {0};
	status_t reVal = kStatus_Fail;

	masterXfer.slaveAddress = devADD;
	masterXfer.direction = kLPI2C_Read;
	masterXfer.subaddress = subAdd;
	masterXfer.subaddressSize = 0x01;
	masterXfer.data = dataBuffer;
	masterXfer.dataSize = dataLen;
	masterXfer.flags = kLPI2C_TransferDefaultFlag;

	reVal = LPI2C_MasterTransferBlocking(LPI2C1, &masterXfer);

	if (reVal != kStatus_Success)
	{
			return 1;
	}

	return 0;
}


