#ifndef _SVNS_RTC_H_
#define _SVNS_RTC_H_

#include "fsl_common.h"


extern snvs_hp_rtc_datetime_t g_rtcDate;




void RTC_Init(void);

unsigned char RTC_CalcWeek(unsigned short _year, unsigned char _mon, unsigned char _day);
unsigned short bcd2bin(unsigned char val);
unsigned char bin2bcd(unsigned short val);

#endif


