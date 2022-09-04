#include "DIALOG.h"
#include  "stdio.h"
#include  "string.h"
#include  "GUI.h"
#include  "EmWinHZFont.h"

#include "include.h"

/*****************************************************************************/
extern const char *PYSearch(char *strInput_py_str);

/*****************************************************************************/
WM_HWIN    hWindowSKB;                                 //键盘窗口专用句柄

/*****************************************************************************/
unsigned char  WindowSKB_TypeFLAG = 1;                 //键盘输入标志 1,EDIT; 2,MULTIEDIT;

/*****************************************************************************/

#define ID_WINDOW_0  (GUI_ID_USER + 0x00)
#define ID_TEXT_0  (GUI_ID_USER + 0x01)
#define ID_TEXT_1  (GUI_ID_USER + 0x02)
#define ID_TEXT_2  (GUI_ID_USER + 0x03)
#define ID_TEXT_3  (GUI_ID_USER + 0x04)
#define ID_TEXT_4  (GUI_ID_USER + 0x05)
#define ID_TEXT_5  (GUI_ID_USER + 0x06)
#define ID_TEXT_6  (GUI_ID_USER + 0x07)
#define ID_TEXT_7  (GUI_ID_USER + 0x08)
#define ID_TEXT_8  (GUI_ID_USER + 0x09)
#define ID_TEXT_9  (GUI_ID_USER + 0x0A)
#define ID_BUTTON_0  (GUI_ID_USER + 0x0B)
#define ID_BUTTON_1  (GUI_ID_USER + 0x0C)
#define ID_BUTTON_2  (GUI_ID_USER + 0x0D)
#define ID_BUTTON_3  (GUI_ID_USER + 0x0E)
#define ID_BUTTON_4  (GUI_ID_USER + 0x0F)
#define ID_BUTTON_5  (GUI_ID_USER + 0x10)
#define ID_BUTTON_6  (GUI_ID_USER + 0x11)
#define ID_BUTTON_7  (GUI_ID_USER + 0x12)
#define ID_BUTTON_8  (GUI_ID_USER + 0x13)
#define ID_BUTTON_9  (GUI_ID_USER + 0x14)
#define ID_BUTTON_10  (GUI_ID_USER + 0x15)
#define ID_BUTTON_11  (GUI_ID_USER + 0x16)
#define ID_BUTTON_12  (GUI_ID_USER + 0x17)
#define ID_BUTTON_13  (GUI_ID_USER + 0x18)
#define ID_BUTTON_14  (GUI_ID_USER + 0x19)
#define ID_BUTTON_15  (GUI_ID_USER + 0x1A)
#define ID_BUTTON_16  (GUI_ID_USER + 0x1B)
#define ID_BUTTON_17  (GUI_ID_USER + 0x1C)
#define ID_BUTTON_18  (GUI_ID_USER + 0x1D)
#define ID_BUTTON_19  (GUI_ID_USER + 0x1E)
#define ID_BUTTON_20  (GUI_ID_USER + 0x1F)
#define ID_BUTTON_21  (GUI_ID_USER + 0x20)
#define ID_BUTTON_22  (GUI_ID_USER + 0x21)
#define ID_BUTTON_23  (GUI_ID_USER + 0x22)
#define ID_BUTTON_24  (GUI_ID_USER + 0x23)
#define ID_BUTTON_25  (GUI_ID_USER + 0x24)
#define ID_BUTTON_26  (GUI_ID_USER + 0x25)
#define ID_BUTTON_27  (GUI_ID_USER + 0x26)
#define ID_BUTTON_28  (GUI_ID_USER + 0x27)
#define ID_BUTTON_29  (GUI_ID_USER + 0x28)
#define ID_BUTTON_30  (GUI_ID_USER + 0x29)
#define ID_BUTTON_31  (GUI_ID_USER + 0x2A)
#define ID_BUTTON_32  (GUI_ID_USER + 0x2B)
#define ID_BUTTON_33  (GUI_ID_USER + 0x2C)
#define ID_BUTTON_34  (GUI_ID_USER + 0x2D)


/*** 按键添加顺序 1_26键; 2_功能键[<-, ->, Space, Enter, <Back]; 3_切换键[Caps, En]; 4_Esc;***/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 305, 175, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "ping", ID_TEXT_0, 5, 5, 60, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "1", ID_TEXT_1, 70, 5, 25, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "2", ID_TEXT_2, 95, 5, 25, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "3", ID_TEXT_3, 120, 5, 25, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "4", ID_TEXT_4, 145, 5, 25, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "5", ID_TEXT_5, 170, 5, 25, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "6", ID_TEXT_6, 195, 5, 25, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "7", ID_TEXT_7, 220, 5, 25, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "8", ID_TEXT_8, 245, 5, 25, 25, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "9", ID_TEXT_9, 270, 5, 25, 25, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "q", ID_BUTTON_0, 5, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "w", ID_BUTTON_1, 35, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "e", ID_BUTTON_2, 65, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "r", ID_BUTTON_3, 95, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "t", ID_BUTTON_4, 125, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "y", ID_BUTTON_5, 155, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "u", ID_BUTTON_6, 185, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "i", ID_BUTTON_7, 215, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "o", ID_BUTTON_8, 245, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "p", ID_BUTTON_9, 275, 35, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "a", ID_BUTTON_10, 5, 70, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "s", ID_BUTTON_11, 35, 70, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "d", ID_BUTTON_12, 64, 70, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "f", ID_BUTTON_13, 95, 70, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "g", ID_BUTTON_14, 125, 70, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "h", ID_BUTTON_15, 155, 70, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "j", ID_BUTTON_16, 185, 71, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "k", ID_BUTTON_17, 215, 70, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "l", ID_BUTTON_18, 245, 70, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "_", ID_BUTTON_19, 275, 70, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "z", ID_BUTTON_20, 5, 105, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "x", ID_BUTTON_21, 35, 105, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "c", ID_BUTTON_22, 65, 105, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "v", ID_BUTTON_23, 95, 105, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "b", ID_BUTTON_24, 125, 105, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "n", ID_BUTTON_25, 155, 105, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "m", ID_BUTTON_26, 185, 105, 25, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "<-", ID_BUTTON_27, 215, 105, 40, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "->", ID_BUTTON_28, 260, 105, 40, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Space", ID_BUTTON_29, 95, 140, 70, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Enter", ID_BUTTON_30, 215, 140, 40, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "<Back", ID_BUTTON_31, 260, 140, 40, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Caps", ID_BUTTON_32, 5, 140, 40, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "En", ID_BUTTON_33, 170, 140, 40, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Esc", ID_BUTTON_34, 50, 140, 40, 30, 0, 0x0, 0 },
};

static const char keyCAP[] = {
'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '_',
'Z', 'X', 'C', 'V', 'B', 'N', 'M', 0};
static const char keyLOW[] = {
'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '_',
'z', 'x', 'c', 'v', 'b', 'n', 'm', 0};
static const char key123[] = {
'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
'!', '@', '%', '*', '(', ')', '_', '-', '+', '=',
'/', ':', ';', '"', ',', '.', '?', 0};



static void _cbDialog(WM_MESSAGE * pMsg) {
	WM_HWIN hItem;
	int	 NCode;
	int	 ID_Num, Id, i, temp;
	char charBUFF[512], tempBUFF[128];   //获取文本输入缓冲区
	static int *pbuffPY[64];
	static char **pstrPY=(char **)&pbuffPY[0];
	char *pstrTEMP;
	static unsigned char capsKEY = 0, inputTYPE = 0;
	// WM_MESSAGE Message;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
	capsKEY = 0, inputTYPE = 0;

	hItem = pMsg->hWin;
	WINDOW_SetBkColor(hItem, (0x00C0C0C0));

	// Initialization of 'ping'
	TEXT_SetTextAlign(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), GUI_TA_LEFT | GUI_TA_VCENTER);
	TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), "");

	// Initialization of '1'
	for(i=ID_TEXT_1; i<=ID_TEXT_9; i++)
	{
		hItem = WM_GetDialogItem(pMsg->hWin, i);
		TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_FontHZ16);
		TEXT_SetText(hItem, "");
	}

	for(ID_Num = ID_BUTTON_0;ID_Num <= ID_BUTTON_34;ID_Num++)
	{
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_Num), &GUI_Font16_ASCII);  //所有键字体设置
		BUTTON_SetFocussable(WM_GetDialogItem(pMsg->hWin, ID_Num), 0);            //清除所有键的 焦点
	}
	break;

	case WM_NOTIFY_PARENT:
	Id	= WM_GetId(pMsg->hWinSrc);
	NCode = pMsg->Data.v;
	switch(Id) {
	//case ID_TEXT_0:
	//	break;                 //ID_TEXT_0 不参与输入法处理
	case ID_TEXT_1:
	case ID_TEXT_2:
	case ID_TEXT_3:
	case ID_TEXT_4:
	case ID_TEXT_5:
	case ID_TEXT_6:
	case ID_TEXT_7:
	case ID_TEXT_8:
	case ID_TEXT_9:                              /*** TEXT_1 --TEXT_2 与下面的 BUTTON 功能合并 ***/
	case ID_BUTTON_0: // Notifications sent by 'q'
	case ID_BUTTON_1: // Notifications sent by 'w'
	case ID_BUTTON_2: // Notifications sent by 'e'
	case ID_BUTTON_3: // Notifications sent by 'r'
	case ID_BUTTON_4: // Notifications sent by 't'
	case ID_BUTTON_5: // Notifications sent by 'y'
	case ID_BUTTON_6: // Notifications sent by 'u'
	case ID_BUTTON_7: // Notifications sent by 'i'
	case ID_BUTTON_8: // Notifications sent by 'o'
	case ID_BUTTON_9: // Notifications sent by 'p'
	case ID_BUTTON_10: // Notifications sent by 'a'
	case ID_BUTTON_11: // Notifications sent by 's'
	case ID_BUTTON_12: // Notifications sent by 'd'
	case ID_BUTTON_13: // Notifications sent by 'f'
	case ID_BUTTON_14: // Notifications sent by 'g'
	case ID_BUTTON_15: // Notifications sent by 'h'
	case ID_BUTTON_16: // Notifications sent by 'j'
	case ID_BUTTON_17: // Notifications sent by 'k'
	case ID_BUTTON_18: // Notifications sent by 'l'
	case ID_BUTTON_20: // Notifications sent by 'z'
	case ID_BUTTON_21: // Notifications sent by 'x'
	case ID_BUTTON_22: // Notifications sent by 'c'
	case ID_BUTTON_23: // Notifications sent by 'v'
	case ID_BUTTON_24: // Notifications sent by 'b'
	case ID_BUTTON_25: // Notifications sent by 'n'
	case ID_BUTTON_26: // Notifications sent by 'm'              /*** 26键_按键功能合并 :Stop ***/
	case ID_BUTTON_27: // Notifications sent by '<-'            /*** 按键功能合并 ***/
	case ID_BUTTON_28: // Notifications sent by '->'            /*** 按键功能合并 ***/
	case ID_BUTTON_19:   // Notifications sent by '_'          /************************ 此处为多余 **************************/
	case ID_BUTTON_29:   // Notifications sent by 'Space' (' ')
	case ID_BUTTON_30:   // Notifications sent by 'Enter'
	case ID_BUTTON_31:   // Notifications sent by '<Back' (<--)
	case ID_BUTTON_32: // Notifications sent by 'Caps'          /*** 结尾不始用 break;  按键'Caps'   'En' 功能合并 ***/
	case ID_BUTTON_33: // Notifications sent by 'EN'
		switch(NCode) {
		case WM_NOTIFICATION_CLICKED:
			break;
		case WM_NOTIFICATION_RELEASED:
			Id = WM_GetId(pMsg->hWinSrc);                   //获取 操作按键的Id;

/*********************************** ID_BUTTON_0 -- ID_BUTTON_26 功能区 ***********************************/
			if(Id >= ID_BUTTON_0  &&  Id <= ID_BUTTON_26)             //26键 发生了操作
			{
				if(inputTYPE==0 || inputTYPE==1 || capsKEY) //goto gotoASCII; ||  goto gotoSYMBOL;  ||  goto gotoPING;
				{
					BUTTON_GetText(pMsg->hWinSrc, &charBUFF[0], sizeof(charBUFF));
					GUI_StoreKeyMsg(charBUFF[0], 1);	//将按键存储到EMWIN（capsKEY //存储按键到EMWIN并退出）
					GUI_StoreKeyMsg(charBUFF[0], 0);
					break;
				}
				else if(inputTYPE == 2  && capsKEY == 0)       //'Ch'  中文输入
				{
					TEXT_GetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &charBUFF[0], sizeof(charBUFF));
					BUTTON_GetText(pMsg->hWinSrc, &tempBUFF[0], sizeof(tempBUFF));
					strcat(charBUFF, tempBUFF);	//connect string
				}
			}
/*********************************** ID_TEXT_1 -- ID_TEXT_9 显示区 ***********************************/
			if(Id >= ID_TEXT_1  &&  Id <= ID_TEXT_9)              //9 个汉字显示TEXT 发生了操作
			{
				if(inputTYPE == 2  && capsKEY == 0)       //'Ch'  中文输入, 才可以操作
				{
				if(TEXT_GetText(WM_GetDialogItem(pMsg->hWin, Id), &tempBUFF[0], sizeof(tempBUFF)) == 0) break;

				/*** EmWin 各种小工具 汉字文本, 修改接口 ***/
				if(WindowSKB_TypeFLAG == 1)                      /*** EDIT ***/
				{
					EDIT_GetText(hItem=WM_GetFocussedWindow(), &charBUFF[0], sizeof(charBUFF));
					strcat(charBUFF, tempBUFF);
					EDIT_SetText(hItem, &charBUFF[0]);
				}
				else if(WindowSKB_TypeFLAG == 2)                 /*** MULTIEDIT ***/
				{
				  MULTIEDIT_GetText(hItem=WM_GetFocussedWindow(), &charBUFF[0], sizeof(tempBUFF));
					strcat(charBUFF, tempBUFF);
					MULTIEDIT_SetText(hItem, &charBUFF[0]);
				}

				memset(charBUFF, 0, sizeof(charBUFF));                  //汉字上屏之后, 清空输入,并重新更新显示与计算
				}
			}
/*****************************************************************************************************/
/*****************************************************************************************************/
			if(Id == ID_BUTTON_19)  // Notifications sent by '_'    /************************ 此处为多余 **************************/
			{
				BUTTON_GetText(pMsg->hWinSrc, &charBUFF[0], sizeof(charBUFF));
				GUI_StoreKeyMsg(charBUFF[0], 1);
				GUI_StoreKeyMsg(charBUFF[0], 0);
				break;
			}
			if(Id == ID_BUTTON_29)  // Notifications sent by 'Space' (' ')
			{
				GUI_StoreKeyMsg(GUI_KEY_SPACE, 1);
				GUI_StoreKeyMsg(GUI_KEY_SPACE, 0);
				break;
			}
			if(Id == ID_BUTTON_30)  // Notifications sent by 'Enter'
			{
				GUI_StoreKeyMsg(GUI_KEY_ENTER, 1);
				GUI_StoreKeyMsg(GUI_KEY_ENTER, 0);
				break;
			}
			if(Id == ID_BUTTON_31)  // Notifications sent by '<Back' (<--)
			{
				/*** 中文, Caps == 0; ID_TEXT_0有文本; 禁用退格 (退格用来清除TEXT控件一个字符) ***/
				i = TEXT_GetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &charBUFF[0], sizeof(charBUFF));
				if((inputTYPE==2) && !capsKEY &&  i)
				{
					charBUFF[i-1] = 0;	                    //删除文本输入缓冲区最后一个字符
				}
				if(((inputTYPE!=2)) || capsKEY || !i)
				{
					GUI_StoreKeyMsg(GUI_KEY_BACKSPACE, 1);
					GUI_StoreKeyMsg(GUI_KEY_BACKSPACE, 0);
					break;
				}
			}
/*****************************************************************************************************/
			/*** 中英文/符号 键盘切换区 ***/
			if(Id == ID_BUTTON_32)       /*** 确认是否 操作了键  'Caps' ***/
			{
				if(inputTYPE ==1) break;                                       //无视 '123' 数字/符号键下,Caps 的操作

				capsKEY = !capsKEY;                                            //切换 Caps 操作标志

				if(capsKEY) BUTTON_SetTextColor(pMsg->hWinSrc, BUTTON_CI_UNPRESSED, GUI_RED); //当前按键变红
				else BUTTON_SetTextColor(pMsg->hWinSrc, BUTTON_CI_UNPRESSED, GUI_BLACK);      //变灰
			}
			else if(Id == ID_BUTTON_33)  /*** 确认是否 操作了键  'En' ***/
			{
				inputTYPE = (inputTYPE >= 2) ? 0 : (inputTYPE+1);              //键盘类型循环切换
				capsKEY = 0;                                                   //只有EN 有操 作 capsKEY 状态清零

				BUTTON_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_32), BUTTON_CI_UNPRESSED, GUI_BLACK);//只要En有操作, Caps就变灰;

				/*** 切换 En 键盘标记 ***/
				if(inputTYPE == 0)      BUTTON_SetText(pMsg->hWinSrc, "En");   //英文键盘标记
				else if(inputTYPE == 1) BUTTON_SetText(pMsg->hWinSrc, "123");  //数字符号键盘标记
				else                    BUTTON_SetText(pMsg->hWinSrc, "Ch");   //中文键盘标记
			}

			if(Id == ID_BUTTON_32  ||  Id == ID_BUTTON_33)                      //仅 中英文切换 和 'CapsKEY' 可以进入
			{
				/*** inputTYPE = 2_中文; 1_数字/符号; 0_英文; ***/
				for(ID_Num=ID_BUTTON_0; ID_Num<=ID_BUTTON_26; ID_Num++)
				{
					/*** 先设置 颜色, 后设置 文本内容 ***/
					if(inputTYPE == 2 && capsKEY == 0)
						BUTTON_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_Num), BUTTON_CI_UNPRESSED, GUI_RED);   //红色
					else
						BUTTON_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_Num), BUTTON_CI_UNPRESSED, GUI_BLACK); //黑色

					/*** 按键 显示 文本设置 ***/
					if(inputTYPE==0  ||  inputTYPE==2)    //英文键盘    "En"    ||    //中文键盘    "Ch"
					{
						if(capsKEY == 0)       charBUFF[0] = keyLOW[ID_Num-ID_BUTTON_0];       //小写键盘
						else if(capsKEY == 1)  charBUFF[0] = keyCAP[ID_Num-ID_BUTTON_0];       //大写键盘
					}
					else if(inputTYPE == 1) charBUFF[0] = key123[ID_Num-ID_BUTTON_0];    //数字/符号键盘    "123"

					charBUFF[1] = 0;                                                 //添加 字符串结束符
					BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_Num), &charBUFF[0]);  //文本载入按键
				}
				memset(charBUFF, 0, sizeof(charBUFF));                  //切换键盘之后, 清空输入,并重新更新显示与计算
			}
/*****************************************************************************************************/
/*****************************************************************************************************/
			/*** 拼音输入法 处理区 ***/
			if((inputTYPE==2 && capsKEY==0) ||  Id==ID_BUTTON_32)       //'Ch'  中文输入     (给CapsKEY 开一条通道)
			{
				if(Id != ID_BUTTON_27  &&  Id != ID_BUTTON_28)     //BUTTON_27--28 从此处跳过; 进入后续操作
				{
					TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &charBUFF[0]);        //TEXT_0 显示设置, 公共资源
					/*** 拼音输入法处理 ***/
					for(i=0; i<sizeof(pbuffPY)/4; i++) pbuffPY[i] = 0;       //清空指针 数组
					pstrPY = (char **)&pbuffPY[0];                           //保存 指针数组 首地址
					pstrTEMP = (char *)PYSearch(&charBUFF[0]);               //使用拼音 输入法, 得到汉字索引头 指针
					//每9个汉字储存一个指针到pbuffPY[i]
					/*** 汉字索引非空 && 索引有内容 && 将索引指针载入指针数组, (直至索引空 || 无内容 || BUFF满) ***/
					for(i=0; pstrTEMP && *pstrTEMP && (i<sizeof(pbuffPY)/4-1); i++)
					{
						pbuffPY[i] = (int *)pstrTEMP;                        //将索引载入指针数组, 间隔9个汉字字符
						//pstrTEMP移动9个汉字
						for(temp=ID_TEXT_1; temp<=ID_TEXT_9; temp++)         //往后偏移 9个字符  (支持任何编码形式)
						{
							/*** 计算每次 9个字符长度实际偏移  (支持任何编码形式) ***/ //使用'pfGetCharSize'来计算一个汉字的字节数
							pstrTEMP = pstrTEMP + GUI_pUC_API->pfGetCharSize(pstrTEMP);
							if(!pstrTEMP || !*pstrTEMP) break;               //指针空, 无内容跳出
						}
					}
				}
				else if(Id == ID_BUTTON_27)			        //'<-'
				{
					/*** 预操作测试是否有内容  &&  当前首地址与保存的是否一样 ***/
					if(*(pstrPY-1) && (pstrPY != (char **)&pbuffPY[0])) pstrPY--;     //当前首地址偏移
				}
				else if(Id == ID_BUTTON_28)			        //'->'
				{
					/*** 预操作测试是否有内容  &&  当前首地址与保存的是否一样 ***/
					if(*(pstrPY+1) && (pstrPY!=(char **)&pbuffPY[sizeof(pbuffPY)/4-1])) pstrPY++;
					else pstrPY = (char **)&pbuffPY[0];                                          //否则载入保存首地址
				}
/*****************************************************************************************************/
/*****************************************************************************************************/
				/*** 输入法, 结果显示区 ***/

				//将字符显示至 ID_TEXT_1~ID_TEXT_9
				for(pstrTEMP = *pstrPY, i=ID_TEXT_1; i<=ID_TEXT_9; i++)
				{
					memset(&tempBUFF[0], 0, sizeof(tempBUFF));
					if(!pstrTEMP || !*pstrTEMP) {TEXT_SetText(WM_GetDialogItem(pMsg->hWin, i), ""); continue;} //(空||无内容)清空当前文本框; 仅结束本次循环

					memcpy(&tempBUFF[0], pstrTEMP, temp = GUI_pUC_API->pfGetCharSize(pstrTEMP)); //temp_汉字的字节数  (支持任何编码)
					TEXT_SetText(WM_GetDialogItem(pMsg->hWin, i), &tempBUFF[0]);    //汉字更新至文本框
					pstrTEMP = pstrTEMP + temp;                                     //偏移一个汉字字符
				}
			}
		}
/*****************************************************************************************************/
/*****************************************************************************************************/
		break;
	case ID_BUTTON_34: // Notifications sent by 'Esc'
		switch(NCode) {
		case WM_NOTIFICATION_CLICKED:
			break;
		case WM_NOTIFICATION_RELEASED:
//			WM_HideWindow(pMsg->hWin);                     //隐藏窗口
			WM_HideWindow(hWindowSKB);
		    //设置这个键盘的父窗口为背景桌面   不加这一行在窗口关闭后再次打开会死机   但是在使用完键盘后需要按退出键
			WM_AttachWindow(hWindowSKB, WM_GetDesktopWindow());

			// //WM_SetFocus(WM_HBKWIN);                      //焦点移至桌面
			// //renew all keys include caps
			// inputTYPE = (inputTYPE <= 0) ? 2 : (inputTYPE-1);              //重新打开, 仅能是 'En' 或 '123'
			// Message.hWinSrc = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_33);
			// Message.MsgId = WM_NOTIFY_PARENT;
			// Message.Data.v = WM_NOTIFICATION_RELEASED;
			// WM_SendMessage(pMsg->hWin, &Message);
		}
		break;
	}
	break;
	case WM_PAINT:
		{
			WM_BringToTop(pMsg->hWin);                        //自动将键盘窗口置顶
		}
		break;
	default:
	WM_DefaultProc(pMsg);
	break;
	}
}
/***************************************************************************************/
/*** 全屏键盘 ： 在焦点窗口显示键盘窗口 ***/
void  WindowSKB_ShowInterface(WM_HWIN  hItem, unsigned char Widget_Type)
{
	int posiX, posiY, sizeX[2], sizeY[2];

	WindowSKB_TypeFLAG = 0;            //设置前先清空
	WindowSKB_TypeFLAG = Widget_Type;  //设置操作的小工具类型

	WM_AttachWindow(hWindowSKB, WM_GetParent(hItem));  //自动变为焦点窗口父的子窗口, 提高运行速度
	WM_ShowWindow(hWindowSKB);                         //显示键盘窗口

	sizeX[0] = WM_GetWindowSizeX(WM_GetParent(hItem));    //文本父窗口 X大小(由于桌面口调用不稳定)
	sizeY[0] = WM_GetWindowSizeY(WM_GetParent(hItem));    //文本父窗口 Y大小
	sizeX[1] = WM_GetWindowSizeX(hWindowSKB);             //键盘窗口 X大小
	sizeY[1] = WM_GetWindowSizeY(hWindowSKB);             //键盘窗口 Y大小
	if(sizeX[0] - WM_GetWindowOrgX(hItem) < sizeX[1])  //源窗口起始_X为X_坐标, 右侧区域放不下键盘
		posiX = sizeX[0] - sizeX[1];                        //键盘贴于右侧边框
	else posiX = WM_GetWindowOrgX(hItem);                 //否则使用源窗口起始_X为X_坐标
	if(sizeY[0]-WM_GetWindowOrgY(hItem)-WM_GetWindowSizeY(hItem) < sizeY[1])//源窗口终点_Y为Y_坐标, 底部区域放不下键盘
	{
		if(WM_GetWindowOrgY(hItem) < sizeY[1])     //顶部区域放不下键盘
		  posiY = sizeY[0] - sizeY[1];                      //直接贴于屏幕正底部
		else posiY = WM_GetWindowOrgY(hItem) - sizeY[1];    //键盘完全置于源窗口起始Y 坐标上方,(Y_最小值为0 (贴于小工具顶部))
	}
	else posiY = WM_GetWindowOrgY(hItem) + WM_GetWindowSizeY(hItem); //否则使用源窗口终点_Y为Y坐标

	WM_MoveTo(hWindowSKB, posiX, posiY);                  //键盘动至桌面坐标 位置posiX, posiY;
}
/***************************************************************************************/
/*** 全屏键盘 : 外部调用 接口 (EMWIN) ***/ //外部循环函数

void  WindowSKB_Interface(void)
{
	WM_HWIN hItem;
	static int SrcX = 0, SrcY = 0;      //保存上次位置
	WM_CALLBACK  *_cbPCB;               //窗口回调控制块

/********************************************************************************/
	hItem = WM_GetFocussedWindow();         //获取焦点窗口

	//确定键盘的位置   //单次调用必须禁用       不重复显示判定
	if(SrcX != WM_GetWindowOrgX(hItem)  &&  SrcY != WM_GetWindowOrgY(hItem)) //输入焦点窗口位置变化
	{
		SrcX = WM_GetWindowOrgX(hItem);
		SrcY = WM_GetWindowOrgY(hItem);
//		/*** 测试坐标是否有效 ***/
//		if(SrcX > 1  &&  SrcY > 1)
		{
//			printf("\r\nX_Size: %d; Y_Size: %d;\r\n", WM_GetWindowSizeX(hItem), WM_GetWindowSizeY(hItem));
//			printf("X_PSize: %d; Y_PSize: %d;\r\n", WM_GetWindowSizeX(WM_GetParent(hItem)), WM_GetWindowSizeY(WM_GetParent(hItem)));
//			printf("X_Start: %d; Y_Start: %d;\r\n", WM_GetWindowOrgX(hItem), WM_GetWindowOrgY(hItem));

			_cbPCB = WM_GetCallback(hItem);           //获取 焦点窗口控制块
//			if(_cbPCB == BUTTON_Callback)         printf("\r\nButton_Focus\r\n");
//			else if(_cbPCB == CHECKBOX_Callback)  printf("\r\nCheckBox_Focus\r\n");
//			else if(_cbPCB == DROPDOWN_Callback)  printf("\r\nDropDown_Focus\r\n");
			if(_cbPCB == EDIT_Callback)
			{
				//lpc_printf("\r\nEdit_Focus\r\n");
				WindowSKB_ShowInterface(hItem, 1);                   //EDIT
			}
//			else if(_cbPCB == FRAMEWIN_Callback)  lpc_printf("\r\nFrameWin_Focus\r\n");
//			else if(_cbPCB == GRAPH_Callback)     printf("\r\nGraph_Focus\r\n");
//			else if(_cbPCB == HEADER_Callback)    printf("\r\nHeader_Focus\r\n");
//			else if(_cbPCB == ICONVIEW_Callback)  lpc_printf("\r\nIconView_Focus\r\n");
//			else if(_cbPCB == IMAGE_Callback)     printf("\r\nImage_Focus\r\n");
//			else if(_cbPCB == KNOB_Callback)      printf("\r\nKnob_Focus\r\n");
//			else if(_cbPCB == LISTBOX_Callback)   printf("\r\nListBox_Focus\r\n");
//			else if(_cbPCB == LISTVIEW_Callback)  printf("\r\nListView_Focus\r\n");
//			else if(_cbPCB == LISTWHEEL_Callback) printf("\r\nListWheel_Focus\r\n");
//			else if(_cbPCB == MENU_Callback)      printf("\r\nMenu_Focus\r\n");
			else if(_cbPCB == MULTIEDIT_Callback)
			{
				//lpc_printf("\r\nMultiEdit_Focus\r\n");
				WindowSKB_ShowInterface(hItem, 2);                  //MULTIEDIT
			}
//			else if(_cbPCB == MULTIPAGE_Callback) printf("\r\nMultiPage_Focus\r\n");
//			else if(_cbPCB == PROGBAR_Callback)   printf("\r\nProgBar_Focus\r\n");
//			else if(_cbPCB == RADIO_Callback)     printf("\r\nRadio_Focus\r\n");
//			else if(_cbPCB == SCROLLBAR_Callback) printf("\r\nScrollBar_Focus\r\n");
//			else if(_cbPCB == SLIDER_Callback)    printf("\r\nSlider_Focus\r\n");
//			else if(_cbPCB == SPINBOX_Callback)   printf("\r\nSpinBox_Focus\r\n");
//			else if(_cbPCB == SWIPELIST_Callback) printf("\r\nSwipeList_Focus\r\n");
//			else if(_cbPCB == TEXT_Callback)      printf("\r\nText_Focus\r\n");
//			else if(_cbPCB == TREEVIEW_Callback)  printf("\r\nTreeView_Focus\r\n");
//			else if(_cbPCB == WINDOW_Callback)    printf("\r\nWindow_Focus\r\n");
		}
//		else if(SrcX<=1  &&  SrcY<=1)       //隐藏窗口,坐标少于 (1,1) 跳出
//		{
//		  WM_HideWindow(hWindowSKB);                          //坐标错误, 隐藏窗口
//		}
/********************************************************************************/
	}
	//GUI_TOUCH_Exec();                         //更新 emWin触摸屏操作
	//GUI_Delay(20);                            //更新窗口
}

/***************************************************************************************/
/*** CreateWindowSKB() ***/ //窗口创建之后, 立即自动隐藏; (必须隐藏)

WM_HWIN CreateWindowSKB(void);
WM_HWIN CreateWindowSKB(void) {

	hWindowSKB = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);

	WM_HideWindow(hWindowSKB);                  //创建后 立即隐藏 (自动隐藏)
	WM_SetFocus(WM_HBKWIN);                     //第一次焦点在桌面窗口

	return hWindowSKB;
}
/***************************************************************************************/

