#include "w25qxx.h"
#include "his_spi.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//W25QXX ��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
#define W25QXX_READ_PERNUM (128)
WV_U16 W25QXX_TYPE = W25Q128; //Ĭ����W25Q128

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector

//��ʼ��SPI FLASH��IO��
// void W25QXX_Init(void)
// {
// 	//RCC->AHB1ENR|=1<<1;     //ʹ��PORTBʱ��
// 	RCC->AHB1ENR|=1<<6;     //ʹ��PORTGʱ��
// 	GPIO_Set(GPIOB,PIN14,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB14�������
// 	GPIO_Set(GPIOG,PIN7,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PG7�������
// 	GPIOG->ODR|=1<<7;		//PG7���1,��ֹNRF����SPI FLASH��ͨ��
// 	W25QXX_CS=1;			//SPI FLASH��ѡ��
// 	SPI1_Init();		   			//��ʼ��SPI
// 	SPI1_SetSpeed(SPI_SPEED_4);		//����Ϊ21Mʱ��,����ģʽ
// 	W25QXX_TYPE=W25QXX_ReadID();	//��ȡFLASH ID.
// }

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
WV_U8 W25QXX_ReadSR(void)
{
	//W25QXX_CS=0;                            //ʹ������
	//SPI1_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������
	//byte=SPI1_ReadWriteByte(0Xff);             //��ȡһ���ֽ�
	//W25QXX_CS=1;                            //ȡ��Ƭѡ

	WV_U8 byte = 0;
	WV_U8 writeBuf[4] = {0};
	WV_U8 readBuf[4] = {0};
	writeBuf[0] = W25X_ReadStatusReg;
	HIS_SPI_Write_then_Read(writeBuf, 1, readBuf, 1);
	byte = readBuf[0];
	return byte;
}
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(WV_U8 sr)
{
	//W25QXX_CS=0;                            //ʹ������
	//SPI1_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������
	//SPI1_ReadWriteByte(sr);               //д��һ���ֽ�
	//W25QXX_CS=1;                            //ȡ��Ƭѡ
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_WriteStatusReg;
	writeBuf[1] = sr;
	HIS_SPI_Write(writeBuf, 2);
}
//W25QXXдʹ��
//��WEL��λ
void W25QXX_Write_Enable(void)
{
	//W25QXX_CS=0;                            //ʹ������
	//SPI1_ReadWriteByte(W25X_WriteEnable);      //����дʹ��
	//W25QXX_CS=1;                            //ȡ��Ƭѡ
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_WriteEnable;
	HIS_SPI_Write(writeBuf, 1);
}
//W25QXXд��ֹ
//��WEL����
void W25QXX_Write_Disable(void)
{
	// W25QXX_CS=0;                            //ʹ������
	// SPI1_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��
	// W25QXX_CS=1;                            //ȡ��Ƭѡ
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_WriteDisable;
	HIS_SPI_Write(writeBuf, 1);
}
//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
WV_U16 W25QXX_ReadID(void)
{
	// WV_U16 Temp = 0;
	// W25QXX_CS=0;
	// SPI1_ReadWriteByte(0x90);//���Ͷ�ȡID����
	// SPI1_ReadWriteByte(0x00);
	// SPI1_ReadWriteByte(0x00);
	// SPI1_ReadWriteByte(0x00);
	// Temp|=SPI1_ReadWriteByte(0xFF)<<8;
	// Temp|=SPI1_ReadWriteByte(0xFF);
	// W25QXX_CS=1;
	// return Temp;
	WV_U16 Temp = 0;
	WV_U8 writeBuf[4] = {0};
	WV_U8 readBuf[4] = {0};
	writeBuf[0] = W25X_ManufactDeviceID;
	HIS_SPI_Write_then_Read(writeBuf, 4, readBuf, 2);
	Temp |= (readBuf[0] << 8);
	Temp |= readBuf[1];
	return Temp;
}
//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(WV_U8 *pBuffer, WV_U32 ReadAddr, WV_U16 NumByteToRead)
{
	// WV_U16 i;
	// W25QXX_CS=0;                            //ʹ������
	// SPI1_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����
	// SPI1_ReadWriteByte((WV_U8)((ReadAddr)>>16));  //����24bit��ַ
	// SPI1_ReadWriteByte((WV_U8)((ReadAddr)>>8));
	// SPI1_ReadWriteByte((WV_U8)ReadAddr);
	// for(i=0;i<NumByteToRead;i++)
	// {
	//     pBuffer[i]=SPI1_ReadWriteByte(0XFF);   //ѭ������
	// }
	// W25QXX_CS=1;

	WV_S32 i, ReadTimes, readEndNum;
	ReadTimes = NumByteToRead / W25QXX_READ_PERNUM;
	readEndNum = NumByteToRead % W25QXX_READ_PERNUM;
	WV_U32 addr = ReadAddr;
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_ReadData;
	//ÿ�ζ�ȡ128���ֽ�
	for (i = 0; i < ReadTimes; i++)
	{
		addr = ReadAddr + i * W25QXX_READ_PERNUM;
		writeBuf[1] = (WV_U8)((addr) >> 16);
		writeBuf[2] = (WV_U8)((addr) >> 8);
		writeBuf[3] = (WV_U8)(addr);
		HIS_SPI_Write_then_Read(writeBuf, 4, &pBuffer[i * W25QXX_READ_PERNUM], W25QXX_READ_PERNUM);
		//WV_printf("\ni=%d,readTime=%d,addr=%X,\n",i,ReadTimes,addr);
		// for(j=0;j<W25QXX_READ_PERNUM;j++)
		// {
		// 	WV_printf("%d=%02x ",i * W25QXX_READ_PERNUM+j,pBuffer[i * W25QXX_READ_PERNUM+j]);
		// }
		// WV_printf("\r\n");
		
	}

	if(readEndNum == 0) return;
	//���ʣ�಻��128���ֽڣ���ʵ���ֽ�����ȡ
	addr = ReadAddr + i * W25QXX_READ_PERNUM;
	writeBuf[1] = (WV_U8)((addr) >> 16);
	writeBuf[2] = (WV_U8)((addr) >> 8);
	writeBuf[3] = (WV_U8)(addr);
	HIS_SPI_Write_then_Read(writeBuf, 4, &pBuffer[i * W25QXX_READ_PERNUM], readEndNum);
}
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(WV_U8 *pBuffer, WV_U32 WriteAddr, WV_U16 NumByteToWrite)
{
	// WV_U16 i;
	// W25QXX_Write_Enable();                  //SET WEL
	// W25QXX_CS=0;                            //ʹ������
	// SPI1_ReadWriteByte(W25X_PageProgram);      //����дҳ����
	// SPI1_ReadWriteByte((WV_U8)((WriteAddr)>>16)); //����24bit��ַ
	// SPI1_ReadWriteByte((WV_U8)((WriteAddr)>>8));
	// SPI1_ReadWriteByte((WV_U8)WriteAddr);
	// for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//ѭ��д��
	// W25QXX_CS=1;                            //ȡ��Ƭѡ
	// W25QXX_Wait_Busy();					   //�ȴ�д�����


	if (NumByteToWrite > 256)
	{
		WV_ERROR("W25QXX_Write_Page Ҫд����ֽ��� ����256");
		return;
	}

	WV_U32 writeNumOneTime = 64;
	WV_U32 writeTimes = NumByteToWrite / writeNumOneTime;
	WV_U32 writeEndNum = NumByteToWrite % writeNumOneTime;
	WV_U8 writeBuf[64 + 8] = {0};
	WV_U32 addr;
	WV_S32 i;
	//дʹ��
	W25QXX_Write_Enable();
	for (i = 0; i < writeTimes; i++)
	{
		W25QXX_Write_Enable();
		memset(writeBuf, 0xff, 64 + 8);
		addr = WriteAddr + i * writeNumOneTime;
		writeBuf[0] = W25X_PageProgram;
		writeBuf[1] = (WV_U8)((addr) >> 16);
		writeBuf[2] = (WV_U8)((addr) >> 8);
		writeBuf[3] = (WV_U8)addr;

		memcpy(&writeBuf[4], &pBuffer[i * writeNumOneTime], writeNumOneTime);
		HIS_SPI_Write(writeBuf, 4 + writeNumOneTime);
		W25QXX_Wait_Busy();
		W25QXX_Write_Disable();
		
	}
	if(writeEndNum == 0) return;
	 W25QXX_Write_Enable();
	//д��ʣ�಻��64�ֽڵ�����
	memset(writeBuf, 0xff, 64 + 8);
	addr = WriteAddr + i * writeNumOneTime;
	writeBuf[0] = W25X_PageProgram;
	writeBuf[1] = (WV_U8)((addr) >> 16);
	writeBuf[2] = (WV_U8)((addr) >> 8);
	writeBuf[3] = (WV_U8)addr;
	memcpy(&writeBuf[4], &pBuffer[i * writeNumOneTime], writeEndNum);
	HIS_SPI_Write(writeBuf, 4 + writeEndNum);
	W25QXX_Wait_Busy();
	W25QXX_Write_Disable();
}
//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(WV_U8 *pBuffer, WV_U32 WriteAddr, WV_U16 NumByteToWrite)
{
	WV_U16 pageremain;
	pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���
	if (NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite; //������256���ֽ�
	while (1)
	{
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break; //д�������
		else	   //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain; //��ȥ�Ѿ�д���˵��ֽ���
			if (NumByteToWrite > 256)
				pageremain = 256; //һ�ο���д��256���ֽ�
			else
				pageremain = NumByteToWrite; //����256���ֽ���
		}
	};
}
//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
WV_U8 W25QXX_BUFFER[4096];
void W25QXX_Write(WV_U8 *pBuffer, WV_U32 WriteAddr, WV_U16 NumByteToWrite)
{
	WV_U32 secpos;
	WV_U16 secoff;
	WV_U16 secremain;
	WV_U16 i;
	WV_U8 *W25QXX_BUF;
	W25QXX_BUF = W25QXX_BUFFER;
	secpos = WriteAddr / 4096; //������ַ
	secoff = WriteAddr % 4096; //�������ڵ�ƫ��
	secremain = 4096 - secoff; //����ʣ��ռ��С
	if (NumByteToWrite <= secremain)
		secremain = NumByteToWrite; //������4096���ֽ�
	while (1)
	{
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); //������������������
		for (i = 0; i < secremain; i++)				  //У������
		{
			if (W25QXX_BUF[secoff + i] != 0XFF)
				break; //��Ҫ����
		}
		if (i < secremain) //��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);	//�����������
			for (i = 0; i < secremain; i++) //����
			{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); //д����������
		}
		else
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); //д�Ѿ������˵�,ֱ��д������ʣ������.
		if (NumByteToWrite == secremain)
			break; //д�������
		else	   //д��δ����
		{
			secpos++;   //������ַ��1
			secoff = 0; //ƫ��λ��Ϊ0

			pBuffer += secremain;		 //ָ��ƫ��
			WriteAddr += secremain;		 //д��ַƫ��
			NumByteToWrite -= secremain; //�ֽ����ݼ�
			if (NumByteToWrite > 4096)
				secremain = 4096; //��һ����������д����
			else
				secremain = NumByteToWrite; //��һ����������д����
		}
	};
}
//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)
{
	// W25QXX_Write_Enable();                  //SET WEL
	// W25QXX_Wait_Busy();
	// W25QXX_CS=0;                            //ʹ������
	// SPI1_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������
	// W25QXX_CS=1;                            //ȡ��Ƭѡ
	// W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
	W25QXX_Write_Enable();
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_ChipErase;
	HIS_SPI_Write(writeBuf, 1);
	W25QXX_Wait_Busy();
}
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(WV_U32 Dst_Addr)
{
	//����falsh�������,������
	// printf("fe:%x\r\n",Dst_Addr);
	// Dst_Addr*=4096;
	// W25QXX_Write_Enable();                  //SET WEL
	// W25QXX_Wait_Busy();
	// W25QXX_CS=0;                            //ʹ������
	// SPI1_ReadWriteByte(W25X_SectorErase);      //������������ָ��
	// SPI1_ReadWriteByte((WV_U8)((Dst_Addr)>>16));  //����24bit��ַ
	// SPI1_ReadWriteByte((WV_U8)((Dst_Addr)>>8));
	// SPI1_ReadWriteByte((WV_U8)Dst_Addr);
	// W25QXX_CS=1;                            //ȡ��Ƭѡ
	// W25QXX_Wait_Busy();   				   //�ȴ��������
	WV_U32 addr;
	WV_U8 writeBuf[4] = {0};
	addr = Dst_Addr * W25Q128_SectorSize;
	writeBuf[0] = W25X_SectorErase;
	writeBuf[1] = (WV_U8)((addr) >> 16);
	writeBuf[2] = (WV_U8)((addr) >> 8);
	writeBuf[3] = (WV_U8)addr;
	W25QXX_Write_Enable();
	W25QXX_Wait_Busy();
	//���Ͳ�������
	HIS_SPI_Write(writeBuf, 4);
	W25QXX_Wait_Busy();
}

//����һ����
//Dst_Addr:���ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Block(WV_U32 Dst_Addr)
{
	//����falsh�������,������
	// printf("fe:%x\r\n",Dst_Addr);
	// Dst_Addr*=4096;
	// W25QXX_Write_Enable();                  //SET WEL
	// W25QXX_Wait_Busy();
	// W25QXX_CS=0;                            //ʹ������
	// SPI1_ReadWriteByte(W25X_SectorErase);      //������������ָ��
	// SPI1_ReadWriteByte((WV_U8)((Dst_Addr)>>16));  //����24bit��ַ
	// SPI1_ReadWriteByte((WV_U8)((Dst_Addr)>>8));
	// SPI1_ReadWriteByte((WV_U8)Dst_Addr);
	// W25QXX_CS=1;                            //ȡ��Ƭѡ
	// W25QXX_Wait_Busy();   				   //�ȴ��������
	WV_U32 addr;
	WV_U8 writeBuf[4] = {0};
	addr = Dst_Addr * W25Q128_BlockSize;
	writeBuf[0] = W25X_BlockErase;
	writeBuf[1] = (WV_U8)((addr) >> 16);
	writeBuf[2] = (WV_U8)((addr) >> 8);
	writeBuf[3] = (WV_U8)addr;
	W25QXX_Write_Enable();
	W25QXX_Wait_Busy();
	//���Ͳ�������
	HIS_SPI_Write(writeBuf, 4);
	W25QXX_Wait_Busy();
}

//�ȴ�����
void W25QXX_Wait_Busy(void)
{
	while ((W25QXX_ReadSR() & 0x01) == 0x01){
		usleep(1000);
	}
		 // �ȴ�BUSYλ���
}
//�������ģʽ
void W25QXX_PowerDown(void)
{
	// W25QXX_CS=0;                            //ʹ������
	// SPI1_ReadWriteByte(W25X_PowerDown);        //���͵�������
	// W25QXX_CS=1;                            //ȡ��Ƭѡ
	// delay_us(3);                               //�ȴ�TPD
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_PowerDown;
	HIS_SPI_Write(writeBuf, 1);
	usleep(3000);
}
//����
void W25QXX_WAKEUP(void)
{
	// W25QXX_CS=0;                            //ʹ������
	// SPI1_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB
	// W25QXX_CS=1;                            //ȡ��Ƭѡ
	// delay_us(3);                               //�ȴ�TRES1
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_ReleasePowerDown;
	HIS_SPI_Write(writeBuf, 1);
	usleep(3000);
}
