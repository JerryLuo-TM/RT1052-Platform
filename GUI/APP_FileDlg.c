#include "include.h"
#include "MainTask.h"

#include "fsl_debug_console.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"

#include "ff.h"
#include "diskio.h"

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_TREEVIEW_0 (GUI_ID_USER + 0x01)

#define FILE_PATH_MAX   _MAX_LFN + 1

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
	{ FRAMEWIN_CreateIndirect, "File",     ID_FRAMEWIN_0, 0, 0, 854, 430, 0, 0x0, 0 },
	{ TREEVIEW_CreateIndirect, "Treeview", ID_TREEVIEW_0, 0, 0, 554, 392, 0, 0x0, 0 },
};

/************************************************************************************
*函数:  ScanFiles
*作者:  lj
*日期:  2019.3.28
*参数:  path:遍历路径名
        hFile:记录文件路径的文件指针结构体
*返回:  None
*描述:  遍历此文件夹及子文件夹下所有文件
************************************************************************************/
static FRESULT ScanFiles(char* path,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,unsigned int *fileNum)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
    TREEVIEW_ITEM_Handle hItem;

#if _USE_LFN
    static char lfn[FILE_PATH_MAX]={0};   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif
    res = f_opendir(&dir, (const TCHAR*)path);	/* Open the directory */
	//PRINTF("Open DIR=%s , RES=%d \r\n",path,res);
    if (res == FR_OK)
    {
        for (;;)
        {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0) break;  /* 若打开失败 或 到结尾，则退出*/
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */ //忽略上级目录
			//PRINTF("fno.fname=%s \r\n",fno.fname);
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) /* It is a directory 文件属性是目录*/
            {
                if(hTree != NULL && hNode != NULL) {
                    //目录，创建结点
                    hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,fn,0);
                    //把结点加入到目录树中
                    TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);
                    PRINTF("DIR %s add OK !\r\n",fn);
				}
				i = strlen((const char*)path);
				sprintf(&path[i],"/%s",fno.fname);
                //在路径最后添加文件夹路径名
				PRINTF("path= %s \r\n",path);
                //遍历此新文件夹下的文件
                res = ScanFiles(path,hTree,hItem,fileNum);

                if (res != FR_OK)break;//打开失败，跳出循环
                path[i] = 0;//末尾添加结束字符
            }
            else /* It is a file. */
            {
                /* 根据要求是否创建目录树 */
                if(hTree != NULL && hNode != NULL)//创建目录树
                {
                    hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,fn,0);//文件，创建树叶
                    TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);//把树叶添加到目录树
										//(*fileNum)++;//记录文件数目
                }
                else
                {
                    (*fileNum)++;//记录文件数目
                }
            }
        }
        f_closedir(&dir);
    }
    return res;
}

void Fill_FileList(char *path,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,unsigned int *fileNum)
{
    TREEVIEW_ITEM_Handle hTreeItemCur;
    char p_path[FILE_PATH_MAX] = {0};  //目录名 指针
	FRESULT res;   //返回结果

    hTreeItemCur = hNode;
    strcpy(p_path, path);		//复制目录名到指针
    res = ScanFiles(p_path,hTree,hNode,fileNum);		//递归扫描文件
	res = res;
	hTreeItemCur = hTreeItemCur;
}

void get_pathname(WM_HWIN hTree,TREEVIEW_ITEM_Handle hNode,unsigned char* path)
{
    unsigned char hItemname[100] = {0};
    unsigned char len = 0;
    TREEVIEW_ITEM_Handle hParent;

    TREEVIEW_ITEM_GetText(hNode, hItemname, 100);//当前句柄的文本
    if(0 != strcmp((char*)hItemname, "2:")) { //不是根目录
        hParent=TREEVIEW_GetItem(hTree, hNode, TREEVIEW_GET_PARENT);//父句柄
        get_pathname(hTree, hParent,path);
    } else {
		if(0 == strcmp((char*)hItemname, "2:")) {
			sprintf((char*)path,"%s","2:");
		}
        return;
    }
    len = strlen((char*)path);
    sprintf((char*)(path+len), "/%s", hItemname);
}

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg)
{
	TREEVIEW_ITEM_Handle   hTreeItemCur;		//TreeView项目句柄
	TREEVIEW_ITEM_Handle   hTreeItem_SDCard;//TreeView项目句柄
	TREEVIEW_ITEM_INFO     ItemInfo;
	TREEVIEW_ITEM_Handle   hNode;
	GUI_HMEM hMem_pathname;
	unsigned int  FileNum=0;
	unsigned char *pathname;
	WM_HWIN hWin=pMsg->hWin;
	int     NCode;
	int     Id;

	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG: {
			//窗口初始化
			{
				FRAMEWIN_SetTitleHeight(hWin, 32);
				FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
				FRAMEWIN_SetText(hWin, "文件管理");
				FRAMEWIN_SetFont(hWin, &GUI_FontHZ32);
				FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);//关闭按钮
			}
			//TreeView初始化
			{
				TREEVIEW_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0),&GUI_FontHZ24);
				TREEVIEW_SetSelMode(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), TREEVIEW_SELMODE_TEXT); //选择模式 【行标记】
				TREEVIEW_SetAutoScrollV(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), 1);    //使用自动滑动条
				TREEVIEW_SetAutoScrollH(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), 1);    //使用自动滑动条
				TREEVIEW_SetHasLines(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0),1);        //项目之间的连接线

				hTreeItemCur = TREEVIEW_GetItem(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), 0, TREEVIEW_GET_LAST);
				hTreeItem_SDCard = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, "2:", 0);//创建 【节点/树叶】 文本项
				TREEVIEW_AttachItem(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), hTreeItem_SDCard, hTreeItemCur, TREEVIEW_INSERT_BELOW);
				//FLASH
				Fill_FileList("2:",WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0),hTreeItem_SDCard,&FileNum);
			}
		} break;

		case WM_NOTIFY_PARENT: {
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (Id) {
				case ID_TREEVIEW_0: {
					/* 查看选中了哪个项目 */
					hNode = TREEVIEW_GetSel(pMsg->hWinSrc);
					/* 获取该项目的信息 */
					TREEVIEW_ITEM_GetInfo(hNode,&ItemInfo);
					if(NCode == WM_NOTIFICATION_RELEASED) { //已释放
						if(ItemInfo.IsNode == 0) { //点击的是目录树的叶子（即文件）
							hMem_pathname = GUI_ALLOC_AllocZero(256);
							pathname = GUI_ALLOC_h2p(hMem_pathname);
							get_pathname(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), hNode,pathname);
							PRINTF("\r\n%s\r\n",pathname);
							GUI_ALLOC_Free(hMem_pathname);
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

void App_File(WM_HWIN hWin, uint32_t idenx)
{
    GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, hWin, 0, 0);
}




//-------------------结束---------------------



//--------------------------------------------
