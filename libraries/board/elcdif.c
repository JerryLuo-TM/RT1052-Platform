#include "fsl_debug_console.h"
#include "fsl_elcdif.h"
#include "fsl_gpio.h"

#include "board.h"

#include "emwin_support.h"
#include "elcdif.h"
#include "ILI9806.h"

#include "GUI.h"

_elcdif_dev lcdelcdif; //管理LCD LTDC的重要参数

//RGB888转RGB565
unsigned short RGB888ToRGB565(unsigned int n888Color)
{
	unsigned short n565Color = 0;
	// 获取RGB单色，并截取高位
	unsigned char cRed   = (n888Color & RGB888_RED) >> 19;
	unsigned char cGreen = (n888Color & RGB888_GREEN) >> 10;
	unsigned char cBlue  = (n888Color & RGB888_BLUE) >> 3;
	// 连接
	n565Color = (cRed << 11) + (cGreen << 5) + (cBlue << 0);
	return n565Color;
}

//LCD控制器时钟初始化
//LCD时钟设置,LCD CLK=24Mhz*loopdiv/postdiv/lcdif1prediv/lcdif1div
//如要设置LCD CLK=9.3Mhz，那么LCD CLK=24*31/8/5/2=9.3Mhz
//loopdiv: PLL5(VIDEO PLL)的loop Divider，可选范围27~54
//postdiv: PLL5(VIDEO PLL)的post Divider，可选范围1，2，4，8
//lcdif1prediv：LCDIF PRE分频值，可选范围1~8
//lcdif1div：   LCDIF 分频值，   可选范围1~8
void BOARD_InitLcdifPixelClock(void)
{
    /*
		(480 + 10 + 4 +2) * (854 + 41 + 4 + 8) *60 = 26M
		24 * ( 44 + 0 ) / 4	=264M
    */
	
	//稳定     44 4 5 2   ==>  264Mhz /10 = 26.4  37.88ns
    clock_video_pll_config_t video_config;

	video_config.loopDivider = 44;
	video_config.postDivider = 4; 
	video_config.numerator   = 0;
	video_config.denominator = 0;

    CLOCK_InitVideoPll(&video_config);//配置Video PLL(PLL5)
    CLOCK_SetMux(kCLOCK_LcdifPreMux, 2);
    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 4); //五分频
    CLOCK_SetDiv(kCLOCK_LcdifDiv, 1);		 //二分频
}

//使能LCD中断
void BOARD_EnableLcdInterrupt(void)
{
    EnableIRQ(LCDIF_IRQn);
}

//LCD控制器初始化
void ELCDIF_Init(void)
{
	elcdif_rgb_mode_config_t elcdif_config; //eLCDIF配置结构体 
	
	PRINTF("LCD Control Init ... ");
	
	LCD_PWM_Init();

	//通过SPI 配置LCD RGB模式 565模式工作
	LCD_DPI_Init();
	
	//初始LCD化时钟
	BOARD_InitLcdifPixelClock();
	
	//设置面板参数
	elcdif_config.panelWidth = APP_IMG_WIDTH;     //面板宽度,单位:像素
	elcdif_config.panelHeight = APP_IMG_HEIGHT;   //面板高度,单位:像素
	elcdif_config.hsw = APP_HSW; //水平同步宽度
	elcdif_config.hfp = APP_HFP; //水平前廊
	elcdif_config.hbp = APP_HBP; //水平后廊
	elcdif_config.vsw = APP_VSW; //垂直同步宽度
	elcdif_config.vfp = APP_VFP; //垂直前廊
	elcdif_config.vbp = APP_VBP; //垂直后廊
	elcdif_config.polarityFlags = APP_POL_FLAGS;						  //极性控制
	elcdif_config.bufferAddr = (uint32_t)s_vram_buffer; //LCD缓冲区首地址
	elcdif_config.pixelFormat = kELCDIF_PixelFormatRGB565;		//输入像素格式
	elcdif_config.dataBus = kELCDIF_DataBus16Bit;				//输出像素格式
	
	//传入面板参数
	lcdelcdif.pwidth=elcdif_config.panelWidth;		//面板宽度,单位:像素
	lcdelcdif.pheight=elcdif_config.panelHeight;	//面板高度,单位:像素
	lcdelcdif.hsw=elcdif_config.hsw;	//水平同步宽度
	lcdelcdif.hfp=elcdif_config.hfp;	//水平前廊
	lcdelcdif.hbp=elcdif_config.hbp;	//水平后廊
	lcdelcdif.vsw=elcdif_config.vsw;	//垂直同步宽度
	lcdelcdif.vfp=elcdif_config.vfp;	//垂直前廊
	lcdelcdif.vbp=elcdif_config.vbp;	//垂直后廊
    lcdelcdif.width=elcdif_config.panelWidth;	//LCD宽度
    lcdelcdif.height=elcdif_config.panelHeight; //LCD高度
    
	ELCDIF_RgbModeInit(LCDIF, &elcdif_config);	//初始化eLCDIF为RGB模式
	ELCDIF_RgbModeStart(LCDIF);	//开启eLCDIF RGB模式
	
	Back_Light(1);	//打开背光
	
	ELCDIF_Clear(RGB888ToRGB565(GUI_BLACK));	//清屏
	
	RT1052_NVIC_SetPriority(LCDIF_IRQn,4,0);	//抢占优先级位5，0位子优先级
	BOARD_EnableLcdInterrupt();
	ELCDIF_EnableInterrupts(LCDIF, kELCDIF_CurFrameDoneInterruptEnable);
	NVIC_EnableIRQ(LCDIF_IRQn);
	ELCDIF_RgbModeStart(LCDIF);
	
	PRINTF("OK \r\n");
}

//LCD清屏
//color:颜色值
void ELCDIF_Clear(uint32_t color)
{
	uint32_t tpval;
	uint32_t i=0; 
#if (LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RAW8)||(LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB565)
	uint16_t *p=(uint16_t*)s_vram_buffer;	//指向帧缓存首地址
#else
	u32 *p = (uint32_t*)elcdif_lcd_framebuf;	//指向帧缓存首地址
#endif 
	tpval =(uint32_t)lcdelcdif.width * lcdelcdif.height;
	for(i=0; i < tpval; i++)
	{
		p[i] = color;
	}
}


