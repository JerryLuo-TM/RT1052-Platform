#ifndef _I2C_H_
#define _I2C_H_

#include "fsl_common.h"
#include "fsl_lpi2c_freertos.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Select USB1 PLL (480 MHz) as master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_DIVIDER (5U)

#define I2C_CLOCK_FREQ        ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (LPI2C_CLOCK_SOURCE_DIVIDER + 1U))

#define I2C_BAUDRATE 100000U

extern lpi2c_master_handle_t i2cHandle;


void BOARD_InitI2C1Pins(void);

void     I2C_Init(void);
status_t I2C_Write(uint32_t devADD,uint32_t subAdd,uint8_t *dataBuff,uint16_t dataLen);
uint32_t I2C_Read(uint32_t devADD,uint32_t subAdd,uint8_t* dataBuffer, uint16_t dataLen);


#endif
