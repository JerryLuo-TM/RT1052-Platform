#ifndef _ELCDIF_H
#define _ELCDIF_H

#include "fsl_common.h"

//#define APP_IMG_HEIGHT 272
//#define APP_IMG_WIDTH 480
//#define APP_HSW 41
//#define APP_HFP 4
//#define APP_HBP 8
//#define APP_VSW 10
//#define APP_VFP 4
//#define APP_VBP 2

//ILI9806 数据手册 61 411页
#define APP_IMG_HEIGHT 480
#define APP_IMG_WIDTH  854
#define APP_HSW 2   //水平同步脉冲宽度 2
#define APP_HFP 1   //水平前边缘 1
#define APP_HBP 2   //水平后边缘 2
#define APP_VSW 5   //垂直同步脉冲宽度 5
#define APP_VFP 0   //垂直前边缘 0
#define APP_VBP 15  //垂直后边缘 15
#define APP_POL_FLAGS (kELCDIF_DataEnableActiveHigh | kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DriveDataOnFallingClkEdge)

#define LCD_PIXEL_FORMAT_RAW8       0X00    
#define LCD_PIXEL_FORMAT_RGB565     0X01    
#define LCD_PIXEL_FORMAT_RGB666     0X02       
#define LCD_PIXEL_FORMAT_XRGB8888   0X03      
#define LCD_PIXEL_FORMAT_RGB888     0X04     

///////////////////////////////////////////////////////////////////////
//用户修改配置部分:

//定义颜色像素格式,一般用RGB565
#define LCD_PIXFORMAT				LCD_PIXEL_FORMAT_RGB565	

//LCD LTDC重要参数集
typedef struct  
{							 
	uint32_t pwidth;	//LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入
	uint32_t pheight;	//LCD面板的高度,固定参数,不随显示方向改变
	uint16_t hsw;		//水平同步宽度
	uint16_t vsw;		//垂直同步宽度
	uint16_t hbp;		//水平后廊
	uint16_t vbp;		//垂直后廊
	uint16_t hfp;		//水平前廊
	uint16_t vfp;		//垂直前廊 
	uint16_t width;		//LCD宽度
	uint16_t height;	//LCD高度
	uint32_t pixsize;	//每个像素所占字节数
}_elcdif_dev; 

extern _elcdif_dev lcdelcdif;	//管理LCD LTDC参数


void BOARD_InitLcdifPixelClock(void);
void BOARD_EnableLcdInterrupt(void);
void ELCDIF_Init(void);

void ELCDIF_Clear(uint32_t color);




#endif






