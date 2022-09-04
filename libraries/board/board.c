/*
 * The Clear BSD License
 * Copyright 2017 NXP
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

#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "board.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
 
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}

/* Get debug console frequency. */
uint32_t BOARD_DebugConsoleSrcFreq(void)
{
    uint32_t freq;

    /* To make it simple, we assume default PLL and divider settings, and the only variable
       from application is use PLL3 source or OSC source */
    if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
    {
        freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }
    else
    {
        freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }

    return freq;
}

/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    uint32_t uartClkSrcFreq = BOARD_DebugConsoleSrcFreq();

    DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
}

/* MPU configuration. */
//设置RT1052需要保护的存储块
//必须对部分存储区域进行MPU保护,否则可能导致程序运行异常 
//Cortex-M7内核的MPU一共有16个region，每个region的起始地址、属性和大小等都可以设置，
//MPU的详细信息以及寄存器内容请参考《Cortex-M7编程手册(ST官方)》中的第4.6节
void BOARD_ConfigMPU(void)
{
    /* Disable I cache and D cache */
    if (SCB_CCR_IC_Msk == (SCB_CCR_IC_Msk & SCB->CCR)) {
        SCB_DisableICache();
    }
    if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR)) {
        SCB_DisableDCache();
    }

    /* Disable MPU */
    ARM_MPU_Disable();

    /* MPU configure:
     * Use ARM_MPU_RASR(DisableExec, AccessPermission, TypeExtField, IsShareable, IsCacheable, IsBufferable, SubRegionDisable, Size) 
     * API in core_cm7.h.
     * param DisableExec       Instruction access (XN) disable bit,0=instruction fetches enabled, 1=instruction fetches disabled.
     * param AccessPermission  Data access permissions, allows you to configure read/write access for User and Privileged mode.
     *      Use MACROS defined in core_cm7.h: ARM_MPU_AP_NONE/ARM_MPU_AP_PRIV/ARM_MPU_AP_URO/ARM_MPU_AP_FULL/ARM_MPU_AP_PRO/ARM_MPU_AP_RO
     * Combine TypeExtField/IsShareable/IsCacheable/IsBufferable to configure MPU memory access attributes.
     *  TypeExtField  IsShareable  IsCacheable  IsBufferable   Memory Attribtue    Shareability        Cache
     *     0             x           0           0             Strongly Ordered    shareable
     *     0             x           0           1              Device             shareable           
     *     0             0           1           0              Normal             not shareable   Outer and inner write through no write allocate           
     *     0             0           1           1              Normal             not shareable   Outer and inner write back no write allocate           
     *     0             1           1           0              Normal             shareable       Outer and inner write through no write allocate    
     *     0             1           1           1              Normal             shareable       Outer and inner write back no write allocate    
     *     1             0           0           0              Normal             not shareable   outer and inner noncache
     *     1             1           0           0              Normal             shareable       outer and inner noncache
     *     1             0           1           1              Normal             not shareable   outer and inner write back write/read acllocate
     *     1             1           1           1              Normal             shareable       outer and inner write back write/read acllocate
     *     2             x           0           0              Device              not shareable   
     *  Above are normal use settings, if your want to see more details or want to config different inner/outter cache policy.      
     *  please refer to Table 4-55 /4-56 in arm cortex-M7 generic user guide <dui0646b_cortex_m7_dgug.pdf>
     * param SubRegionDisable  Sub-region disable field. 0=sub-region is enabled, 1=sub-region is disabled.
     * param Size              Region size of the region to be configured. use ARM_MPU_REGION_SIZE_xxx MACRO in core_cm7.h.
     */

    /* Region 0 setting: Memory with Device type, not shareable, non-cacheable. */
    MPU->RBAR = ARM_MPU_RBAR(0, 0xC0000000U);											//region 0,起始地址0XC000 0000
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_512MB);	//可执行,全访问,设备,禁止共享,禁止cache,禁止缓存,禁止子region,512MB大小

    /* Region 1 setting: Memory with Device type, not shareable,  non-cacheable. */
    MPU->RBAR = ARM_MPU_RBAR(1, 0x80000000U);											//region 1,起始地址0X8000 0000,SDRAM首地址
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_1GB);	  //可执行,全访问,设备,禁止共享,禁止cache,禁止缓存,禁止子region,1GB大小

/* Region 2 setting */
#if defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1)
    /* Setting Memory with Normal type, not shareable, outer/inner write back. */
    MPU->RBAR = ARM_MPU_RBAR(2, 0x60000000U);											//region 2,起始地址0X6000 0000,FlexSPI首地址	
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_512MB);	//可执行,全访问,写回,禁止共享,允许cache,允许缓存,禁止子region,512MB大小
#else
    /* Setting Memory with Device type, not shareable, non-cacheable. */
    MPU->RBAR = ARM_MPU_RBAR(2, 0x60000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_512MB);
#endif
		
    /* Region 3 setting: Memory with Device type, not shareable, non-cacheable. */
    MPU->RBAR = ARM_MPU_RBAR(3, 0x00000000U);											//region 3,起始地址0X0000 0000,ITCM首地址(包含ROMCP,DTCM,OCRAM)
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_1GB);	//可执行,全访问,设备,禁止共享,禁止cache,禁止缓存,禁止子region,1GB大小
		
    /* Region 4 setting: Memory with Normal type, not shareable, outer/inner write back */
    MPU->RBAR = ARM_MPU_RBAR(4, 0x00000000U);											//region 4,起始地址0X0000 0000,ITCM首地址,相同地址段设置,region大的覆盖小的
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_128KB);	//可执行,全访问,写回,禁止共享,允许cache,允许缓存,禁止子region,128KB大小
		
    /* Region 5 setting: Memory with Normal type, not shareable, outer/inner write back */
    MPU->RBAR = ARM_MPU_RBAR(5, 0x20000000U);											//region 5,起始地址0X2000 0000,DTCM首地址,相同地址段设置,region大的覆盖小的
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_128KB);	//可执行,全访问,写回,禁止共享,允许cache,允许缓存,禁止子region,128KB大小
		
    /* Region 6 setting: Memory with Normal type, not shareable, outer/inner write back */
    MPU->RBAR = ARM_MPU_RBAR(6, 0x20200000U);											//region 6,起始地址0X2020 0000,OCRAM首地址,相同地址段设置,region大的覆盖小的
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_256KB);	//可执行,全访问,写回,允许共享,允许cache,禁止缓存,禁止子region,256KB大小
		
    /* Region 7 setting: Memory with Normal type, not shareable, outer/inner write back */
    MPU->RBAR = ARM_MPU_RBAR(7, 0x80000000U);											//region 7,起始地址0X8000 0000,SDRAM首地址,相同地址段设置,region大的覆盖小的
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_16MB);	//可执行,全访问,写回,禁止共享,允许cache,允许缓存,禁止子region,2MB大小 
		
    /* Region 8 setting, set last 16MB of SDRAM can't be accessed by cache, glocal variables which are not expected to be
     * accessed by cache can be put here */
    /* Memory with Normal type, not shareable, non-cacheable */
    MPU->RBAR = ARM_MPU_RBAR(15, 0x80600000);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 1, 1, 1, 0, ARM_MPU_REGION_SIZE_32MB);
		
    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
		
    /* Enable I cache and D cache */
    SCB_EnableDCache();
    SCB_EnableICache();
}


//中断优先级分组设置
//grpx: 优先级组，可选择如下5组中的一组
//NVIC_PRIORITYGROUP_0: 0位抢占优先级
//                      4位子优先级
//NVIC_PRIORITYGROUP_1: 1位抢占优先级
//                      3位子优先级
//NVIC_PRIORITYGROUP_2: 2位抢占优先级
//                      2位子优先级
//NVIC_PRIORITYGROUP_3: 3位抢占优先级
//                      1位子优先级
//NVIC_PRIORITYGROUP_4: 4位抢占优先级
//                      0位子优先级
//
void RT1052_NVIC_SetPriorityGrouping(uint8_t grpx)
{
	NVIC_SetPriorityGrouping((uint32_t)grpx);
}

//设置中断优先级
//IRQn:	中断源
//prep：抢占优先级
//subp：子优先级
void RT1052_NVIC_SetPriority(IRQn_Type IRQn,uint32_t prep,uint32_t subp)
{ 
	uint32_t prioritygroup = 0x00;
  
	prioritygroup=NVIC_GetPriorityGrouping();
	NVIC_SetPriority(IRQn,NVIC_EncodePriority(prioritygroup,prep,subp));
}






