#include "MainTask.h"

/*
*********************************************************************************************************
*                                       宏定义
*********************************************************************************************************
*/
#define MAG          3
#define ID_WINDOW_0 	(GUI_ID_USER + 0x00)
#define ID_TEXT_0 	    (GUI_ID_USER + 0x01)
#define ID_BUTTON_0     (GUI_ID_USER + 0xA0)
#define ID_BUTTON_1     (GUI_ID_USER + 0xB0)

/* Exported constants --------------------------------------------------------*/
#define GUI_STCOLOR_LIGHTBLUE   0x00DCA939
#define GUI_STCOLOR_DARKBLUE    0x00522000

/*
*********************************************************************************************************
*                                       变量
*********************************************************************************************************
*/
GUI_HMEM hMemQR;

/*
*********************************************************************************************************
*	函 数 名: _cbButtonBack
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonBack(WM_MESSAGE * pMsg)
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin))
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(100, 100, 72);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE);
				GUI_DrawBitmap(&bmReturn, 45, 45);
			}
			else
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(100, 100, 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);
				GUI_DrawBitmap(&bmReturn, 45, 45);
			}
			break;

		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbButtonList
*	功能说明: 按钮回调函数
*	形    参: pMsg  消息指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbButtonList(WM_MESSAGE * pMsg)
{
	WM_HWIN  hWin;

	hWin  = pMsg->hWin;
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			if (BUTTON_IsPressed(hWin))
			{
				GUI_SetColor(GUI_DARKGRAY);
				GUI_AA_FillCircle(0, 100, 72);
				GUI_SetBkColor(GUI_DARKGRAY);
				GUI_SetColor(GUI_WHITE);

				GUI_DrawBitmap(&bmSetting, 7, 45);
			}
			else
			{
				GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_AA_FillCircle(0, 100 , 72);
				GUI_SetBkColor(GUI_STCOLOR_LIGHTBLUE);
				GUI_SetColor(GUI_WHITE);

				GUI_DrawBitmap(&bmSetting, 7, 45);
			}
			break;

		default:
			BUTTON_Callback(pMsg);
	}
}

/*
*********************************************************************************************************
*	               对话框控件列表
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateReserved[] = {
	{ WINDOW_CreateIndirect, "Window",      ID_WINDOW_0,    0,    0,   854, 480, 0, 0x0, 0},
	{ TEXT_CreateIndirect,   "",            ID_TEXT_0,      50,   80,  754, 40,  0, 0},

	{ BUTTON_CreateIndirect, "MusicList",   ID_BUTTON_1,     0,    380,  100, 100, 0, 0, 0},
	{ BUTTON_CreateIndirect, "DlgBack",     ID_BUTTON_0,   754,    380,  100, 100, 0, 0, 0},

};

/*
*********************************************************************************************************
*	函 数 名: InitDialogSys
*	功能说明: 对话框回调函数的初始化消息
*	形    参：pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDialogReserved(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;


	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonBack);

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
	WM_SetHasTrans(hItem);
	WM_SetCallback(hItem, _cbButtonList);

	/* 初始化SD卡部分 */
	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetFont(hItem, &GUI_Font24B_ASCII);
    TEXT_SetText(hItem, "http://www.kpuav.com");

	/* 设置抗锯齿因数 */
	GUI_AA_SetFactor(MAG);
	hMemQR = GUI_QR_Create("http://www.kpuav.com/", 10, GUI_QR_ECLEVEL_L, 0);
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallbackReserved
*	功能说明: 回调函数
*	形    参: pMsg   消息指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackReserved(WM_MESSAGE * pMsg)
{
	GUI_QR_INFO Info;
	int NCode, Id;
	WM_HWIN hWin = pMsg->hWin;

	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG: {
			InitDialogReserved(pMsg);//初始化按钮窗口
		} break;

		case WM_PAINT: {
			GUI_QR_GetInfo(hMemQR, &Info);
			GUI_QR_Draw(hMemQR, (WM_GetWindowSizeX(hWin) - Info.Width*10)/2, 125);
		} break;

		case WM_NOTIFY_PARENT: {
			Id = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id)
			{
					/* 关闭对话框 */
					case ID_BUTTON_0:
						switch(NCode)
						{
								case WM_NOTIFICATION_RELEASED:
									GUI_QR_Delete(hMemQR);
									GUI_EndDialog(hWin, 0);
								break;
						}
						break;
					case ID_BUTTON_1:
						switch(NCode)
						{
								case WM_NOTIFICATION_RELEASED:
									//GUI_QR_Delete(hMemQR);
									//GUI_EndDialog(hWin, 0);
								break;
						}
						break;
			}
		} break;

		default: {
			WM_DefaultProc(pMsg);
		} break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: App_Reserved
*	功能说明: 未使用的应用全部使用此对话框
*	形    参: hWin 所创建对话框
*	返 回 值: 无
*********************************************************************************************************
*/
void App_Reserved(WM_HWIN hWin)
{
	GUI_CreateDialogBox(_aDialogCreateReserved,
						GUI_COUNTOF(_aDialogCreateReserved),
						&_cbCallbackReserved,
						hWin,
						0,
						0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
