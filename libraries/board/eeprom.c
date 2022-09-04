#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "i2c.h"
#include "eeprom.h"

status_t I2C_EEPROM_TEST(void)
{
	uint8_t dataBuff[8] = {0};
	
	/*给数组赋值01234567*/
	for(uint8_t i=0;i<8;i++)
	{
		dataBuff[i] = i;
	}
	
	if(!I2C_Write(AT24C256_ADDR,0,dataBuff,8))//写数据
	{
		//PRINTF("Write to EEPROM Sucess!!! dataBuff = %d , %d , %d , %d , %d , %d , %d , %d\r\n",dataBuff[0],dataBuff[1],dataBuff[2],dataBuff[3],dataBuff[4],dataBuff[5],dataBuff[6],dataBuff[7]);
	}
	else
	{
		return kStatus_Fail;
	}
	
	memset(dataBuff,0,8);	//清空数据
	
	if(!I2C_Read(AT24C256_ADDR,0,dataBuff,8))//读数据
	{
		//PRINTF("Read from EEPROM Sucess!!! dataBuff = %d , %d , %d , %d , %d , %d , %d , %d\r\n",dataBuff[0],dataBuff[1],dataBuff[2],dataBuff[3],dataBuff[4],dataBuff[5],dataBuff[6],dataBuff[7]);
	}
	else
	{
		return kStatus_Fail;
	}
	
	return kStatus_Success;
}

