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
*����:  ScanFiles
*����:  lj
*����:  2019.3.28
*����:  path:����·����
        hFile:��¼�ļ�·�����ļ�ָ��ṹ��
*����:  None
*����:  �������ļ��м����ļ����������ļ�
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
			if (res != FR_OK || fno.fname[0] == 0) break;  /* ����ʧ�� �� ����β�����˳�*/
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */ //�����ϼ�Ŀ¼
			//PRINTF("fno.fname=%s \r\n",fno.fname);
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) /* It is a directory �ļ�������Ŀ¼*/
            {
                if(hTree != NULL && hNode != NULL) {
                    //Ŀ¼���������
                    hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,fn,0);
                    //�ѽ����뵽Ŀ¼����
                    TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);
                    PRINTF("DIR %s add OK !\r\n",fn);
				}
				i = strlen((const char*)path);
				sprintf(&path[i],"/%s",fno.fname);
                //��·���������ļ���·����
				PRINTF("path= %s \r\n",path);
                //���������ļ����µ��ļ�
                res = ScanFiles(path,hTree,hItem,fileNum);

                if (res != FR_OK)break;//��ʧ�ܣ�����ѭ��
                path[i] = 0;//ĩβ��ӽ����ַ�
            }
            else /* It is a file. */
            {
                /* ����Ҫ���Ƿ񴴽�Ŀ¼�� */
                if(hTree != NULL && hNode != NULL)//����Ŀ¼��
                {
                    hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,fn,0);//�ļ���������Ҷ
                    TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);//����Ҷ��ӵ�Ŀ¼��
										//(*fileNum)++;//��¼�ļ���Ŀ
                }
                else
                {
                    (*fileNum)++;//��¼�ļ���Ŀ
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
    char p_path[FILE_PATH_MAX] = {0};  //Ŀ¼�� ָ��
	FRESULT res;   //���ؽ��

    hTreeItemCur = hNode;
    strcpy(p_path, path);		//����Ŀ¼����ָ��
    res = ScanFiles(p_path,hTree,hNode,fileNum);		//�ݹ�ɨ���ļ�
	res = res;
	hTreeItemCur = hTreeItemCur;
}

void get_pathname(WM_HWIN hTree,TREEVIEW_ITEM_Handle hNode,unsigned char* path)
{
    unsigned char hItemname[100] = {0};
    unsigned char len = 0;
    TREEVIEW_ITEM_Handle hParent;

    TREEVIEW_ITEM_GetText(hNode, hItemname, 100);//��ǰ������ı�
    if(0 != strcmp((char*)hItemname, "2:")) { //���Ǹ�Ŀ¼
        hParent=TREEVIEW_GetItem(hTree, hNode, TREEVIEW_GET_PARENT);//�����
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
	TREEVIEW_ITEM_Handle   hTreeItemCur;		//TreeView��Ŀ���
	TREEVIEW_ITEM_Handle   hTreeItem_SDCard;//TreeView��Ŀ���
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
			//���ڳ�ʼ��
			{
				FRAMEWIN_SetTitleHeight(hWin, 32);
				FRAMEWIN_SetTextAlign(hWin, GUI_TA_HCENTER | GUI_TA_VCENTER);
				FRAMEWIN_SetText(hWin, "�ļ�����");
				FRAMEWIN_SetFont(hWin, &GUI_FontHZ32);
				FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);//�رհ�ť
			}
			//TreeView��ʼ��
			{
				TREEVIEW_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0),&GUI_FontHZ24);
				TREEVIEW_SetSelMode(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), TREEVIEW_SELMODE_TEXT); //ѡ��ģʽ ���б�ǡ�
				TREEVIEW_SetAutoScrollV(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), 1);    //ʹ���Զ�������
				TREEVIEW_SetAutoScrollH(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), 1);    //ʹ���Զ�������
				TREEVIEW_SetHasLines(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0),1);        //��Ŀ֮���������

				hTreeItemCur = TREEVIEW_GetItem(WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0), 0, TREEVIEW_GET_LAST);
				hTreeItem_SDCard = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE, "2:", 0);//���� ���ڵ�/��Ҷ�� �ı���
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
					/* �鿴ѡ�����ĸ���Ŀ */
					hNode = TREEVIEW_GetSel(pMsg->hWinSrc);
					/* ��ȡ����Ŀ����Ϣ */
					TREEVIEW_ITEM_GetInfo(hNode,&ItemInfo);
					if(NCode == WM_NOTIFICATION_RELEASED) { //���ͷ�
						if(ItemInfo.IsNode == 0) { //�������Ŀ¼����Ҷ�ӣ����ļ���
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




//-------------------����---------------------



//--------------------------------------------
