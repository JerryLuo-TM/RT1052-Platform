#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "fsl_common.h"

//NRF24L01����ģʽ
#define RX 0
#define TX 1

#define   CE(n)     (n?GPIO_PinWrite(GPIO1,20U,1):GPIO_PinWrite(GPIO1,20U,0))
#define   CSN(n)    (n?GPIO_PinWrite(GPIO1,27U,1):GPIO_PinWrite(GPIO1,27U,0))
#define   SCK(n)    (n?GPIO_PinWrite(GPIO1,31U,1):GPIO_PinWrite(GPIO1,31U,0))
#define   MOSI(n)   (n?GPIO_PinWrite(GPIO1,28U,1):GPIO_PinWrite(GPIO1,28U,0))
#define   MISO      GPIO_PinRead(GPIO1,30U)
//#define   IRQ       GPIO_PinRead(GPIO1,29U)

//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define TX_ADR_WIDTH    5   	 
#define RX_ADR_WIDTH    5   	 
#define READ_REG        0x00  	// ���Ĵ���ָ��
#define WRITE_REG       0x20 	  // д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	  // ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP             0xFF  	// ����
#define MAX_TX  				0x10    //�ﵽ����ʹ����ж�
#define TX_OK   				0x20    //TX��������ж�
#define RX_OK   				0x40    //���յ������ж�
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG2         0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������

//*********************************************NRF24L01*************************************
extern unsigned char RF_channel;     //�����ŵ�ȡֵ0-127
extern unsigned char RF_Speed;       //(����ģʽ��)�������� 1Mbps-0x06   2Mbps-0x0E  ����ģʽҪ+1��LNA����
extern unsigned char PLOAD_WIDTH; 
extern unsigned char TX_ADDRESS[5];	//NRF24L01�����ַ
extern unsigned char RX_ADDRESS[5];	//NRF24L01���յ�ַ

/* ���������ַ�����ĸ��ֽڵĵ�ַ  */
extern unsigned char RX_ADDRESS_P1[5]; //Data pipe 1��ַ,5�ֽ���Ч��ַ,����������Data pipe 0�ĸ����ֽ���ͬ
extern unsigned char RX_ADDRESS_P2[5]; //Data pipe 2��ַ,��Data pipe 1��������ֽڵ�ַ
extern unsigned char RX_ADDRESS_P3[5]; //Data pipe 3��ַ,��Data pipe 1��������ֽڵ�ַ
extern unsigned char RX_ADDRESS_P4[5]; //Data pipe 4��ַ,��Data pipe 1��������ֽڵ�ַ
extern unsigned char RX_ADDRESS_P5[5]; //Data pipe 5��ַ,��Data pipe 1��������ֽڵ�ַ

extern unsigned char RX_pipe_CH;
extern bool CRC_EN;  //false=8bit CRC   true(1)6bit CRC
extern bool AUTO_ACK_EN;
extern bool RF_lose;

struct NRF24L01_Par_T
{
	unsigned char RF_Speed; 		  //ͨѶ����
	bool CRC_EN;									//CRCУ��
	bool AUTO_ACK_EN;             //�Զ�Ӧ��
	unsigned char RX_pipe_CH; 		//����ͨ��
	unsigned char PLOAD_WIDTH;    //�غɰ���
	unsigned char RF_channel;			//�����ŵ�
	unsigned char RX_ADDRESS[5];			//���յ�ַ
	unsigned char TX_ADDRESS[5];			//���͵�ַ
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
