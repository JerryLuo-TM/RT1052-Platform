#include "fsl_common.h"

#ifndef _EMWIN_SUPPORT_H_
#define _EMWIN_SUPPORT_H_

#define LCD_WIDTH  854
#define LCD_HEIGHT 480

/* Use larger fonts */
#define GUI_NORMAL_FONT (&GUI_Font24_ASCII)
#define GUI_LARGE_FONT (&GUI_Font32B_ASCII)

extern uint8_t *s_vram_buffer;

extern int BOARD_Touch_Poll(void);
extern volatile bool g_InputSignal;
extern volatile bool g_TouchPadInputSignal;

void BOARD_InitLcdifPixelClock(void);
void APP_ELCDIF_Init(void);

#endif
