#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "fsl_common.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define FT5316_ADDR  0x38

#define TOUCH_PAD_INT_GPIO 			    GPIO2
#define TOUCH_PAD_INT_GPIO_PIN 	        (30U)
#define TOUCH_PAD_INT_IOMUXC			IOMUXC_GPIO_AD_B0_11_GPIO1_IO11

extern int Pointx1,Pointy1;
extern QueueHandle_t g_touch_queue_handle;

typedef struct 
{
    int  X;
    int  Y;
}POINT;

typedef enum {
    kTouch_Down     = 0, /*!< The state changed to touched. */
    kTouch_Up       = 1, /*!< The state changed to not touched. */
    kTouch_Contact  = 2, /*!< There is a continuous touch being detected. */
    kTouch_Reserved = 3  /*!< No touch information available. */
} touch_event_t;

typedef enum {
    MSG_ID_TOUCH_PRESS = 0,
    MSG_ID_TOUCH_NONE,
} touch_msg_event_id_t;

typedef struct{
    touch_msg_event_id_t msg_id;
    uint8_t *msg_data;
} touch_general_msg_t;

status_t Touch_Init(void);
void LCD_PININT_Init(void);
bool touch(POINT *touch_point,touch_event_t *touch_event);

#endif


