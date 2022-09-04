#ifndef _ILI9806_H_
#define _ILI9806_H_

#include "fsl_common.h"

#define LCD_REST(n)      GPIO_PinWrite( GPIO2, 24U, n );
#define ILI9806_CS(n)    GPIO_PinWrite( GPIO2, 25U, n );
#define ILI9806_SDI(n)   GPIO_PinWrite( GPIO3, 26U, n );
#define ILI9806_SCL(n)   GPIO_PinWrite( GPIO2, 23U, n );

#define Back_Light(n)   GPIO_PinWrite( GPIO2, 31U, n );

#define RGB888_RED   0x00FF0000
#define RGB888_GREEN 0x0000FF00
#define RGB888_BLUE  0x000000FF

void LCD_DPI_Init(void);
void LCD_PWM_Init(void);

#endif
