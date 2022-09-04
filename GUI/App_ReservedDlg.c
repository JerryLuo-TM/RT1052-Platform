#include "MainTask.h"

/*
*********************************************************************************************************
*                                       �궨��
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
*                                       ����
*********************************************************************************************************
*/
GUI_HMEM hMemQR;

/*
*********************************************************************************************************
*	�� �� ��: _cbButtonBack
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	�� �� ��: _cbButtonList
*	����˵��: ��ť�ص�����
*	��    ��: pMsg  ��Ϣָ��
*	�� �� ֵ: ��
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
*	               �Ի���ؼ��б�
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
*	�� �� ��: InitDialogSys
*	����˵��: �Ի���ص������ĳ�ʼ����Ϣ
*	��    �Σ�pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
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

	/* ��ʼ��SD������ */
	hItem = WM_GetDialogItem(hWin, ID_TEXT_0);
	TEXT_SetTextColor(hItem, GUI_BLACK);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetFont(hItem, &GUI_Font24B_ASCII);
    TEXT_SetText(hItem, "http://www.kpuav.com");

	/* ���ÿ�������� */
	GUI_AA_SetFactor(MAG);
	hMemQR = GUI_QR_Create("http://www.kpuav.com/", 10, GUI_QR_ECLEVEL_L, 0);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackReserved
*	����˵��: �ص�����
*	��    ��: pMsg   ��Ϣָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackReserved(WM_MESSAGE * pMsg)
{
	GUI_QR_INFO Info;
	int NCode, Id;
	WM_HWIN hWin = pMsg->hWin;

	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG: {
			InitDialogReserved(pMsg);//��ʼ����ť����
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
					/* �رնԻ��� */
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
*	�� �� ��: App_Reserved
*	����˵��: δʹ�õ�Ӧ��ȫ��ʹ�ô˶Ի���
*	��    ��: hWin �������Ի���
*	�� �� ֵ: ��
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
