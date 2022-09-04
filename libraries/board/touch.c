#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "pin_mux.h"

#include "include.h"
#include "board.h"
#include "GUI.h"
#include "i2c.h"
#include "led.h"
#include "touch.h"


QueueHandle_t g_touch_queue_handle;

POINT Pre_Touch_Point;
GUI_PID_STATE State;

void touch_task(void *pvParameters)
{
	POINT touch_point;
	touch_event_t touch_event;
	GUI_PID_STATE pid_state;
	touch_general_msg_t touch_msg_queue;
    for (;;)
    {
		xQueueReceive(g_touch_queue_handle, &touch_msg_queue, portMAX_DELAY);
		portENTER_CRITICAL();
		if(touch(&touch_point, &touch_event))
		{
			pid_state.x = touch_point.X;
			pid_state.y = touch_point.Y;
			pid_state.Pressed = (touch_event == kTouch_Down) || (touch_event == kTouch_Contact);
			pid_state.Layer = 0;
			//PRINTF("X = %d ,Y = %d \r\n", pid_state.x, pid_state.y);
			//GUI_TOUCH_StoreStateEx(&pid_state);
			GUI_PID_StoreState(&pid_state);
		}
		portEXIT_CRITICAL();
    }
}

bool touch(POINT *touch_point,touch_event_t *touch_event)
{
	static bool pressed = false;
	static int Pointx1, Pointy1;
	uint8_t Touch_Buff[8];

	/* read touch parameter */
	I2C_Read(FT5316_ADDR, 0x00, &Touch_Buff[0], 7);

	if(Touch_Buff[2] & 0x0F)
	{
		//caculate touch point
		Pointx1 = abs((short int)(Touch_Buff[3] & 0x0F) << 8 | (short int)Touch_Buff[4]);
		Pointy1 = abs((short int)(Touch_Buff[5] & 0x0F) << 8 | (short int)Touch_Buff[6]);
		//bandrate check
		if(Pointx1 >= 854 || Pointy1 >= 480) {
			*touch_event = kTouch_Reserved;
			return true;
		}else {
			touch_point->X = Pointx1;
			touch_point->Y = Pointy1;
			if(pressed == true){
				*touch_event = kTouch_Contact;
			}else {
				*touch_event = kTouch_Down;
			}
		}
	}else {
		//PRINTF("double event \r\n");
		pressed = false;
		touch_point->X = Pointx1;
		touch_point->Y = Pointy1;
		*touch_event = kTouch_Up;
	}

	return true;
}

//**************************************
// FT5316 触摸屏驱动芯片初始化   触摸屏扫描速度支持100HZ
//**************************************
status_t Touch_Init(void)
{
	unsigned char val, ID;

	val=0;   I2C_Write(FT5316_ADDR, 0x00, &val, 1); //进入正常操作模式 0x00=正常模式  0x01=系统信息模式 0x04=测试模式
	val=1;   I2C_Write(FT5316_ADDR, 0xA4, &val, 1); //中断模式控制寄存器
	val=22;  I2C_Write(FT5316_ADDR, 0x80, &val, 1); //触摸有效值设置寄存器
	val=12;  I2C_Write(FT5316_ADDR, 0x88, &val, 1); //激活状态周期设置寄存器

	//读取寄存器数据
	I2C_Read(FT5316_ADDR,0xA8,&ID,1);

	//触摸中断初始化
	LCD_PININT_Init();

	if(g_touch_queue_handle == NULL) {
		g_touch_queue_handle = xQueueCreate(30, sizeof(touch_general_msg_t));
	}

	xTaskCreate(touch_task, "touch_task", 256 / sizeof(uint32_t), NULL, 8, NULL);

	if(ID != 0x79) {
		Touch.Init_statue = false;
		return kStatus_Fail;
	}

	Touch.Init_statue = true;
	return kStatus_Success;
}

//触摸引脚中断初始化
void LCD_PININT_Init(void)
{
	gpio_pin_config_t rst_int_config ;

	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_14_GPIO2_IO30, 0U);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_14_GPIO2_IO30, 0x10B0u);

	//INT配置成中断输入
	rst_int_config.direction     = kGPIO_DigitalInput;
	rst_int_config.outputLogic   = 0;
	rst_int_config.interruptMode = kGPIO_IntRisingEdge;
	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

	//设置优先级
	RT1052_NVIC_SetPriority(GPIO2_Combined_16_31_IRQn,5,0);//抢占优先级位5，0位子优先级

	//使能引脚中断
	GPIO_PortEnableInterrupts(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);

	//使能IO中断
	EnableIRQ(GPIO2_Combined_16_31_IRQn);
}


//触摸中断函数
void GPIO2_Combined_16_31_IRQHandler(void)
{
	touch_general_msg_t touch_msg_queue;
	BaseType_t xHigherPriorityTaskWorken;

    /* 确认是触摸芯片的中断 */
    if(GPIO_GetPinsInterruptFlags(TOUCH_PAD_INT_GPIO) & 1U << TOUCH_PAD_INT_GPIO_PIN)
    {
		/* 清除中断标志 */
		GPIO_PortClearInterruptFlags(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);

		touch_msg_queue.msg_id = MSG_ID_TOUCH_PRESS;
		touch_msg_queue.msg_data = NULL;

		if(g_touch_queue_handle != NULL){
			xQueueSendFromISR(g_touch_queue_handle, &touch_msg_queue, &xHigherPriorityTaskWorken);
			if(xHigherPriorityTaskWorken == pdTRUE) {
				portYIELD_FROM_ISR(xHigherPriorityTaskWorken);
			}
		}
	}

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}





