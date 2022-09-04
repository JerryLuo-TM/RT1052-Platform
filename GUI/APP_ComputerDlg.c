#include "include.h"
#include "MainTask.h"

#include "fsl_sd.h"
#include "fsl_sd_disk.h"

#include "ff.h"
#include "diskio.h"

WM_HWIN  hWinComputer = WM_HWIN_NULL;  	/* 2401窗口句柄 */

uint64_t ullSdUnusedCapacity;
uint64_t ullSdCapacity;

uint64_t ullNANDUnusedCapacity;
uint64_t ullNANDCapacity;

#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_TEXT_0 	    (GUI_ID_USER + 0x01)
#define ID_TEXT_1 	    (GUI_ID_USER + 0x02)
#define ID_TEXT_2 	    (GUI_ID_USER + 0x03)
#define ID_TEXT_3 	    (GUI_ID_USER + 0x04)
#define ID_TEXT_4 	    (GUI_ID_USER + 0x05)
#define ID_TEXT_5 	    (GUI_ID_USER + 0x06)
#define ID_PROGBAR_0    (GUI_ID_USER + 0x07)
#define ID_PROGBAR_1    (GUI_ID_USER + 0x08)
#define ID_PROGBAR_2    (GUI_ID_USER + 0x09)

#define ID_BUTTON_0     (GUI_ID_USER + 0x0A)


static const GUI_WIDGET_CREATE_INFO _aDialogCreateComputer[] = {
	{ FRAMEWIN_CreateIndirect,	"Framewin",   ID_FRAMEWIN_0, 0, 0, 854, 430, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,		"本地磁盘",    ID_TEXT_0,  3,   90, 400,24, 0,0},
	{ TEXT_CreateIndirect,		"移动磁盘",    ID_TEXT_1,  3,  226, 400,24, 0,0},

	{ PROGBAR_CreateIndirect,    NULL,		ID_PROGBAR_0,    40, 155+2, 250, 25, 0, 0},
	{ TEXT_CreateIndirect,       " ",		ID_TEXT_2,       40, 190+2, 300, 24, 0, 0},

	{ PROGBAR_CreateIndirect,    NULL,		ID_PROGBAR_1,    400, 155+2, 250, 25, 0, 0},
	{ TEXT_CreateIndirect,       " ",		ID_TEXT_3,       400, 190+2, 300, 24, 0, 0},

	{ TEXT_CreateIndirect,       "SD:",		ID_TEXT_4,  40,  130+2, 200, 16, 0, 0},
	{ TEXT_CreateIndirect,       "NAND:",	ID_TEXT_5,  400, 130+2, 200, 16, 0, 0},

	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 707, 320, 80, 50, 0, 0x0, 0 },
};

//我的电脑容量信息刷新
static void Refresh_SD(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	FATFS *fs;
	DWORD fre_clust, fre_sect, tot_sect;
	FRESULT res;
	char buf[50];

	/* 初始化SD卡部分 */
	res = f_getfree("SD:", &fre_clust, &fs);
	if(res == 0) {
		tot_sect = (fs->n_fatent - 2) * fs->csize;		//所有扇区
		fre_sect = fre_clust * fs->csize;				//空余扇区
		ullSdCapacity = tot_sect>>1;
		ullSdUnusedCapacity = fre_sect>>1;
	} else {
		ullSdCapacity = 0;
		ullSdUnusedCapacity = 0;
	}

	PROGBAR_SetFont(WM_GetDialogItem(hWin, ID_PROGBAR_0), &GUI_Font13_1);
	PROGBAR_SetMinMax(WM_GetDialogItem(hWin, ID_PROGBAR_0), 0, ullSdCapacity>>10);
	PROGBAR_SetValue(WM_GetDialogItem(hWin, ID_PROGBAR_0), (ullSdCapacity - ullSdUnusedCapacity)>>10);

	TEXT_SetTextColor(WM_GetDialogItem(hWin, ID_TEXT_2), 0x646f71);
	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_2), &GUI_FontHZ16);
	sprintf(buf, "已用:%lldMB, 容量:%lldMB", (ullSdCapacity - ullSdUnusedCapacity)>>10, ullSdCapacity>>10);
	TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_2), buf);

	/* 初始化NAND部分 */
	ullNANDCapacity=0;//spifi_obj.devSize;
	ullNANDUnusedCapacity=0;//spifi_obj.devSize;

	PROGBAR_SetFont(WM_GetDialogItem(hWin, ID_PROGBAR_1),&GUI_Font13_1);
	PROGBAR_SetMinMax(WM_GetDialogItem(hWin, ID_PROGBAR_1), 0, ullNANDCapacity>>10);
	PROGBAR_SetValue(WM_GetDialogItem(hWin, ID_PROGBAR_1), (ullNANDCapacity - ullNANDUnusedCapacity)>>10);

	TEXT_SetTextColor(WM_GetDialogItem(hWin, ID_TEXT_3), 0x646f71);
	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_3), &GUI_FontHZ16);
	sprintf(buf, "已用:%lldKByte, 容量:%lldKByte", (ullNANDCapacity - ullNANDUnusedCapacity)>>10, ullNANDCapacity>>10);
	TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_3), buf);
}
//我的电脑窗口【初始化】
static void InitDialogComputer(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	FATFS *fs;
	DWORD fre_clust, fre_sect, tot_sect;
	FRESULT res;
	char buf[50];

	//初始化窗口标题
	FRAMEWIN_SetTitleHeight(hWin, 32);
	FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
	FRAMEWIN_SetFont(hWin, &GUI_FontHZ24);
	FRAMEWIN_SetText(hWin, "我的电脑");
	//设置窗口关闭按钮 下面三个是系统自带的不好用
	//FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
	FRAMEWIN_SetMoveable(hWin, 0);  //窗口可移动 0
	FRAMEWIN_SetResizeable(hWin,0); //窗口可缩放 0

	BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0),&GUI_FontHZ24);
	BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), "退出");

	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_0), &GUI_FontHZ24);
	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_1), &GUI_FontHZ24);
	TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), "本地磁盘");
	TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), "可移动磁盘");

	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_4), &GUI_FontHZ16);
	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_5), &GUI_FontHZ16);
	TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), "内存卡(SD:)");
	TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), "FLASH(NAND:)");

	/* 初始化SD卡部分 */
	res=f_getfree("2:",&fre_clust,&fs);
	if(res==0)
	{
		tot_sect = (fs->n_fatent - 2) * fs->csize; //所有扇区
		fre_sect = fre_clust * fs->csize;					 //空余扇区
		ullSdCapacity=tot_sect>>1;
		ullSdUnusedCapacity=fre_sect>>1;
	}else
	{
		ullSdCapacity = 0;
		ullSdUnusedCapacity = 0;
	}

	PROGBAR_SetFont(WM_GetDialogItem(hWin, ID_PROGBAR_0), &GUI_Font13_1);
	PROGBAR_SetMinMax(WM_GetDialogItem(hWin, ID_PROGBAR_0), 0, ullSdCapacity>>10);
	PROGBAR_SetValue(WM_GetDialogItem(hWin, ID_PROGBAR_0), (ullSdCapacity - ullSdUnusedCapacity)>>10);

	TEXT_SetTextColor(WM_GetDialogItem(hWin, ID_TEXT_2), 0x646f71);
	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_2), &GUI_FontHZ16);
	sprintf(buf, "已用:%lldMB, 容量:%lldMB", (ullSdCapacity - ullSdUnusedCapacity)>>10, ullSdCapacity>>10);
	TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_2), buf);

	/* 初始化NAND部分 */
	ullNANDCapacity=0;//spifi_obj.devSize;
	ullNANDUnusedCapacity=0;//spifi_obj.devSize;

	PROGBAR_SetFont(WM_GetDialogItem(hWin, ID_PROGBAR_1),&GUI_Font13_1);
	PROGBAR_SetMinMax(WM_GetDialogItem(hWin, ID_PROGBAR_1), 0, ullNANDCapacity>>10);
	PROGBAR_SetValue(WM_GetDialogItem(hWin, ID_PROGBAR_1), (ullNANDCapacity - ullNANDUnusedCapacity)>>10);

	TEXT_SetTextColor(WM_GetDialogItem(hWin, ID_TEXT_3), 0x646f71);
	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_3), &GUI_FontHZ16);
	sprintf(buf, "已用:%lldKByte, 容量:%lldKByte", (ullNANDCapacity - ullNANDUnusedCapacity)>>10, ullNANDCapacity>>10);
	TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_3), buf);
}

//我的电脑窗口【回调函数】
static void _cbCallbackComputer(WM_MESSAGE * pMsg)
{
    int     NCode;
	int     Id;
    switch (pMsg->MsgId)
    {
		case WM_PRE_PAINT: {
			GUI_MULTIBUF_Begin();
		} break;

		case WM_POST_PAINT: {
			GUI_MULTIBUF_End();
		} break;

		case WM_PAINT: {
			GUI_DrawGradientV(0,			   /* 左上角X 位置 */
								0,			           /* 左上角Y 位置 */
								853,	   	         /* 右下角X 位置 */
								479,  	   	       /* 右下角Y 位置 */
								GUI_WHITE,	       /* 矩形最左侧要绘制的颜色 */
								GUI_LIGHTBLUE);    /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(0,			   /* 左上角X 位置 */
								117,			   			 /* 左上角Y 位置 */
								650,	             /* 右下角X 位置 */
								118,               /* 右下角Y 位置 */
								GUI_RED,	         /* 矩形最左侧要绘制的颜色 */
								GUI_YELLOW);       /* 矩形最右侧要绘制的颜色 */

			GUI_DrawGradientH(0,			   /* 左上角X 位置 */
								253,			   			 /* 左上角Y 位置 */
								650,	             /* 右下角X 位置 */
								254,               /* 右下角Y 位置 */
								GUI_RED,	         /* 矩形最左侧要绘制的颜色 */
								GUI_YELLOW);       /* 矩形最右侧要绘制的颜色 */
		} break;

        case WM_INIT_DIALOG: {
			InitDialogComputer(pMsg);
		} break;

		case MSG_Computer_SD: {
			Refresh_SD(pMsg);
		} break;

        case WM_NOTIFY_PARENT: {
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) {
				case ID_BUTTON_0: //关闭
					if(NCode == WM_NOTIFICATION_RELEASED) {
						WM_MESSAGE Message;
						Message.hWinSrc = hWinComputer;
						Message.MsgId   = WM_DELETE;
						Message.Data.v  = 0;
						WM_SendMessage(hWinStatus, &Message);
						// GUI_EndDialog(pMsg->hWin, 0); //删除窗口
						WM_DeleteWindow(hWinComputer);
						hWinComputer = NULL;
					}
				break;
			}
		} break;

        default: {
			WM_DefaultProc(pMsg);
		} break;
    }
}


//创建我的电脑窗口
void App_Computer(WM_HWIN hWin, uint32_t idenx)
{
	WM_MESSAGE Message;
	if(hWinComputer == NULL) {
		hWinComputer = GUI_CreateDialogBox(_aDialogCreateComputer, 
								GUI_COUNTOF(_aDialogCreateComputer), 
								_cbCallbackComputer, 
								hWin, 
								0, 
								0);

		Message.hWinSrc = hWinComputer;
		Message.MsgId   = WM_CREATE;
		Message.Data.v  = idenx;
		WM_SendMessage(hWinStatus, &Message);

		PRINTF("App_Computer idenx = %d  src= 0x%x \r\n", idenx, hWinComputer);
	} else {
		WM_ShowWindow(hWinComputer); 
	}
}
