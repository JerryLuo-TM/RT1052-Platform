#include "include.h"
#include "MainTask.h"

WM_HWIN hWinInfo;

#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x00)
#define ID_TEXT_0     (GUI_ID_USER + 0x02)
#define ID_TEXT_1     (GUI_ID_USER + 0x03)
#define ID_TEXT_2     (GUI_ID_USER + 0x04)
#define ID_TEXT_3     (GUI_ID_USER + 0x05)
#define ID_TEXT_4     (GUI_ID_USER + 0x06)
#define ID_TEXT_5     (GUI_ID_USER + 0x07)
#define ID_TEXT_6     (GUI_ID_USER + 0x08)
#define ID_TEXT_7     (GUI_ID_USER + 0x09)
#define ID_TEXT_8     (GUI_ID_USER + 0x0A)
#define ID_TEXT_9     (GUI_ID_USER + 0x0B)

//资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
	{ FRAMEWIN_CreateIndirect, "",		ID_FRAMEWIN_0,  680, 300, 155, 104, 0,  0x0, 0 },

	{ TEXT_CreateIndirect, "核心占用:",  ID_TEXT_0,    3,   0, 150,  16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "",			ID_TEXT_1, 3+80,   0,  72,  16, 0, 0x64, 0 },

	{ TEXT_CreateIndirect, "电池电压:",  ID_TEXT_2,    3, 16, 150, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "",			ID_TEXT_3, 3+80, 16,  72, 16, 0, 0x64, 0 },

	{ TEXT_CreateIndirect, "输入电压:",  ID_TEXT_4,    3, 32, 150, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "",			ID_TEXT_5, 3+80, 32,  72, 16, 0, 0x64, 0 },

	{ TEXT_CreateIndirect, "充电电流:",  ID_TEXT_6,    3, 48, 150, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "",			ID_TEXT_7, 3+80, 48,  72, 16, 0, 0x64, 0 },

	{ TEXT_CreateIndirect, "充电状态:",  ID_TEXT_8,    3, 64, 150, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "",			ID_TEXT_9, 3+76, 64,  72, 16, 0, 0x64, 0 },
};

//初始化对话框
void Init_inofDialog(WM_MESSAGE * pMsg)
{
	char buf[48];
	WM_HWIN hWin = pMsg->hWin;
	//窗口初始化
	FRAMEWIN_SetMoveable(hWin, 1);    //窗口可移动 0
	FRAMEWIN_SetTitleHeight(hWin,16); //设置标题高度
	FRAMEWIN_SetTitleVis(hWin, 1);    //设置标题可见性

	//初始化控件
	for(uint32_t i = ID_TEXT_0; i <= ID_TEXT_9; i += 2){
		TEXT_SetTextAlign(WM_GetDialogItem(hWin, i), GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(WM_GetDialogItem(hWin, i), &GUI_FontHZ16);
		TEXT_SetTextColor(WM_GetDialogItem(hWin, i), GUI_MAKE_COLOR(0x007C18CC));
	}

	sprintf((char*)buf, "%d %%", osCPU_Usage);
	TEXT_SetFont(WM_GetDialogItem(hWin,ID_TEXT_1), &GUI_FontHZ16);
	TEXT_SetTextColor(WM_GetDialogItem(hWin,ID_TEXT_1), GUI_MAKE_COLOR(0x007C18CC));
	TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), (char*)buf);

	sprintf((char*)buf, "%4.2f V", 0.0f);
	TEXT_SetFont(WM_GetDialogItem(hWin,ID_TEXT_3), &GUI_FontHZ16);
	TEXT_SetTextColor(WM_GetDialogItem(hWin,ID_TEXT_3), GUI_MAKE_COLOR(0x007C18CC));
	TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_3), (char*)buf);

	sprintf((char*)buf, "%4.2f V", 0.0f);
	TEXT_SetFont(WM_GetDialogItem(hWin,ID_TEXT_5), &GUI_FontHZ16);
	TEXT_SetTextColor(WM_GetDialogItem(hWin,ID_TEXT_5), GUI_MAKE_COLOR(0x007C18CC));
	TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_5), (char*)buf);

	sprintf((char*)buf, "%4.2f A", 0.0f);
	TEXT_SetFont(WM_GetDialogItem(hWin,ID_TEXT_7), &GUI_FontHZ16);
	TEXT_SetTextColor(WM_GetDialogItem(hWin,ID_TEXT_7), GUI_MAKE_COLOR(0x007C18CC));
	TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_7), (char*)buf);

	TEXT_SetFont(WM_GetDialogItem(hWin,ID_TEXT_9), &GUI_FontHZ16);
	TEXT_SetTextColor(WM_GetDialogItem(hWin,ID_TEXT_9), GUI_MAKE_COLOR(GUI_BLACK));
	TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_9), "未充电");
}

//客户窗口重绘
static void PaintDialogSysInfo(WM_MESSAGE * pMsg)
{
	static BQ25895_T ls_BQ25895;
	static uint16_t  ls_osCPU_Usage;
	WM_HWIN hItem;
	WM_HWIN hWin = pMsg->hWin;
	char buf[48];

	if(ls_osCPU_Usage != osCPU_Usage) {
		hItem = WM_GetDialogItem(hWin, ID_TEXT_1);
		sprintf((char*)buf, "%d %%",osCPU_Usage);
		TEXT_SetFont(hItem, &GUI_FontHZ16);
		TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x007C18CC));
		TEXT_SetText(hItem, (char*)buf);
	}

	if(ls_BQ25895.vbat != BQ25895.vbat) {
		hItem = WM_GetDialogItem(hWin,ID_TEXT_3);
		sprintf((char*)buf, "%4.2f V",BQ25895.vbat);
		TEXT_SetFont(hItem, &GUI_FontHZ16);
		TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x007C18CC));
		TEXT_SetText(hItem, (char*)buf);
	}

	if(ls_BQ25895.vbus != BQ25895.vbus) {
		hItem = WM_GetDialogItem(hWin, ID_TEXT_5);
		sprintf((char*)buf, "%4.2f V", BQ25895.vbus);
		TEXT_SetFont(hItem, &GUI_FontHZ16);
		TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x007C18CC));
		TEXT_SetText(hItem, (char*)buf);
	}

	if(ls_BQ25895.chgc != BQ25895.chgc) {
		hItem = WM_GetDialogItem(hWin, ID_TEXT_7);
		sprintf((char*)buf, "%4.2f A", BQ25895.chgc);
		TEXT_SetFont(hItem, &GUI_FontHZ16);
		TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x007C18CC));
		TEXT_SetText(hItem, (char*)buf);
	}

	if(ls_BQ25895.charge_status != BQ25895.charge_status) {
		hItem = WM_GetDialogItem(hWin,ID_TEXT_9);
		TEXT_SetFont(hItem, &GUI_FontHZ16);
		if(BQ25895.charge_status == 0){
			TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(GUI_BLACK));
			TEXT_SetText(hItem, "未充电");
		} else if(BQ25895.charge_status == 1){
			TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(GUI_LIGHTBLUE));
			TEXT_SetText(hItem, "预充电");
		} else if(BQ25895.charge_status == 2){
			TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(GUI_BLUE));
			TEXT_SetText(hItem, "快速充电");
		} else if(BQ25895.charge_status == 3){
			TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(GUI_BLACK));
			TEXT_SetText(hItem, "充电完成");
		} else {
			TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(GUI_RED));
			TEXT_SetText(hItem, "未知状态");
		}
	}

	//保存上一次的数据
	ls_osCPU_Usage = osCPU_Usage;
	memcpy(&ls_BQ25895, &BQ25895, sizeof(ls_BQ25895));
}

/*
*********************************************************************************************************
*	函 数 名: _cbDialogSysInfo
*	功能说明: 回调函数
*	形    参: pMsg  WM_MESSAGE类型指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbDialogSysInfo(WM_MESSAGE * pMsg)
{
	//WM_HWIN hWin = pMsg->hWin;
	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG: {
			Init_inofDialog(pMsg);
		} break;

		case MSG_SYSINFO: {
			PaintDialogSysInfo(pMsg);
		} break;

		case WM_PAINT: {
			GUI_SetAlpha(0xFF);//设置窗口边框的可见性
		} break;

		case WM_CREATE: {
			GUI_AA_SetFactor(3);       
		} break;

		default : {
		} break;
	}
	FRAMEWIN_Callback(pMsg);
}

static void _cbClient(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG: {
			;
		} break;

		case WM_PAINT: {
			GUI_SetAlpha(0x80);
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			GUI_SetAlpha(0);
		} break;
			

		default: {
			WM_DefaultProc(pMsg);
		} break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: CreateSysInfoDlg
*	功能说明: 系统信息对话框
*	形    参: 无
*	返 回 值: 窗口句柄
*********************************************************************************************************
*/
void CreateSysInfoDlg(void)
{
	WM_HWIN hClient;

	hWinInfo = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialogSysInfo, WM_HBKWIN, 0, 0);
  	WM_SetHasTrans(hWinInfo); //设置有透明标记
	WM_SetCallback(hWinInfo, _cbDialogSysInfo);
	//设置客户区属性
	hClient = WM_GetClientWindow(hWinInfo); //获取客户区窗口句柄
	WM_SetHasTrans(hClient); //设置有透明标记
	WM_SetCallback(hClient, _cbClient);
}
