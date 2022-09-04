#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

#include "fsl_snvs_hp.h"
#include "fsl_snvs_lp.h"
#include "svns_rtc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


snvs_hp_rtc_datetime_t g_rtcDate;

void RTC_Init(void)
{
    snvs_lp_srtc_datetime_t srtcDate;
    snvs_hp_rtc_config_t snvsRtcConfig;
    snvs_lp_srtc_config_t snvsSrtcConfig;

	  (void)snvsSrtcConfig;

	SNVS_HP_RTC_GetDefaultConfig(&snvsRtcConfig);
    SNVS_HP_RTC_Init(SNVS, &snvsRtcConfig);

    PRINTF("SNVS LP SRTC example:\r\n");

    /* Set SRTC time to default time and date and start the SRTC */
    SNVS_LP_SRTC_StartTimer(SNVS);

    SNVS_HP_RTC_TimeSynchronize(SNVS);
    SNVS_HP_RTC_StartTimer(SNVS);

    //SNVS_HP_RTC_EnableInterrupts(SNVS, kSNVS_RTC_AlarmInterruptEnable);
    //EnableIRQ(EXAMPLE_SNVS_IRQn);

    SNVS_HP_RTC_GetDatetime(SNVS, &g_rtcDate);

	if(g_rtcDate.year < 2000U){
		srtcDate.year = 2020U;
		srtcDate.month = 1U;
		srtcDate.day = 1U;
		srtcDate.hour = 8U;
		srtcDate.minute = 0;
		srtcDate.second = 0;

		SNVS_LP_SRTC_SetDatetime(SNVS, &srtcDate);

		SNVS_HP_RTC_TimeSynchronize(SNVS);
		SNVS_HP_RTC_StartTimer(SNVS);
	}

    PRINTF("Current datetime: %04d-%02d-%02d %02d:%02d:%02d\r\n", g_rtcDate.year, g_rtcDate.month, g_rtcDate.day,
            g_rtcDate.hour, g_rtcDate.minute, g_rtcDate.second);


}

/*
*********************************************************************************************************
*	函 数 名: bsp_CalcWeek
*	功能说明: 根据日期计算星期几
*	形    参: _year _mon _day  年月日  (年是2字节整数，月和日是字节整数）
*	返 回 值: 周几 （1-7） 7表示周日
*********************************************************************************************************
*/
unsigned char RTC_CalcWeek(unsigned short _year, unsigned char _mon, unsigned char _day)
{
	/*
		蔡勒（Zeller）公式
		历史上的某一天是星期几？未来的某一天是星期几？关于这个问题，有很多计算公式（两个通用计算公式和
		一些分段计算公式），其中最著名的是蔡勒（Zeller）公式。
		即w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1

		公式中的符号含义如下，
		w：星期；
		c：年的高2位，即世纪-1
		y：年（两位数）；
		m：月（m大于等于3，小于等于14，即在蔡勒公式中，某年的1、2月要看作上一年的13、14月来计算，
		比如2003年1月1日要看作2002年的13月1日来计算）；
		d：日；
		[ ]代表取整，即只要整数部分。

		算出来的W除以7，余数是几就是星期几。如果余数是0，则为星期日。
        如果结果是负数，负数求余数则需要特殊处理：
            负数不能按习惯的余数的概念求余数，只能按数论中的余数的定义求余。为了方便
        计算，我们可以给它加上一个7的整数倍，使它变为一个正数，然后再求余数

		以2049年10月1日（100周年国庆）为例，用蔡勒（Zeller）公式进行计算，过程如下：
		蔡勒（Zeller）公式：w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
		=49+[49/4]+[20/4]-2×20+[26× (10+1)/10]+1-1
		=49+[12.25]+5-40+[28.6]
		=49+12+5-40+28
		=54 (除以7余5)
		即2049年10月1日（100周年国庆）是星期5。
	*/
	uint8_t y, c, m, d;
	int16_t w;

	if (_mon >= 3)
	{
		m = _mon;
		y = _year % 100;
		c = _year / 100;
		d = _day;
	}
	else	/* 某年的1、2月要看作上一年的13、14月来计算 */
	{
		m = _mon + 12;
		y = (_year - 1) % 100;
		c = (_year - 1) / 100;
		d = _day;
	}

	w = y + y / 4 +  c / 4 - 2 * c + ((uint16_t)26*(m+1))/10 + d - 1;
	if (w == 0)
	{
		w = 7;	/* 表示周日 */
	}
	else if (w < 0)	/* 如果w是负数，则计算余数方式不同 */
	{
		w = 7 - (-w) % 7;
	}
	else
	{
		w = w % 7;
	}

	/* modified by eric2013 -- 2016-12-25 */
	if (w == 0)
	{
		w = 7;	/* 表示周日 */
	}

	return w;
}

//BCD码转为二进制
unsigned short bcd2bin(unsigned char val)
{
	return (val & 0x0f) + (val >> 4) * 10;
}

//二进制转为BCD码
unsigned char bin2bcd(unsigned short val)
{
	return ((val / 10) << 4) + val % 10;
}





