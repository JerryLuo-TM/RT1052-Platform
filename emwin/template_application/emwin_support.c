/*
 * The Clear BSD License
 * Copyright (c) 2017, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "fsl_debug_console.h"
#include "fsl_elcdif.h"
#include "fsl_gpio.h"

#include "GUI.h"
#include "WM.h"
#include "GUIDRV_Lin.h"
#include "emwin_support.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "elcdif.h"
#include "touch.h"
#include "malloc.h"

#define COLOR_CONVERSION GUICC_M565     //GUICC_M565  驱动颜色
#define DISPLAY_DRIVER   GUIDRV_LIN_16  //GUIDRV_LIN_16

#define LCD_BITS_PER_PIXEL 16
#define LCD_BYTES_PER_PIXEL (LCD_BITS_PER_PIXEL / 8)

#define GUI_BUFFERS 3  //使用多缓冲时的缓冲数量

uint8_t *s_vram_buffer;
uint32_t VRAM_SIZE;

static volatile int32_t s_LCDpendingBuffer = -1;

void LCDIF_IRQHandler(void)
{
    uint32_t intStatus;

    intStatus = ELCDIF_GetInterruptStatus(LCDIF);
    ELCDIF_ClearInterruptStatus(LCDIF, intStatus);

    if (intStatus & kELCDIF_CurFrameDone) {
        if (s_LCDpendingBuffer >= 0) {
            /* Send a confirmation that the given buffer is visible */
            GUI_MULTIBUF_Confirm(s_LCDpendingBuffer);
            s_LCDpendingBuffer = -1;
        }
    }

/*  Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    exception return operation might vector to incorrect interrupt 
*/
#if defined __CORTEX_M && (__CORTEX_M == 4U)
#error
    __DSB();
#endif
}

/*******************************************************************************
 * Application implemented functions required by emWin library
 ******************************************************************************/
void LCD_X_Config(void)
{
    VRAM_SIZE = LCD_WIDTH * LCD_HEIGHT * LCD_BYTES_PER_PIXEL;
    //PRINTF("s_vram_buffer size=%d\r\n",(VRAM_SIZE * GUI_BUFFERS * LCD_BYTES_PER_PIXEL));
    s_vram_buffer = mymalloc(SRAMEXC, VRAM_SIZE * GUI_BUFFERS);
    //PRINTF("s_vram_buffer address: 0x%x \r\n",s_vram_buffer);

    GUI_MULTIBUF_Config(GUI_BUFFERS);

    GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);

    LCD_SetSizeEx(0, LCD_WIDTH, LCD_HEIGHT);    //设置可见区尺寸
    LCD_SetVSizeEx(0, LCD_WIDTH, LCD_HEIGHT);   //设置虚拟显示区尺寸
    LCD_SetVRAMAddrEx(0, (void *)s_vram_buffer);
}

int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void *p)
{
    uint32_t addr;
    int result = 0;
    LCD_X_SHOWBUFFER_INFO *pData;
    switch (Cmd) {
        case LCD_X_INITCONTROLLER: {
            ELCDIF_Init();
        } break;

        case LCD_X_SHOWBUFFER: {
            pData = (LCD_X_SHOWBUFFER_INFO *)p;

            /* Calculate address of the given buffer */
            addr = (uint32_t)s_vram_buffer + VRAM_SIZE * pData->Index;

            /* Make the given buffer visible */
            ELCDIF_SetNextBufferAddr(LCDIF, addr);

            /* Remember buffer index to be used by ISR */
            s_LCDpendingBuffer = pData->Index;
            while (s_LCDpendingBuffer >= 0);
        } break;

        default: {
            result = -1;
        } break;
    }

    return result;
}

void GUI_X_Init(void)
{
}

/* Dummy RTOS stub required by emWin */
void GUI_X_InitOS(void)
{
}

/* Dummy RTOS stub required by emWin */
void GUI_X_Lock(void)
{
}

/* Dummy RTOS stub required by emWin */
void GUI_X_Unlock(void)
{
}

/* Dummy RTOS stub required by emWin */
U32 GUI_X_GetTaskId(void)
{
    return 0;
}

/*
*********************************************************************************************************
*                                          GUI_X_ExecIdle()
*********************************************************************************************************
*/
void GUI_X_ExecIdle(void)
{
	vTaskDelay(1);
}

/*
*********************************************************************************************************
*                                        TIMING FUNCTIONS
*
* Notes: Some timing dependent routines of uC/GUI require a GetTime and delay funtion.
*        Default time unit (tick), normally is 1 ms.
*********************************************************************************************************
*/

//获取系统时间,此处时间单元为1ms
GUI_TIMER_TIME GUI_X_GetTime(void)
{
	return ((GUI_TIMER_TIME)xTaskGetTickCount() * 1000) / configTICK_RATE_HZ;
}

//GUI延时函数
void GUI_X_Delay(int Period)
{
	//调用操作系统的延时函数挂起1ms
	vTaskDelay(Period*configTICK_RATE_HZ/1000);
}

void *emWin_memcpy(void *pDst, const void *pSrc, long size)
{
    return memcpy(pDst, pSrc, size);
}

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }

