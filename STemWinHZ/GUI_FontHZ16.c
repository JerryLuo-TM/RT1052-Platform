#include "GUI.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//STemwin 汉字显示(FLASH外置字库)
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/3/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

extern void GUIPROP_X_DispChar(U16P c);
extern int GUIPROP_X_GetCharDistX(U16P c);

GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ16_CharInfo[2] = 
{    
	{ 8, 	8, 	1, (void*)"0"},  
	{ 16, 	16, 2, (void*)"0"},
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ16_PropHZ = {
      0x4081, 
      0xFFFF, 
      &GUI_FontHZ16_CharInfo[1],
      (void *)0, 
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ16_PropASC = {
      0x0000, 
      0x007F, 
      &GUI_FontHZ16_CharInfo[0],
      (void GUI_CONST_STORAGE *)&GUI_FontHZ16_PropHZ, 
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16 = 
{
      GUI_FONTTYPE_PROP_USER, 
      16, 
      16, 
      1,  
      1,  
      (void GUI_CONST_STORAGE *)&GUI_FontHZ16_PropASC
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16x2 = 
{
      GUI_FONTTYPE_PROP_USER, 
      16, 
      16, 
      2,  
      2,  
      (void GUI_CONST_STORAGE *)&GUI_FontHZ16_PropASC
};



