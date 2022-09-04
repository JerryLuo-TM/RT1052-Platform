#ifndef __HZFONTUPDATA_H
#define __HZFONTUPDATA_H


//字库信息结构体定义
//用来保存字库基本信息，地址，大小等
__packed typedef struct
{
	unsigned char fontok;				//字库存在标志，0XAA，字库正常；其他，字库不存在
	unsigned int  ugbkaddr; 			//unigbk的地址
	unsigned int  ugbksize;			//unigbk的大小	 
	unsigned int  f16addr;			//gbk16地址
	unsigned int  gbk16size;			//gbk16的大小		 
	unsigned int  f24addr;			//gbk24地址
	unsigned int  gbk24size;			//gbk24的大小 	 
	unsigned int  f32addr;			//gbk32地址
	unsigned int  gbk32size;			//gbk32的大小 
}_font_info; 

extern _font_info ftinfo;	//字库信息结构体

unsigned char emwin_update_font(unsigned char* src);

#endif
