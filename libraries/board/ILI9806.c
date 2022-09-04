#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "fsl_elcdif.h"
#include "fsl_pwm.h"
#include "fsl_xbara.h"

#include "ILI9806.h"
#include "emwin_support.h"

//软件延时  初始化用
void Delayms3(int count)  // 大约1ms延时函数
{
	volatile unsigned long int i,j,Delaynum=SystemCoreClock/20000;
	for(i=0;i<count;i++)
		for(j=0;j<Delaynum;j++); 
}

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
PinsLCD_SPI:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: A7, peripheral:  GPIO3, signal: 'gpio_io, 26', pin_signal: GPIO_EMC_40, direction: OUTPUT, pull_up_down_config: Pull_Up_22K_Ohm}
  - {pin_num: A12, peripheral: GPIO2, signal: 'gpio_io, 24', pin_signal: GPIO_B1_08, direction: OUTPUT, gpio_init_state: 'true', pull_up_down_config: Pull_Up_22K_Ohm}
  - {pin_num: A13, peripheral: GPIO2, signal: 'gpio_io, 25', pin_signal: GPIO_B1_09, direction: OUTPUT, pull_up_down_config: Pull_Up_22K_Ohm}
  - {pin_num: B12, peripheral: GPIO2, signal: 'gpio_io, 23', pin_signal: GPIO_B1_07, direction: OUTPUT, pull_up_down_config: Pull_Up_22K_Ohm}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
void PinsLCD_SPI(void) 
{
	CLOCK_EnableClock(kCLOCK_Iomuxc);

	gpio_pin_config_t gpio2_pinB12_config = {kGPIO_DigitalOutput,0U,kGPIO_NoIntmode};
	GPIO_PinInit(GPIO2, 23U, &gpio2_pinB12_config);

	gpio_pin_config_t gpio2_pinA12_config = {kGPIO_DigitalOutput,1U,kGPIO_NoIntmode};
	GPIO_PinInit(GPIO2, 24U, &gpio2_pinA12_config);

	gpio_pin_config_t gpio2_pinA13_config = {kGPIO_DigitalOutput,0U,kGPIO_NoIntmode};
	GPIO_PinInit(GPIO2, 25U, &gpio2_pinA13_config);

	gpio_pin_config_t gpio3_pinA7_config  = {kGPIO_DigitalOutput,0U,kGPIO_NoIntmode};
	GPIO_PinInit(GPIO3, 26U, &gpio3_pinA7_config);

	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_07_GPIO2_IO23,0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_08_GPIO2_IO24,0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_09_GPIO2_IO25,0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0U);
		
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_07_GPIO2_IO23,0xD0B0u);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_08_GPIO2_IO24,0xD0B0u);                               
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_09_GPIO2_IO25,0xD0B0u);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_40_GPIO3_IO26,0xD0B0u);
}

/*  ipg_clk = 134MHz
	如果不分频 16bit 装在器在 134M 会溢出
*/
void LCD_PWM_Init(void) 
{
	pwm_config_t pwmConfig;
    pwm_signal_param_t pwmSignal[2];
    uint32_t pwmSourceClockInHz;
    uint32_t pwmFrequencyInHz = 10000;

	/* GPIO_B1_15 is configured as FLEXPWM4_PWMA03 */
	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_15_FLEXPWM4_PWMA03,0U);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_15_FLEXPWM4_PWMA03, 0x10B0U);

    // CLOCK_SetDiv(kCLOCK_AhbDiv, 0x2); /* Set AHB PODF to 2, divide by 3 */
    // CLOCK_SetDiv(kCLOCK_IpgDiv, 0x3); /* Set IPG PODF to 3, divede by 4 */

    /* Set the PWM Fault inputs to a low value */
    XBARA_Init(XBARA1);
	XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm4Fault0);
	XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm4Fault1);
    XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm1234Fault2);
    XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm1234Fault3);
	// PWM4->SM[3].DISMAP[0] = 0;

	PWM_GetDefaultConfig(&pwmConfig);
	/* Use full cycle reload */
    pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle;
    pwmConfig.pairOperation = kPWM_ComplementaryPwmA;
	pwmConfig.prescale = kPWM_Prescale_Divide_128; // pwm clk = fclk/128 
    pwmConfig.enableDebugMode = true;
    if (PWM_Init(PWM4, kPWM_Module_3, &pwmConfig) == kStatus_Fail) {
        PRINTF("PWM initialization failed\n");
        return;
    }

	pwmSourceClockInHz = CLOCK_GetFreq(kCLOCK_IpgClk);
	PRINTF("\r\n PWM CLOCK_GetFreq(kCLOCK_IpgClk) = %d \r\n", pwmSourceClockInHz);

    pwmSignal[0].pwmChannel = kPWM_PwmA;
    pwmSignal[0].level = kPWM_HighTrue;
    pwmSignal[0].dutyCyclePercent = 80;
    pwmSignal[1].pwmChannel = kPWM_PwmB;
    pwmSignal[1].level = kPWM_HighTrue;
    pwmSignal[1].dutyCyclePercent = 80;

	PWM_SetupPwm(PWM4, kPWM_Module_3, pwmSignal, 1, kPWM_SignedCenterAligned, pwmFrequencyInHz, pwmSourceClockInHz);
	PWM_SetPwmLdok(PWM4, kPWM_Control_Module_3, true); //加载设定占空比参数
	PWM_StartTimer(PWM4, kPWM_Control_Module_3);
	PWM_UpdatePwmDutycycle(PWM4, kPWM_Module_3, kPWM_PwmA, kPWM_SignedCenterAligned, 80);
	PWM_SetPwmLdok(PWM4, kPWM_Control_Module_3, true);
}

//ILI9806 SPI传输时序
unsigned char ILI9806_SPI_RW(unsigned char uchar)
{
	unsigned char bit_ctr;
	for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
	{
		if((uchar&0x80)==0x00){ILI9806_SDI(0);}
		else{ILI9806_SDI(1);}    
		uchar=uchar<<1;            
		ILI9806_SCL(1);                      
		//uchar|=MISO;       		 
		ILI9806_SCL(0);            		  
	}
	return(uchar);           		 
}

//ILI9806写命令参数
void ILI9806_WriteComm(unsigned char CMD)
{
	ILI9806_SCL(0);
	ILI9806_SDI(0);
	ILI9806_SCL(1);
	ILI9806_SCL(0);
	ILI9806_SPI_RW(CMD);
}

//ILI9806写数据参数
void ILI9806_WriteData(unsigned char DAT)
{
	ILI9806_SCL(0);
	ILI9806_SDI(1);
	ILI9806_SCL(1);
	ILI9806_SCL(0);
	ILI9806_SPI_RW(DAT);
}

void LCD_DPI_Init(void)
{
	//SPI引脚初始化
	PinsLCD_SPI();
	
	//片选拉低 使能
	ILI9806_CS(0);
	
	//复位LCD 液晶屏
	LCD_REST(1);
	Delayms3(1);
	LCD_REST(0);
	Delayms3(50);
	LCD_REST(1);
	Delayms3(50);
	
    //上面的这两个延时一定要加上    尤其是最后一个延时 必须要防止还未复位就操作液晶
	ILI9806_WriteComm(0xFF); // EXTC Command Set enable register 
	ILI9806_WriteData(0xFF); 
	ILI9806_WriteData(0x98); 
	ILI9806_WriteData(0x06); 

	ILI9806_WriteComm(0xBA); // SPI Interface Setting 
	ILI9806_WriteData(0xE0); 

	ILI9806_WriteComm(0xBC); // GIP 1 
	ILI9806_WriteData(0x03); 
	ILI9806_WriteData(0x0F); 
	ILI9806_WriteData(0x63); 
	ILI9806_WriteData(0x69); 
	ILI9806_WriteData(0x01); 
	ILI9806_WriteData(0x01); 
	ILI9806_WriteData(0x1B); 
	ILI9806_WriteData(0x11); 
	ILI9806_WriteData(0x70); 
	ILI9806_WriteData(0x73); 
	ILI9806_WriteData(0xFF); 
	ILI9806_WriteData(0xFF); 
	ILI9806_WriteData(0x08); 
	ILI9806_WriteData(0x09); 
	ILI9806_WriteData(0x05); 
	ILI9806_WriteData(0x00);
	ILI9806_WriteData(0xEE); 
	ILI9806_WriteData(0xE2); 
	ILI9806_WriteData(0x01); 
	ILI9806_WriteData(0x00);
	ILI9806_WriteData(0xC1); 

	ILI9806_WriteComm(0xBD); // GIP 2 
	ILI9806_WriteData(0x01); 
	ILI9806_WriteData(0x23); 
	ILI9806_WriteData(0x45); 
	ILI9806_WriteData(0x67); 
	ILI9806_WriteData(0x01); 
	ILI9806_WriteData(0x23); 
	ILI9806_WriteData(0x45); 
	ILI9806_WriteData(0x67); 

	ILI9806_WriteComm(0xBE); // GIP 3 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x22); 
	ILI9806_WriteData(0x27); 
	ILI9806_WriteData(0x6A); 
	ILI9806_WriteData(0xBC); 
	ILI9806_WriteData(0xD8); 
	ILI9806_WriteData(0x92); 
	ILI9806_WriteData(0x22); 
	ILI9806_WriteData(0x22); 

	ILI9806_WriteComm(0xC7); // Vcom 
	ILI9806_WriteData(0x1E);
	 
	ILI9806_WriteComm(0xED); // EN_volt_reg 
	ILI9806_WriteData(0x7F); 
	ILI9806_WriteData(0x0F); 
	ILI9806_WriteData(0x00); 

	ILI9806_WriteComm(0xC0); // Power Control 1
	ILI9806_WriteData(0xE3); 
	ILI9806_WriteData(0x0B); 
	ILI9806_WriteData(0x00);
	 
	ILI9806_WriteComm(0xFC);
	ILI9806_WriteData(0x08); 

	ILI9806_WriteComm(0xDF); // Engineering Setting 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x02); 

	ILI9806_WriteComm(0xF3); // DVDD Voltage Setting 
	ILI9806_WriteData(0x74); 

	ILI9806_WriteComm(0xB4); // Display Inversion Control 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x00); 
  
  ILI9806_WriteComm(0x2A); // column
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x03); 
  ILI9806_WriteData(0x55);

	ILI9806_WriteComm(0xF7); // 480x854
	ILI9806_WriteData(0x81); 

	ILI9806_WriteComm(0xB1); // Frame Rate 
	ILI9806_WriteData(0x00); 
	ILI9806_WriteData(0x10); 
	ILI9806_WriteData(0x14); 

	ILI9806_WriteComm(0xF1); // Panel Timing Control 
	ILI9806_WriteData(0x29); 
	ILI9806_WriteData(0x8A); 
	ILI9806_WriteData(0x07); 

	ILI9806_WriteComm(0xF2); //Panel Timing Control 
	ILI9806_WriteData(0x40); 
	ILI9806_WriteData(0xD2); 
	ILI9806_WriteData(0x50); 
	ILI9806_WriteData(0x28); 

	ILI9806_WriteComm(0xC1); // Power Control 2 
	ILI9806_WriteData(0x17);
	ILI9806_WriteData(0X85); 
	ILI9806_WriteData(0x85); 
	ILI9806_WriteData(0x20); 

	ILI9806_WriteComm(0xE0); 
	ILI9806_WriteData(0x00); //P1 
	ILI9806_WriteData(0x0C); //P2 
	ILI9806_WriteData(0x15); //P3 
	ILI9806_WriteData(0x0D); //P4 
	ILI9806_WriteData(0x0F); //P5 
	ILI9806_WriteData(0x0C); //P6 
	ILI9806_WriteData(0x07); //P7 
	ILI9806_WriteData(0x05); //P8 
	ILI9806_WriteData(0x07); //P9 
	ILI9806_WriteData(0x0B); //P10 
	ILI9806_WriteData(0x10); //P11 
	ILI9806_WriteData(0x10); //P12 
	ILI9806_WriteData(0x0D); //P13 
	ILI9806_WriteData(0x17); //P14 
	ILI9806_WriteData(0x0F); //P15 
	ILI9806_WriteData(0x00); //P16 

	ILI9806_WriteComm(0xE1); 
	ILI9806_WriteData(0x00); //P1 
	ILI9806_WriteData(0x0D); //P2 
	ILI9806_WriteData(0x15); //P3 
	ILI9806_WriteData(0x0E); //P4 
	ILI9806_WriteData(0x10); //P5 
	ILI9806_WriteData(0x0D); //P6 
	ILI9806_WriteData(0x08); //P7 
	ILI9806_WriteData(0x06); //P8 
	ILI9806_WriteData(0x07); //P9 
	ILI9806_WriteData(0x0C); //P10 
	ILI9806_WriteData(0x11); //P11 
	ILI9806_WriteData(0x11); //P12 
	ILI9806_WriteData(0x0E); //P13 
	ILI9806_WriteData(0x17); //P14 
	ILI9806_WriteData(0x0F); //P15 
	ILI9806_WriteData(0x00); //P16
	
	ILI9806_WriteComm(0x35); //Tearing Effect ON 
	ILI9806_WriteData(0x00); 

	ILI9806_WriteComm(0x36); //扫描方向 Bit7 Bit6 Bit5
	ILI9806_WriteData(0x60);         //  MY   MX   MV

	ILI9806_WriteComm(0x3A);  //DPI Interface Selection
	ILI9806_WriteData(0x75);  //0x77 24   0x75 16

	ILI9806_WriteComm(0xB6); // SPI Interface Setting 
	ILI9806_WriteData(0xA2);

	//16bus remap 24
	// ILI9806_WriteComm(0xB8);
	// ILI9806_WriteData(0x20);

	ILI9806_WriteComm(0x11); //Exit Sleep 
	Delayms3(20);
	ILI9806_WriteComm(0x29); // Display On 
	//ILI9806_WriteComm(0x20);
	Delayms3(10);
}




