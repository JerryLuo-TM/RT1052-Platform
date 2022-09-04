#ifndef _LED_H_
#define _LED_H_

#include "fsl_gpio.h"

#define LED_GPIO_RED        GPIO1
#define LED_GPIO_RED_PIN    26U

#define LED_GPIO_GREEN      GPIO1
#define LED_GPIO_GREEN_PIN  25U

#define LED_GPIO_BLUE       GPIO1
#define LED_GPIO_BLUE_PIN   21U


#define LED_R(n)  (n?GPIO_PinWrite(LED_GPIO_RED,LED_GPIO_RED_PIN,1):GPIO_PinWrite(LED_GPIO_RED,LED_GPIO_RED_PIN,0))
#define LED_G(n)  (n?GPIO_PinWrite(LED_GPIO_GREEN,LED_GPIO_GREEN_PIN,1):GPIO_PinWrite(LED_GPIO_GREEN,LED_GPIO_GREEN_PIN,0))
#define LED_B(n)  (n?GPIO_PinWrite(LED_GPIO_BLUE,LED_GPIO_BLUE_PIN,1):GPIO_PinWrite(LED_GPIO_BLUE,LED_GPIO_BLUE_PIN,0))

#define LED_R_Toggle (LED_GPIO_RED->DR   ^= (1<<LED_GPIO_RED_PIN))      //输出电平翻转
#define LED_G_Toggle (LED_GPIO_GREEN->DR ^= (1<<LED_GPIO_GREEN_PIN))    //输出电平翻转
#define LED_B_Toggle (LED_GPIO_BLUE->DR  ^= (1<<LED_GPIO_BLUE_PIN))     //输出电平翻转


void LED_Init(void);

#endif


// 9587 8700 0360 1733
//ETC 5201 1640 2302 1860 1733


