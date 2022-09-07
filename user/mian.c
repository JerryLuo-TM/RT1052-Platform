#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_elcdif.h"
#include "fsl_lpi2c.h"
#include "fsl_trng.h"
#include "fsl_sd.h"
#include "fsl_sd_disk.h"
#include "fsl_snvs_hp.h"
#include "fsl_snvs_lp.h"

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
//fatfs file system
#include "ff.h"
#include "diskio.h"
//FreeRTOS kernel includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "malloc.h"
#include "led.h"
#include "sdram.h"
#include "uart2.h"
#include "i2c.h"
#include "sai.h"
#include "bq25895.h"
#include "eeprom.h"
#include "touch.h"
#include "ILI9806.h"
#include "elcdif.h"
#include "TF.h"
#include "NRF24L01.h"
#include "svns_rtc.h"
#include "wm8960.h"

#include "MainTask.h"
#include "CPU_UTILS.h"
#include "include.h"


BQ25895_T	BQ25895;
Touch_T		Touch;
Audio_T		Audio;
TF_T		TF;
NRF24L01_T	NRF24L01;

SemaphoreHandle_t IIC1 = NULL;

//xSemaphoreTake(IIC1,portMAX_DELAY);
//xSemaphoreGive(IIC1);

void Init_TRNG(void)
{
	trng_config_t trngConfig;
	TRNG_GetDefaultConfig(&trngConfig);

	trngConfig.sampleMode = kTRNG_SampleModeVonNeumann;

	/* Initialize TRNG */
	TRNG_Init(TRNG, &trngConfig);

	//TRNG_GetRandomData(TRNG, &data, 1);
}

void led_task(void *pvParameters)
{
	uint32_t rand_num = 0;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
		rand_num += 1;
		if((rand_num % 10) == 0) {
			LED_G_Toggle;
		}
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/10);
    }
}

void gui_task(void *pvParameters)
{
	GUI_MainTask();
    for (;;)
    {
		LED_B_Toggle;
		WindowSKB_Interface();
		GUI_Delay(20);
    }
}

void battery_task(void *pvParameters)
{
	int vbus, vsys, vbat, chgc, charge_status;
	int fault, vbus_type, temp;
	static bool bq_need_reinit = true;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
		bq2589x_reset_watchdog_timer();
		vbus = bq2589x_adc_read_vbus_volt();
		vsys = bq2589x_adc_read_sys_volt();
		vbat = bq2589x_adc_read_battery_volt();
		chgc = bq2589x_adc_read_charge_current();
		charge_status = bq2589x_get_charging_status();

		fault = bq25895_read_fault_reg();
		vbus_type = bq2589x_get_vbus_type();
		temp = bq2589x_adc_read_temperature();

		BQ25895.vbus = (float)((float)vbus) / 1000.0f;
		BQ25895.vbat = (float)((float)vbat) / 1000.0f;
		BQ25895.chgc = (float)((float)chgc) / 1000.0f;
		BQ25895.charge_status = charge_status;

		if ((vbus_type != 0) && (bq_need_reinit == true)) {
			Init_BQ25895();
			bq_need_reinit = false;
		} else if (vbus_type == 0) {
			bq_need_reinit = true;
		}

		PRINTF("\r\n");
		PRINTF(" ------------------ bq25895 ------------------ \r\n");
		PRINTF("[REG 0x0E] vbat = %d mV \r\n", vbat);
		PRINTF("[REG 0x11] vbus = %d mV \r\n", vbus);
		PRINTF("[REG 0x0F] vsys = %d mV \r\n", vsys);
		PRINTF("[REG 0x12] chgc = %d  \r\n", chgc);
		PRINTF("[REG 0x10] temp = %d  \r\n", temp);
		PRINTF("[REG 0x0B] vbus_type = %d \r\n", vbus_type);
		PRINTF("[REG 0x0B] charge_status = %d  [0:no chg  1:pre chg  2:fast chg  3:chg done]\r\n", charge_status);
		PRINTF("[REG 0x0C] fault = %d  \r\n", fault);
		PRINTF(" ------------------ end ------------------ \r\n");

		if(hWinInfo != NULL) {
			WM_SendMessageNoPara(hWinInfo, MSG_SYSINFO);
		}

		SNVS_HP_RTC_GetDatetime(SNVS, &g_rtcDate);
		g_rtcDate.dow = RTC_CalcWeek(g_rtcDate.year, g_rtcDate.month, g_rtcDate.day);
		if(hWinInfo != NULL) {
			WM_SendMessageNoPara(hWinStatus, MSG_UPDATERTC);
		}

		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/1);
    }
}

void tf_task(void *pvParameters)
{
	BYTE      work[_MAX_SS];
	FRESULT   result;
	FATFS     SD_fs;
	uint32_t  TF_Size;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
		if(TF.Card_Error == false) {
			if(BOARD_USDHC_CD_STATUS() == 1) {
				TF.Card_ready = false;
				if(TF.Card_Has_Mount == true) {
					result = f_mount(NULL,"2:",1);
					if(result == FR_OK) {
						PRINTF("umount OK ! \r\n");
						TF.Card_Has_Mount = false;
						//if(hWinStatus!=NULL){WM_SendMessageNoPara(hWinStatus, MSG_SDPlugOut);}
						//if(hWinComputer!=NULL){WM_SendMessageNoPara(WM_GetClientWindow(hWinComputer), MSG_Computer_SD);}
					} else {
						PRINTF("f_mount uninstall fail ! \r\n");
					}
				}
			} else {
				if((TF.Card_Has_Mount == false)&&(sdcardWaitCardInsert() == kStatus_Success)) {
					result = f_mount(&SD_fs,"2:",1U);
					if(result == FR_OK) {
						TF.ERROR_Num = 0;
						TF.Card_Has_Mount = true;
						TF.Card_ready = true;
						PRINTF("f_mount OK ! \r\n");

						TF_Size = g_sd.blockCount/2/1024;
						PRINTF("SD Card blockSize  %d Kbyte \r\n", g_sd.blockSize);
						PRINTF("SD Card blockCount %d       \r\n", g_sd.blockCount);
						PRINTF("SD Card busClock   %d Mhz   \r\n", g_sd.busClock_Hz/1000000U);
						PRINTF("SD Card version    %d       \r\n", g_sd.version);
						PRINTF("SD Card size       %d Mb    \r\n", TF_Size);
						//if(hWinStatus!=NULL){WM_SendMessageNoPara(hWinStatus, MSG_SDPlugIn);}
						//if(hWinComputer!=NULL){WM_SendMessageNoPara(WM_GetClientWindow(hWinComputer), MSG_Computer_SD);}
					} else if(result == FR_NO_FILESYSTEM) {
						result = f_mkfs("2:", FM_ANY, 0, work, sizeof(work));
						if(result == FR_OK) {
							PRINTF("f_mkfs OK ! \r\n");
						} else {
							TF.ERROR_Num += 1;
							PRINTF("f_mkfs fail ! \r\n");
						}
					} else {
						TF.ERROR_Num += 1;
						PRINTF("f_mount fail ! \r\n");
					}
				}
			}
		}
		if(TF.ERROR_Num >= 10) {
			TF.Card_Error = true;
			TF.ERROR_Num = 10;
			PRINTF("we had f_mount several times but error , so TF maybe have serious error!! \r\n");
		}
		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/5);
	}
}

void sys_init(void)
{

	IIC1 = xSemaphoreCreateMutex();

	/* Init board hardware. */
	RT1052_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	BOARD_ConfigMPU();
	BOARD_InitBootPins();
	BOARD_USDHCClockConfiguration();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();
	Init_TRNG();
	LED_Init();
	I2C_Init();
	RTC_Init();

	my_mem_init(SRAMIN);
	my_mem_init(SRAMEXC);

	if (WM8960_Init(NULL) == kStatus_Success) { PRINTF("WM8960   by  IIC1 Init config success!!! \r\n"); }
	if (I2C_EEPROM_TEST() == kStatus_Success) { PRINTF("AT24256  by  IIC1 EEPROM Init success!!! \r\n"); }
	if (Touch_Init()      == kStatus_Success) { PRINTF("FT5316   by  IIC1 Touch  Init success!!! \r\n"); }
	if (Init_BQ25895()    == kStatus_Success) { PRINTF("BQ25895  by  IIC1 Power  Init success!!! \r\n"); }
	if (BOARD_InitUART2() == kStatus_Success) { PRINTF("BOARD UART2  Init success!!! \r\n");			 }
	if (BOARD_InitSEMC()  == kStatus_Success) { PRINTF("BOARD SDRAM  Init success!!! \r\n");			 }
	if (NRF24L01_Init(TX) == kStatus_Success) { PRINTF("NRF24L01 By Sim SPI Init success!!! \r\n");		 }

	//SAI_Inint();

	PRINTF("CpuClk = %d Mhz \r\n", CLOCK_GetFreq(kCLOCK_CpuClk)/1000000UL);
	PRINTF("SystemCoreClock = %d Mhz \r\n", SystemCoreClock/1000000UL); 
	PRINTF("length uint32_t = %d \r\n", sizeof(uint32_t));
}

void create_task(void)
{
	taskENTER_CRITICAL();
	if( xTaskCreate(led_task,		"led_task",			128 / sizeof(uint32_t),   NULL, 6, NULL) != pdPASS){ LED_R(1); }
	if( xTaskCreate(battery_task,	"battery_task",		1024 / sizeof(uint32_t),  NULL, 1, NULL) != pdPASS){ LED_R(1); }
	if( xTaskCreate(tf_task,		"tf_task",			2048 / sizeof(uint32_t),  NULL, 1, NULL) != pdPASS){ LED_R(1); }
	// if( xTaskCreate(gui_task,		"gui_task",			(1024 * 3) / sizeof(uint32_t),  NULL, 5, NULL) != pdPASS){ LED_R(1); }
	taskEXIT_CRITICAL();
}

int main(void)
{
	sys_init();

	create_task();

	vTaskStartScheduler();

	while(1);
}

void HardFault_Handler(void)
{
	/* USER CODE BEGIN HardFault_IRQn 0 */

	/* USER CODE END HardFault_IRQn 0 */
	while (1)
	{
		printf("HardFault_Handler \r\n");
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}


