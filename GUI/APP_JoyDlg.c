#include "include.h"
#include "MainTask.h"
#include "fsl_pwm.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_BUTTON_0     (GUI_ID_USER + 0x01)
#define ID_BUTTON_1     (GUI_ID_USER + 0x02)
#define ID_BUTTON_2     (GUI_ID_USER + 0x03)
#define ID_BUTTON_3     (GUI_ID_USER + 0x04)
#define ID_BUTTON_4     (GUI_ID_USER + 0x05)
#define ID_SLIDER_0		(GUI_ID_USER + 0x06)
#define ID_SLIDER_1		(GUI_ID_USER + 0x07)

#define ID_TEXT_0        (GUI_ID_USER + 0x10)
#define ID_TEXT_1        (GUI_ID_USER + 0x11)

WM_HWIN  hWinJoy = WM_HWIN_NULL;
TaskHandle_t JOY_task_handle;

static const GUI_WIDGET_CREATE_INFO _aDialogCreateJOY[] = {
	{ FRAMEWIN_CreateIndirect,  "Framewin", ID_FRAMEWIN_0, 0, 0, 854, 430, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, 	"Button",   ID_BUTTON_0,  45, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, 	"Button",   ID_BUTTON_1, 215, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, 	"Button",   ID_BUTTON_2, 385, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, 	"Button",   ID_BUTTON_3, 555, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, 	"Close",    ID_BUTTON_4, 725, 320, 80, 50, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect,	"Slider",	ID_SLIDER_0, 173, 50, 262, 47, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect,	"Slider",	ID_SLIDER_1, 173, 150, 262, 47, 0, 0x0, 0 },
};

//无线收发对话框回调函数
void _cbDialogJOY(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	int     NCode;
	int     Id;
	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG: {
			//初始化窗口标题
			FRAMEWIN_SetTitleHeight(hWin, 32);
			FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetText(hWin, "UAV JOY");
			FRAMEWIN_SetFont(hWin, &GUI_FontHZ24);
			//设置窗口关闭按钮 下面三个是系统自带的不好用
			//FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
			FRAMEWIN_SetMoveable(hWin, 0);  //窗口可移动 0
			FRAMEWIN_SetResizeable(hWin,0); //窗口可缩放 0

			BUTTON_SetFont(WM_GetDialogItem(hWin, ID_BUTTON_0), &GUI_FontHZ24);
			BUTTON_SetFont(WM_GetDialogItem(hWin, ID_BUTTON_1), &GUI_FontHZ24);
			BUTTON_SetFont(WM_GetDialogItem(hWin, ID_BUTTON_2), &GUI_FontHZ24);
			BUTTON_SetFont(WM_GetDialogItem(hWin, ID_BUTTON_3), &GUI_FontHZ24);
			BUTTON_SetFont(WM_GetDialogItem(hWin, ID_BUTTON_4), &GUI_FontHZ24);

			BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON_0), "RST=0");
			BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON_1), "RST=1");
			BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON_2), "GPIO=0");
			BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON_3), "GPIO=1");
			BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON_4), "Close");
			//初始化滑块
			SLIDER_SetRange(WM_GetDialogItem(hWin, ID_SLIDER_0), 5, 100);
			SLIDER_SetNumTicks(WM_GetDialogItem(hWin, ID_SLIDER_0), 10);
			SLIDER_SetWidth(WM_GetDialogItem(hWin, ID_SLIDER_0), 20);
			SLIDER_SetValue(WM_GetDialogItem(hWin, ID_SLIDER_0), 80);

			SLIDER_SetRange(WM_GetDialogItem(hWin, ID_SLIDER_1), 5, 100);
			SLIDER_SetNumTicks(WM_GetDialogItem(hWin, ID_SLIDER_1), 10);
			SLIDER_SetWidth(WM_GetDialogItem(hWin, ID_SLIDER_1), 20);
			SLIDER_SetValue(WM_GetDialogItem(hWin, ID_SLIDER_1), 80);
		} break;

		//通知父窗口 
		case WM_NOTIFY_PARENT: {
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id) {
				case ID_BUTTON_0: break;
				case ID_BUTTON_1: break;
				case ID_BUTTON_2: break;
				case ID_BUTTON_3: break;
				case ID_BUTTON_4: {
					if(NCode == WM_NOTIFICATION_RELEASED){
						WM_MESSAGE Message;
						Message.hWinSrc = hWinJoy;
						Message.MsgId   = WM_DELETE;
						Message.Data.v  = 6;
						WM_SendMessage(hWinStatus, &Message);
						// vTaskDelete(JOY_task_handle); //删除任务
						//GUI_EndDialog(hWin, 0); //删除窗口
						WM_DeleteWindow(hWinJoy);
						hWinJoy = NULL;
					}
				} break;
				case ID_SLIDER_0: break;
				case ID_SLIDER_1: break;
			}
		} break;

		default: {
			WM_DefaultProc(pMsg);
		} break;
	}
}

void App_JOYDLG(WM_HWIN hWin, uint32_t idenx)
{
	WM_MESSAGE Message;
	if(hWinJoy == NULL) {
		//创建对话框
		hWinJoy = GUI_CreateDialogBox(_aDialogCreateJOY, 
							GUI_COUNTOF(_aDialogCreateJOY), 
							_cbDialogJOY, 
							hWin, 
							0, 
							0);

		Message.hWinSrc = hWinJoy;
		Message.MsgId   = WM_CREATE;
		Message.Data.v  = idenx;
		WM_SendMessage(hWinStatus, &Message);

		PRINTF("App_JOYDLG idenx = %d  src= 0x%x \r\n", idenx, hWinJoy);
	} else {
		WM_ShowWindow(hWinJoy); 
	}
}
