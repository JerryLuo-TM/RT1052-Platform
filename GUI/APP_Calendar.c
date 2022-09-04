#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_snvs_hp.h"
#include "fsl_snvs_lp.h"

#include "include.h"
#include "MainTask.h"
#include "DIALOG.h"

#include "HZfontupdata.h"
#include "emwin_support.h"
#include "EmWinHZFont.h"

#include "svns_rtc.h"

/*
*********************************************************************************************************
*                                       宏定义
*********************************************************************************************************
*/
#define countof(Obj) (sizeof(Obj) / sizeof(Obj[0]))
#define DEG2RAD      (3.1415926f / 180)
#define MAG          3

#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)

#define ID_TEXT_0   (GUI_ID_USER + 0x01)
#define ID_TEXT_1   (GUI_ID_USER + 0x02)
#define ID_TEXT_2   (GUI_ID_USER + 0x03)
#define ID_TEXT_3   (GUI_ID_USER + 0x04)

#define ID_BUTTON_0   (GUI_ID_USER + 0x10)
#define ID_BUTTON_1   (GUI_ID_USER + 0x11)

WM_HWIN  hWinTime = WM_HWIN_NULL;

/*
*********************************************************************************************************
*                                       数值
*********************************************************************************************************
*/
typedef struct {
	GUI_POINT        aPoints[7];   /* 多边形坐标 */
	float            Angle;        /* 旋转角度 */

	GUI_POINT        aPoints1[7];
	float            Angle1;

	GUI_POINT        aPoints2[7];
	float            Angle2;
} PARAM;

/* 三组指针坐标 */
static const GUI_POINT _aNeedle[] = {
	{ MAG * ( -1), MAG * (  0 ) },
	{ MAG * (-1), MAG * (-50 ) },
	{ MAG * (0), MAG * (-65 ) },
	{ MAG * ( 1), MAG * (-50 ) },
	{ MAG * ( 1), MAG * (0 ) },
};

static const GUI_POINT _aNeedle1[] = {
	{ MAG * ( -2), MAG * (  0 ) },
	{ MAG * (-2), MAG * (-50 ) },
	{ MAG * (0), MAG * (-65 ) },
	{ MAG * ( 2), MAG * (-50 ) },
	{ MAG * ( 2), MAG * (0 ) },
};

static const GUI_POINT _aNeedle2[] = {
	{ MAG * ( -2), MAG * (  0 ) },
	{ MAG * (-2), MAG * (-40 ) },
	{ MAG * (0), MAG * (-55 ) },
	{ MAG * ( 2), MAG * (-40 ) },
	{ MAG * ( 2), MAG * (0 ) },
};

/* 用于星期的显示 */
static const char ucWeekDay[7][2] = {
	"一",
	"二",
	"三",
	"四",
	"五",
	"六",
	"日"
};


/*
*********************************************************************************************************
*	               对话框控件列表
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateRTC[] = {
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 854, 430, 0, 0x64, 0 },

	{ TEXT_CreateIndirect,      "日期：",    GUI_ID_TEXT0,    180,   40,  300, 36, 0,0},
	{ TEXT_CreateIndirect,      "时间：",    GUI_ID_TEXT1,    490,   40,  300, 36, 0,0},
	{ TEXT_CreateIndirect,      "星期：",    GUI_ID_TEXT2,    180,   80, 300, 36, 0,0},
	{ TEXT_CreateIndirect,      "闹钟：",    GUI_ID_TEXT3,    490,   80, 300, 36, 0,0},

	{ BUTTON_CreateIndirect, "设置",    ID_BUTTON_0,  200,  320, 80, 50, 0, 0, 0 },
	{ BUTTON_CreateIndirect, "退出",    ID_BUTTON_1,  570,  320, 80, 50, 0, 0, 0 },
};

/*
*********************************************************************************************************
*	函 数 名: Clock_DrawDisp
*	功能说明: 更新时钟表盘
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void Clock_DrawDisp(void)
{
	PARAM	Param;/* PARAM类型变量 */

	int      t0;   /* 用于三个指针的计数 */
	int      t1;
	int      t2;

	/* 第一个指针计数，用于旋转秒针 */
	t0 = 360 - g_rtcDate.second * 6;

	/* 第二个指针计数，用于旋转分针 */
	t1 = 360 - g_rtcDate.minute * 6;

	/* 第三个指针计数，用于旋转时针 时针旋转与分针有关 */
	t2 = 360 - g_rtcDate.hour * 30 - (30*(g_rtcDate.minute *6 )/360);

	/* 旋转第一个指针 */
	Param.Angle= t0;
	Param.Angle *= 3.1415926f / 180.0f;
	GUI_RotatePolygon(Param.aPoints, _aNeedle, countof(_aNeedle), Param.Angle);

	/* 旋转第二个指针 */
	Param.Angle1= t1;
	Param.Angle1 *= 3.1415926f / 180.0f;
	GUI_RotatePolygon(Param.aPoints1, _aNeedle1, countof(_aNeedle1), Param.Angle1);

	/* 旋转第三个指针 */
	Param.Angle2= t2;
	Param.Angle2 *= 3.1415926f / 180.0f;
	GUI_RotatePolygon(Param.aPoints2, _aNeedle2, countof(_aNeedle2), Param.Angle2);

	GUI_DrawBitmap(&bmclock, 340, 120);

	/* 使能抗锯齿，多任务的情况下，此函数一定要实时调用，比如截图任务切换回来就出错了 */
	GUI_AA_EnableHiRes();

	GUI_SetColor(GUI_BLUE);
	GUI_AA_FillPolygon(Param.aPoints, countof(_aNeedle), MAG * 430, MAG * 210);
	GUI_SetColor(GUI_BLACK);
	GUI_AA_FillPolygon(Param.aPoints1, countof(_aNeedle1), MAG * 430, MAG * 210);
	GUI_SetColor(GUI_RED);
	GUI_AA_FillPolygon(Param.aPoints2, countof(_aNeedle2), MAG * 430, MAG * 210);
}


/*
*********************************************************************************************************
*	函 数 名: Clock_Update
*	功能说明: 更新时间
*	形    参: pMsg  消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void Clock_Update(WM_MESSAGE * pMsg)
{
	char buf[30];
	WM_HWIN hWin = pMsg->hWin;

	/* 更新时间 */
	sprintf(buf, "时间: %.2d:%.2d:%.2d", g_rtcDate.hour, g_rtcDate.minute, g_rtcDate.second);
	TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), buf);

	/* 更新日期 */
	sprintf(buf, "日期: %d/%d/%d", g_rtcDate.year, g_rtcDate.month, g_rtcDate.day);
	TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0), buf);

	/* 更新星期 */
	sprintf(buf, "星期: %.2s", ucWeekDay[g_rtcDate.dow-1]);
	TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2), buf);

	/* 更新闹钟 */
	sprintf(buf, "闹钟: %0.2d:%0.2d:%0.2d ", 0, 0, 0);
	TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3), buf);
}


//初始化RTC界面
void InitDialogRTC(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;

	//创建定时器
	WM_CreateTimer(WM_GetClientWindow(hWin), 0, 10, 0);

	//【 窗口初始化 标题 】
	{
		FRAMEWIN_SetTitleHeight(hWin, 32);
		FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
		FRAMEWIN_SetText(hWin, "系统时间");
		FRAMEWIN_SetFont(hWin, &GUI_FontHZ24);
		FRAMEWIN_SetMoveable(hWin, 0);  //窗口可移动 0
		FRAMEWIN_SetResizeable(hWin,0); //窗口可缩放 0
	}
	//【 按键初始化 】
	{
		BUTTON_SetFont(WM_GetDialogItem(hWin, ID_BUTTON_0), &GUI_FontHZ32);
		BUTTON_SetFont(WM_GetDialogItem(hWin, ID_BUTTON_1), &GUI_FontHZ32);

		BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON_0), "设置");
		BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON_1), "退出");
	}
	//【初始化文本】
	{
		TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0), &GUI_FontHZ32);
		TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT1), &GUI_FontHZ32);
		TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT2), &GUI_FontHZ32);
		TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT3), &GUI_FontHZ32);
	}
	Clock_Update(pMsg);

	/* 使能抗锯齿 */
	GUI_AA_EnableHiRes();

	/* 设置抗锯齿因数 */
	GUI_AA_SetFactor(MAG);
}


//时钟回调函数
static void _cbCallbackRTC(WM_MESSAGE * pMsg)
{
	int NCode, Id;
	WM_HWIN hWin = pMsg->hWin;
	GUI_RECT rRTC={310, 120, 490, 300};

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG: {
			InitDialogRTC(pMsg);
		} break;

		case WM_PRE_PAINT: {
			GUI_MULTIBUF_Begin();
		} break;

		case WM_PAINT: {
			Clock_DrawDisp();
		} break;

		case WM_POST_PAINT: {
			GUI_MULTIBUF_End();
		} break;

		/* 定时1秒更新一次时间 */
		case WM_TIMER: {
			WM_InvalidateRect(hWin, &rRTC);
			Clock_Update(pMsg);
			WM_RestartTimer(pMsg->Data.v, 1000);
		} break;

		case WM_NOTIFY_PARENT: {
			Id = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id) {
				case ID_BUTTON_0: { /* 设置时间，日期和闹钟 */ 
					if(NCode == WM_NOTIFICATION_RELEASED) {
						App_CalendarChild(hWin);
					}
				} break;

				case ID_BUTTON_1: { /* 关闭对话框 */ 
					if(NCode == WM_NOTIFICATION_RELEASED) {
						WM_MESSAGE Message;
						Message.hWinSrc = hWinTime;
						Message.MsgId   = WM_DELETE;
						Message.Data.v  = 1;
						WM_SendMessage(hWinStatus, &Message);
						GUI_AA_DisableHiRes();
						// GUI_EndDialog(hWin, 0);
						WM_DeleteWindow(hWinTime);
						hWinTime = NULL;
					}
				} break;
			}
		} break;

		default: {
			WM_DefaultProc(pMsg);
		} break;
    }
}

void App_Calendar(WM_HWIN hWin, uint32_t idenx)
{
	WM_MESSAGE Message;
	if(hWinTime == NULL) {
		hWinTime = GUI_CreateDialogBox(_aDialogCreateRTC,
							GUI_COUNTOF(_aDialogCreateRTC),
							&_cbCallbackRTC,
							hWin,
							0,
							0);
		Message.hWinSrc = hWinTime;
		Message.MsgId   = WM_CREATE;
		Message.Data.v  = idenx;
		WM_SendMessage(hWinStatus, &Message);

		PRINTF("App_Calendar idenx = %d  src= 0x%x \r\n", idenx, hWinTime);
	} else {
		WM_ShowWindow(hWinTime); 
	}
}




