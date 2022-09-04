#include "fsl_debug_console.h"
#include "fsl_common.h"

#include "HZfontupdata.h"
#include "emwin_support.h"
#include "EmWinHZFont.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

#include "include.h"
#include "DIALOG.h"
#include "NRF24L01.h"
#include "MainTask.h"



//单选框横向 编组 【1-255】  左右各一个单选框，未选中的为-1 否则为0
#define RADIO_GROUP0 1
#define RADIO_GROUP1 2
#define RADIO_GROUP2 3

WM_HWIN hWinWireless_SET = NULL;
//滚轮内容【空】 用于清空滚轮参数
char *NULL_CODE[1] = {NULL};
//滚轮行高
uint8_t LineHeight = 30;
//指向当前编辑的编辑框
int Edit_Post=0;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_2 (GUI_ID_USER + 0x03)
#define ID_TEXT_3 (GUI_ID_USER + 0x04)
#define ID_TEXT_4 (GUI_ID_USER + 0x05)
#define ID_TEXT_5 (GUI_ID_USER + 0x06)
#define ID_TEXT_6 (GUI_ID_USER + 0x07)
#define ID_TEXT_7 (GUI_ID_USER + 0x08)

#define ID_RADIO_0 (GUI_ID_USER + 0x10)
#define ID_RADIO_1 (GUI_ID_USER + 0x11)
#define ID_RADIO_2 (GUI_ID_USER + 0x12)
#define ID_RADIO_3 (GUI_ID_USER + 0x13)
#define ID_RADIO_4 (GUI_ID_USER + 0x14)
#define ID_RADIO_5 (GUI_ID_USER + 0x15)

#define ID_EDIT_0 (GUI_ID_USER + 0x20)
#define ID_EDIT_1 (GUI_ID_USER + 0x21)
#define ID_EDIT_2 (GUI_ID_USER + 0x22)
#define ID_EDIT_3 (GUI_ID_USER + 0x23)
#define ID_EDIT_4 (GUI_ID_USER + 0x24)
#define ID_EDIT_5 (GUI_ID_USER + 0x25)
#define ID_EDIT_6 (GUI_ID_USER + 0x26)
#define ID_EDIT_7 (GUI_ID_USER + 0x27)
#define ID_EDIT_8 (GUI_ID_USER + 0x28)
#define ID_EDIT_9 (GUI_ID_USER + 0x29)
#define ID_EDIT_10 (GUI_ID_USER + 0x2A)
#define ID_EDIT_11 (GUI_ID_USER + 0x2B)

#define ID_CHECKBOX_0   (GUI_ID_USER + 0x30)
#define ID_CHECKBOX_1   (GUI_ID_USER + 0x31)
#define ID_CHECKBOX_2   (GUI_ID_USER + 0x32)
#define ID_CHECKBOX_3   (GUI_ID_USER + 0x33)
#define ID_CHECKBOX_4   (GUI_ID_USER + 0x34)
#define ID_CHECKBOX_5   (GUI_ID_USER + 0x35)

#define ID_BUTTON_0 (GUI_ID_USER + 0x40)
#define ID_BUTTON_1 (GUI_ID_USER + 0x41)

#define ID_LISTWHEEL_0 (GUI_ID_USER + 0x50)

//NRF24L01应用界面对话框初始化选项
static const GUI_WIDGET_CREATE_INFO _aDialogCreateWirelwss_SET[] = {
	{ FRAMEWIN_CreateIndirect, "无线通信（设置）", ID_FRAMEWIN_0, 0, 0, 854, 428, 0, 0x64, 0 },

	{ TEXT_CreateIndirect, "通讯速率：",ID_TEXT_0, 15, 15, 120, 24, 0, 0x64, 0 },
	{ RADIO_CreateIndirect, "1Mbps", ID_RADIO_0, 150, 15+3, 100, 24, 0, 0x1, 0 },
	{ RADIO_CreateIndirect, "2Mbps", ID_RADIO_1, 265, 15+3, 100, 24, 0, 0x1, 0 },

	{ TEXT_CreateIndirect, "CRC校验：",ID_TEXT_1, 15, 60, 120, 24, 0, 0x64, 0 },
	{ RADIO_CreateIndirect, "Open", ID_RADIO_2,  150, 60+3, 100, 24, 0, 0x1, 0 },
	{ RADIO_CreateIndirect, "Close", ID_RADIO_3, 265, 60+3, 100, 24, 0, 0x1, 0 },

	{ TEXT_CreateIndirect, "自动应答：",ID_TEXT_2,    15, 105,   120, 24, 0, 0x64, 0 },
	{ RADIO_CreateIndirect, "Open",     ID_RADIO_4,  150, 105+3, 100, 24, 0, 0x1, 0 },
	{ RADIO_CreateIndirect, "Close",    ID_RADIO_5,  265, 105+3, 100, 24, 0, 0x1, 0 },

	{ TEXT_CreateIndirect, "接收通道",  ID_TEXT_3,      15, 150, 120, 24, 0, 0x64, 0 },
	{ CHECKBOX_CreateIndirect, "RX0", ID_CHECKBOX_0, 150, 150+2, 80,  26, 0, 0x0, 0 },
	{ CHECKBOX_CreateIndirect, "RX1", ID_CHECKBOX_1, 235, 150+2, 80,  26, 0, 0x0, 0 },
	{ CHECKBOX_CreateIndirect, "RX2", ID_CHECKBOX_2, 320, 150+2, 80,  26, 0, 0x0, 0 },
	{ CHECKBOX_CreateIndirect, "RX3", ID_CHECKBOX_3, 405, 150+2, 80,  26, 0, 0x0, 0 },
	{ CHECKBOX_CreateIndirect, "RX4", ID_CHECKBOX_4, 490, 150+2, 80,  26, 0, 0x0, 0 },
	{ CHECKBOX_CreateIndirect, "RX5", ID_CHECKBOX_5, 575, 150+2, 80,  26, 0, 0x0, 0 },

	{ TEXT_CreateIndirect, "载荷包长：", ID_TEXT_4,  15, 195,   120, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_0, 150, 195+3,  80, 24, 0, 0x64, 0 },

	{ TEXT_CreateIndirect, "无线信道：", ID_TEXT_5, 15,  240,   120, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_1, 150, 240+3,  80, 24, 0, 0x64, 0 },

	{ TEXT_CreateIndirect, "接收地址：", ID_TEXT_6, 15, 285, 150, 20, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_2, 150, 285+3,  80, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_3, 240, 285+3,  80, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_4, 330, 285+3,  80, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_5, 420, 285+3,  80, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_6, 510, 285+3,  80, 24, 0, 0x64, 0 },

	{ TEXT_CreateIndirect, "发送地址：", ID_TEXT_7, 15, 330, 150, 20, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_7,  150, 330+3,  80, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_8,  240, 330+3,  80, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_9,  330, 330+3,  80, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_10, 420, 330+3,  80, 24, 0, 0x64, 0 },
	{ EDIT_CreateIndirect, "EDIT",       ID_EDIT_11, 510, 330+3,  80, 24, 0, 0x64, 0 },

	{ LISTWHEEL_CreateIndirect, "Listwheel", ID_LISTWHEEL_0, 720, 30, 130, 200, 0, 0x0, 0 },

	{ BUTTON_CreateIndirect, "写入", ID_BUTTON_0, 650, 330, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "退出", ID_BUTTON_1, 750, 330, 80, 50, 0, 0x0, 0 },

};

//在LISTWHEEL0上绘制红色指示行
static int ListWheel0_OwnerDraw(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo)
{
    switch(pDrawItemInfo->Cmd) {
        case WIDGET_ITEM_DRAW_OVERLAY: {
            GUI_SetColor(GUI_RED);
            GUI_DrawHLine(60,35,95);
            GUI_DrawHLine(90,35,95);
		} break;

        default: {
			return LISTWHEEL_OwnerDraw(pDrawItemInfo);
		}
    }
    return 0;
}

//初始化设置窗口
void InitDialogNRF2401_SET(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	unsigned int i,j;
	char buf[16];
	//【 窗口初始化 标题 】
	{
		FRAMEWIN_SetTitleHeight(hWin, 32);
		FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
		FRAMEWIN_SetText(hWin, "无线通信（设置）");
		FRAMEWIN_SetFont(hWin, &GUI_FontHZ24);
		FRAMEWIN_SetMoveable(hWin, 0);  //窗口可移动 0
		FRAMEWIN_SetResizeable(hWin,0); //窗口可缩放 0
	}
	//【 文本初始化 设置选项】
	{
		for(i = ID_TEXT_0; i <= ID_TEXT_7; i++) {
			//文本字体初始化
			TEXT_SetFont(WM_GetDialogItem(hWin, i), &GUI_FontHZ24);
			//设置文本对齐方式
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, i), GUI_TA_LEFT | GUI_TA_VCENTER);
		}

		TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_0), "通讯速率：");
		TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_1), "CRC校验： ");
		TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_2), "自动应答  ");
		TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_3), "接收通道：");
		TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_4), "载荷包长：");
		TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_5), "无线信道：");
		TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_6), "接收地址：");
	}
	//【 单选框初始化 通讯速率 】
	{
		RADIO_SetText(WM_GetDialogItem(hWin, ID_RADIO_0), "1Mbps", 0);
		RADIO_SetText(WM_GetDialogItem(hWin, ID_RADIO_1), "2Mbps", 0);

		RADIO_SetFont(WM_GetDialogItem(hWin, ID_RADIO_0), GUI_FONT_24_ASCII);//不可用自建字库要出错
		RADIO_SetFont(WM_GetDialogItem(hWin, ID_RADIO_1), GUI_FONT_24_ASCII);

		RADIO_SetGroupId(WM_GetDialogItem(hWin, ID_RADIO_0), RADIO_GROUP0);//设置单选组按钮
		RADIO_SetGroupId(WM_GetDialogItem(hWin, ID_RADIO_1), RADIO_GROUP0);//设置单选组按钮

		if(NRF24L01_Par.RF_Speed & 0x08) {
			RADIO_SetValue(WM_GetDialogItem(hWin, ID_RADIO_1), 0);
		} else {
			RADIO_SetValue(WM_GetDialogItem(hWin, ID_RADIO_0),0);
		}
	}
	//【 单选框初始化 CRC开关 】
	{
		RADIO_SetText(WM_GetDialogItem(hWin, ID_RADIO_2), "Close", 0);
		RADIO_SetText(WM_GetDialogItem(hWin, ID_RADIO_3), "Open", 0);

		RADIO_SetFont(WM_GetDialogItem(hWin, ID_RADIO_2), GUI_FONT_24_ASCII);
		RADIO_SetFont(WM_GetDialogItem(hWin, ID_RADIO_3), GUI_FONT_24_ASCII);

		RADIO_SetGroupId(WM_GetDialogItem(hWin, ID_RADIO_2), RADIO_GROUP1);//设置单选组按钮
		RADIO_SetGroupId(WM_GetDialogItem(hWin, ID_RADIO_3), RADIO_GROUP1);//设置单选组按钮

		if(NRF24L01_Par.CRC_EN == true) {
			RADIO_SetValue(WM_GetDialogItem(hWin, ID_RADIO_3), 0);
		} else {
			RADIO_SetValue(WM_GetDialogItem(hWin, ID_RADIO_2), 0);
		}
	}
	//【 单选框初始化 自动应答 】初始化单选框【自动应答】
	{
		RADIO_SetText(WM_GetDialogItem(hWin, ID_RADIO_4), "Close", 0);
		RADIO_SetText(WM_GetDialogItem(hWin, ID_RADIO_5), "Open", 0);

		RADIO_SetFont(WM_GetDialogItem(hWin, ID_RADIO_4), GUI_FONT_24_ASCII);
		RADIO_SetFont(WM_GetDialogItem(hWin, ID_RADIO_5), GUI_FONT_24_ASCII);

		RADIO_SetGroupId(WM_GetDialogItem(hWin, ID_RADIO_4), RADIO_GROUP2);//设置单选组按钮
		RADIO_SetGroupId(WM_GetDialogItem(hWin, ID_RADIO_5), RADIO_GROUP2);//设置单选组按钮

		if(NRF24L01_Par.AUTO_ACK_EN == true) {
			RADIO_SetValue(WM_GetDialogItem(hWin, ID_RADIO_5), 0);
		} else {
			RADIO_SetValue(WM_GetDialogItem(hWin, ID_RADIO_4), 0);
		}
	}
	//【 复选框初始化 接收通道 】
	{
		for(i = ID_CHECKBOX_0; i <= ID_CHECKBOX_5; i++) { CHECKBOX_SetFont(WM_GetDialogItem(hWin, i), GUI_FONT_24_ASCII);}
		for(i = ID_CHECKBOX_0; i <= ID_CHECKBOX_5; i++) { CHECKBOX_SetTextColor(WM_GetDialogItem(hWin, i), GUI_BLACK);}
		for(i = ID_CHECKBOX_0; i <= ID_CHECKBOX_5; i++) { CHECKBOX_SetNumStates(WM_GetDialogItem(hWin, i), 2);}
		CHECKBOX_SetText(WM_GetDialogItem(hWin, ID_CHECKBOX_0), "RX1");
		CHECKBOX_SetText(WM_GetDialogItem(hWin, ID_CHECKBOX_1), "RX2");
		CHECKBOX_SetText(WM_GetDialogItem(hWin, ID_CHECKBOX_2), "RX3");
		CHECKBOX_SetText(WM_GetDialogItem(hWin, ID_CHECKBOX_3), "RX4");
		CHECKBOX_SetText(WM_GetDialogItem(hWin, ID_CHECKBOX_4), "RX5");
		CHECKBOX_SetText(WM_GetDialogItem(hWin, ID_CHECKBOX_5), "RX6");

		CHECKBOX_SetState(WM_GetDialogItem(hWin, ID_CHECKBOX_0), (NRF24L01_Par.RX_pipe_CH & 0x01) >> 0);
		CHECKBOX_SetState(WM_GetDialogItem(hWin, ID_CHECKBOX_1), (NRF24L01_Par.RX_pipe_CH & 0x02) >> 1);
		CHECKBOX_SetState(WM_GetDialogItem(hWin, ID_CHECKBOX_2), (NRF24L01_Par.RX_pipe_CH & 0x04) >> 2);
		CHECKBOX_SetState(WM_GetDialogItem(hWin, ID_CHECKBOX_3), (NRF24L01_Par.RX_pipe_CH & 0x08) >> 3);
		CHECKBOX_SetState(WM_GetDialogItem(hWin, ID_CHECKBOX_4), (NRF24L01_Par.RX_pipe_CH & 0x10) >> 4);
		CHECKBOX_SetState(WM_GetDialogItem(hWin, ID_CHECKBOX_5), (NRF24L01_Par.RX_pipe_CH & 0x20) >> 5);

		WM_DisableWindow(WM_GetDialogItem(hWin, ID_CHECKBOX_0));//通道一 禁止选择
	}
	//【 编辑框初始化 载荷长 】
	{
		EDIT_SetTextAlign(WM_GetDialogItem(hWin, ID_EDIT_0), GUI_TA_HCENTER |GUI_TA_VCENTER);
		EDIT_SetDecMode(WM_GetDialogItem(hWin, ID_EDIT_0), NRF24L01_Par.PLOAD_WIDTH, 1, 32, 0, GUI_EDIT_NORMAL);//十进制模式
		//EDIT_SetValue(WM_GetDialogItem(hWin, ID_EDIT_0),NRF24L01_Par.PLOAD_WIDTH);
		EDIT_SetFont(WM_GetDialogItem(hWin, ID_EDIT_0), GUI_FONT_24_ASCII);
		EDIT_EnableBlink(WM_GetDialogItem(hWin, ID_EDIT_0), 500, 1);//光标闪烁
		EDIT_SetTextColor(WM_GetDialogItem(hWin, ID_EDIT_0), EDIT_CI_ENABLED, GUI_MAKE_COLOR(0x00000000));
	}
	//【 编辑框初始化 无线信道 】
	{
		EDIT_SetTextAlign(WM_GetDialogItem(hWin, ID_EDIT_1), GUI_TA_HCENTER | GUI_TA_VCENTER);
		//EDIT_SetHexMode(WM_GetDialogItem(hWin, ID_EDIT_1),NRF24L01_Par.RF_channel,0,0xFF);
		//EDIT_SetValue(WM_GetDialogItem(hWin, ID_EDIT_1),NRF24L01_Par.RF_channel);
		sprintf((char*)buf, "0x%02X", NRF24L01_Par.RF_channel);
		EDIT_SetText(WM_GetDialogItem(hWin, ID_EDIT_1), buf);
		EDIT_SetFont(WM_GetDialogItem(hWin, ID_EDIT_1), GUI_FONT_24_ASCII);
		EDIT_EnableBlink(WM_GetDialogItem(hWin, ID_EDIT_1), 500, 1);//光标闪烁
		EDIT_SetTextColor(WM_GetDialogItem(hWin, ID_EDIT_1), EDIT_CI_ENABLED, GUI_MAKE_COLOR(0x00000000));
		EDIT_SetMaxLen(WM_GetDialogItem(hWin, ID_EDIT_1), 4);
	}
	//【 编辑框初始化 接收地址 】
	{
		//设置对齐方式
		for(i = ID_EDIT_2; i <= ID_EDIT_6; i++) {
			EDIT_SetTextAlign(WM_GetDialogItem(hWin, i), GUI_TA_HCENTER | GUI_TA_VCENTER);
		}
		//设置字体
		for(i = ID_EDIT_2; i <= ID_EDIT_6; i++) {
			EDIT_SetFont(WM_GetDialogItem(hWin, i), GUI_FONT_24_ASCII);
		}
		//设置背景颜色
		for(i = ID_EDIT_2; i <= ID_EDIT_6; i++) {
			EDIT_SetTextColor(WM_GetDialogItem(hWin, i), EDIT_CI_ENABLED, GUI_MAKE_COLOR(0x00000000));
		}
		//光标闪烁  500ms 开启
		for(i = ID_EDIT_2; i <= ID_EDIT_6; i++) {
			EDIT_EnableBlink(WM_GetDialogItem(hWin, i), 500, 1);
		}
		//设置内容
		for(i = ID_EDIT_2, j = 0; i <= ID_EDIT_6; i++, j++) {
			sprintf((char*)buf, "0x%02X", NRF24L01_Par.RX_ADDRESS[j]);
			EDIT_SetText(WM_GetDialogItem(hWin, i), buf);
		}
	}
	//【 编辑框初始化 发送地址 】
	{
		//设置对齐方式
		for(i = ID_EDIT_7; i <= ID_EDIT_11; i++) {
			EDIT_SetTextAlign(WM_GetDialogItem(hWin, i), GUI_TA_HCENTER | GUI_TA_VCENTER);
		}
		//设置字体
		for(i = ID_EDIT_7; i <= ID_EDIT_11; i++) {
			EDIT_SetFont(WM_GetDialogItem(hWin, i), GUI_FONT_24_ASCII);
		}
		//设置背景颜色
		for(i = ID_EDIT_7; i <= ID_EDIT_11; i++) {
			EDIT_SetTextColor(WM_GetDialogItem(hWin, i), EDIT_CI_ENABLED, GUI_MAKE_COLOR(0x00000000));
		}
		//光标闪烁  500ms 开启
		for(i = ID_EDIT_7; i <= ID_EDIT_11; i++) {
			EDIT_EnableBlink(WM_GetDialogItem(hWin, i), 500, 1);
		}
		//设置内容
		for(i = ID_EDIT_7, j = 0; i <= ID_EDIT_11; i++, j++) {
			sprintf((char*)buf, "0x%02X", NRF24L01_Par.TX_ADDRESS[j]);
			EDIT_SetText(WM_GetDialogItem(hWin, i), buf);
		}
	}
	//【 Listwhell 初始化 模拟键盘 】
	{
		LISTWHEEL_SetFont(WM_GetDialogItem(hWin, ID_LISTWHEEL_0), &GUI_FontHZ24);							//设置字体
		LISTWHEEL_SetTextAlign(WM_GetDialogItem(hWin, ID_LISTWHEEL_0), GUI_TA_VCENTER|GUI_TA_HCENTER);		//对齐方式
		LISTWHEEL_SetSnapPosition(WM_GetDialogItem(hWin, ID_LISTWHEEL_0), (150-LineHeight)/2);				//设置吸附的位置
		LISTWHEEL_SetLineHeight(WM_GetDialogItem(hWin, ID_LISTWHEEL_0), LineHeight);						//设置绘制数据项所使用的行高
		LISTWHEEL_SetTextColor(WM_GetDialogItem(hWin, ID_LISTWHEEL_0), LISTWHEEL_CI_SEL, 0xEE8822);			//设置选中的条目的文本颜色
		//设置Year的LISTWHEEL
		LISTWHEEL_SetOwnerDraw(WM_GetDialogItem(hWin, ID_LISTWHEEL_0), ListWheel0_OwnerDraw);				//绘制红色指示行
		//添加数据项
		for(i = 0; i <= 0xFF; i++) {
			sprintf(buf, "%d", i);
			LISTWHEEL_AddString(WM_GetDialogItem(hWin, ID_LISTWHEEL_0), (char*)buf);
		}
	}
  //【 按键初始化 】
	{
		BUTTON_SetFont(WM_GetDialogItem(hWin, ID_BUTTON_0), &GUI_FontHZ24);
		BUTTON_SetFont(WM_GetDialogItem(hWin, ID_BUTTON_1), &GUI_FontHZ24);

		BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON_0), "写入");
		BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON_1), "退出");
	}
}

//写入参数
void Write_Par(WM_MESSAGE * pMsg)
{
	char buf[10];
	unsigned char temp_char;
	unsigned int  i,j;
	unsigned int  temp_int;
	WM_HWIN hWin = pMsg->hWin;
	//通讯速率
	if(RADIO_GetValue(WM_GetDialogItem(hWin, ID_RADIO_0)) == 0) {
		NRF24L01_Par.RF_Speed &= !0x08; //1Mbps
	} else {
		NRF24L01_Par.RF_Speed |= 0x08; //2Mbps
	}
	PRINTF("RF_Speed=0x%X \r\n", NRF24L01_Par.RF_Speed);

	//CRC校验  （16位）
	if(RADIO_GetValue(WM_GetDialogItem(hWin, ID_RADIO_2)) == 0) {
		NRF24L01_Par.CRC_EN = false; //关闭
	} else {
		NRF24L01_Par.CRC_EN = true; //打开 并且16bit
	}
	PRINTF("CRC_EN=%d \r\n", NRF24L01_Par.CRC_EN);

	//自动应答（所有通道）
	if(RADIO_GetValue(WM_GetDialogItem(hWin, ID_RADIO_4)) == 0) {
		NRF24L01_Par.AUTO_ACK_EN = false; //关闭应答
	} else {
		NRF24L01_Par.AUTO_ACK_EN = true;  //打开应答
	}
	PRINTF("AUTO_ACK_EN=%d \r\n", NRF24L01_Par.AUTO_ACK_EN);

	//接收通道
	temp_char = 0x0;
	temp_char |= (CHECKBOX_GetState(WM_GetDialogItem(hWin, ID_CHECKBOX_0)) << 0);
	temp_char |= (CHECKBOX_GetState(WM_GetDialogItem(hWin, ID_CHECKBOX_1)) << 1);
	temp_char |= (CHECKBOX_GetState(WM_GetDialogItem(hWin, ID_CHECKBOX_2)) << 2);
	temp_char |= (CHECKBOX_GetState(WM_GetDialogItem(hWin, ID_CHECKBOX_3)) << 3);
	temp_char |= (CHECKBOX_GetState(WM_GetDialogItem(hWin, ID_CHECKBOX_4)) << 4);
	temp_char |= (CHECKBOX_GetState(WM_GetDialogItem(hWin, ID_CHECKBOX_5)) << 5);
	NRF24L01_Par.RX_pipe_CH = (temp_char & 0x3F) | 0x01;
	PRINTF("RX_pipe_CH=%d \r\n", NRF24L01_Par.RX_pipe_CH);

	//载荷长
	temp_int = EDIT_GetValue(WM_GetDialogItem(hWin, ID_EDIT_0));
	if(temp_int < 1) {
		temp_int = 1;
	} else if(temp_int > 32) {
		temp_int = 32;
	}
	NRF24L01_Par.PLOAD_WIDTH = temp_int;
	PRINTF("PLOAD_WIDTH=%d \r\n", NRF24L01_Par.PLOAD_WIDTH);

	//无线信道
	temp_int = EDIT_GetNumChars(WM_GetDialogItem(hWin, ID_EDIT_1));
	EDIT_GetText(WM_GetDialogItem(hWin, ID_EDIT_1), buf, temp_int + 1);
	temp_char = 0;
	if(buf[2] > 'F') {buf[2] = 'F';}
	if(buf[3] > 'F') {buf[3] = 'F';}
	if(buf[2] >= 'A') {temp_char |= (buf[2] - 55) << 4;} else {temp_char |= (buf[2]-'0') << 4;}
	if(buf[3] >= 'A') {temp_char |= (buf[3] - 55);} else {temp_char |= (buf[3]-'0');} //字符   'A'-55=10;
	NRF24L01_Par.RF_channel = temp_char;
	PRINTF("channel=%s 0x%02x\r\n", buf, temp_char);

	//接收地址
	for(i = ID_EDIT_2, j = 0; i <= ID_EDIT_6; i++, j++) {
		temp_int = EDIT_GetNumChars(WM_GetDialogItem(hWin, i));
		EDIT_GetText(WM_GetDialogItem(hWin, i), buf, temp_int + 1);
		temp_char = 0;
		if(buf[2]>'F'){buf[2]='F';}
		if(buf[3]>'F'){buf[3]='F';}
		if(buf[2]>='A'){temp_char|=(buf[2]-55)<<4;}else{temp_char|=(buf[2]-'0')<<4;}
		if(buf[3]>='A'){temp_char|=(buf[3]-55);}else{temp_char|=(buf[3]-'0');}//字符   'A'-55=10;
		NRF24L01_Par.RX_ADDRESS[j]=temp_char;
		PRINTF("address[%d]=0x%02x \r\n",j,temp_char);
	}

	//发送地址
	for(i = ID_EDIT_7,j=0; i <= ID_EDIT_11; i++, j++)
	{
		temp_int=EDIT_GetNumChars(WM_GetDialogItem(hWin, i));
		EDIT_GetText(WM_GetDialogItem(hWin, i),buf,temp_int+1);
		temp_char=0;
		if(buf[2] > 'F'){buf[2]='F';}
		if(buf[3] > 'F'){buf[3]='F';}
		if(buf[2] >= 'A'){temp_char |= (buf[2] - 55)<<4;}else{temp_char |= (buf[2]-'0')<<4;}
		if(buf[3] >= 'A'){temp_char |= (buf[3] - 55);}else{temp_char |= (buf[3]-'0');}//字符   'A'-55=10;
		NRF24L01_Par.TX_ADDRESS[j] = temp_char;
		PRINTF("address[%d]=0x%02x \r\n",j,temp_char);
	}
}

//退出设置
void Exit_Par(WM_MESSAGE * pMsg)
{
	WM_MESSAGE Msg;

	CRC_EN      = NRF24L01_Par.CRC_EN;
	AUTO_ACK_EN = NRF24L01_Par.AUTO_ACK_EN;
	RF_Speed    = NRF24L01_Par.RF_Speed;
	PLOAD_WIDTH = NRF24L01_Par.PLOAD_WIDTH;
	RF_channel  = NRF24L01_Par.RF_channel;
	RX_pipe_CH  = NRF24L01_Par.RX_pipe_CH;

	RX_ADDRESS[0] = NRF24L01_Par.RX_ADDRESS[0];
	RX_ADDRESS[1] = NRF24L01_Par.RX_ADDRESS[1];
	RX_ADDRESS[2] = NRF24L01_Par.RX_ADDRESS[2];
	RX_ADDRESS[3] = NRF24L01_Par.RX_ADDRESS[3];
	RX_ADDRESS[4] = NRF24L01_Par.RX_ADDRESS[4];

	TX_ADDRESS[0] = NRF24L01_Par.TX_ADDRESS[0];
	TX_ADDRESS[1] = NRF24L01_Par.TX_ADDRESS[1];
	TX_ADDRESS[2] = NRF24L01_Par.TX_ADDRESS[2];
	TX_ADDRESS[3] = NRF24L01_Par.TX_ADDRESS[3];
	TX_ADDRESS[4] = NRF24L01_Par.TX_ADDRESS[4];

	Msg.MsgId = WM_INIT_DIALOG;

	WM_SendMessage(WM_GetClientWindow(hWinWireless), &Msg);
}

void  WindowSKB_ShowInterface(WM_HWIN hItem, unsigned char Widget_Type);

//无线收发对话框回调函数
void _cbDialogWireless_SET(WM_MESSAGE * pMsg)
{
	unsigned int i;
	char buf[16];
	unsigned char val;
	int     NCode;
	int     Id;
	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG: {
			InitDialogNRF2401_SET(pMsg);
		} break;

		case WM_NOTIFY_PARENT: {
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id) {
				case ID_RADIO_0:
				case ID_RADIO_1:
				case ID_RADIO_2:
				case ID_RADIO_3:
				case ID_RADIO_4:
				case ID_RADIO_5: {
					Edit_Post = 0;
				} break;
				case ID_CHECKBOX_0:
				case ID_CHECKBOX_1:
				case ID_CHECKBOX_2:
				case ID_CHECKBOX_3:
				case ID_CHECKBOX_4:
				case ID_CHECKBOX_5: {
					Edit_Post = 0;
				} break;
				case ID_BUTTON_0: { //写入参数
					if (NCode == WM_NOTIFICATION_RELEASED) {
						Write_Par(pMsg);
					}
				} break;
				case ID_BUTTON_1: { //退出
					if (NCode == WM_NOTIFICATION_RELEASED) {
						Exit_Par(pMsg);
						GUI_EndDialog(pMsg->hWin, 0);
					}
				} break;
				case ID_EDIT_0: { //载荷长
					if(NCode == WM_NOTIFICATION_RELEASED) {
						Edit_Post = Id;
						val = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, ID_EDIT_0));
						LISTWHEEL_SetText(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0), (GUI_ConstString*)NULL_CODE); //写入空覆盖已经存在的
						for(i = 0; i <= 255; i++) {
							sprintf(buf, "%d", i);
							LISTWHEEL_AddString(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0), (char*)buf);
						}
						LISTWHEEL_SetPos(WM_GetDialogItem(pMsg->hWin,ID_LISTWHEEL_0), val);
					}
				} break;
				case ID_EDIT_1: //无线信道
				case ID_EDIT_2: //接收地址0
				case ID_EDIT_3: //接收地址1
				case ID_EDIT_4: //接收地址2
				case ID_EDIT_5: //接收地址3
				case ID_EDIT_6: { //接收地址4
					if(NCode == WM_NOTIFICATION_RELEASED) { //已释放
						Edit_Post = Id;
						LISTWHEEL_SetText(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0), (GUI_ConstString*)NULL_CODE);
						for(i = 0; i <= 0xFF; i++) {
							sprintf(buf,"0x%02X",i);
							LISTWHEEL_AddString(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0), (char*)buf);
						}
						EDIT_GetText(WM_GetDialogItem(pMsg->hWin, Id), buf, EDIT_GetNumChars(WM_GetDialogItem(pMsg->hWin, Id))+1);
						val=0;
						if(buf[2]>='A'){val|=(buf[2]-55)<<4;}else{val|=(buf[2]-'0')<<4;}
						if(buf[3]>='A'){val|=(buf[3]-55);}else{val|=(buf[3]-'0');}//字符   'A'-55=10;
						LISTWHEEL_SetPos(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0), val);
					}
				} break;
				case ID_EDIT_7:  //发送地址0
				case ID_EDIT_8:  //发送地址1
				case ID_EDIT_9:  //发送地址2
				case ID_EDIT_10: //发送地址3
				case ID_EDIT_11: { //发送地址4
					if(NCode == WM_NOTIFICATION_RELEASED) { //已释放
						Edit_Post = Id;
						LISTWHEEL_SetText(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0), (GUI_ConstString*)NULL_CODE);
						for(i=0; i <= 0xFF; i++) {
							sprintf(buf, "0x%02X", i);
							LISTWHEEL_AddString(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0), (char*)buf);
						}
						EDIT_GetText(WM_GetDialogItem(pMsg->hWin, Id), buf, EDIT_GetNumChars(WM_GetDialogItem(pMsg->hWin, Id)) + 1);
						val = 0;
						if(buf[2]>='A'){val|=(buf[2]-55)<<4;}else{val|=(buf[2]-'0')<<4;}
						if(buf[3]>='A'){val|=(buf[3]-55);}else{val|=(buf[3]-'0');}//字符   'A'-55=10;
						LISTWHEEL_SetPos(WM_GetDialogItem(pMsg->hWin,ID_LISTWHEEL_0),val);
					}
				} break;
				case ID_LISTWHEEL_0: { //滚轮
					if((NCode == WM_NOTIFICATION_SEL_CHANGED) && (Edit_Post != 0)) {
						val = LISTWHEEL_GetPos(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0));
						switch (Edit_Post) {
							case ID_EDIT_0: {
								val = val>32?32:val;
								val = val==0?1:val;
								EDIT_SetValue(WM_GetDialogItem(pMsg->hWin, Edit_Post), val);
							} break;
							case ID_EDIT_1: {
								val = val>0x7F?0x7F:val;
								sprintf((char*)buf, "0x%02X", val);
								EDIT_SetText(WM_GetDialogItem(pMsg->hWin, Edit_Post), buf);
								EDIT_SetValue(WM_GetDialogItem(pMsg->hWin, Edit_Post), val);
							} break;
							case ID_EDIT_2:
							case ID_EDIT_3:
							case ID_EDIT_4:
							case ID_EDIT_5:
							case ID_EDIT_6:
							case ID_EDIT_7:
							case ID_EDIT_8:
							case ID_EDIT_9:
							case ID_EDIT_10:
							case ID_EDIT_11: {
								sprintf((char*)buf, "0x%02X", val);EDIT_SetText(WM_GetDialogItem(pMsg->hWin, Edit_Post), buf);
								EDIT_SetValue(WM_GetDialogItem(pMsg->hWin, Edit_Post), val);
							} break;
						}
					} 
				} break;
			}
		} break;

		default: {
			WM_DefaultProc(pMsg);
		} break;
	}
}

/*********************************************************************
*
*       CreateWindow
*/
void App_Wireless_SET(WM_HWIN hWin)
{
	//创建对话框
	hWinWireless_SET = GUI_CreateDialogBox( _aDialogCreateWirelwss_SET,
											GUI_COUNTOF(_aDialogCreateWirelwss_SET),
											_cbDialogWireless_SET,
											hWin,
											0,
											0);
}






