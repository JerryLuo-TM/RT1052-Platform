#include "fsl_debug_console.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"

#include "ff.h"
#include "diskio.h"

#include "include.h"
#include "touch.h"
#include "malloc.h"
#include "MainTask.h"

#include "HZfontupdata.h"
#include "emwin_support.h"
#include "EmWinHZFont.h"

// #include "system_MIMXRT1052.h"
#include "clock_config.h"
#include "fsl_clock.h"

#define RECOMMENDED_MEMORY (1024L * 2000)

extern uint32_t SystemCoreClock;

uint8_t *p_BkpicFile = NULL; //����ͼƬ����ָ��
GUI_MEMDEV_Handle   hMempic;
WM_HWIN  hIcon_Desk; //����ͼ��С���߾��
WM_HWIN  hIcon_Task; //����ͼ��С���߾��

BITMAP_ITEM _aBitmapItem[] =
{
	{NULL,     "Camera",     "�ҵĵ���" },
	{NULL,     "Clock",      "ʱ��"     },
	{NULL,     "File",       "�ļ�����" },
	{NULL,     "iBook",      "���ܵ��" },
	{NULL,     "Messages",   "2.4G����" },
	{NULL,     "Music",      "����"     },

	{NULL,      "Photo",      "��Ƭ"   },
	{NULL,      "Set",        "����"   },
	{NULL,      "Video",      "��Ƶ"   },
	{NULL,      "Calc",		  "������" },
};

BITMAP_ITEM _aBitmapItem_min[] =
{
	{NULL,     "Camera",     "�ҵĵ���" },
	{NULL,     "Clock",      "ʱ��"     },
	{NULL,     "File",       "�ļ�����" },
	{NULL,     "iBook",      "���ܵ��" },
	{NULL,     "Messages",   "2.4G����" },
	{NULL,     "Music",      "����"     },

	{NULL,      "Photo",      "��Ƭ"   },
	{NULL,      "Set",        "����"   },
	{NULL,      "Video",      "��Ƶ"   },
	{NULL,      "Calc",		  "������" },
};

static void (* _apModules0[])(WM_HWIN hWin, uint32_t idenx) =
{
	App_Computer,
	App_Calendar,
	App_File,
	App_SmartBattery,
	App_Wireless,
	App_WIFIDLG,

	App_JOYDLG,
	APP_PMUDLG,
	App_WIFIDLG,
	App_WIFIDLG,
	App_WIFIDLG,
};


WM_HWIN Query_taskIcon_handle(uint8_t idenx)
{
	return NULL;
}
/*
*********************************************************************************************************
*	�� �� ��: Load_Picture
*	����˵��: ���ڴ濨�������汳��
*	��    ��:
*	�� �� ֵ: 0�ɹ�  1ʧ��
*********************************************************************************************************
*/
int Load_Picture(void)
{
	FIL filex;
	FILINFO filestat;
	FRESULT result;
	unsigned int bw;

	//���������ֽ
	PRINTF("\r\n... Load BK Picture ...\r\n");
	//����ͼƬ��Ϣ
	result = f_stat("2:HeziOS/BK1.bmp", &filestat);
	if(result != FR_OK) {
		return -1; //��ȡ�ļ�״̬����
	}
	PRINTF("Read file state=%d size=%d OK ! \r\n",result,filestat.fsize);

	p_BkpicFile = (unsigned char*)mymalloc(SRAMEXC, filestat.fsize);
	if(p_BkpicFile == NULL) {
		myfree(SRAMEXC, p_BkpicFile);
		return -1; //���붯̬�ڴ�ʧ��
	}
	PRINTF("Apply Memory address=0x%x  OK ! \r\n",p_BkpicFile);

	result = f_open(&filex, "2:HeziOS/BK1.bmp", FA_READ | FA_OPEN_ALWAYS);
	if(result != FR_OK) {
		f_close(&filex);
		return -1;
	}
	PRINTF("Open file result=%d OK ! \r\n",result);

	do{
		result = f_read (&filex, (void*)p_BkpicFile, filestat.fsize, &bw);
	}while(bw < filestat.fsize);//�ȴ���ȡ���
	PRINTF("Read file to Point OK ! \r\n");

	f_close(&filex);//�ر��ļ�
	PRINTF("Close file OK ! \r\n");

	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: Load_Icon
*	����˵��: ���ڴ濨��������ͼ��
*	��    ��:
*	�� �� ֵ: 0�ɹ�  1ʧ��
*********************************************************************************************************
*/
#if defined(SD_LOAD_MODE)
int Load_Icon(void)
{
	unsigned int  i;
	unsigned int  bw;
	unsigned char tmp;
	unsigned char *Point;			//�ļ��ڴ�ָ��
	char fil_addr[] = "2:Icon/";	//����ͼ������Ŀ¼
	char fil_type[] = ".bmp";		//��׺����
	char name_buf[30];				//����·��������

	FIL     filex;
	FRESULT result;
	FILINFO filestat;
	GUI_BITMAP *Bitmap;

	PRINTF("\r\n... Load Desk Icon ...\r\n");
	//��������ͼ��
	//��һҳͼƬ
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++)
	{
		memset(name_buf,NULL,sizeof(name_buf));						//���·��������
		strcat((char*)name_buf,fil_addr);   							//����·��
		strcat((char*)name_buf,_aBitmapItem[i].pTextEn); //�����ļ���
		strcat((char*)name_buf,fil_type);   							//�����ļ�����

		result=f_stat((const TCHAR*)name_buf,&filestat);
		PRINTF("Read file state=%d size=%d OK ! \r\n",result,filestat.fsize);

		Point=(unsigned char*)mymalloc(SRAMEXC,filestat.fsize);
		if(Point==NULL){myfree(SRAMEXC,Point);PRINTF("Icon malloc fail \r\n");continue;}//���붯̬�ڴ�ʧ��
		PRINTF("Apply %s Memory address=0x%x  OK ! \r\n",name_buf,Point);

		result=f_open(&filex,(const TCHAR*)name_buf,FA_READ);
		if(result!=FR_OK){f_close(&filex);continue;}
		PRINTF("Open file result=%d OK ! \r\n",result);

		bw=0;
		do{
		result = f_read (&filex,(void*)Point,filestat.fsize,&bw);
		}while(bw<filestat.fsize);//�ȴ���ȡ���
		PRINTF("Read file to Point OK ! \r\n");

		f_close(&filex);//�ر��ļ�
		PRINTF("Close file OK ! \r\n");

		//bmp real data offset = Point+10 ������
		for(bw = *(uint32_t *)(Point+10); bw < filestat.fsize ; bw += 4)
		{
			*(Point+bw+3)=*(Point+bw+3)^0xff;//��λ�ֽ� ȡ���
			tmp=*(Point+bw);				 //�������ֽں͵������ֽ�����
			*(Point+bw)=*(Point+bw+2);
			*(Point+bw+2)=tmp;
		}

		_aBitmapItem[i].pBitmap = (GUI_BITMAP *)mymalloc(SRAMEXC,filestat.fsize);
		if(_aBitmapItem[i].pBitmap==NULL){myfree(SRAMEXC,_aBitmapItem[i].pBitmap);return -1;}//���붯̬�ڴ�ʧ��

		Bitmap = _aBitmapItem[i].pBitmap;
		Bitmap->XSize = GUI_BMP_GetXSize(Point);
		Bitmap->YSize = GUI_BMP_GetYSize(Point);
		Bitmap->BytesPerLine =288;
		Bitmap->BitsPerPixel =32;
		Bitmap->pData =(unsigned char*)(Point+*(uint32_t *)(Point+10));
		Bitmap->pPal = NULL;
		Bitmap->pMethods = GUI_DRAW_BMP888;    //GUI_DRAW_BMP8888
	}
	return 0;
}
#else
int Load_Icon(void)
{
	unsigned int  i;
	unsigned int  bw;
	unsigned char *Point;			//�ļ��ڴ�ָ��
	char fil_addr[] = "2:Icon/";	//����ͼ������Ŀ¼
	char fil_type[] = ".dta";		//��׺����
	char name_buf[48];				//����·��������

	FIL     filex;
	FRESULT result;
	FILINFO filestat;
	GUI_BITMAP *Bitmap;

	PRINTF("\r\n... Load Desk Icon ...\r\n");
	//��������ͼ��
	//��һҳͼƬ
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++) {
		memset(name_buf, NULL, sizeof(name_buf));			//���·��������
		strcat((char*)name_buf, fil_addr);					//����·��
		strcat((char*)name_buf, _aBitmapItem[i].pTextEn);	//�����ļ���
		strcat((char*)name_buf, fil_type);					//�����ļ�����

		result = f_stat((const TCHAR*)name_buf, &filestat);
		PRINTF("f_stat[%d] name:%s size:%d OK \r\n", result, name_buf, filestat.fsize);

		Point = (unsigned char*)mymalloc(SRAMEXC, filestat.fsize);
		PRINTF("mymalloc SRAMEXC 0x%x \r\n", Point);
		if(Point == NULL) {
			myfree(SRAMEXC, Point);
			continue; //���붯̬�ڴ�ʧ��
		}

		result = f_open(&filex, (const TCHAR*)name_buf, FA_READ);
		PRINTF("f_open[%d] \r\n", result);
		if(result != FR_OK) {
			f_close(&filex);
			continue;
		}

		bw = 0;
		do {
			result = f_read (&filex,(void*)Point, filestat.fsize, &bw);
		} while(bw < filestat.fsize); //�ȴ���ȡ���
		PRINTF("f_read[%d] \r\n", result);

		result = f_close(&filex);//�ر��ļ�
		PRINTF("f_close[%d] \r\n", result);

		_aBitmapItem[i].pBitmap = (GUI_BITMAP *)mymalloc(SRAMEXC, filestat.fsize);
		PRINTF("mymalloc SRAMEXC 0x%x \r\n", _aBitmapItem[i].pBitmap);
		if(_aBitmapItem[i].pBitmap == NULL) {
			myfree(SRAMEXC, _aBitmapItem[i].pBitmap);
			return -1; //���붯̬�ڴ�ʧ��
		}

		Bitmap = _aBitmapItem[i].pBitmap;
		Bitmap->XSize = 72;
		Bitmap->YSize = 72;
		Bitmap->BytesPerLine =288;
		Bitmap->BitsPerPixel =32;
		Bitmap->pData =(unsigned char*)(Point+0x10);
		Bitmap->pPal = NULL;
		Bitmap->pMethods = GUI_DRAW_BMP8888;    //GUI_DRAW_BMP8888
	}
	return 0;
}

int Load_task_Icon(void)
{
	unsigned int  i;
	unsigned int  bw;
	unsigned char *Point;			 //�ļ��ڴ�ָ��
	char fil_addr[] = "2:taskicon/"; //����ͼ������Ŀ¼
	char fil_type[] = ".dta";		 //��׺����
	char name_buf[30];				 //����·��������

	FIL     filex;
	FRESULT result;
	FILINFO filestat;
	GUI_BITMAP *Bitmap;

	PRINTF("\r\n... Load Task Icon ...\r\n");
	//��������ͼ��
	//��һҳͼƬ
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem_min); i++) {
		memset(name_buf, NULL, sizeof(name_buf));				//���·��������
		strcat((char*)name_buf, fil_addr);						//����·��
		strcat((char*)name_buf, _aBitmapItem_min[i].pTextEn);	//�����ļ���
		strcat((char*)name_buf, fil_type);						//�����ļ�����

		result = f_stat((const TCHAR*)name_buf, &filestat);
		PRINTF("f_stat[%d] name:%s size:%d \r\n", result, name_buf, filestat.fsize);

		Point = (unsigned char*)mymalloc(SRAMEXC, filestat.fsize);
		PRINTF("mymalloc SRAMEXC 0x%x \r\n", Point);
		if(Point == NULL) {
			myfree(SRAMEXC,Point);
			continue; //���붯̬�ڴ�ʧ��
		}

		result = f_open(&filex, (const TCHAR*)name_buf, FA_READ);
		PRINTF("f_open[%d] \r\n", result);
		if(result != FR_OK) {
			f_close(&filex);
			continue;
		}

		bw = 0;
		do {
			result = f_read (&filex, (void*)Point, filestat.fsize, &bw);
		}while(bw < filestat.fsize); //�ȴ���ȡ���
		PRINTF("f_read[%d] \r\n", result);

		result = f_close(&filex);//�ر��ļ�
		PRINTF("f_close[%d] \r\n", result);

		_aBitmapItem_min[i].pBitmap = (GUI_BITMAP *)mymalloc(SRAMEXC, filestat.fsize);
		PRINTF("mymalloc SRAMEXC 0x%x \r\n", _aBitmapItem_min[i].pBitmap);
		if(_aBitmapItem_min[i].pBitmap == NULL) {
			myfree(SRAMEXC,_aBitmapItem_min[i].pBitmap);
			return -1; //���붯̬�ڴ�ʧ��
		}

		Bitmap = _aBitmapItem_min[i].pBitmap;
		Bitmap->XSize = 42;
		Bitmap->YSize = 42;
		Bitmap->BytesPerLine =168;
		Bitmap->BitsPerPixel =32;
		Bitmap->pData =(unsigned char*)(Point+0x10);
		Bitmap->pPal = NULL;
		Bitmap->pMethods = GUI_DRAW_BMP8888;    //GUI_DRAW_BMP8888
	}
	return 0;
}

#endif

/*
*********************************************************************************************************
*	�� �� ��: _CreateICONVIEW
*	����˵��: ��������ͼ��С����
*	��    ��:
*	�� �� ֵ: ��
*********************************************************************************************************
*/
WM_HWIN _CreateICONVIEW(void)
{
	WM_HWIN hIcon;
	int i;

	/*��ָ��λ�ô���ָ���ߴ��ICONVIEW С����*/
	hIcon = ICONVIEW_CreateEx(  40,			/* С���ߵ��������أ��ڸ������У�*/
								10,			/* С���ߵ��������أ��ڸ������У�*/
								LCD_GetXSize() - 40,				/* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
								LCD_GetYSize() - 60 -10,			/* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
								WM_HBKWIN,	/* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
								WM_CF_SHOW | WM_CF_HASTRANS,		/* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */
								ICONVIEW_CF_AUTOSCROLLBAR_V,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	 /* Ĭ����0�����������ʵ������������ֱ������ */
								GUI_ID_ICONVIEW0, /* С���ߵĴ���ID */
								100,       /* ͼ���ˮƽ�ߴ� */
								120);      /* ͼ��Ĵ�ֱ�ߴ�,ͼ����ļ������������棬��Ҫ����ICONVIEW�ĸ߶ȣ�����Text��ʾ������*/

	/* ��ICONVIEW С���������ͼ�� */
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++) {
		ICONVIEW_AddBitmapItem(hIcon, _aBitmapItem[i].pBitmap, _aBitmapItem[i].pTextCn);
	}

	ICONVIEW_SetFont(hIcon, &GUI_FontHZ24);

	/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_WHITE | 0x80000000);

	/* ����X����ı߽�ֵΪ0��Ĭ�ϲ���0, Y����Ĭ����0����������Ҳ����һ�����ã������Ժ��޸� */
	ICONVIEW_SetFrame(hIcon, GUI_COORD_X, 0);
	ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 0);

	/* ����ͼ����x ��y �����ϵļ�ࡣ*/
	ICONVIEW_SetSpace(hIcon, GUI_COORD_X, 34);
	ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, 25);

	/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
	ICONVIEW_SetIconAlign(hIcon, ICONVIEW_IA_HCENTER|ICONVIEW_IA_VCENTER);

	//ICONVIEW_SetTextColor(hIcon, ICONVIEW_CI_UNSEL, 0xF020A0);
	return hIcon;
}

WM_HWIN _CreateStatusICONVIEW(void)
{
	WM_HWIN hIcon;
	/*��ָ��λ�ô���ָ���ߴ��ICONVIEW С����*/
	hIcon = ICONVIEW_CreateEx(  85,			/* С���ߵ��������أ��ڸ������У�*/
								0,			/* С���ߵ��������أ��ڸ������У�*/
								600,        /* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
								55, 	    /* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
								hWinStatus,	/* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
								WM_CF_SHOW | WM_CF_HASTRANS, /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */
								ICONVIEW_CF_AUTOSCROLLBAR_V,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	 /* Ĭ����0�����������ʵ������������ֱ������ */
								GUI_ID_ICONVIEW1, /* С���ߵĴ���ID */
								42,       /* ͼ���ˮƽ�ߴ� */
								42);      /* ͼ��Ĵ�ֱ�ߴ�,ͼ����ļ������������棬��Ҫ����ICONVIEW�ĸ߶ȣ�����Text��ʾ������*/

	/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_MAKE_COLOR(0x002E3B42));

	ICONVIEW_SetFrame(hIcon, GUI_COORD_X, 5);
	ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 5);

	ICONVIEW_SetSpace(hIcon, GUI_COORD_X, 5);
	ICONVIEW_SetSpace(hIcon, GUI_COORD_Y, 5);

	/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
	ICONVIEW_SetIconAlign(hIcon, ICONVIEW_IA_HCENTER|ICONVIEW_IA_VCENTER);

	return hIcon;
}

/*
*********************************************************************************************************
*	�� �� ��: _cbBkWindow
*	����˵��: ���洰�ڻص�����
*	��    ��:
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbBkWindow(WM_MESSAGE * pMsg)
{
	int Id, NCode;
	int x, y;
	uint8_t idenx;
	switch(pMsg->MsgId) {
		case WM_PRE_PAINT: {
			//GUI_MULTIBUF_Begin();
		} break;

		case WM_POST_PAINT: {
			//GUI_MULTIBUF_End();
		} break;

		case WM_PAINT: {
			if(p_BkpicFile == NULL) {
				GUI_SetBkColor(GUI_BLACK);
				GUI_Clear(); //��������
			}else{
				// GUI_BMP_Draw((void*)p_BkpicFile, 0, 0);
				GUI_MEMDEV_WriteOpaqueAt(hMempic, 0, 0);
			}

			/* �Ƚ�3��6x6ԲȦ��ʾ���� */
			GUI_SetColor(GUI_WHITE);
			x = LCD_GetXSize()/2 - 10 / 2 - 30;
			y = LCD_GetYSize()/1 - 10 / 2 - 80;

			GUI_SetColor(GUI_WHITE);
			GUI_FillCircle( x,      y , 5 );
			GUI_FillCircle( x + 30, y , 5 );
			GUI_FillCircle( x + 60, y , 5 );
			GUI_FillCircle( x, y, 8);//ѡ���һ����  Ŀǰ��֧�ֻ�������
		} break;

		case WM_NOTIFY_PARENT: {
			Id   = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id) {
				case GUI_ID_ICONVIEW0: {
					if(NCode == WM_NOTIFICATION_RELEASED) {
						idenx = ICONVIEW_GetSel(pMsg->hWinSrc); //��ȡѡ�б��ͼ��
						ICONVIEW_SetSel(hIcon_Desk, -1); //��ѡ��״̬���
						if(idenx < GUI_COUNTOF(_aBitmapItem)) {
							if(idenx < 8) {
								_apModules0[idenx](WM_HBKWIN, idenx);
							}
						}
					}
				} break;
			}
		} break;

		case WM_CREATE: {
			GUI_AA_SetFactor(3);
		} break;

		default: {
			WM_DefaultProc(pMsg);
		} break;
	}
}

void GUI_MainTask(void)
{
	unsigned int  xpost, counts;
	unsigned char buf[128];

	GUI_Init();
	WM_MULTIBUF_Enable(1);	//�򿪶໺��
	GUI_CURSOR_Show();		//��ʾ���
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_RED);
	GUI_Clear();			//��������

	if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
		PRINTF("Not enough memory available Free:%d !\r\n",GUI_ALLOC_GetNumUsedBytes());
		return;
	}
	PRINTF("memory used:%d free:%d \r\n", GUI_ALLOC_GetNumUsedBytes(), GUI_ALLOC_GetNumFreeBytes());
	PRINTF("GUI demo start.\r\n");

	/*******************************************************************************/
	/* ����Ƥ�� *************************************************************/
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);

	//�ڲ����ڴ濨�ڼ��ӡ�����Ϣ��
	//GUI_DispNextLine(); ����
	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_VCENTER);		//�����ı����뷽ʽ ����� ��ֱ����
	GUI_SetTextStyle(GUI_TS_NORMAL);					//�����ı�����ģʽ ����
	GUI_SetFont(GUI_FONT_24B_ASCII);					//��������
	GUI_SetColor(GUI_LIGHTGREEN);						//����������ɫ
	GUI_GotoXY(0, 12);									//���ó�ʼ�ı�����

	//������X�����  ���ѡ����ˮƽ����  ��һ���ַ���һ�����ʾ��X�������
	GUI_DispString("Feiling Embedded OK1050-C \r\n");
	GUI_DispString("i.MX RT1050 @Arm Cortex-M7 \r\n");
	GUI_DispString("Copyright (C) 2019-2029 Author:Luo Jian \r\n");
	GUI_DispString("SRAM:512KB SDRAM:32MB SPI FLASH:16MB \r\n");
	GUI_DispString("Core:V1.2  Driver:V1.0  Software:V1.0 \r\n");
	GUI_DispString("System:FreeRTOS Kernel V10.0.1 \r\n");
	sprintf((char*)buf, "kCLOCK_CpuClk: %d MHz   SystemCoreClock: %d MHz \r\n", CLOCK_GetFreq(kCLOCK_CpuClk)/1000000UL, SystemCoreClock/1000000UL);
	GUI_DispString((char*)buf);

	//��ӡEMWIN�汾��Ϣ
	GUI_DispString("UI Interface Version(emWin):");
	GUI_DispString(GUI_GetVersionString());
	GUI_DispNextLine();
	//��ӡϵͳӲ����ʼ����Ϣ
	//���� FT5316
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("FT5316 Touch Init...  ");
	if(Touch.Init_statue == true) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}
	//��Դ BQ25895
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("BQ25895 Power Init...  ");
	if(BQ25895.Init_statue == true) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}
	//������Ƶ WM8960
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("WM8960 Audio Init...  ");
	if(Audio.Init_statue == true) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}
	//���� NRF24L01
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("NRF24L01 RF Init...  ");
	if(NRF24L01.Init_statue == true) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//�ȴ�SD��׼�����
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Wait SD Card Inserte");
	xpost = GUI_GetDispPosX();
	while(!TF.Card_ready) {
		if(++counts > 10) {
			counts = 0;
			GUI_GotoX(xpost);
			GUI_DispCEOL();//��� '*'
		}
		GUI_DispString(" ~");
		GUI_Delay(100);
	}
	GUI_SetColor(GUI_LIGHTBLUE);
	GUI_DispString("  OK \r\n");
	//��ӡSD����Ϣ
	sprintf((char*)buf, "SDMMC: Clock=%d Mhz Size=%d MB \r\n", g_sd.busClock_Hz/1000000U, g_sd.blockCount/2/1024);
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString((char*)buf);
	//�����ֿ�
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Load Font Into Memory...  ");
	if(emwin_update_font("2:") == kStatus_Success) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//�������汳��ͼƬ
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Load BK Picture Into Memory...  ");
	if(Load_Picture()==kStatus_Success) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//��������ͼ��
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Load Icon Into Memory...  ");
	if(Load_Icon() == kStatus_Success) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//��������ͼ��
	GUI_SetColor(GUI_LIGHTGREEN);
	GUI_DispString("Load Tsak Icon Into Memory...  ");
	if(Load_task_Icon() == kStatus_Success) {
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispString("OK \r\n");
	} else {
		GUI_SetColor(GUI_LIGHTRED);
		GUI_DispString("FAIL \r\n");
	}

	//��������λ�ý���ϵͳ
	GUI_SetColor(GUI_YELLOW);
	GUI_SetFont(&GUI_FontHZ24);
	GUI_DispString("\r\n...�����ֿ������ϣ��봥������λ�ÿ�ʼ...");

	hMempic = GUI_MEMDEV_CreateFixed(	0,
										0,
										LCD_GetXSize(),
										LCD_GetYSize(),
										GUI_MEMDEV_NOTRANS,
										GUI_MEMDEV_APILIST_16,
										GUICC_M565);
	//���ر���ͼƬ
	if(p_BkpicFile == NULL) {
		GUI_SetBkColor(GUI_BLACK);
		GUI_Clear(); //��������
	} else {
		GUI_MEMDEV_Select(hMempic);
		GUI_BMP_Draw((void*)p_BkpicFile, 0, 0);
		GUI_MEMDEV_Select(0);
	}

//	GUI_MULTIBUF_Begin();
//	  GUI_BMP_Draw((void*)p_BkpicFile,0,0);
//	GUI_MULTIBUF_End();

	WM_SetCallback(WM_HBKWIN, _cbBkWindow);

	/* ��1������ͼ�� */
	hIcon_Desk = _CreateICONVIEW(); //100 * 3
	ICONVIEW_SetSel(hIcon_Desk, -1);

	CreateSysInfoDlg();

	CreateSysStatusDlg();

	hIcon_Task = _CreateStatusICONVIEW();

	CreateWindowSKB();
}


