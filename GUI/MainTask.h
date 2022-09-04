#ifndef _MAINTASK_H_
#define _MAINTASK_H_

#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"
#include "GRAPH.h"
#include "MENU.h"
#include "MULTIPAGE.h"
#include "ICONVIEW.h"
#include "TREEVIEW.h"
#include "CALENDAR.h"

#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"

#include "EmWinHZFont.h"
#include "HZfontupdata.h"
#include "emwin_support.h"
#include "cpu_utils.h"

//自定义消息
#define  MSG_NRF24L01        (WM_USER + 0)
#define  MSG_SYSINFO         (WM_USER + 1)
#define  MSG_UPDATERTC       (WM_USER + 2)
#define  MSG_Computer_SD     (WM_USER + 3)
#define  MSG_BQ25895	     (WM_USER + 4)

#define MSG_SDPlugIn      (GUI_ID_USER + 0x10)
#define MSG_SDPlugOut     (GUI_ID_USER + 0x11)

typedef struct{
	GUI_BITMAP   * pBitmap;
	const char   * pTextEn;
	const char   * pTextCn;
} BITMAP_ITEM;

typedef struct{
	uint8_t  idenx;
	uint8_t  is_active;  
	WM_HWIN  src_handle;
} WM_task_info_t;

typedef struct
{
    float cell_vol[6];
	float batter_vol;
	float TS1,TS2;
	float CC;

	float  BAT_All_mAh;
	float  Bat_Used_mAh;
	float  Bat_mAh_Per;
	float  Residual_Life;
	float  Total_DSG_CHG_mAh;

	unsigned char BAT_STATE; //0x00 未配对 0x01配对中 0x02配对完成
	unsigned char ErrorStatus;
} BAT_Par_t;
extern BAT_Par_t BAT_Par;

extern BITMAP_ITEM _aBitmapItem_min[];


extern GUI_CONST_STORAGE GUI_BITMAP bmDestWin;
extern GUI_CONST_STORAGE GUI_BITMAP bmclock;

extern GUI_CONST_STORAGE GUI_BITMAP bmCalculator;
extern GUI_CONST_STORAGE GUI_BITMAP bmCamera;
extern GUI_CONST_STORAGE GUI_BITMAP bmClock;
extern GUI_CONST_STORAGE GUI_BITMAP bmFile;
extern GUI_CONST_STORAGE GUI_BITMAP bmiBook;
extern GUI_CONST_STORAGE GUI_BITMAP bmMessages;
extern GUI_CONST_STORAGE GUI_BITMAP bmMusic;
extern GUI_CONST_STORAGE GUI_BITMAP bmNotes;
extern GUI_CONST_STORAGE GUI_BITMAP bmPhoto;
extern GUI_CONST_STORAGE GUI_BITMAP bmSet;
extern GUI_CONST_STORAGE GUI_BITMAP bmVideo;
extern GUI_CONST_STORAGE GUI_BITMAP bmWeather;

extern WM_HWIN  hIcon_Desk; //桌面图标小工具句柄
extern WM_HWIN  hIcon_Task; //任务图标小工具句柄

extern WM_HWIN hWinInfo;
extern WM_HWIN hWinStatus;
extern WM_HWIN hWinTime;
extern WM_HWIN hWinAlarmSet;

extern WM_HWIN  hWinBattery;
extern WM_HWIN  hWinWireless;
extern WM_HWIN  hWinJoy;

/*
*********************************************************************************************************
*                                       图片和图标
*********************************************************************************************************
*/








/*
*********************************************************************************************************
*                                       函数声明
*********************************************************************************************************
*/
void GUI_MainTask(void);
void CreateS2ysInfoDlg(void);
void CreateSysStatusDlg(void);
void App_WIFIDLG(WM_HWIN hWin, uint32_t idenx);
void App_Computer(WM_HWIN hWin, uint32_t idenx);
void App_File(WM_HWIN hWin, uint32_t idenx);
void App_SmartBattery(WM_HWIN hWin, uint32_t idenx);
void APP_PMUDLG(WM_HWIN hWin, uint32_t idenx);
void App_JOYDLG(WM_HWIN hWin, uint32_t idenx);

void App_Wireless(WM_HWIN hWin, uint32_t idenx);
void App_Wireless_SET(WM_HWIN hWin);

void App_Calendar(WM_HWIN hWin, uint32_t idenx);
void App_CalendarChild(WM_HWIN hWin);

void App_Reserved(WM_HWIN hWin, uint32_t idenx);
void CreateSysInfoDlg(void);

WM_HWIN CreateWindowSKB(void); //键盘窗口创建 EmWIN 函数
unsigned char WindowSKB_Interface(void);  //EMWIN FullKey_Pinyin 维护接口

#endif


