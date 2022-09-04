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

WM_HWIN  hWinWIFI = WM_HWIN_NULL;
TaskHandle_t WIFI_task_handle;

static const GUI_WIDGET_CREATE_INFO _aDialogCreateWIFI[] = {
	{ FRAMEWIN_CreateIndirect,  "Framewin", ID_FRAMEWIN_0, 0, 0, 854, 430, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, 	"Button",   ID_BUTTON_0,  45, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, 	"Button",   ID_BUTTON_1, 215, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, 	"Button",   ID_BUTTON_2, 385, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, 	"Button",   ID_BUTTON_3, 555, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, 	"Close",    ID_BUTTON_4, 725, 320, 80, 50, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect,	"Slider",	ID_SLIDER_0, 173, 50, 262, 47, 0, 0x0, 0 },
	{ SLIDER_CreateIndirect,	"Slider",	ID_SLIDER_1, 173, 150, 262, 47, 0, 0x0, 0 },
};

#include "NRF24L01.h"

void Send_RF_Par(void)
{
	static unsigned char heartbeat = 0;
	short *p_data = NULL;
	unsigned char Check_Sum = 0;
	//清空缓冲区
	memset(TxBuf, 0, sizeof(TxBuf));
	TxBuf[0] = heartbeat++;

	p_data = (short*)&TxBuf[1];
	*p_data = -200;
	p_data = (short*)&TxBuf[3];
	*p_data = -123;
	p_data = (short*)&TxBuf[5];
	*p_data = 200;
	p_data = (short*)&TxBuf[7];
	*p_data = 420;
	for(int i = 0; i < 31; i += 1) {
		Check_Sum += TxBuf[i];
	}
	TxBuf[31] = Check_Sum;
	nRF24L01_TxPacket(TxBuf);
}
//无线收发对话框回调函数
void _cbDialogWIFI(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	int     NCode;
	int     Id;
	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG: {
			//初始化窗口标题
			FRAMEWIN_SetTitleHeight(hWin, 32);
			FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetText(hWin, "ESP8266 WIFI");
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
			NRF24L01_Init(TX);
			// __aeabi_assert("crush", __FILE__, __LINE__);
		} break;

		//通知父窗口 
		case WM_NOTIFY_PARENT: {
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id) {
				case ID_BUTTON_0:  {
					Send_RF_Par();
				} break;
				case ID_BUTTON_1: break;
				case ID_BUTTON_2: break;
				case ID_BUTTON_3: break;
				case ID_BUTTON_4: {
					if(NCode == WM_NOTIFICATION_RELEASED){
						WM_MESSAGE Message;
						Message.hWinSrc = hWinWIFI;
						Message.MsgId   = WM_DELETE;
						Message.Data.v  = 5;
						WM_SendMessage(hWinStatus, &Message);
						vTaskDelete(WIFI_task_handle); //删除任务
						//GUI_EndDialog(hWin, 0); //删除窗口
						WM_DeleteWindow(hWinWIFI);
						hWinWIFI = NULL;
					}
				} break;
				case ID_SLIDER_0: {
					unsigned int pwm_value;
					if(NCode == WM_NOTIFICATION_RELEASED) {
						// 此消息可用于存储 EEPROM
						// pwm_calue = SLIDER_GetValue(WM_GetDialogItem(hWin, ID_SLIDER_0));
					} else if(NCode == WM_NOTIFICATION_VALUE_CHANGED) {
						pwm_value = SLIDER_GetValue(WM_GetDialogItem(hWin, ID_SLIDER_0));
						PWM_UpdatePwmDutycycle(PWM4, kPWM_Module_3, kPWM_PwmA, kPWM_SignedCenterAligned, pwm_value);
						PWM_SetPwmLdok(PWM4, kPWM_Control_Module_3, true);
					}
				} break;
				case ID_SLIDER_1: {
					unsigned int pwm_value;
					if(NCode == WM_NOTIFICATION_RELEASED) {
						// 此消息可用于存储 EEPROM
						// pwm_calue = SLIDER_GetValue(WM_GetDialogItem(hWin, ID_SLIDER_0));
					} else if(NCode == WM_NOTIFICATION_VALUE_CHANGED) {
						pwm_value = SLIDER_GetValue(WM_GetDialogItem(hWin, ID_SLIDER_0));
						PWM_UpdatePwmDutycycle(PWM4, kPWM_Module_3, kPWM_PwmA, kPWM_SignedCenterAligned, pwm_value);
						PWM_SetPwmLdok(PWM4, kPWM_Control_Module_3, true);
					}
				} break;
			}
		} break;

		default: {
			WM_DefaultProc(pMsg);
		} break;
	}
}

void WIFI_Task(void *pvParameters)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		Send_RF_Par();
		vTaskDelayUntil(&xLastWakeTime,configTICK_RATE_HZ/100);
	}
}

void App_WIFIDLG(WM_HWIN hWin, uint32_t idenx)
{
	WM_MESSAGE Message;
	if(hWinWIFI == NULL) {
		//创建对话框
		hWinWIFI = GUI_CreateDialogBox(_aDialogCreateWIFI, 
							GUI_COUNTOF(_aDialogCreateWIFI), 
							_cbDialogWIFI, 
							hWin, 
							0, 
							0);

		xTaskCreate(WIFI_Task, "WIFI_Task", 1024 /sizeof(uint32_t), NULL, 4, &WIFI_task_handle);

		Message.hWinSrc = hWinWIFI;
		Message.MsgId   = WM_CREATE;
		Message.Data.v  = idenx;
		WM_SendMessage(hWinStatus, &Message);

		PRINTF("App_WIFIDLG idenx = %d  src= 0x%x \r\n", idenx, hWinWIFI);
	} else {
		WM_ShowWindow(hWinWIFI); 
	}
}
