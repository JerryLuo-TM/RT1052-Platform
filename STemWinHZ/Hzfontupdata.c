#include "fsl_debug_console.h"

#include "ff.h"
#include "diskio.h"

#include "malloc.h"

#include "DIALOG.h"
#include "HZfontupdata.h"

//  5738496 �ֽ� / 4096 =1401.0
#define FONTSECSIZE	 	1401
//�ֿ���flash�е����ƫ��
#define FONTINFOADDR 	0x80000000

//���������ֿ������Ϣ����ַ����С��
_font_info ftinfo;

//�ֿ����ڴ����е�·��
unsigned char*const GBK_PATH[4]=
{
	"/FONT/UNIGBK.BIN",	//UNIGBK.BIN�Ĵ��λ��
	"/FONT/HZ16.DZK",	//GBK16�Ĵ��λ��
	"/FONT/HZ24.DZK",	//GBK24�Ĵ��λ��
	"/FONT/HZ32.DZK",	//GBK32�Ĵ��λ��
}; 
//����ʱ����ʾ��Ϣ
unsigned char*const UPDATE_REMIND_TBL[4]=
{
	"Updating UNIGBK.BIN",//��ʾ���ڸ���UNIGBK.bin
	"Updating HZ16.DZK",	//��ʾ���ڸ���GBK16
	"Updating HZ24.DZK",	//��ʾ���ڸ���GBK24
	"Updating HZ32.DZK",	//��ʾ���ڸ���GBK32
};

//����ĳһ��
//fxpath:·��
//fx:���µ����� 0,ungbk;1,gbk16;2,gbk24;3,gbk32;
//����ֵ:0,�ɹ�;����,ʧ��.
unsigned char emwin_updata_fontx(unsigned char *fxpath,unsigned char fx)
{
	unsigned int flashaddr=0;								    
	FIL * fftemp;
	unsigned char *tempbuf;
 	unsigned char res;	
	unsigned int  bw;
	unsigned char rval=0;
	
	fftemp=(FIL*)mymalloc(SRAMEXC,sizeof(FIL));
	if(fftemp==NULL)rval=1;
	
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//���ļ�ʧ��
	
	tempbuf=mymalloc(SRAMEXC,fftemp->obj.objsize); //����ռ�
	if(tempbuf==NULL)rval=1;
	
	PRINTF("font %d size=%d! \r\n",fx,fftemp->obj.objsize);
	
 	if(rval==0)
	{
		switch(fx)
		{
			case 0:												//����UNIGBK.BIN
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//��Ϣͷ֮�󣬽���UNIGBKת�����
				ftinfo.ugbksize=fftemp->obj.objsize;					//UNIGBK��С
				flashaddr=ftinfo.ugbkaddr;
				break;
			case 1:
				ftinfo.f16addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBK֮�󣬽���GBK16�ֿ�
				ftinfo.gbk16size=fftemp->obj.objsize;					//GBK16�ֿ��С
				flashaddr=ftinfo.f16addr;						//GBK16����ʼ��ַ
				break;
			case 2:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16֮�󣬽���GBK24�ֿ�
				ftinfo.gbk24size=fftemp->obj.objsize;					//GBK24�ֿ��С
				flashaddr=ftinfo.f24addr;						//GBK24����ʼ��ַ
				break;
			case 3:
				ftinfo.f32addr=ftinfo.f24addr+ftinfo.gbk24size;	//GBK24֮�󣬽���GBK32�ֿ�
				ftinfo.gbk32size=fftemp->obj.objsize;					//GBK32�ֿ��С
				flashaddr=ftinfo.f32addr;						//GBK32����ʼ��ַ
				break;
		}
		
		PRINTF("Font %d address=0x%08X \r\n",fx,flashaddr);
		bw=0;
		if(res==FR_OK)
		{
			do{
			res = f_read(fftemp,(unsigned char*)flashaddr,fftemp->obj.objsize,&bw);
			}while(bw<fftemp->obj.objsize);
			
			PRINTF("Font read finish ! \r\n");
		}
		
		f_close(fftemp);		
	}
	myfree(SRAMEXC,fftemp);	  //�ͷ��ڴ�
	myfree(SRAMEXC,tempbuf);	//�ͷ��ڴ�
	return res;
}


//���������ļ�,UNIGBK,GBK12,GBK16,GBK24,GBK32һ�����
//src:�ֿ���Դ����."0:",SD��;"1:",FLASH��,"2:",U��.
//��ʾ��Ϣ�����С										  
//����ֵ:0,���³ɹ�;
//		 ����,�������.	
unsigned char emwin_update_font(unsigned char* src)
{
	unsigned char table[sizeof(ftinfo)];
	unsigned char *pname;
	unsigned char res=0;		   
 	unsigned short i;
	FIL *fftemp;
	unsigned char rval=0; 
	res=0XFF;
	ftinfo.fontok=0XFF;
	pname=mymalloc(SRAMEXC,100);	//����100�ֽ��ڴ�  
	fftemp=(FIL*)mymalloc(SRAMEXC,sizeof(FIL));	//�����ڴ�	
	
	if(pname==NULL||fftemp==NULL)
	{
		myfree(SRAMEXC,fftemp);
		myfree(SRAMEXC,pname);
		return 5;		//�ڴ�����ʧ��
	}
	
	for(i=0;i<4;i++)	//�Ȳ����ļ�UNIGBK,GBK16,GBK24,GBK32�Ƿ����� 
	{
		strcpy((char*)pname,(char*)src);				 //copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK_PATH[i]); //׷�Ӿ����ļ�·�� 
		res=f_open(fftemp,(const TCHAR*)pname,FA_READ);	//���Դ�
		
		PRINTF("num=%d res=%d str:%s \r\n",i,res,pname);
		
		if(res)
		{
			rval|=1<<7;	//��Ǵ��ļ�ʧ��  
			break;		//������,ֱ���˳�
		}
	} 
	myfree(SRAMEXC,fftemp);	//�ͷ��ڴ�
	
	if(rval==0)				//�ֿ��ļ�������.
	{
		for(i=0;i<4;i++)	//���θ���UNIGBK,GBK16,GBK24,GBK32
		{
			strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
			strcat((char*)pname,(char*)GBK_PATH[i]); 		//׷�Ӿ����ļ�·�� 
      res=emwin_updata_fontx(pname,i);	//�����ֿ�
			if(res)
			{
				myfree(SRAMEXC,pname);
				return 1+i;
			}
		}
		//ȫ�����º���
		ftinfo.fontok=0XAA;
		memcpy((unsigned char*)FONTINFOADDR, (unsigned char*)&ftinfo, sizeof(ftinfo));     //�����ֿ���Ϣ
	}
	myfree(SRAMEXC,pname);//�ͷ��ڴ� 
	
  PRINTF("0x%08X ",ftinfo.f16addr);
	PRINTF("0x%08X ",ftinfo.f24addr);
	PRINTF("0x%08X ",ftinfo.f32addr);
	
	memcpy(table,(unsigned char*)FONTINFOADDR, sizeof(ftinfo));
	return rval;//�޴���.			 
}






