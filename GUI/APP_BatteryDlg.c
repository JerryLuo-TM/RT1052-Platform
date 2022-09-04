#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"

#include "ff.h"
#include "diskio.h"

#include "include.h"
#include "touch.h"
#include "malloc.h"
#include "MainTask.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

#include "HZfontupdata.h"
#include "emwin_support.h"
#include "EmWinHZFont.h"

#include "NRF24L01.h"


#define ID_FRAMEWIN_0    (GUI_ID_USER + 0x00)
#define ID_BUTTON_0      (GUI_ID_USER + 0x01)
#define ID_BUTTON_1      (GUI_ID_USER + 0x02)
#define ID_TEXT_0        (GUI_ID_USER + 0x10)
#define ID_TEXT_1        (GUI_ID_USER + 0x11)
#define ID_TEXT_2        (GUI_ID_USER + 0x12)
#define ID_TEXT_3        (GUI_ID_USER + 0x13)
#define ID_TEXT_4        (GUI_ID_USER + 0x14)
#define ID_TEXT_5        (GUI_ID_USER + 0x15)
#define ID_TEXT_6        (GUI_ID_USER + 0x16)
#define ID_TEXT_7        (GUI_ID_USER + 0x17)
#define ID_TEXT_8        (GUI_ID_USER + 0x18)


BAT_Par_t BAT_Par;
Sys_Status_t Sys_Status;
WM_HWIN  hWinBattery = WM_HWIN_NULL;
TaskHandle_t OS_BATTERY_task;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0,  0,   0, 854, 430, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, "���/�Ͽ�",   ID_BUTTON_0,  734, 275, 100, 50, 0,  0x0, 0 },
	{ BUTTON_CreateIndirect, "�˳�",   		ID_BUTTON_1,  734, 330, 100, 50, 0,  0x0, 0 },
	{ TEXT_CreateIndirect,   "��ѹ",        ID_TEXT_0, 630,  30, 64, 32, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,   "����",		ID_TEXT_1, 630, 140, 64, 32, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,   "��ǰ����",	ID_TEXT_2, 45,  280 ,32*4, 32, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,   "ʣ�����", 	ID_TEXT_3, 220, 280, 32*4, 32, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,   "�¶�1", 		ID_TEXT_4, 385, 280, 32*2+16, 32, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,   "�¶�2", 		ID_TEXT_5, 495, 280, 32*2+16, 32, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,   "״̬", 	 	ID_TEXT_6, 640, 280, 32*2,    32, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,   "ͨѶ�Ͽ�", 	ID_TEXT_7, 640, 314, 32*2,    64, 0, 0x64, 0 }, //Ӳ������ ϵͳ��ѹ ϵͳ��ѹ �ŵ��· �ŵ����
	{ TEXT_CreateIndirect,   "", 	       ID_TEXT_8, 640, 280, 32*2,    32, 0, 0x64, 0 }, //�رմ�����
};

//��ʾһ�ŵ�صĵ���ͼ��
//x y ͼ�����Ͻ�����
//xlen ylen ������ �� & ��
//volte ��ص�ѹ 0-9.9V
//Per ��ص����ٷֱ�
void ShowBat_Rect(unsigned int x,unsigned int y,unsigned int xlen,unsigned int ylen,float volte,float Per,unsigned int cell)
{
	unsigned int height,R=5;
	//�������ĸ߶�
	if (Per > 1.0f) {
		height=(ylen - 10) * 1.0f;
	} else {
		height=(ylen - 10) * Per;
	}

	//����ر߿�
	GUI_SetColor(GUI_WHITE);GUI_SetBkColor(GUI_BLACK);
	GUI_DrawRoundedRect(x-1, y-1, x+xlen+1, y+ylen+1, R);
	GUI_DrawRoundedRect(x, y, x+xlen, y+ylen, R);

	//������ڲ�
	if(Per < 0.1f) {
		GUI_SetColor(GUI_RED);
		GUI_SetBkColor(GUI_BLACK);
	} else {
		GUI_SetColor(GUI_GREEN);
		GUI_SetBkColor(GUI_BLACK);
	}
	GUI_FillRoundedRect(x+5, y+ylen-height-5, x+xlen-5, y+ylen-5, 1);
	//��ʾ��ر�־�·��ľ����ѹ
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_GotoXY(x+(xlen-12*4-6)/2, y+ylen+10);
	GUI_DispFloatFix(volte,4,2);GUI_DispChar('V');

	//��ʾ�ڼ�Ƭ���
	GUI_SetColor(GUI_YELLOW);GUI_SetBkColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_GotoXY(x+(xlen-12*5)/2, y+ylen+35);
	GUI_DispString("CELL");GUI_DispDecMin(cell);

	//��ʾ�����ٷֱ�
	GUI_SetColor(GUI_WHITE);GUI_SetBkColor(GUI_BLACK);
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_GotoXY(x+(xlen-(12*3))/2,y-30);
	GUI_DispDecMin(Per*100.0f);GUI_DispChar('%');
}

//�����ػ�
void RE_PAINT(void)
{
	GUI_SetColor(GUI_GRAY);
	GUI_DrawHLine(260,20,834);
	GUI_DrawVLine(620,30,240);
	GUI_DrawVLine(620,280,380);

	ShowBat_Rect( 20, 40, 80, 150, BAT_Par.cell_vol[0], BAT_Par.cell_vol[0] / 4.20f, 1);
	ShowBat_Rect(120, 40, 80, 150, BAT_Par.cell_vol[1], BAT_Par.cell_vol[1] / 4.20f, 2);
	ShowBat_Rect(220, 40, 80, 150, BAT_Par.cell_vol[2], BAT_Par.cell_vol[2] / 4.20f, 3);
	ShowBat_Rect(320, 40, 80, 150, BAT_Par.cell_vol[3], BAT_Par.cell_vol[3] / 4.20f, 4);
	ShowBat_Rect(420, 40, 80, 150, BAT_Par.cell_vol[4], BAT_Par.cell_vol[4] / 4.20f, 5);
	ShowBat_Rect(520, 40, 80, 150, BAT_Par.cell_vol[5], BAT_Par.cell_vol[5] / 4.20f, 6);
	{
		//��ѹ����
		GUI_SetColor(GUI_GREEN);GUI_SetBkColor(GUI_BLACK);
		GUI_SetFont(GUI_FONT_D48);
		GUI_GotoXY(654,75);GUI_DispFloat(BAT_Par.batter_vol,4);

		GUI_GotoX(GUI_GetDispPosX()+5);GUI_GotoY(GUI_GetDispPosY()+20);
		GUI_SetFont(&GUI_Font32_ASCII);GUI_DispChar('V');

		//��������
		GUI_SetColor(GUI_BLUE);GUI_SetBkColor(GUI_BLACK);
		GUI_SetFont(GUI_FONT_D48);
		GUI_GotoXY(654,185);GUI_DispFloat(BAT_Par.CC,4);

		GUI_GotoX(GUI_GetDispPosX()+5);GUI_GotoY(GUI_GetDispPosY()+20);
		GUI_SetFont(&GUI_Font32_ASCII);GUI_DispChar('A');
	}

	GUI_SetColor(GUI_WHITE);GUI_SetBkColor(GUI_BLACK);
	GUI_SetFont(GUI_FONT_D32);GUI_GotoXY(20,330);
	GUI_DispDecMin(BAT_Par.BAT_All_mAh-BAT_Par.Bat_Used_mAh);
	GUI_GotoX(GUI_GetDispPosX()+5);
	GUI_SetFont(&GUI_FontHZ32);GUI_DispString("mAH");

	GUI_SetFont(GUI_FONT_D32);GUI_GotoXY(250,330);
	GUI_DispDecMin(BAT_Par.Bat_mAh_Per);
	GUI_GotoX(GUI_GetDispPosX()+5);
	GUI_SetFont(&GUI_FontHZ32);GUI_DispChar('%');

	GUI_SetFont(GUI_FONT_D32);GUI_GotoXY(370,330);
	GUI_DispDecMin(BAT_Par.TS1);
	GUI_GotoX(GUI_GetDispPosX()+5);
	GUI_SetFont(&GUI_FontHZ32);GUI_DispString("��");

	GUI_SetFont(GUI_FONT_D32);GUI_GotoXY(495,330);
	GUI_DispDecMin(BAT_Par.TS2);
	GUI_GotoX(GUI_GetDispPosX()+5);
	GUI_SetFont(&GUI_FontHZ32);GUI_DispString("��");
}

//��ʼ����ش���
void InitDialogBattery(WM_MESSAGE * pMsg)
{
	unsigned int i;
	WM_HWIN hWin = pMsg->hWin;

  //������ʱ��
	WM_CreateTimer(WM_GetClientWindow(hWin), 0, 100, 0);
	//���ڳ�ʼ��
	{
		FRAMEWIN_SetClientColor(hWin,GUI_BLACK);//��������Ϊ��ɫ
		FRAMEWIN_SetTitleHeight(hWin, 32);
		FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
		FRAMEWIN_SetText(hWin, "���ܵ����Ϣ");
		FRAMEWIN_SetFont(hWin, &GUI_FontHZ32);
		//FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);//�رհ�ť
	}
	//�ײ�����
	{
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), &GUI_FontHZ32);
		BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), "���");

		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), &GUI_FontHZ32);
		BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), "�˳�");

	}
	//�ı���ʼ��
	{
		for(i=ID_TEXT_0;i<=ID_TEXT_6;i++)
		{
			TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, i), &GUI_FontHZ32);
			TEXT_SetTextAlign(WM_GetDialogItem(pMsg->hWin, i), GUI_TA_LEFT | GUI_TA_VCENTER);
			TEXT_SetBkColor(WM_GetDialogItem(pMsg->hWin, i), GUI_BLACK);
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, i), GUI_GRAY);
		}
		//״̬�ı�
		TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), &GUI_FontHZ32);
		TEXT_SetTextAlign(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_TA_HCENTER | GUI_TA_VCENTER);
		TEXT_SetWrapMode(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),GUI_WRAPMODE_WORD);
		TEXT_SetBkColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_BLACK);
		TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_RED);
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"ͨѶ�Ͽ�");
	}
}
//���״̬�����
void Stat_update(WM_MESSAGE * pMsg)
{
	static unsigned int times=0;
	times+=1;if(times>10)times=0;
	if((BAT_Par.BAT_STATE == 0x02) && (RF_lose == false))
	{
		if((BAT_Par.ErrorStatus&0x3F)==0)//û����
		{
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_GREEN);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"ͨѶ����");
		}
		else if(BAT_Par.ErrorStatus&0x20)//Ӳ������
		{
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_RED);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"Ӳ������");
		}
		else if(BAT_Par.ErrorStatus&0x10)//Ӳ������
		{
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_YELLOW);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"��������");
		}
		else if(BAT_Par.ErrorStatus&0x08)//��ѹ����
		{
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_YELLOW);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"��ѹ����");
		}
		else if(BAT_Par.ErrorStatus&0x04)//��ѹ����
		{
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_YELLOW);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"��ѹ����");
		}
		else if(BAT_Par.ErrorStatus&0x02)//��ѹ����
		{
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_YELLOW);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"��·����");
		}
		else if(BAT_Par.ErrorStatus&0x01)//��ѹ����
		{
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_YELLOW);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"��������");
		}
	}
	else
	{
		if(times < 5) {
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_RED);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"ͨѶ�Ͽ�");
		} else {
			TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), GUI_RED);
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),"");
		}
	}
}
//��ش��ڻص�����
static void _cbDialog(WM_MESSAGE * pMsg)
{
	int     NCode;
	int     Id;
	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG: {
			InitDialogBattery(pMsg);
		} break;

		case WM_PAINT: {
			RE_PAINT();
		} break;

		case WM_TIMER: { //��ʱ������״̬��
				Stat_update(pMsg);
				WM_RestartTimer(pMsg->Data.v, 100);
		} break;

		case WM_NOTIFY_PARENT: {
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id) {
				case ID_BUTTON_0: {
					//δ�������״̬
					if(NCode == WM_NOTIFICATION_RELEASED) {
						if(BAT_Par.BAT_STATE == 0x00)
						{
							BAT_Par.BAT_STATE = 0x01;//����б�־
							BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), "ȡ��");
						}
						else if(BAT_Par.BAT_STATE == 0x01)
						{
							BAT_Par.BAT_STATE = 0x00;//����б�־
							BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), "���");
						}
						else if(BAT_Par.BAT_STATE == 0x02)
						{
							BAT_Par.BAT_STATE = 0x00;//����б�־
							BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), "���");
						}
					}
				} break;

				case ID_BUTTON_1: {
					if(NCode == WM_NOTIFICATION_RELEASED) {
						WM_MESSAGE Message;
						Message.hWinSrc = hWinBattery;
						Message.MsgId   = WM_DELETE;
						Message.Data.v  = 3;
						WM_SendMessage(hWinStatus, &Message);
						vTaskDelete(OS_BATTERY_task); //ɾ������
						//GUI_EndDialog(pMsg->hWin, 0); //ɾ������
						WM_DeleteWindow(hWinBattery);
						hWinBattery = NULL;
					}
				} break;
			}
		} break;

		default: {
			WM_DefaultProc(pMsg);
		} break;
	}
}
//�����ҵ�ͨѶ��Ϣ
void Send_MY_Par(void)
{
	unsigned char CKA = 0, CKB = 0;
	unsigned char i = 0;
	//��ջ�����
	memset(TxBuf, 0, sizeof(TxBuf));
	TxBuf[0] = 0xAA;
	TxBuf[1] = 0xBB;
	TxBuf[2] = RF_Speed;
	TxBuf[3] = RF_channel;
	TxBuf[4] = (unsigned char)(Sys_Status.User_ID>>24);
	TxBuf[5] = (unsigned char)(Sys_Status.User_ID>>16);
	TxBuf[6] = (unsigned char)(Sys_Status.User_ID>>8);
	TxBuf[7] = (unsigned char)(Sys_Status.User_ID);
	TxBuf[8] = (unsigned char)(Sys_Status.User_ID&0xAA);
	for(i = 0; i < 30; i += 1) {
		CKA = CKA + TxBuf[i];
		CKB = CKB + CKA;
	}
	TxBuf[30] = CKA;
	TxBuf[31] = CKB;
	nRF24L01_TxPacket(TxBuf);
}

//��ؽ�������
void Battery_Task(void *pvParameters)
{
	unsigned int turn=0;
	TickType_t xLastWakeTime= xTaskGetTickCount();
	while(1)
	{
		turn += 1;if(turn > 10) {turn = 0;}
		if(BAT_Par.BAT_STATE == 0x01) { //���״̬
			if(turn == 0) {
				TX_ADDRESS[0] = 0x19;
				TX_ADDRESS[1] = 0x95;
				TX_ADDRESS[2] = 0x08;
				TX_ADDRESS[3] = 0x18;
				TX_ADDRESS[4] = 0x00;
				NRF24L01_Init(TX);//���÷���ģʽ
				Send_MY_Par();    //�����ҵı�����Ϣ
			} else if(turn == 1) {
				RX_ADDRESS[0] = Sys_Status.User_ID>>24;
				RX_ADDRESS[1] = Sys_Status.User_ID>>16;
				RX_ADDRESS[2] = Sys_Status.User_ID>>8;
				RX_ADDRESS[3] = Sys_Status.User_ID;
				RX_ADDRESS[4] = Sys_Status.User_ID&0xAA;
				NRF24L01_Init(RX);//���ý���ģʽ
			} else {
				if(nRF24L01_RxPacket(RxBuf)) {
					BAT_Par.BAT_STATE = 0x02;
					BUTTON_SetText(WM_GetDialogItem(hWinBattery, ID_BUTTON_0), "�Ͽ�");
					PRINTF("NRF24L01 connect OK %d \r\n" ,RxBuf[0]);
				}
			}
		} else if(BAT_Par.BAT_STATE == 0x02) { //�����ɽ������
			if(nRF24L01_RxPacket(RxBuf)) {
				//״̬�Ĵ���
				BAT_Par.ErrorStatus=RxBuf[4];
				//��Ƭ��ص�ѹ
				BAT_Par.cell_vol[0]=(float)(((unsigned short)RxBuf[5]<<8)|(unsigned short)RxBuf[6])/1000.0f;
				BAT_Par.cell_vol[1]=(float)(((unsigned short)RxBuf[7]<<8)|(unsigned short)RxBuf[8])/1000.0f;
				BAT_Par.cell_vol[2]=(float)(((unsigned short)RxBuf[9]<<8)|(unsigned short)RxBuf[10])/1000.0f;
				BAT_Par.cell_vol[3]=(float)(((unsigned short)RxBuf[11]<<8)|(unsigned short)RxBuf[12])/1000.0f;
				BAT_Par.cell_vol[4]=(float)(((unsigned short)RxBuf[13]<<8)|(unsigned short)RxBuf[14])/1000.0f;
				BAT_Par.cell_vol[5]=(float)(((unsigned short)RxBuf[15]<<8)|(unsigned short)RxBuf[16])/1000.0f;
				//����ܵ�ѹ
				BAT_Par.batter_vol=(float)((unsigned int)RxBuf[17]<<16|(unsigned int)RxBuf[18]<<8|(unsigned int)RxBuf[19])/1000.0f;
				//NTC�¶�
				BAT_Par.TS2=(float)((char)RxBuf[20]);
				BAT_Par.TS1=(float)((char)RxBuf[21]);
				//����
				BAT_Par.CC=(float)(short)((unsigned short)RxBuf[22]<<8|(unsigned short)RxBuf[23])/100.0f;
				//������
				BAT_Par.BAT_All_mAh=(float)((int)((unsigned int)RxBuf[24]<<16|(unsigned int)RxBuf[25]<<8|(unsigned int)RxBuf[26]))/100.0f;
				//��ʹ������
				BAT_Par.Bat_Used_mAh=(float)((int)((unsigned int)RxBuf[27]<<16|(unsigned int)RxBuf[28]<<8|(unsigned int)RxBuf[29]))/100.0f;
				//���ʣ������
				BAT_Par.Bat_mAh_Per=(float)((short)((unsigned short)RxBuf[30]<<8|(unsigned short)RxBuf[31]))/100.0f;

				WM_Paint(hWinBattery);//���´���
			}

			//ÿ��һ���ʼ������ģʽ  ��ֹ�Ͽ�
			if(turn == 0) {
				RX_ADDRESS[0]=Sys_Status.User_ID>>24;
				RX_ADDRESS[1]=Sys_Status.User_ID>>16;
				RX_ADDRESS[2]=Sys_Status.User_ID>>8;
				RX_ADDRESS[3]=Sys_Status.User_ID;
				RX_ADDRESS[4]=Sys_Status.User_ID&0xAA;
				NRF24L01_Init(RX);
			}
		}
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/10);
	}
}

//��ʼ�����ܵ�ؽṹ�����
void BatterySrtuctInit(void)
{
	memset(&BAT_Par, 0, sizeof(BAT_Par));
	// ð��CPU ID
	Sys_Status.User_ID = 0x12345678;
}

void App_SmartBattery(WM_HWIN hWin, uint32_t idenx)
{
	WM_MESSAGE Message;
	if(hWinBattery == NULL) {
		//��սṹ�����
		BatterySrtuctInit();

		//�����Ի��� ���ضԻ�����
		hWinBattery = GUI_CreateDialogBox(_aDialogCreate,
								GUI_COUNTOF(_aDialogCreate),
								_cbDialog, hWin,
								0,
								0);

		//�������ܵ���շ�����
		xTaskCreate(Battery_Task,"Battery_Task",1024,NULL,2,&OS_BATTERY_task);

		Message.hWinSrc = hWinBattery;
		Message.MsgId   = WM_CREATE;
		Message.Data.v  = idenx;
		WM_SendMessage(hWinStatus, &Message);

		PRINTF("App_SmartBattery idenx = %d  src= 0x%x \r\n", idenx, hWinBattery);
	} else {
		WM_ShowWindow(hWinBattery);
	}
}





/*************************** End of file ****************************/
