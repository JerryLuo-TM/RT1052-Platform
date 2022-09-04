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
*                                        �궨��
*********************************************************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_SPINBOX_0  (GUI_ID_USER + 0x01)
#define ID_SPINBOX_1  (GUI_ID_USER + 0x02)
#define ID_SPINBOX_2  (GUI_ID_USER + 0x03)
#define ID_SPINBOX_3  (GUI_ID_USER + 0x04)
#define ID_SPINBOX_4  (GUI_ID_USER + 0x05)
#define ID_SPINBOX_5  (GUI_ID_USER + 0x06)

WM_HWIN hWinAlarmSet;

/*
*********************************************************************************************************
*                                        ����
*********************************************************************************************************
*/
const char *apDays[] =
{
	"һ",
	"��",
	"��",
	"��",
	"��",
	"��",
	"��",
};

const char *apmonths[]=
{
	"1��",
	"2��",
	"3��",
	"4��",
	"5��",
	"6��",
	"7��",
	"8��",
	"9��",
	"10��",
	"11��",
	"12��",
};

/*
*********************************************************************************************************
*	                               �Ի���ؼ��б�
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateSetTimeAlarm[] = {
	{ FRAMEWIN_CreateIndirect,  "ϵͳʱ�䣨���ã�", ID_FRAMEWIN_0, 0, 0, 854, 430, 0, 0x64, 0 },

	{ TEXT_CreateIndirect,      "ʱ������",		GUI_ID_TEXT0,		95,22, 100, 24, 0,0},
	{ TEXT_CreateIndirect,      "��������",		GUI_ID_TEXT1,		295,22, 100, 24, 0,0},

	{ CHECKBOX_CreateIndirect,  "����ʱ������",		GUI_ID_CHECK0,	60,270,210,24, 0,0},
	{ CHECKBOX_CreateIndirect,  "������������",		GUI_ID_CHECK1,	280,270,210,24, 0,0},

	{ TEXT_CreateIndirect,      "ʱ:",	GUI_ID_TEXT2,	40, 59, 29, 24, 0,0},
	{ TEXT_CreateIndirect,      "��:",	GUI_ID_TEXT3,	40, 106+15,29, 24, 0,0},
	{ TEXT_CreateIndirect,      "��:",	GUI_ID_TEXT4,	40, 156+30,29, 24, 0,0},

	{ BUTTON_CreateIndirect,    "ȷ��", GUI_ID_BUTTON0, 650, 330, 80, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,    "ȡ��", GUI_ID_BUTTON1, 750, 330, 80, 50, 0, 0x0, 0 },

	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_0, 80, 56, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_1, 80, 103 + 15, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_2, 80, 149 + 30, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_3, 280, 56, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_4, 280, 103 + 15, 130, 35, 0, 0x0, 0 },
	{ SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_5, 280, 153 + 30, 130, 35, 0, 0x0, 0 },
};

void InitDialogSetTimeAlarm(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;

	//�� ���ڳ�ʼ�� ���� ��
	{
		FRAMEWIN_SetTitleHeight(hWin, 32);
		FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
		FRAMEWIN_SetText(hWin, "ϵͳʱ�䣨���ã�");
		FRAMEWIN_SetFont(hWin, &GUI_FontHZ24);
		FRAMEWIN_SetMoveable(hWin, 0);  //���ڿ��ƶ� 0
		FRAMEWIN_SetResizeable(hWin,0); //���ڿ����� 0
	}

	/* �����ؼ���ʼ�� */
	CALENDAR_SetDefaultSize(CALENDAR_SI_HEADER, 35 );
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_X, 40 );
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_Y, 28 );

	CALENDAR_SetDefaultFont(CALENDAR_FI_CONTENT, &GUI_FontHZ16);
	CALENDAR_SetDefaultFont(CALENDAR_FI_HEADER,  &GUI_FontHZ16);
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_WEEKDAY, 0xFF00FF);

	CALENDAR_SetDefaultDays(apDays);
	CALENDAR_SetDefaultMonths(apmonths);

	CALENDAR_Create(hWin,
					500,
					50,
					g_rtcDate.year,
					g_rtcDate.month,
					g_rtcDate.day,
					2,
					GUI_ID_CALENDAR0,
					WM_CF_SHOW);

	/* �ı��ؼ���ʼ�� */
    TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT0), &GUI_FontHZ24);
    TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT1), &GUI_FontHZ24);
    TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT2), &GUI_FontHZ24);
    TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT3), &GUI_FontHZ24);
    TEXT_SetFont(WM_GetDialogItem(hWin, GUI_ID_TEXT4), &GUI_FontHZ24);

	/* ��ѡ���ʼ�� */
    CHECKBOX_SetText(WM_GetDialogItem(hWin, GUI_ID_CHECK0), "����ʱ������");
    CHECKBOX_SetFont(WM_GetDialogItem(hWin, GUI_ID_CHECK0), &GUI_FontHZ24);
    CHECKBOX_SetText(WM_GetDialogItem(hWin, GUI_ID_CHECK1), "������������");
    CHECKBOX_SetFont(WM_GetDialogItem(hWin, GUI_ID_CHECK1), &GUI_FontHZ24);

	/* ��ť��ʼ�� */
    BUTTON_SetFont(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), &GUI_FontHZ24);
    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), "ȷ��");
	BUTTON_SetFont(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), &GUI_FontHZ24);
    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), "ȡ��");

	/* �༭���ʼ�� */
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_0)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_1)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_2)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_3)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_4)), GUI_TA_HCENTER | GUI_TA_VCENTER);
	EDIT_SetTextAlign(SPINBOX_GetEditHandle(WM_GetDialogItem(hWin,ID_SPINBOX_5)), GUI_TA_HCENTER | GUI_TA_VCENTER);

	/* SPINBOX��ʼ�� */
	SPINBOX_SetEdge(WM_GetDialogItem(hWin, ID_SPINBOX_0), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin, ID_SPINBOX_1), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin, ID_SPINBOX_2), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin, ID_SPINBOX_3), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin, ID_SPINBOX_4), SPINBOX_EDGE_CENTER);
	SPINBOX_SetEdge(WM_GetDialogItem(hWin, ID_SPINBOX_5), SPINBOX_EDGE_CENTER);

	SPINBOX_SetFont(WM_GetDialogItem(hWin, ID_SPINBOX_0), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin, ID_SPINBOX_1), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin, ID_SPINBOX_2), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin, ID_SPINBOX_3), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin, ID_SPINBOX_4), &GUI_FontHZ16);
	SPINBOX_SetFont(WM_GetDialogItem(hWin, ID_SPINBOX_5), &GUI_FontHZ16);

	SPINBOX_SetRange(WM_GetDialogItem(hWin, ID_SPINBOX_0), 0, 23);  /* ��Χ���� Сʱ */
	SPINBOX_SetRange(WM_GetDialogItem(hWin, ID_SPINBOX_1), 0, 59);  /* ��Χ���� ���� */
	SPINBOX_SetRange(WM_GetDialogItem(hWin, ID_SPINBOX_2), 0, 59);  /* ��Χ���� ���� */

	SPINBOX_SetRange(WM_GetDialogItem(hWin, ID_SPINBOX_3), 0, 23);  /* ��Χ���� ����Сʱ */
	SPINBOX_SetRange(WM_GetDialogItem(hWin, ID_SPINBOX_4), 0, 59);  /* ��Χ���� ���ӷ��� */
	SPINBOX_SetRange(WM_GetDialogItem(hWin, ID_SPINBOX_5), 0, 59);  /* ��Χ���� �������� */

	SPINBOX_SetValue(WM_GetDialogItem(hWin, ID_SPINBOX_0), g_rtcDate.hour);   /* ʱ */
	SPINBOX_SetValue(WM_GetDialogItem(hWin, ID_SPINBOX_1), g_rtcDate.minute); /* �� */
	SPINBOX_SetValue(WM_GetDialogItem(hWin, ID_SPINBOX_2), g_rtcDate.second); /* �� */

	SPINBOX_SetValue(WM_GetDialogItem(hWin, ID_SPINBOX_3), g_rtcDate.hour);    /* ����ʱ */
	SPINBOX_SetValue(WM_GetDialogItem(hWin, ID_SPINBOX_4), g_rtcDate.minute);  /* ���ӷ� */
	SPINBOX_SetValue(WM_GetDialogItem(hWin, ID_SPINBOX_5), g_rtcDate.second);  /* ������ */
}

static void _cbCallbackSetTimeAlarm(WM_MESSAGE * pMsg)
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId)
    {
		case WM_INIT_DIALOG: {
			InitDialogSetTimeAlarm(pMsg);
		} break;

		case WM_KEY: {
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key)
			{
				case GUI_KEY_ESCAPE:
					GUI_EndDialog(hWin, 1);
				break;

				case GUI_KEY_ENTER:
					GUI_EndDialog(hWin, 0);
				break;
			}
		} break;

		case WM_NOTIFY_PARENT: {
			Id = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			if(Id==GUI_ID_BUTTON0)
			{
				if(NCode == WM_NOTIFICATION_RELEASED) {
					/* �Ƿ񱣴�ʱ������ */
					if(CHECKBOX_GetState(WM_GetDialogItem(hWin,GUI_ID_CHECK0)) == 1) {
						CALENDAR_DATE CALENDAR_Date;
						snvs_lp_srtc_datetime_t RTC_SET_Structure;

						CALENDAR_GetSel(WM_GetDialogItem(hWin,GUI_ID_CALENDAR0), &CALENDAR_Date);
						RTC_SET_Structure.second    = SPINBOX_GetValue(WM_GetDialogItem(hWin, ID_SPINBOX_2));
						RTC_SET_Structure.minute    = SPINBOX_GetValue(WM_GetDialogItem(hWin, ID_SPINBOX_1));
						RTC_SET_Structure.hour      = SPINBOX_GetValue(WM_GetDialogItem(hWin, ID_SPINBOX_0));

						RTC_SET_Structure.year      = CALENDAR_Date.Year;
						RTC_SET_Structure.month     = CALENDAR_Date.Month;
						RTC_SET_Structure.day       = CALENDAR_Date.Day;

						SNVS_LP_SRTC_SetDatetime(SNVS, &RTC_SET_Structure);
						SNVS_HP_RTC_TimeSynchronize(SNVS);
						SNVS_HP_RTC_StartTimer(SNVS);
					}

					/* �Ƿ񱣴��������� */
					if(CHECKBOX_GetState(WM_GetDialogItem(hWin,GUI_ID_CHECK1)) == 1) {
						//SNVS_HP_RTC_SetAlarm(SNVS, &rtcDate);
					}

					GUI_EndDialog(hWin, 0);
				}
			} else if(Id == GUI_ID_BUTTON1) {
				if(NCode == WM_NOTIFICATION_RELEASED) {
					GUI_EndDialog(hWin, 0);
				}
			}
		} break;

        default: {
			WM_DefaultProc(pMsg);
		} break;
    }
}

void App_CalendarChild(WM_HWIN hWin)
{
	hWinAlarmSet = GUI_CreateDialogBox( _aDialogCreateSetTimeAlarm,
										GUI_COUNTOF(_aDialogCreateSetTimeAlarm),
										&_cbCallbackSetTimeAlarm,
										hWin,
										0,
										0);

	/* ����Ϊģ̬���� */
	WM_MakeModal(hWinAlarmSet);
}

