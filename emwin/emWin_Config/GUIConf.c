#include "GUI.h"
#include "malloc.h"
#include "fsl_debug_console.h"

//����EMWIN�ڴ��С
#define GUI_NUMBYTES  (4 * 1024 * 1024)
#define GUI_BLOCKSIZE 0X80  //���С

//GUI_X_Config
//��ʼ����ʱ�����,��������emwin��ʹ�õ��ڴ�
void GUI_X_Config(void) 
{
	uint32_t *aMemory = mymalloc(SRAMEXC, GUI_NUMBYTES);   //���ⲿSDRAM�з���GUI_NUMBYTES�ֽڵ��ڴ�
	GUI_ALLOC_AssignMemory((void*)aMemory, GUI_NUMBYTES); //Ϊ�洢����ϵͳ����һ���洢��
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //���ô洢���ƽ���ߴ�,����Խ��,���õĴ洢������Խ��
	GUI_SetDefaultFont(GUI_FONT_8X16); //����Ĭ������
	PRINTF("Emwin EXRAM address: 0x%x \r\n", aMemory);
}

