#ifndef _SDRAM_H_
#define _SDRAM_H_

#include "fsl_common.h"

status_t BOARD_InitSEMC(void);
void SEMC_SDRAMReadWrite32Bit(void);
void SEMC_SDRAMReadWrite16Bit(void);
void SEMC_SDRAMReadWrite8Bit(void);




#endif


