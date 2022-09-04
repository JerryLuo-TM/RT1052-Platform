#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

#include "include.h"
#include "NRF24L01.h"
#include "string.h"
#include "LED.h"

struct NRF24L01_Par_T NRF24L01_Par;

bool RF_lose=0;
unsigned char TxBuf[32];
unsigned char RxBuf[32];

unsigned char sta=0; //状态标识
#define RX_DR  (sta&0x0040)>>6
#define TX_DS  (sta&0x0020)>>5
#define MAX_RT (sta&0x0010)>>4
#define TX_FULL sta&0x0001
//*********************************************NRF24L01*************************************
unsigned char RF_channel=0x6E;     //无线信道取值0-127
unsigned char RF_Speed=0x06;       //(发射模式下)无线速率 1Mbps-0x06   2Mbps-0x0E  接收模式要+1把LNA给打开
unsigned char PLOAD_WIDTH=32;
//unsigned char TX_ADDRESS[5]= {0x19,0x95,0x08,0x18,0x00};	//NRF24L01发射地址
unsigned char TX_ADDRESS[5]= {0x19,0x95,0x08,0x18,0x00};	//NRF24L01发射地址
unsigned char RX_ADDRESS[5]= {0x19,0x95,0x08,0x18,0x11};	//NRF24L01接收地址

/* 下面五个地址公用四个字节的地址  */
unsigned char RX_ADDRESS_P1[5]={0x19,0x95,0x08,0x18,0x00}; //Data pipe 1地址,5字节有效地址,不过不能与Data pipe 0的高四字节相同
unsigned char RX_ADDRESS_P2[5]={0x19,0x95,0x08,0x18,0x11}; //Data pipe 2地址,与Data pipe 1共享高四字节地址
unsigned char RX_ADDRESS_P3[5]={0x19,0x95,0x08,0x18,0x22}; //Data pipe 3地址,与Data pipe 1共享高四字节地址
unsigned char RX_ADDRESS_P4[5]={0x19,0x95,0x08,0x18,0x33}; //Data pipe 4地址,与Data pipe 1共享高四字节地址
unsigned char RX_ADDRESS_P5[5]={0x19,0x95,0x08,0x18,0x44}; //Data pipe 5地址,与Data pipe 1共享高四字节地址

unsigned char RX_pipe_CH=0x01;	//接收通道
bool CRC_EN=true;    						//false=8bit CRC   true(1)6bit CRC
bool AUTO_ACK_EN=false; 				//自动应答

//60us延时程序
void Delay_60ns(void)
{
	__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();
}

//- {pin_num: H11, pin_signal: GPIO_AD_B1_13, label: NRF_IRQ,  identifier: NRF_IRQ}
//- {pin_num: G12, pin_signal: GPIO_AD_B1_14, label: NRF_MISO, identifier: NRF_MISO}
//- {pin_num: H12, pin_signal: GPIO_AD_B1_12, label: NRF_MOSI, identifier: NRF_MOSI}
//- {pin_num: J14, pin_signal: GPIO_AD_B1_15, label: NRF_SCK,  identifier: NRF_SCK}
//- {pin_num: J13, pin_signal: GPIO_AD_B1_11, label: NRF_CSN,  identifier: NRF_CSN}
//- {pin_num: L12, pin_signal: GPIO_AD_B1_04, label: NRF_CE,   identifier: NRF_CE}
//- NRF24L01接收模块初始化
status_t NRF24L01_Init(unsigned char mode)
{
	PRINTF("\r\n*** NRF24L01 Init Start &SIM_SPI *** \r\n");
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_04_GPIO1_IO20,0U);                                    
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_11_GPIO1_IO27,0U);
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_15_GPIO1_IO31,0U);    
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_12_GPIO1_IO28,0U);
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_14_GPIO1_IO30,0U);  //MISO
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_13_GPIO1_IO29,0U);  //IRQ                            
	
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_04_GPIO1_IO20,0xD0B0u);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_11_GPIO1_IO27,0xD0B0u);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_15_GPIO1_IO31,0xD0B0u);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_12_GPIO1_IO28,0xD0B0u);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_14_GPIO1_IO30,0x10B0u);
  IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_13_GPIO1_IO29,0x10B0u);
	
	//NRF_CE
  gpio_pin_config_t NRF_CE_config = {kGPIO_DigitalOutput,0U,kGPIO_NoIntmode};
  GPIO_PinInit(GPIO1, 20U, &NRF_CE_config);

  //NRF_CSN
  gpio_pin_config_t NRF_CSN_config = {kGPIO_DigitalOutput,0U,kGPIO_NoIntmode};
  GPIO_PinInit(GPIO1, 27U, &NRF_CSN_config);

  //NRF_SCK
  gpio_pin_config_t NRF_SCK_config = {kGPIO_DigitalOutput,0U,kGPIO_NoIntmode};
  GPIO_PinInit(GPIO1, 31U, &NRF_SCK_config);
	
  //NRF_MOSI
  gpio_pin_config_t NRF_MOSI_config = {kGPIO_DigitalOutput,0U,kGPIO_NoIntmode};
  GPIO_PinInit(GPIO1, 28U, &NRF_MOSI_config);

	//NRF_MISO
  gpio_pin_config_t NRF_MISO_config = {kGPIO_DigitalInput,0U,kGPIO_NoIntmode};
  GPIO_PinInit(GPIO1, 30U, &NRF_MISO_config);

  //NRF_IRQ 
  gpio_pin_config_t NRF_IRQ_config = {kGPIO_DigitalInput,0U,kGPIO_NoIntmode};
  GPIO_PinInit(GPIO1, 29U, &NRF_IRQ_config);

	
 	CE(0);    // chip enable
 	CSN(1);   // Spi  disable 
 	SCK(0);   //
	
	SPI_RW_Reg_RX(WRITE_REG + CONFIG2, 0x0E|(~mode));    //寄存器 0x00
	SPI_RW_Reg_RX(FLUSH_TX,0xff);                        //冲洗TX FIFO
	SPI_RW_Reg_RX(FLUSH_RX,0xff);                        //冲洗RX FIFO
	if(AUTO_ACK_EN==true){SPI_RW_Reg_RX(WRITE_REG + EN_AA, 0x3F);} //频道0应答允许 寄存器 0x01
		else {SPI_RW_Reg_RX(WRITE_REG + EN_AA, 0x00);}
	if((RX_pipe_CH&0x01)==0){RX_pipe_CH|=0x01;}            //第一个通道必须打开
	SPI_RW_Reg_RX(WRITE_REG + EN_RXADDR, RX_pipe_CH&0x3F); //通道0允许接收 寄存器 0x02
	SPI_RW_Reg_RX(WRITE_REG + SETUP_AW, 0x03);           //收发地址长度            寄存器 0x03
	SPI_RW_Reg_RX(WRITE_REG + SETUP_RETR, 0x1F); 				 //设置自动重发间隔时间:750us + 86us;自动重发次数:15次  寄存器 0x04
	SPI_RW_Reg_RX(WRITE_REG + RF_CH, RF_channel&0x7F);        //工作频率          寄存器 0x05
	SPI_RW_Reg_RX(WRITE_REG + RF_SETUP, RF_Speed+1);   	 //设置发射速率功率  寄存器 0x06
	SPI_RW_Reg_RX(WRITE_REG + STATUS, 0x7E);             //复位STATUS寄存器以此清除MAX_RT中断标志	
  SPI_RW_Reg_RX(WRITE_REG+OBSERVE_TX,0x00);						 //复位OBSERVE_TX寄存器 
	SPI_RW_Reg_RX(WRITE_REG+CD,0x00);  		    					 //复位CD寄存器 	  载波检测
	
	SPI_Write_Buf_RX(WRITE_REG + TX_ADDR,   TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	SPI_Write_Buf_RX(WRITE_REG + RX_ADDR_P0,RX_ADDRESS, RX_ADR_WIDTH);    //设置RX_P0节点地址  写接收端地址  寄存器0x0A
	
	SPI_Write_Buf_RX(WRITE_REG+RX_ADDR_P1,RX_ADDRESS_P1,RX_ADR_WIDTH); 	//设置RX_P1节点地址		  	  
	SPI_Write_Buf_RX(WRITE_REG+RX_ADDR_P2,RX_ADDRESS_P2,RX_ADR_WIDTH); 	//设置RX_P2节点地址
	SPI_Write_Buf_RX(WRITE_REG+RX_ADDR_P3,RX_ADDRESS_P3,RX_ADR_WIDTH); 	//设置RX_P3节点地址
	SPI_Write_Buf_RX(WRITE_REG+RX_ADDR_P4,RX_ADDRESS_P4,RX_ADR_WIDTH); 	//设置RX_P4节点地址
	SPI_Write_Buf_RX(WRITE_REG+RX_ADDR_P5,RX_ADDRESS_P5,RX_ADR_WIDTH); 	//设置RX_P5节点地址    
	
	SPI_RW_Reg_RX(WRITE_REG+RX_PW_P0,32);  	//设置RX_P0通道接收Payload的字节数32 字节有效数据长度 寄存器 0x11 
	SPI_RW_Reg_RX(WRITE_REG+RX_PW_P1,32);  	//设置RX_P1通道接收Payload的字节数32 
	SPI_RW_Reg_RX(WRITE_REG+RX_PW_P2,32);  	//设置RX_P2通道接收Payload的字节数32 
	SPI_RW_Reg_RX(WRITE_REG+RX_PW_P3,32);  	//设置RX_P3通道接收Payload的字节数32 
	SPI_RW_Reg_RX(WRITE_REG+RX_PW_P4,32);  	//设置RX_P4通道接收Payload的字节数32 
	SPI_RW_Reg_RX(WRITE_REG+RX_PW_P5,32);  	//设置RX_P5通道接收Payload的字节数32 
  
  SPI_RW_Reg_RX(WRITE_REG+FIFO_STATUS,0x11);//复位FIFO_STATUS寄存器	  寄存器0x17
  SPI_RW_Reg_RX(WRITE_REG+0x1c,0x3f);		    //使能RX_P0动态Payload长度
	SPI_RW_Reg_RX(WRITE_REG+0x1d,0x06);
	
	CE(1);
	memset(RxBuf,0,sizeof(RxBuf));
	memset(TxBuf,0,sizeof(TxBuf));
	
	if(NRF24L01_Check()==0)
	{
		NRF24L01.Init_statue=true;
		return kStatus_Success;
	}
	else
	{
		NRF24L01.Init_statue=false;
		return kStatus_Fail;
	}
}

//函数：unsigned int SPI_RW(unsigned int uchar)
//功能：NRF24L01的SPI写时序
unsigned char SPI_RW_RX(unsigned char uchar)
{
	  unsigned char bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
   	{
			if((uchar&0x80)==0x00){MOSI(0);}
			else{MOSI(1);}
			uchar=uchar<<1;
			Delay_60ns();Delay_60ns();
			SCK(1);
			Delay_60ns();Delay_60ns();
			uchar|=MISO;
			SCK(0);
   	}
    return(uchar);
}

//函数：uchar SPI_Read(uchar reg)
//功能：NRF24L01的SPI时序
unsigned char SPI_Read_RX(unsigned char reg)
{
	unsigned char reg_val;
	CSN(0);                // CSN low, initialize SPI communication...
	SPI_RW_RX(reg);            // Select register to read from..
	reg_val = SPI_RW_RX(0);    // ..then read registervalue
	CSN(1);                // CSN high, terminate SPI communication
	return(reg_val);        // return register value
}

//功能：NRF24L01读写寄存器函数
unsigned char SPI_RW_Reg_RX(unsigned char reg, unsigned char value)
{
	unsigned char status;
	CSN(0);                   // CSN low, init SPI transaction
	status = SPI_RW_RX(reg);      // select register
	SPI_RW_RX(value);             // ..and write value to it..
	CSN(1);                   // CSN high again
	return(status);            // return nRF24L01 status uchar
}

//函数：unsigned int SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
//功能: 用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数
unsigned char SPI_Read_Buf_RX(unsigned char reg, unsigned char *pBuf, unsigned char uchars)
{
	unsigned char status,uchar_ctr;
	CSN(0);                    		// Set CSN low, init SPI tranaction
	status = SPI_RW_RX(reg);       		// Select register to write to and read status uchar
	for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
		{pBuf[uchar_ctr] = SPI_RW_RX(0);}    // 
	CSN(1);                           
	return(status);                    // return nRF24L01 status uchar
}

//函数：unsigned int SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
//功能: 用于写数据：为寄存器地址，pBuf：为待写入数据地址，uchars：写入数据的个数
unsigned char SPI_Write_Buf_RX(unsigned char reg, unsigned char *pBuf, unsigned char uchars)
{
	unsigned char status,uchar_ctr;
	CSN(0);            //SPI使能       
	status = SPI_RW_RX(reg);   
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++)  
	{SPI_RW_RX(pBuf[uchar_ctr]);}
	CSN(1);            //关闭SPI
	return(status);    //
}

//函数：unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
//功能：数据读取后放如rx_buf接收缓冲区中
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
  unsigned char revale=0;
	static unsigned char BZ=0;
	sta=SPI_Read_RX(STATUS);	//读取状态寄存其来判断数据接收状况
	if(RX_DR)				//判断是否接收到数据
	{								
	  CE(0); 			  //SPI使能
		SPI_Read_Buf_RX(RD_RX_PLOAD,rx_buf,PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		revale =1;			//读取数据完成标志
	}
	SPI_RW_Reg_RX(WRITE_REG+STATUS,sta);   //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标志
	CE(1);
	if(revale==1){BZ=10;}
	BZ--;
	if(BZ==0){BZ=1;RF_lose=1;}
	else{RF_lose=0;}
	return revale;
}

//检测24L01是否存在
//返回值:0，成功;1，失败
unsigned char NRF24L01_Check(void)
{
	unsigned char ID_ADD[5]={0};
	unsigned char i;
	PRINTF("NRF24L01_Check ... \r\n");
	SPI_Write_Buf_RX(WRITE_REG+TX_ADDR,(unsigned char*)TX_ADDRESS,5);//写入5个字节的地址.	
	SPI_Read_Buf_RX(TX_ADDR,ID_ADD,5); //读出写入的地址  
	for(i=0;i<5;i++)
	{
		PRINTF("TX ADDR Write[%d]=0x%02x , TX ADDR Read[%d]=0x%02x \r\n",i,TX_ADDRESS[i],i,ID_ADD[i]);
		if(ID_ADD[i]!=TX_ADDRESS[i]){break;}
	}
	if(i!=5)
	{
		PRINTF("NRF24L01 Self Check fail! \r\n");
		return 1;  //没检测24L01错误
	}
	PRINTF("NRF24L01 Self Check OK! \r\n");
	return 0;		 //检测到24L01
}

//函数：void nRF24L01_TxPacket(unsigned char * tx_buf)
//功能：发送 tx_buf中数据
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	SPI_RW_Reg_RX(0xE1,0xff); 						//清空发送FIFO指令
	SPI_RW_Reg_RX(WRITE_REG+STATUS,0x70); //状态寄存器 清除所有中断标识	
	CE(0);
	SPI_Write_Buf_RX(WR_TX_PLOAD, tx_buf, PLOAD_WIDTH); 			     
	if(CRC_EN==0){SPI_RW_Reg_RX(WRITE_REG + CONFIG2, 0x72);}  //屏蔽所有中断+关闭CRC+上电
	else{SPI_RW_Reg_RX(WRITE_REG + CONFIG2, 0x7E);}           //屏蔽所有中断+打开CRC（2字节）+上电
	CE(1);
}



