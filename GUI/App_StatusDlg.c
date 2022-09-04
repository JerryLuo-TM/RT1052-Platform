#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_snvs_hp.h"
#include "fsl_snvs_lp.h"

#include "MainTask.h"
#include "ff.h"
#include "diskio.h"

#include "led.h"
#include "svns_rtc.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0            (GUI_ID_USER + 0x00)
#define ID_BUTTON_0            (GUI_ID_USER + 0x01)
#define ID_TEXT_0              (GUI_ID_USER + 0x02)
#define ID_TEXT_1              (GUI_ID_USER + 0x03)
#define ID_BUTTON_1            (GUI_ID_USER + 0x04)
/*
*********************************************************************************************************
*                                       变量
*********************************************************************************************************
*/
extern uint16_t CPUUsage;
uint8_t s_ucSDDetect = 0;
WM_HWIN hWinStatus;

#define TASK_ICON_NUMBER 32
WM_task_info_t WM_TASK_ICON_INFO[TASK_ICON_NUMBER];


/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
	{ WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 430, 854, 55, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 5, 5, 45, 45, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,   "TIME",   ID_TEXT_0, 750, 5+2, 100, 24, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,   "DATA",   ID_TEXT_1, 750, 30-2, 100, 24, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, "cut",    ID_BUTTON_1, 695, 5, 45, 45, 0, 0x0, 0 },
};

uint32_t uint_to_str(uint64_t number, uint8_t *str_array)
{
	unsigned int i, value, bits;
	unsigned char byte_array[16];

	bits = 0;
	value = number;
	do {
		byte_array[bits++] = value % 10;
		value /= 10;
	}while(value);

	for(i = 0; i < bits; i++)
	{
		str_array[i] = byte_array[bits - i -1] + '0';
	}
	str_array[i] = '\0';

	return bits;
}

void WriteByte2File(unsigned char data,void *p)
{
	unsigned int nWriteten;
	f_write((FIL*)p, &data, 1, &nWriteten);
}

void emWin_CreateBMPPicture(int x0, int y0, int xSize, int ySize)
{
	FIL     filex;
	FRESULT result;
	uint32_t lc_time, len;
	char fil_addr[] = "2:";		//桌面图标所在目录
	char fil_type[] = ".bmp";	//后缀类型
	char name_buf[30];			//完整路径缓冲区
	char file_name[16];

	lc_time = (uint32_t)g_rtcDate.hour * 10000 + (uint32_t)g_rtcDate.minute * 100 + (uint32_t)g_rtcDate.second;
	memset(file_name, NULL, sizeof(file_name));
	len = uint_to_str(lc_time, (uint8_t*)file_name);
	PRINTF("len %d cut file name %d %s \r\n", len, lc_time, file_name);

	memset(name_buf, NULL, sizeof(name_buf));	//清空路径换乘区
	strcat((char*)name_buf, fil_addr);			//复制路径
	strcat((char*)name_buf, file_name);			//复制文件名
	strcat((char*)name_buf, fil_type);			//复制文件类型

	PRINTF("cut file = %s \r\n", name_buf);

	result = f_open(&filex, (const TCHAR *)name_buf, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
	PRINTF("f_open[%d] \r\n", result);
	if(result != FR_OK){

		return ;
	}
	GUI_BMP_SerializeEx(WriteByte2File, x0, y0, xSize, ySize, &filex);

	f_close(&filex);  //写完，关闭文件
}

static void Caculate_RTC(WM_MESSAGE * pMsg)
{
	snvs_hp_rtc_datetime_t last_rtcDate, rtcDate;
	unsigned char buf[30];
	WM_HWIN hWin = pMsg->hWin;

	rtcDate = g_rtcDate;
	if((rtcDate.year != last_rtcDate.year) || (rtcDate.month != last_rtcDate.month) || (rtcDate.day != last_rtcDate.day)) {
		sprintf((char*)buf, "%4d/%d/%d", rtcDate.year, rtcDate.month, rtcDate.day);
		TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_1), (char*)buf);
	}

	if((rtcDate.hour != last_rtcDate.hour) || (rtcDate.minute != last_rtcDate.minute) || (rtcDate.second != last_rtcDate.second)) {
		sprintf((char*)buf, "%.2d:%.2d", rtcDate.hour, rtcDate.minute);
		TEXT_SetText(WM_GetDialogItem(hWin,ID_TEXT_0), (char*)buf);
	}

	memcpy(&last_rtcDate, &rtcDate, sizeof(snvs_hp_rtc_datetime_t));
}

//开始按钮回调函数
static void _cbButton(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	hWin = pMsg->hWin;
	switch (pMsg->MsgId) {
		case WM_PAINT: {
			if (BUTTON_IsPressed(hWin)){
				GUI_SetAlpha(0x80);
				GUI_DrawBitmap(&bmDestWin, 0, 0);
				GUI_SetAlpha(0);
			}else{
				GUI_DrawBitmap(&bmDestWin, 0, 0);
			}
		} break;

		default: {
			BUTTON_Callback(pMsg);
		} break;
	}
}


static void _cbDialog(WM_MESSAGE * pMsg) {
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	uint8_t  idenx;
	uint32_t i;
	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG: {
			hItem = pMsg->hWin;
			WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x002E3B42));

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
			TEXT_SetFont(hItem, GUI_FONT_20_ASCII);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetTextColor(hItem, GUI_WHITE);
			TEXT_SetText(hItem, "00:00");

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
			TEXT_SetFont(hItem, GUI_FONT_20_ASCII);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetTextColor(hItem, GUI_WHITE);
			TEXT_SetText(hItem, "2020/1/1");

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			BUTTON_SetBitmap(hItem, BUTTON_CI_UNPRESSED, &bmDestWin);
			WM_SetCallback(hItem, _cbButton);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			BUTTON_SetFont(hItem, &GUI_FontHZ16);
			BUTTON_SetText(hItem, "截图");
		} break;

		case WM_PAINT: {
			GUI_SetColor(GUI_WHITE);
			GUI_DrawLine(75, 5, 75, 45);
		} break;

		case MSG_UPDATERTC: {
			Caculate_RTC(pMsg);
		} break;

		case WM_CREATE: {
			idenx = pMsg->Data.v;//桌面图标偏移
			ICONVIEW_AddBitmapItem(hIcon_Task, _aBitmapItem_min[idenx].pBitmap, "");
			for(i = 0 ; i < TASK_ICON_NUMBER ; i++) {
				if(WM_TASK_ICON_INFO[i].src_handle == NULL) {
					WM_TASK_ICON_INFO[i].idenx      = idenx;
					WM_TASK_ICON_INFO[i].src_handle = pMsg->hWinSrc;
					WM_TASK_ICON_INFO[i].is_active  = 1;
					PRINTF("CREATE item ok \r\n");
					break;
				}
			}
		} break;

		case WM_DELETE: {
			PRINTF("WM_DELETE handle \r\n");
			idenx = pMsg->Data.v;//桌面图标偏移
			PRINTF("WM_DELETE index = %d \r\n", idenx);
			for(i = 0 ; i < TASK_ICON_NUMBER ; i++) {
				if(((WM_TASK_ICON_INFO[i].idenx & 0x7F) == idenx) && (WM_TASK_ICON_INFO[i].src_handle != NULL)){
					ICONVIEW_DeleteItem(hIcon_Task, i);
					WM_TASK_ICON_INFO[i].idenx 		= 0;
					WM_TASK_ICON_INFO[i].src_handle = NULL;
					WM_TASK_ICON_INFO[i].is_active  = 0;
					PRINTF("WM_DELETE item ok \r\n");
					break;
				}
			}
			//删除的不是最后一个小图标,需要插入算法,把后面的图标移动到前面来
			if(WM_TASK_ICON_INFO[i+1].src_handle  != NULL) {
				do {
					WM_TASK_ICON_INFO[i].idenx      = WM_TASK_ICON_INFO[i+1].idenx;
					WM_TASK_ICON_INFO[i].src_handle = WM_TASK_ICON_INFO[i+1].src_handle;
					i++;
				} while (WM_TASK_ICON_INFO[i].src_handle != NULL);
				WM_TASK_ICON_INFO[i].idenx      = 0;
				WM_TASK_ICON_INFO[i].src_handle = NULL;
			}
		} break;

		case WM_NOTIFY_PARENT: {
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id) {
				case ID_BUTTON_0: { //桌面
					if(NCode == WM_NOTIFICATION_RELEASED) {
						for(i = 0 ; i < TASK_ICON_NUMBER ; i++){
							if(WM_TASK_ICON_INFO[i].src_handle  != NULL) {
								WM_HideWindow(WM_TASK_ICON_INFO[i].src_handle);
								WM_TASK_ICON_INFO[i].is_active  = 0;
							} else {
								break;
							}
						}
					}
				} break;

				case ID_BUTTON_1: { //截图
					if(NCode == WM_NOTIFICATION_RELEASED) {
						//taskENTER_CRITICAL();
						emWin_CreateBMPPicture(0, 0, LCD_GetXSize(), LCD_GetYSize());
						//taskEXIT_CRITICAL();
					}
				} break;

				case GUI_ID_ICONVIEW1: {
					if(NCode == WM_NOTIFICATION_RELEASED) {
						idenx = ICONVIEW_GetSel(pMsg->hWinSrc);
						// 清除活跃状态
						for(i = 0; i < TASK_ICON_NUMBER; i++) {
							if(WM_TASK_ICON_INFO[i].is_active == 1) {
								if(i != idenx) {
									WM_HideWindow(WM_TASK_ICON_INFO[i].src_handle);
									WM_TASK_ICON_INFO[i].is_active  = 0;
								}
							}
						}
						if(WM_TASK_ICON_INFO[idenx].src_handle  != NULL) {
							PRINTF("Show window %d handle =0x%X \r\n", idenx, WM_TASK_ICON_INFO[idenx].src_handle);
							WM_ShowWindow(WM_TASK_ICON_INFO[idenx].src_handle);
							WM_TASK_ICON_INFO[idenx].is_active  = 1;
						}
					}
				} break;
			}
		} break;

		default: WM_DefaultProc(pMsg); break;
	}
}

//创建任务栏窗口
void CreateSysStatusDlg(void)
{
	memset(WM_TASK_ICON_INFO, 0, sizeof(WM_TASK_ICON_INFO));

	/* 创建一个对话框, 类型XP系统的任务栏 */
	hWinStatus = GUI_CreateDialogBox(_aDialogCreate,
									GUI_COUNTOF(_aDialogCreate),
									_cbDialog,
									WM_HBKWIN,
									0,
									0);
}


