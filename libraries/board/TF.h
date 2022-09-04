#ifndef _TF_H_
#define _TF_H_

#include "fsl_common.h"

status_t sdcardWaitCardInsert(void);

void BOARD_USDHCClockConfiguration(void);
int SD_MOUNT(void);

#endif

