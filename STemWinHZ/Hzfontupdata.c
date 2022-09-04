#include "fsl_debug_console.h"

#include "ff.h"
#include "diskio.h"

#include "malloc.h"

#include "DIALOG.h"
#include "HZfontupdata.h"

//  5738496 字节 / 4096 =1401.0
#define FONTSECSIZE	 	1401
//字库再flash中的相对偏移
#define FONTINFOADDR 	0x80000000

//用来保存字库基本信息，地址，大小等
_font_info ftinfo;

//字库存放在磁盘中的路径
unsigned char*const GBK_PATH[4]=
{
	"/FONT/UNIGBK.BIN",	//UNIGBK.BIN的存放位置
	"/FONT/HZ16.DZK",	//GBK16的存放位置
	"/FONT/HZ24.DZK",	//GBK24的存放位置
	"/FONT/HZ32.DZK",	//GBK32的存放位置
}; 
//更新时的提示信息
unsigned char*const UPDATE_REMIND_TBL[4]=
{
	"Updating UNIGBK.BIN",//提示正在更新UNIGBK.bin
	"Updating HZ16.DZK",	//提示正在更新GBK16
	"Updating HZ24.DZK",	//提示正在更新GBK24
	"Updating HZ32.DZK",	//提示正在更新GBK32
};

//更新某一个
//fxpath:路径
//fx:更新的内容 0,ungbk;1,gbk16;2,gbk24;3,gbk32;
//返回值:0,成功;其他,失败.
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
 	if(res)rval=2;//打开文件失败
	
	tempbuf=mymalloc(SRAMEXC,fftemp->obj.objsize); //分配空间
	if(tempbuf==NULL)rval=1;
	
	PRINTF("font %d size=%d! \r\n",fx,fftemp->obj.objsize);
	
 	if(rval==0)
	{
		switch(fx)
		{
			case 0:												//更新UNIGBK.BIN
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//信息头之后，紧跟UNIGBK转换码表
				ftinfo.ugbksize=fftemp->obj.objsize;					//UNIGBK大小
				flashaddr=ftinfo.ugbkaddr;
				break;
			case 1:
				ftinfo.f16addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBK之后，紧跟GBK16字库
				ftinfo.gbk16size=fftemp->obj.objsize;					//GBK16字库大小
				flashaddr=ftinfo.f16addr;						//GBK16的起始地址
				break;
			case 2:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16之后，紧跟GBK24字库
				ftinfo.gbk24size=fftemp->obj.objsize;					//GBK24字库大小
				flashaddr=ftinfo.f24addr;						//GBK24的起始地址
				break;
			case 3:
				ftinfo.f32addr=ftinfo.f24addr+ftinfo.gbk24size;	//GBK24之后，紧跟GBK32字库
				ftinfo.gbk32size=fftemp->obj.objsize;					//GBK32字库大小
				flashaddr=ftinfo.f32addr;						//GBK32的起始地址
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
	myfree(SRAMEXC,fftemp);	  //释放内存
	myfree(SRAMEXC,tempbuf);	//释放内存
	return res;
}


//更新字体文件,UNIGBK,GBK12,GBK16,GBK24,GBK32一起更新
//src:字库来源磁盘."0:",SD卡;"1:",FLASH盘,"2:",U盘.
//提示信息字体大小										  
//返回值:0,更新成功;
//		 其他,错误代码.	
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
	pname=mymalloc(SRAMEXC,100);	//申请100字节内存  
	fftemp=(FIL*)mymalloc(SRAMEXC,sizeof(FIL));	//分配内存	
	
	if(pname==NULL||fftemp==NULL)
	{
		myfree(SRAMEXC,fftemp);
		myfree(SRAMEXC,pname);
		return 5;		//内存申请失败
	}
	
	for(i=0;i<4;i++)	//先查找文件UNIGBK,GBK16,GBK24,GBK32是否正常 
	{
		strcpy((char*)pname,(char*)src);				 //copy src内容到pname
		strcat((char*)pname,(char*)GBK_PATH[i]); //追加具体文件路径 
		res=f_open(fftemp,(const TCHAR*)pname,FA_READ);	//尝试打开
		
		PRINTF("num=%d res=%d str:%s \r\n",i,res,pname);
		
		if(res)
		{
			rval|=1<<7;	//标记打开文件失败  
			break;		//出错了,直接退出
		}
	} 
	myfree(SRAMEXC,fftemp);	//释放内存
	
	if(rval==0)				//字库文件都存在.
	{
		for(i=0;i<4;i++)	//依次更新UNIGBK,GBK16,GBK24,GBK32
		{
			strcpy((char*)pname,(char*)src);				//copy src内容到pname
			strcat((char*)pname,(char*)GBK_PATH[i]); 		//追加具体文件路径 
      res=emwin_updata_fontx(pname,i);	//更新字库
			if(res)
			{
				myfree(SRAMEXC,pname);
				return 1+i;
			}
		}
		//全部更新好了
		ftinfo.fontok=0XAA;
		memcpy((unsigned char*)FONTINFOADDR, (unsigned char*)&ftinfo, sizeof(ftinfo));     //保存字库信息
	}
	myfree(SRAMEXC,pname);//释放内存 
	
  PRINTF("0x%08X ",ftinfo.f16addr);
	PRINTF("0x%08X ",ftinfo.f24addr);
	PRINTF("0x%08X ",ftinfo.f32addr);
	
	memcpy(table,(unsigned char*)FONTINFOADDR, sizeof(ftinfo));
	return rval;//无错误.			 
}






