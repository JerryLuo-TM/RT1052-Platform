#ifndef __HZFONTUPDATA_H
#define __HZFONTUPDATA_H


//�ֿ���Ϣ�ṹ�嶨��
//���������ֿ������Ϣ����ַ����С��
__packed typedef struct
{
	unsigned char fontok;				//�ֿ���ڱ�־��0XAA���ֿ��������������ֿⲻ����
	unsigned int  ugbkaddr; 			//unigbk�ĵ�ַ
	unsigned int  ugbksize;			//unigbk�Ĵ�С	 
	unsigned int  f16addr;			//gbk16��ַ
	unsigned int  gbk16size;			//gbk16�Ĵ�С		 
	unsigned int  f24addr;			//gbk24��ַ
	unsigned int  gbk24size;			//gbk24�Ĵ�С 	 
	unsigned int  f32addr;			//gbk32��ַ
	unsigned int  gbk32size;			//gbk32�Ĵ�С 
}_font_info; 

extern _font_info ftinfo;	//�ֿ���Ϣ�ṹ��

unsigned char emwin_update_font(unsigned char* src);

#endif
