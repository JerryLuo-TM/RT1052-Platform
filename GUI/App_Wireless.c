#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

#include "include.h"
#include "DIALOG.h"
#include "NRF24L01.h"
#include "MainTask.h"

TaskHandle_t OS_NRF24L01_task;

WM_HWIN  hWinWireless = WM_HWIN_NULL;  	/* 2401窗口句柄 */

bool Last_Show_HEX = false, Show_HEX = false;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)

#define ID_TEXT_0   (GUI_ID_USER + 0x01)
#define ID_TEXT_1   (GUI_ID_USER + 0x02)
#define ID_TEXT_2   (GUI_ID_USER + 0x03)
#define ID_TEXT_3   (GUI_ID_USER + 0x04)

#define ID_BUTTON_0   (GUI_ID_USER + 0x0B)
#define ID_BUTTON_1   (GUI_ID_USER + 0x0C)
#define ID_BUTTON_2   (GUI_ID_USER + 0x0D)
#define ID_BUTTON_3   (GUI_ID_USER + 0x0E)
#define ID_BUTTON_4   (GUI_ID_USER + 0x0F)

#define ID_TEXT_4   (GUI_ID_USER + 0x10)
#define ID_TEXT_5   (GUI_ID_USER + 0x11)
#define ID_TEXT_6   (GUI_ID_USER + 0x12)
#define ID_TEXT_7   (GUI_ID_USER + 0x13)
#define ID_TEXT_8   (GUI_ID_USER + 0x14)
#define ID_TEXT_9   (GUI_ID_USER + 0x15)
#define ID_TEXT_10   (GUI_ID_USER + 0x16)
#define ID_TEXT_11   (GUI_ID_USER + 0x17)
#define ID_TEXT_12   (GUI_ID_USER + 0x18)
#define ID_TEXT_13   (GUI_ID_USER + 0x19)
#define ID_TEXT_14   (GUI_ID_USER + 0x1A)
#define ID_TEXT_15   (GUI_ID_USER + 0x1B)
#define ID_TEXT_16   (GUI_ID_USER + 0x1C)
#define ID_TEXT_17   (GUI_ID_USER + 0x1D)
#define ID_TEXT_18   (GUI_ID_USER + 0x1E)
#define ID_TEXT_19   (GUI_ID_USER + 0x1F)
#define ID_TEXT_20   (GUI_ID_USER + 0x20)
#define ID_TEXT_21   (GUI_ID_USER + 0x21)
#define ID_TEXT_22   (GUI_ID_USER + 0x22)
#define ID_TEXT_23   (GUI_ID_USER + 0x23)
#define ID_TEXT_24   (GUI_ID_USER + 0x24)
#define ID_TEXT_25   (GUI_ID_USER + 0x25)
#define ID_TEXT_26   (GUI_ID_USER + 0x26)
#define ID_TEXT_27   (GUI_ID_USER + 0x27)
#define ID_TEXT_28   (GUI_ID_USER + 0x28)
#define ID_TEXT_29   (GUI_ID_USER + 0x29)
#define ID_TEXT_30   (GUI_ID_USER + 0x2A)
#define ID_TEXT_31   (GUI_ID_USER + 0x2B)
#define ID_TEXT_32   (GUI_ID_USER + 0x2C)
#define ID_TEXT_33   (GUI_ID_USER + 0x2D)
#define ID_TEXT_34   (GUI_ID_USER + 0x2E)
#define ID_TEXT_35   (GUI_ID_USER + 0x2F)

//NRF24L01应用界面对话框初始化选项
static const GUI_WIDGET_CREATE_INFO _aDialogCreateWirelwss[] = {
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 854, 430, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_0, 40, 15, 200, 24, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_1, 320, 15, 192, 24, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_2, 605, 15, 188, 24, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_3, 190, 45, 444, 24, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 67, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 227, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 387, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_3, 547, 320, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_4, 707, 320, 80, 50, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_4, 35, 80, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_5, 165, 80, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_6, 295, 80, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_7, 425, 80, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_8, 555, 80, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_9, 685, 80, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_10, 35, 120, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_11, 165, 120, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_12, 295, 120, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_13, 425, 120, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_14, 555, 120, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_15, 685, 120, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_16, 35, 160, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_17, 165, 160, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_18, 295, 160, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_19, 425, 160, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_20, 555, 160, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_21, 685, 160, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_22, 35, 200, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_23, 165, 200, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_24, 295, 200, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_25, 425, 200, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_26, 555, 200, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_27, 685, 200, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_28, 35, 240, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_29, 165, 240, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_30, 295, 240, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_31, 425, 240, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_32, 555, 240, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_33, 685, 240, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_34, 35, 280, 120, 16, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_35, 165, 280, 120, 16, 0, 0x64, 0 },
};

//更新主界面参数
void NRF2401_Update(WM_MESSAGE * pMsg)
{
	uint32_t i,ID;
	uint8_t buf[40];
	static uint8_t last_rxbuf[32];
	WM_HWIN hWin = pMsg->hWin;
	if(Last_Show_HEX != Show_HEX) {
		if(Show_HEX == true) {
			for(i = 0, ID = ID_TEXT_4; i < sizeof(RxBuf); i++,ID++) {
				sprintf((char*)buf,"RxBuf[ %d ] :  0x%02X",i,RxBuf[i]);
				TEXT_SetText(WM_GetDialogItem(hWin,ID), (char*)buf);
			}
		} else {
			for(i = 0, ID = ID_TEXT_4; i < sizeof(RxBuf); i++, ID++) {
				sprintf((char*)buf, "RxBuf[ %d ] :  %3d", i, RxBuf[i]);
				TEXT_SetText(WM_GetDialogItem(hWin,ID), (char*)buf);
			}
		}
	} else {
		if(Show_HEX == true) {
			for(i = 0, ID = ID_TEXT_4; i < sizeof(RxBuf); i++, ID++) {
				if(last_rxbuf[i] != RxBuf[i]) {
					sprintf((char*)buf, "RxBuf[ %d ] :  0x%02X", i, RxBuf[i]);
					TEXT_SetText(WM_GetDialogItem(hWin,ID), (char*)buf);
				}
			}
		} else {
			for(i = 0, ID = ID_TEXT_4; i < sizeof(RxBuf); i++, ID++) {
				if(last_rxbuf[i] != RxBuf[i]) {
					sprintf((char*)buf, "RxBuf[ %d ] :  %3d", i, RxBuf[i]);
					TEXT_SetText(WM_GetDialogItem(hWin,ID), (char*)buf);
				}
			}
		}
	}
	memcpy(last_rxbuf, RxBuf, sizeof(RxBuf));
	Last_Show_HEX = Show_HEX;
}

//初始化界面
void InitDialogNRF2401(WM_MESSAGE * pMsg)
{
	unsigned int ID;
	unsigned char buf[100];
	WM_HWIN hWin = pMsg->hWin;
	//获取2401配置参数
	NRF24L01_Par.RF_Speed = RF_Speed;
	NRF24L01_Par.CRC_EN   = CRC_EN;
	NRF24L01_Par.AUTO_ACK_EN   = AUTO_ACK_EN;
	NRF24L01_Par.RX_pipe_CH    = RX_pipe_CH;
	NRF24L01_Par.RF_channel		 = RF_channel;
	NRF24L01_Par.PLOAD_WIDTH	 = PLOAD_WIDTH;
	memcpy(&NRF24L01_Par.RX_ADDRESS[0], &RX_ADDRESS[0], 5);
	memcpy(&NRF24L01_Par.TX_ADDRESS[0], &TX_ADDRESS[0], 5);

	//初始化2401模块
	NRF24L01_Init(RX);
	//开启定时器
	WM_CreateTimer(WM_GetClientWindow(hWin), 0, 500, 0);

	//初始化窗口标题
	FRAMEWIN_SetTitleHeight(hWin, 32);
	FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
	FRAMEWIN_SetText(hWin, "无线通信");
	FRAMEWIN_SetFont(hWin, &GUI_FontHZ24);
	//设置窗口关闭按钮 下面三个是系统自带的不好用
	//FRAMEWIN_AddCloseButton(hItem, FRAMEWIN_BUTTON_RIGHT, 0);
	FRAMEWIN_SetMoveable(hWin, 0);  //窗口可移动 0
	FRAMEWIN_SetResizeable(hWin,0); //窗口可缩放 0
	//配置参数
	{
		if(RF_Speed&0x08){sprintf((char*)buf,"通讯速率: %dMbps",2);}
			else{sprintf((char*)buf,"通讯速率: %dMbps",1);}
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), (char*)buf);

		sprintf((char*)buf,"载荷长度: %d",PLOAD_WIDTH);
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), (char*)buf);

		sprintf((char*)buf,"无线信道: 0x%02X",RF_channel);
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), (char*)buf);

		sprintf((char*)buf,"接收地址 = 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",RX_ADDRESS[0],RX_ADDRESS[1],RX_ADDRESS[2],RX_ADDRESS[3],RX_ADDRESS[4]);
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), (char*)buf);

		TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &GUI_FontHZ24);
		TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &GUI_FontHZ24);
		TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &GUI_FontHZ24);
		TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &GUI_FontHZ24);

		TEXT_SetTextAlign(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextAlign(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextAlign(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextAlign(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), GUI_TA_LEFT | GUI_TA_VCENTER);

		TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), GUI_MAKE_COLOR(0x00FF00FF));
		TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), GUI_MAKE_COLOR(0x00FF00FF));
		TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), GUI_MAKE_COLOR(0x00FF00FF));
		TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), GUI_MAKE_COLOR(0x00FF0080));
	}
	//底部按键
	{
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), &GUI_FontHZ32);
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), &GUI_FontHZ24);
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2), &GUI_FontHZ24);
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3), &GUI_FontHZ32);
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4), &GUI_FontHZ32);

		BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), "DEC");
		BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), "功能一");
		BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2), "功能二");
		BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3), "设置");
		BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4), "退出");
	}
    //接收区
	{
		// Initialization of 'Text'
		unsigned int i;
		//设置字体
		for(i=ID_TEXT_4;i<=ID_TEXT_35;i++){TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, i), GUI_FONT_16B_1);}
		//设置对齐方式
		for(i=ID_TEXT_4;i<=ID_TEXT_35;i++){TEXT_SetTextAlign(WM_GetDialogItem(pMsg->hWin, i), GUI_TA_LEFT | GUI_TA_VCENTER);}
		if(Show_HEX==true)
		{
			for(i=0, ID=ID_TEXT_4; i<sizeof(RxBuf); i++, ID++) {
				sprintf((char*)buf, "RxBuf[ %d ] :  0x%02X", i, RxBuf[i]);
				TEXT_SetText(WM_GetDialogItem(hWin,ID), (char*)buf);
			}
		}
		else
		{
			for(i=0, ID=ID_TEXT_4; i<sizeof(RxBuf); i++, ID++) {
					sprintf((char*)buf,"RxBuf[ %d ] :  %3d", i, RxBuf[i]);
					TEXT_SetText(WM_GetDialogItem(hWin,ID), (char*)buf);
			}
		}
	}
	NRF2401_Update(pMsg);
}

//无线收发对话框回调函数
void _cbDialogWireless(WM_MESSAGE * pMsg)
{
	int     NCode;
	int     Id;
	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
			InitDialogNRF2401(pMsg);
		break;

		case WM_TIMER:
			WM_RestartTimer(pMsg->Data.v, 500);
		break;

		case MSG_NRF24L01:
			NRF2401_Update(pMsg);
		break;

		//通知父窗口
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id)
			{
				case ID_BUTTON_0: //进制转换
					switch(NCode)
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
								if(Show_HEX==true){Show_HEX=false;BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), "DEC");}
									else{Show_HEX=true;BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), "HEX");}
								WM_SendMessageNoPara(WM_GetClientWindow(hWinWireless), MSG_NRF24L01);
							break;
					}
				break;

				case ID_BUTTON_1: //空
				break;

				case ID_BUTTON_2: //空
				break;

				case ID_BUTTON_3: //设置
					App_Wireless_SET(pMsg->hWin);
				break;

				case ID_BUTTON_4: //关闭
					if(NCode == WM_NOTIFICATION_RELEASED) {
						WM_MESSAGE Message;
						Message.hWinSrc = hWinWireless;
						Message.MsgId   = WM_DELETE;
						Message.Data.v  = 4;
						WM_SendMessage(hWinStatus, &Message);
						vTaskDelete(OS_NRF24L01_task);//删除任务
						// GUI_EndDialog(pMsg->hWin, 0); //删除窗口
						WM_DeleteWindow(hWinWireless);
						hWinWireless = NULL;
					}
				break;
			}
		break;

		default:
			WM_DefaultProc(pMsg);
		break;
	}
}
//获取NRF24L01的数据
void NRF24L01_Task(void *pvParameters)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		if(nRF24L01_RxPacket(RxBuf)) {
			WM_SendMessageNoPara(WM_GetClientWindow(hWinWireless), MSG_NRF24L01);
		}
		vTaskDelayUntil(&xLastWakeTime,configTICK_RATE_HZ/100);
	}
}

//创建无线收发窗口
void App_Wireless(WM_HWIN hWin, uint32_t idenx)
{
	WM_MESSAGE Message;
	if(hWinWireless == NULL) {
		//创建对话框
		hWinWireless =  GUI_CreateDialogBox(_aDialogCreateWirelwss,
						GUI_COUNTOF(_aDialogCreateWirelwss),
						_cbDialogWireless,
						hWin,
						0,
						0);
		//模态对话框
		//WM_MakeModal(hWinWireless);
		//创建无线收发任务
		xTaskCreate(NRF24L01_Task, "NRF24L01_Task", 512 /sizeof(uint32_t), NULL, 4, &OS_NRF24L01_task);

		Message.hWinSrc = hWinWireless;
		Message.MsgId   = WM_CREATE;
		Message.Data.v  = idenx;
		WM_SendMessage(hWinStatus, &Message);

		PRINTF("App_Wireless idenx = %d  src= 0x%x \r\n", idenx, hWinWireless);
	} else {
		WM_ShowWindow(hWinWireless);
	}
}


/*************************** End of file ****************************/
