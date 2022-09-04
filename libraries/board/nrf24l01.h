#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "fsl_common.h"

//NRF24L01工作模式
#define RX 0
#define TX 1

#define   CE(n)     (n?GPIO_PinWrite(GPIO1,20U,1):GPIO_PinWrite(GPIO1,20U,0))
#define   CSN(n)    (n?GPIO_PinWrite(GPIO1,27U,1):GPIO_PinWrite(GPIO1,27U,0))
#define   SCK(n)    (n?GPIO_PinWrite(GPIO1,31U,1):GPIO_PinWrite(GPIO1,31U,0))
#define   MOSI(n)   (n?GPIO_PinWrite(GPIO1,28U,1):GPIO_PinWrite(GPIO1,28U,0))
#define   MISO      GPIO_PinRead(GPIO1,30U)
//#define   IRQ       GPIO_PinRead(GPIO1,29U)

//***************************************NRF24L01寄存器指令*******************************************************
#define TX_ADR_WIDTH    5   	 
#define RX_ADR_WIDTH    5   	 
#define READ_REG        0x00  	// 读寄存器指令
#define WRITE_REG       0x20 	  // 写寄存器指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xA0  	// 写待发数据指令
#define FLUSH_TX        0xE1 	  // 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define NOP             0xFF  	// 保留
#define MAX_TX  				0x10    //达到最大发送次数中断
#define TX_OK   				0x20    //TX发送完成中断
#define RX_OK   				0x40    //接收到数据中断
//*************************************SPI(nRF24L01)寄存器地址****************************************************
#define CONFIG2         0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define STATUS          0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道0接收数据长度
#define RX_PW_P2        0x13  // 接收频道0接收数据长度
#define RX_PW_P3        0x14  // 接收频道0接收数据长度
#define RX_PW_P4        0x15  // 接收频道0接收数据长度
#define RX_PW_P5        0x16  // 接收频道0接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置

//*********************************************NRF24L01*************************************
extern unsigned char RF_channel;     //无线信道取值0-127
extern unsigned char RF_Speed;       //(发射模式下)无线速率 1Mbps-0x06   2Mbps-0x0E  接收模式要+1把LNA给打开
extern unsigned char PLOAD_WIDTH; 
extern unsigned char TX_ADDRESS[5];	//NRF24L01发射地址
extern unsigned char RX_ADDRESS[5];	//NRF24L01接收地址

/* 下面五个地址公用四个字节的地址  */
extern unsigned char RX_ADDRESS_P1[5]; //Data pipe 1地址,5字节有效地址,不过不能与Data pipe 0的高四字节相同
extern unsigned char RX_ADDRESS_P2[5]; //Data pipe 2地址,与Data pipe 1共享高四字节地址
extern unsigned char RX_ADDRESS_P3[5]; //Data pipe 3地址,与Data pipe 1共享高四字节地址
extern unsigned char RX_ADDRESS_P4[5]; //Data pipe 4地址,与Data pipe 1共享高四字节地址
extern unsigned char RX_ADDRESS_P5[5]; //Data pipe 5地址,与Data pipe 1共享高四字节地址

extern unsigned char RX_pipe_CH;
extern bool CRC_EN;  //false=8bit CRC   true(1)6bit CRC
extern bool AUTO_ACK_EN;
extern bool RF_lose;

struct NRF24L01_Par_T
{
	unsigned char RF_Speed; 		  //通讯速率
	bool CRC_EN;									//CRC校验
	bool AUTO_ACK_EN;             //自动应答
	unsigned char RX_pipe_CH; 		//接收通道
	unsigned char PLOAD_WIDTH;    //载荷包长
	unsigned char RF_channel;			//无线信道
	unsigned char RX_ADDRESS[5];			//接收地址
	unsigned char TX_ADDRESS[5];			//发送地址
};
extern struct NRF24L01_Par_T NRF24L01_Par;

void Delay_60ns(void);
status_t NRF24L01_Init(unsigned char mode);
unsigned char SPI_RW_RX(unsigned char uchar);
unsigned char SPI_Read_RX(unsigned char reg);
unsigned char SPI_RW_Reg_RX(unsigned char reg, unsigned char value);
unsigned char SPI_Read_Buf_RX(unsigned char reg, unsigned char *pBuf, unsigned char uchars);
unsigned char SPI_Write_Buf_RX(unsigned char reg, unsigned char *pBuf, unsigned char uchars);
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf);
void nRF24L01_TxPacket(unsigned char * tx_buf);


unsigned char NRF24L01_Check(void);

extern unsigned char TxBuf[32];
extern unsigned char RxBuf[32];

void NRF24L01_Task(void *pvParameters);

#endif  
