#include "fsl_debug_console.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"

#include "ff.h"
#include "diskio.h"

#include "include.h"
#include "touch.h"
#include "malloc.h"
#include "MainTask.h"

#include "HZfontupdata.h"
#include "emwin_support.h"
#include "EmWinHZFont.h"

// #include "system_MIMXRT1052.h"
#include "clock_config.h"
#include "fsl_clock.h"

#define RECOMMENDED_MEMORY (1024L * 2000)

extern uint32_t SystemCoreClock;

uint8_t *p_BkpicFile = NULL; //背景图片数据指针
GUI_MEMDEV_Handle   hMempic;
WM_HWIN  hIcon_Desk; //桌面图标小工具句柄
WM_HWIN  hIcon_Task; //任务图标小工具句柄

BITMAP_ITEM _aBitmapItem[] =
{
	{NULL,     "Camera",     "我的电脑" },
	{NULL,     "Clock",      "时钟"     },
	{NULL,     "File",       "文件管理" },
	{NULL,     "iBook",      "智能电池" },
	{NULL,     "Messages",   "2.4G无线" },
	{NULL,     "Music",      "音乐"     },

	{NULL,      "Photo",      "照片"   },
	{NULL,      "Set",        "设置"   },
	{NULL,      "Video",      "视频"   },
	{NULL,      "Calc",		  "计算器" },
};

BITMAP_ITEM _aBitmapItem_min[] =
{
	{NULL,     "Camera",     "我的电脑" },
	{NULL,     "Clock",      "时钟"     },
	{NULL,     "File",       "文件管理" },
	{NULL,     "iBook",      "智能电池" },
	{NULL,     "Messages",   "2.4G无线" },
	{NULL,     "Music",      "音乐"     },

	{NULL,      "Photo",      "照片"   },
	{NULL,      "Set",        "设置"   },
	{NULL,      "Video",      "视频"   },
	{NULL,      "Calc",		  "计算器" },
};

static void (* _apModules0[])(WM_HWIN hWin, uint32_t idenx) =
{
	App_Computer,
	App_Calendar,
	App_File,
	App_SmartBattery,
	App_Wireless,
	App_WIFIDLG,

	App_JOYDLG,
	APP_PMUDLG,
	App_WIFIDLG,
	App_WIFIDLG,
	App_WIFIDLG,
};


WM_HWIN Query_taskIcon_handle(uint8_t idenx)
{
	return NULL;
}
/*
*********************************************************************************************************
*	函 数 名: Load_Picture
*	功能说明: 从内存卡加载桌面背景
*	形    参:
*	返 回 值: 0成功  1失败
*********************************************************************************************************
*/
int Load_Picture(void)
{
	FIL filex;
	FILINFO filestat;
	FRESULT result;
	unsigned int bw;

	//加载桌面壁纸
	PRINTF("\r\n... Load BK Picture ...\r\n");
	//加载图片信息
	result = f_stat("2:HeziOS/BK1.bmp", &filestat);
	if(result != FR_OK) {
		return -1; //读取文件状态错误
	}
	PRINTF("Read file state=%d size=%d OK ! \r\n",result,filestat.fsize);

	p_BkpicFile = (unsigned char*)mymalloc(SRAMEXC, filestat.fsize);
	if(p_BkpicFile == NULL) {
		myfree(SRAMEXC, p_BkpicFile);
		return -1; //申请动态内存失败
	}
	PRINTF("Apply Memory address=0x%x  OK ! \r\n",p_BkpicFile);

	result = f_open(&filex, "2:HeziOS/BK1.bmp", FA_READ | FA_OPEN_ALWAYS);
	if(result != FR_OK) {
		f_close(&filex);
		return -1;
	}
	PRINTF("Open file result=%d OK ! \r\n",result);

	do{
		result = f_read (&filex, (void*)p_BkpicFile, filestat.fsize, &bw);
	}while(bw < filestat.fsize);//等待读取完毕
	PRINTF("Read file to Point OK ! \r\n");

	f_close(&filex);//关闭文件
	PRINTF("Close file OK ! \r\n");

	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: Load_Icon
*	功能说明: 从内存卡加载桌面图标
*	形    参:
*	返 回 值: 0成功  1失败
*********************************************************************************************************
*/
#if defined(SD_LOAD_MODE)
int Load_Icon(void)
{
	unsigned int  i;
	unsigned int  bw;
	unsigned char tmp;
	unsigned char *Point;			//文件内存指针
	char fil_addr[] = "2:Icon/";	//桌面图标所在目录
	char fil_type[] = ".bmp";		//后缀类型
	char name_buf[30];				//完整路径缓冲区

	FIL     filex;
	FRESULT result;
	FILINFO filestat;
	GUI_BITMAP *Bitmap;

	PRINTF("\r\n... Load Desk Icon ...\r\n");
	//加载桌面图标
	//第一页图片
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++)
	{
		memset(name_buf,NULL,sizeof(name_buf));						//清空路径换乘区
		strcat((char*)name_buf,fil_addr);   							//复制路径
		strcat((char*)name_buf,_aBitmapItem[i].pTextEn); //复制文件名
		strcat((char*)name_buf,fil_type);   							//复制文件类型

		result=f_stat((const TCHAR*)name_buf,&filestat);
		PRINTF("Read file state=%d size=%d OK ! \r\n",result,filestat.fsize);

		Point=(unsigned char*)mymalloc(SRAMEXC,filestat.fsize);
		if(Point==NULL){myfree(SRAMEXC,Point);PRINTF("Icon malloc fail \r\n");continue;}//申请动态内存失败
		PRINTF("Apply %s Memory address=0x%x  OK ! \r\n",name_buf,Point);

		result=f_open(&filex,(const TCHAR*)name_buf,FA_READ);
		if(result!=FR_OK){f_close(&filex);continue;}
		PRINTF("Open file result=%d OK ! \r\n",result);

		bw=0;
		do{
		result = f_read (&filex,(void*)Point,filestat.fsize,&bw);
		}while(bw<filestat.fsize);//等待读取完毕
		PRINTF("Read file to Point OK ! \r\n");

		f_close(&filex);//关闭文件
		PRINTF("Close file OK ! \r\n");

		//bmp real data offset = Point+10 倒序处理
		for(bw = *(uint32_t *)(Point+10); bw < filestat.fsize ; bw += 4)
		{
			*(Point+bw+3)=*(Point+bw+3)^0xff;//高位字节 取异或
			tmp=*(Point+bw);				 //调换低字节和第三个字节数据
			*(Point+bw)=*(Point+bw+2);
			*(Point+bw+2)=tmp;
		}

		_aBitmapItem[i].pBitmap = (GUI_BITMAP *)mymalloc(SRAMEXC,filestat.fsize);
		if(_aBitmapItem[i].pBitmap==NULL){myfree(SRAMEXC,_aBitmapItem[i].pBitmap);return -1;}//申请动态内存失败

		Bitmap = _aBitmapItem[i].pBitmap;
		Bitmap->XSize = GUI_BMP_GetXSize(Point);
		Bitmap->YSize = GUI_BMP_GetYSize(Point);
		Bitmap->BytesPerLine =288;
		Bitmap->BitsPerPixel =32;
		Bitmap->pData =(unsigned char*)(Point+*(uint32_t *)(Point+10));
		Bitmap->pPal = NULL;
		Bitmap->pMethods = GUI_DRAW_BMP888;    //GUI_DRAW_BMP8888
	}
	return 0;
}
#else
int Load_Icon(void)
{
	unsigned int  i;
	unsigned int  bw;
	unsigned char *Point;			//文件内存指针
	char fil_addr[] = "2:Icon/";	//桌面图标所在目录
	char fil_type[] = ".dta";		//后缀类型
	char name_buf[48];				//完整路径缓冲区

	FIL     filex;
	FRESULT result;
	FILINFO filestat;
	GUI_BITMAP *Bitmap;

	PRINTF("\r\n... Load Desk Icon ...\r\n");
	//加载桌面图标
	//第一页图片
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++) {
		memset(name_buf, NULL, sizeof(name_buf));			//清空路径换乘区
		strcat((char*)name_buf, fil_addr);					//复制路径
		strcat((char*)name_buf, _aBitmapItem[i].pTextEn);	//复制文件名
		strcat((char*)name_buf, fil_type);					//复制文件类型

		result = f_stat((const TCHAR*)name_buf, &filestat);
		PRINTF("f_stat[%d] name:%s size:%d OK \r\n", result, name_buf, filestat.fsize);

		Point = (unsigned char*)mymalloc(SRAMEXC, filestat.fsize);
		PRINTF("mymalloc SRAMEXC 0x%x \r\n", Point);
		if(Point == NULL) {
			myfree(SRAMEXC, Point);
			continue; //申请动态内存失败
		}

		result = f_open(&filex, (const TCHAR*)name_buf, FA_READ);
		PRINTF("f_open[%d] \r\n", result);
		if(result != FR_OK) {
			f_close(&filex);
			continue;
		}

		bw = 0;
		do {
			result = f_read (&filex,(void*)Point, filestat.fsize, &bw);
		} while(bw < filestat.fsize); //等待读取完毕
		PRINTF("f_read[%d] \r\n", result);

		result = f_close(&filex);//关闭文件
		PRINTF("f_close[%d] \r\n", result);

		_aBitmapItem[i].pBitmap = (GUI_BITMAP *)mymalloc(SRAMEXC, filestat.fsize);
		PRINTF("mymalloc SRAMEXC 0x%x \r\n", _aBitmapItem[i].pBitmap);
		if(_aBitmapItem[i].pBitmap == NULL) {
			myfree(SRAMEXC, _aBitmapItem[i].pBitmap);
			return -1; //申请动态内存失败
		}

		Bitmap = _aBitmapItem[i].pBitmap;
		Bitmap->XSize = 72;
		Bitmap->YSize = 72;
		Bitmap->BytesPerLine =288;
		Bitmap->BitsPerPixel =32;
		Bitmap->pData =(unsigned char*)(Point+0x10);
		Bitmap->pPal = NULL;
		Bitmap->pMethods = GUI_DRAW_BMP8888;    //GUI_DRAW_BMP8888
	}
	return 0;
}

int Load_task_Icon(void)
{
	unsigned int  i;
	unsigned int  bw;
	unsigned char *Point;			 //文件内存指针
	char fil_addr[] = "2:taskicon/"; //桌面图标所在目录
	char fil_type[] = ".dta";		 //后缀类型
	char name_buf[30];				 //完整路径缓冲区

	FIL     filex;
	FRESULT result;
	FILINFO filestat;
	GUI_BITMAP *Bitmap;

	PRINTF("\r\n... Load Task Icon ...\r\n");
	//加载桌面图标
	//第一页图片
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem_min); i++) {
		memset(name_buf, NULL, sizeof(name_buf));				//清空路径换乘区
		strcat((char*)name_buf, fil_addr);						//复制路径
		strcat((char*)name_buf, _aBitmapItem_min[i].pTextEn);	//复制文件名
		strcat((char*)name_buf, fil_type);						//复制文件类型

		result = f_stat((const TCHAR*)name_buf, &filestat);
		PRINTF("f_stat[%d] name:%s size:%d \r\n", result, name_buf, filestat.fsize);

		Point = (unsigned char*)mymalloc(SRAMEXC, filestat.fsize);
		PRINTF("mymalloc SRAMEXC 0x%x \r\n", Point);
		if(Point == NULL) {
			myfree(SRAMEXC,Point);
			continue; //申请动态内存失败
		}

		result = f_open(&filex, (const TCHAR*)name_buf, FA_READ);
		PRINTF("f_open[%d] \r\n", result);
		if(result != FR_OK) {
			f_close(&filex);
			continue;
		}

		bw = 0;
		do {
			result = f_read (&filex, (void*)Point, filestat.fsize, &bw);
		}while(bw < filestat.fsize); //等待读取完毕
		PRINTF("f_read[%d] \r\n", result);

		result = f_close(&filex);//关闭文件
		PRINTF("f_close[%d] \r\n", result);

		_aBitmapItem_min[i].pBitmap = (GUI_BITMAP *)mymalloc(SRAMEXC, filestat.fsize);
		PRINTF("mymalloc SRAMEXC 0x%x \r\n", _aBitmapItem_min[i].pBitmap);
		if(_aBitmapItem_min[i].pBitmap == NULL) {
			myfree(SRAMEXC,_aBitmapItem_min[i].pBitmap);
			return -1; //申请动态内存失败
		}

		Bitmap = _aBitmapItem_min[i].pBitmap;
		Bitmap->XSize = 42;
		Bitmap->YSize = 42;
		Bitmap->BytesPerLine =168;
		Bitmap->BitsPerPixel =32;
		Bitmap->pData =(unsigned char*)(Point+0x10);
		Bitmap->pPal = NULL;
		Bitmap->pMethods = GUI_DRAW_BMP8888;    //GUI_DRAW_BMP8888
	}
	return 0;
}

#endif

/*
*********************************************************************************************************
*	函 数 名: _CreateICONVIEW
*	功能说明: 创建桌面图标小工具
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/
WM_HWIN _CreateICONVIEW(void)
{
	WM_HWIN hIcon;
	int i;

	/*在指定位置创建指定尺寸的ICONVIEW 小工具*/
	hIcon = ICONVIEW_CreateEx(  40,			/* 小工具的最左像素（在父坐标中）*/
								10,			/* 小工具的最上像素（在父坐标中）*/
								LCD_GetXSize() - 40,				/* 小工具的水平尺寸（单位：像素）*/
								LCD_GetYSize() - 60 -10,			/* 小工具的垂直尺寸（单位：像素）*/
								WM_HBKWIN,	/* 父窗口的句柄。如果为0 ，则新小工具将成为桌面（顶级窗口）的子窗口 */
								WM_CF_SHOW | WM_CF_HASTRANS,		/* 窗口创建标记。为使小工具立即可见，通常使用 WM_CF_SHOW */
								ICONVIEW_CF_AUTOSCROLLBAR_V,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	 /* 默认是0，如果不够现实可设置增减垂直滚动条 */
								GUI_ID_ICONVIEW0, /* 小工具的窗口ID */
								100,       /* 图标的水平尺寸 */
								120);      /* 图标的垂直尺寸,图标和文件都包含在里面，不要大于ICONVIEW的高度，导致Text显示不完整*/

	/* 向ICONVIEW 小工具添加新图标 */
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++) {
		ICONVIEW_AddBitmapItem(hIcon, _aBitmapItem[i].pBitmap, _aBitmapItem[i].pTextCn);
	}

	ICONVIEW_SetFont(hIcon, &GUI_FontHZ24);

	/* 设置小工具的背景色 32 位颜色值的前8 位可用于alpha混合处理效果*/
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_WHITE | 0x80000000);

	/* 设置X方向的边界值为0，默认不是0, Y方向默认是0，这里我们也进行一下设置，方便以后修改 */
	ICONVIEW_SetFrame(hIcon, GUI_COORD_X, 0);
	ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 0);

	/* 设置图标在x 或y 方向上的间距。*/
	ICONVIEW_SetSpace(hIcon, GUI_COORD_X, 34);
	ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, 25);

	/* 设置对齐方式 在5.22版本中最新加入的 */
	ICONVIEW_SetIconAlign(hIcon, ICONVIEW_IA_HCENTER|ICONVIEW_IA_VCENTER);

	//ICONVIEW_SetTextColor(hIcon, ICONVIEW_CI_UNSEL, 0xF020A0);
	return hIcon;
}

WM_HWIN _CreateStatusICONVIEW(void)
{
	WM_HWIN hIcon;
	/*在指定位置创建指定尺寸的ICONVIEW 小工具*/
	hIcon = ICONVIEW_CreateEx(  85,			/* 小工具的最左像素（在父坐标中）*/
								0,			/* 小工具的最上像素（在父坐标中）*/
								600,        /* 小工具的水平尺寸（单位：像素）*/
								55, 	    /* 小工具的垂直尺寸（单位：像素）*/
								hWinStatus,	/* 父窗口的句柄。如果为0 ，则新小工具将成为桌面（顶级窗口）的子窗口 */
								WM_CF_SHOW | WM_CF_HASTRANS, /* 窗口创建标记。为使小工具立即可见，通常使用 WM_CF_SHOW */
								ICONVIEW_CF_AUTOSCROLLBAR_V,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	 /* 默认是0，如果不够现实可设置增减垂直滚动条 */
								GUI_ID_ICONVIEW1, /* 小工具的窗口ID */
								42,       /* 图标的水平尺寸 */
								42);      /* 图标的垂直尺寸,图标和文件都包含在里面，不要大于ICONVIEW的高度，导致Text显示不完整*/

	/* 设置小工具的背景色 32 位颜色值的前8 位可用于alpha混合处理效果*/
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_MAKE_COLOR(0x002E3B42));

	ICONVIEW_SetFrame(hIcon, GUI_COORD_X, 5);
	ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 5);

	ICONVIEW_SetSpace(hIcon, GUI_COORD_X, 5);
	ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, 5);

	/* 设置对齐方式 在5.22版本中最新加入的 */
	ICONVIEW_SetIconAlign(hIcon, ICONVIEW_IA_HCENTER|ICONVIEW_IA_VCENTER);

	return hIcon;
}

/*
*********************************************************************************************************
*	函 数 名: _cbBkWindow
*	功能说明: 桌面窗口回调函数
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbBkWindow(WM_MESSAGE * pMsg)
{
	int Id, NCode;
	int x, y;
	uint8_t idenx;
	switch(pMsg->MsgId) {
		case WM_PRE_PAINT: {
			//GUI_MULTIBUF_Begin();
		} break;

		case WM_POST_PAINT: {
			//GUI_MULTIBUF_End();
		} break;

		case WM_PAINT: {
			if(p_BkpicFile == NULL) {
				GUI_SetBkColor(GUI_BLACK);
				GUI_Clear(); //清屏函数
			}else{
				// GUI_BMP_Draw((void*)p_BkpicFile, 0, 0);
				GUI_MEMDEV_WriteOpaqueAt(hMempic, 0, 0);
			}

			/* 先将3个6x6圆圈显示出来 */
			GUI_SetColor(GUI_WHITE);
			x = LCD_GetXSize()/2 - 10 / 2 - 30;
			y = LCD_GetYSize()/1 - 10 / 2 - 80;

			GUI_SetColor(GUI_WHITE);
			GUI_FillCircle( x,      y , 5 );
			GUI_FillCircle( x + 30, y , 5 );
			GUI_FillCircle( x + 60, y , 5 );
			GUI_FillCircle( x, y, 8);//选择第一个点  目前不支持滑屏操作
		} break;

		case WM_NOTIFY_PARENT: {
			Id   = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id) {
				case GUI_ID_ICONVIEW0: {
					if(NCode == WM_NOTIFICATION_RELEASED) {
						idenx = ICONVIEW_GetSel(pMsg->hWinSrc); //获取选中编号图标
						ICONVIEW_SetSel(hIcon_Desk, -1); //将选中状态清除
						if(idenx < GUI_COUNTOF(_aBitmapItem)) {
							if(idenx < 8) {
								_apModules0[idenx](WM_HBKWIN, idenx);
							}
						}
					}
				} break;
			}
		} break;

		case WM_CREATE: {
			GUI_AA_SetFactor(3);
		} break;

		default: {
			WM_DefaultProc(pMsg);
		} break;
	}
}

void GUI_MainTask(void)
{
	unsigned int  xpost, counts;
	unsigned char buf[128];

	GUI_Init();
	WM_MULTIBUF_Enable(1);	//打开多缓冲
	GUI_CURSOR_Show();		//显示光标
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_RED);
	GUI_Clear();			//清屏函数

	if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
		PRINTF("Not enough memory available Free:%d !\r\n",GUI_ALLOC_GetNumUsedBytes());
		return;
	}
	PRINTF("memory used:%d free:%d \r\n", GUI_ALLOC_GetNumUsedBytes(), GUI_ALLOC_GetNumFreeBytes());
	PRINTF("GUI demo start.\r\n");

	/*******************************************************************************/
	/* 设置皮肤 *************************************************************/
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);

	//在插入内存卡期间打印相关信息；
	//GUI_DispNextLine(); 换行
	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_VCENTER);		//设置文本对齐方式 左对齐 垂直居中
	GUI_SetTextStyle(GUI_TS_NORMAL);					//设置文本绘制模式 正常
	GUI_SetFont(GUI_FONT_24B_ASCII);					//设置字体
	GUI_SetColor(GUI_LIGHTGREEN);						//设置字体颜色
	GUI_GotoXY(0, 12);									//设置初始文本坐标

	//设置了X坐标后  如果选择了水平居中  第一个字符串一半会显示在X坐标左边
	GUI_DispString("Feiling Embedded OK1050-C \r\n");
	GUI_DispString("i.MX RT1050 @Arm Cortex-M7 \r\n");
	GUI_DispString("Copyright (C) 2019-2029 Author:Luo Jian \r\n");
	GUI_DispString("SRAM:512KB SDRAM:32MB SPI FLASH:16MB \r\n");
	GUI_DispString("Core:V1.2  Driver:V1.0  Software:V1.0 \r\n");
	GUI_DispString("System:FreeRTOS Kernel V10.0.1 \r\n");
	sprintf((char*)buf, "kCLOCK_CpuClk: %d MHz   SystemCoreClock: %d MHz \r\n", CLOCK_GetFreq(kCLOCK_CpuClk)/1000000UL, SystemCoreClock/1000000UL);
	GUI_DispString((char*)buf);

	//打印EMWIN版本信息
	GUI_DispString("UI Interface Version(emWin):");
	GUI_DispString(GUI_GetVersionString());
	GUI_DispNextLine();
	//打印系统硬件初始化信息
	//触摸 FT5316
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("FT5316 Touch Init...  ");
	if(Touch.Init_statue == true) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}
	//电源 BQ25895
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("BQ25895 Power Init...  ");
	if(BQ25895.Init_statue == true) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}
	//数字音频 WM8960
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("WM8960 Audio Init...  ");
	if(Audio.Init_statue == true) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}
	//无线 NRF24L01
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("NRF24L01 RF Init...  ");
	if(NRF24L01.Init_statue == true) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//等待SD卡准备完毕
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Wait SD Card Inserte");
	xpost = GUI_GetDispPosX();
	while(!TF.Card_ready) {
		if(++counts > 10) {
			counts = 0;
			GUI_GotoX(xpost);
			GUI_DispCEOL();//清除 '*'
		}
		GUI_DispString(" ~");
		GUI_Delay(100);
	}
	GUI_SetColor(GUI_LIGHTBLUE);
	GUI_DispString("  OK \r\n");
	//打印SD卡信息
	sprintf((char*)buf, "SDMMC: Clock=%d Mhz Size=%d MB \r\n", g_sd.busClock_Hz/1000000U, g_sd.blockCount/2/1024);
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString((char*)buf);
	//加载字库
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Load Font Into Memory...  ");
	if(emwin_update_font("2:") == kStatus_Success) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//加载桌面背景图片
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Load BK Picture Into Memory...  ");
	if(Load_Picture()==kStatus_Success) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//加载桌面图标
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Load Icon Into Memory...  ");
	if(Load_Icon() == kStatus_Success) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//加载任务图标
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Load Tsak Icon Into Memory...  ");
	if(Load_task_Icon() == kStatus_Success) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//触摸任意位置进入系统
	GUI_SetColor(GUI_YELLOW);
	GUI_SetFont(&GUI_FontHZ24);
	GUI_DispString("\r\n...中文字库加载完毕！请触摸任意位置开始...");

	hMempic = GUI_MEMDEV_CreateFixed(	0,
										0,
										LCD_GetXSize(),
										LCD_GetYSize(),
										GUI_MEMDEV_NOTRANS,
										GUI_MEMDEV_APILIST_16,
										GUICC_M565);
	//加载背景图片
	if(p_BkpicFile == NULL) {
		GUI_SetBkColor(GUI_BLACK);
		GUI_Clear(); //清屏函数
	} else {
		GUI_MEMDEV_Select(hMempic);
		GUI_BMP_Draw((void*)p_BkpicFile, 0, 0);
		GUI_MEMDEV_Select(0);
	}

//	GUI_MULTIBUF_Begin();
//	  GUI_BMP_Draw((void*)p_BkpicFile,0,0);
//	GUI_MULTIBUF_End();

	WM_SetCallback(WM_HBKWIN, _cbBkWindow);

	/* 第1个界面图标 */
	hIcon_Desk = _CreateICONVIEW(); //100 * 3
	ICONVIEW_SetSel(hIcon_Desk, -1);

	CreateSysInfoDlg();

	CreateSysStatusDlg();

	hIcon_Task = _CreateStatusICONVIEW();

	CreateWindowSKB();
}


