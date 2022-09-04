#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include "fsl_common.h"
#include "fsl_debug_console.h"

typedef struct
{
	float  MCU_temputer;
	float  POW_temputer;
	float  TS_temputer;

	float  Battery_volate;
	float  Battery_current;
	float  Charge_current;
	float  Battery_power;
	float  VBUS;
	bool   BKpic_load;
	uint32_t User_ID;
}Sys_Status_t;
extern Sys_Status_t Sys_Status;

//SD卡参数信息
typedef struct
{
	bool Card_ready;  		//TF卡准备
	bool Card_Error;  		//坏卡标记
	bool Card_Has_Mount;  //枚举标志
	uint32_t ERROR_Num;   //TF错误计数器
}TF_T;
extern TF_T TF;

//触摸相关参数
typedef	 struct
{
	bool Init_statue;	//初始化情况
	unsigned char Touch_num;
}Touch_T;
extern Touch_T Touch;

//电池参数
typedef struct
{
	float vbat;
	float vbus;
	float sysv;
	float temp;
	float chgc;
	unsigned char vbus_type;
	unsigned char charge_status;
	bool Init_statue;	//初始化情况
}BQ25895_T;
extern BQ25895_T BQ25895;

typedef	 struct
{
	bool Init_statue; //初始化情况
}Audio_T;
extern Audio_T Audio;

typedef	 struct
{
	bool Init_statue; //初始化情况
}NRF24L01_T;
extern NRF24L01_T NRF24L01;


#endif



