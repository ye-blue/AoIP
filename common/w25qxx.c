#include "w25qxx.h"
#include "his_spi.h"

//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
#define W25QXX_READ_PERNUM (128)
WV_U16 W25QXX_TYPE = W25Q128; //默认是W25Q128

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector

//初始化SPI FLASH的IO口
// void W25QXX_Init(void)
// {
// 	//RCC->AHB1ENR|=1<<1;     //使能PORTB时钟
// 	RCC->AHB1ENR|=1<<6;     //使能PORTG时钟
// 	GPIO_Set(GPIOB,PIN14,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB14推挽输出
// 	GPIO_Set(GPIOG,PIN7,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PG7推挽输出
// 	GPIOG->ODR|=1<<7;		//PG7输出1,防止NRF干扰SPI FLASH的通信
// 	W25QXX_CS=1;			//SPI FLASH不选中
// 	SPI1_Init();		   			//初始化SPI
// 	SPI1_SetSpeed(SPI_SPEED_4);		//设置为21M时钟,高速模式
// 	W25QXX_TYPE=W25QXX_ReadID();	//读取FLASH ID.
// }

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
WV_U8 W25QXX_ReadSR(void)
{
	//W25QXX_CS=0;                            //使能器件
	//SPI1_ReadWriteByte(W25X_ReadStatusReg);    //发送读取状态寄存器命令
	//byte=SPI1_ReadWriteByte(0Xff);             //读取一个字节
	//W25QXX_CS=1;                            //取消片选

	WV_U8 byte = 0;
	WV_U8 writeBuf[4] = {0};
	WV_U8 readBuf[4] = {0};
	writeBuf[0] = W25X_ReadStatusReg;
	HIS_SPI_Write_then_Read(writeBuf, 1, readBuf, 1);
	byte = readBuf[0];
	return byte;
}
//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25QXX_Write_SR(WV_U8 sr)
{
	//W25QXX_CS=0;                            //使能器件
	//SPI1_ReadWriteByte(W25X_WriteStatusReg);   //发送写取状态寄存器命令
	//SPI1_ReadWriteByte(sr);               //写入一个字节
	//W25QXX_CS=1;                            //取消片选
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_WriteStatusReg;
	writeBuf[1] = sr;
	HIS_SPI_Write(writeBuf, 2);
}
//W25QXX写使能
//将WEL置位
void W25QXX_Write_Enable(void)
{
	//usleep(1000);
	//W25QXX_CS=0;                            //使能器件
	//SPI1_ReadWriteByte(W25X_WriteEnable);      //发送写使能
	//W25QXX_CS=1;                            //取消片选
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_WriteEnable;
	HIS_SPI_Write(writeBuf, 1);
	//usleep(10000);
}
//W25QXX写禁止
//将WEL清零
void W25QXX_Write_Disable(void)
{
	// W25QXX_CS=0;                            //使能器件
	// SPI1_ReadWriteByte(W25X_WriteDisable);     //发送写禁止指令
	// W25QXX_CS=1;                            //取消片选
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_WriteDisable;
	HIS_SPI_Write(writeBuf, 1);
	//usleep(10000);
}
//读取芯片ID
//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
WV_U16 W25QXX_ReadID(void)
{
	// WV_U16 Temp = 0;
	// W25QXX_CS=0;
	// SPI1_ReadWriteByte(0x90);//发送读取ID命令
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
//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(WV_U8 *pBuffer, WV_U32 ReadAddr, WV_U16 NumByteToRead)
{
	// WV_U16 i;
	// W25QXX_CS=0;                            //使能器件
	// SPI1_ReadWriteByte(W25X_ReadData);         //发送读取命令
	// SPI1_ReadWriteByte((WV_U8)((ReadAddr)>>16));  //发送24bit地址
	// SPI1_ReadWriteByte((WV_U8)((ReadAddr)>>8));
	// SPI1_ReadWriteByte((WV_U8)ReadAddr);
	// for(i=0;i<NumByteToRead;i++)
	// {
	//     pBuffer[i]=SPI1_ReadWriteByte(0XFF);   //循环读数
	// }
	// W25QXX_CS=1;

	WV_S32 i, ReadTimes, readEndNum;
	ReadTimes = NumByteToRead / W25QXX_READ_PERNUM;
	readEndNum = NumByteToRead % W25QXX_READ_PERNUM;
	WV_U32 addr = ReadAddr;
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_ReadData;
	//每次读取128个字节
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
	//最后剩余不足128个字节，按实际字节数读取
	addr = ReadAddr + i * W25QXX_READ_PERNUM;
	writeBuf[1] = (WV_U8)((addr) >> 16);
	writeBuf[2] = (WV_U8)((addr) >> 8);
	writeBuf[3] = (WV_U8)(addr);
	HIS_SPI_Write_then_Read(writeBuf, 4, &pBuffer[i * W25QXX_READ_PERNUM], readEndNum);
}
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
#define _W25QXX_WRITE_NUM_ (128)
void W25QXX_Write_Page(WV_U8 *pBuffer, WV_U32 WriteAddr, WV_U16 NumByteToWrite)
{
	// WV_U16 i;
	// W25QXX_Write_Enable();                  //SET WEL
	// W25QXX_CS=0;                            //使能器件
	// SPI1_ReadWriteByte(W25X_PageProgram);      //发送写页命令
	// SPI1_ReadWriteByte((WV_U8)((WriteAddr)>>16)); //发送24bit地址
	// SPI1_ReadWriteByte((WV_U8)((WriteAddr)>>8));
	// SPI1_ReadWriteByte((WV_U8)WriteAddr);
	// for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//循环写数
	// W25QXX_CS=1;                            //取消片选
	// W25QXX_Wait_Busy();					   //等待写入结束


	if (NumByteToWrite > 256)
	{
		WV_ERROR("W25QXX_Write_Page 要写入的字节数 超过256");
		return;
	}

	WV_U32 writeNumOneTime = _W25QXX_WRITE_NUM_;
	WV_U32 writeTimes = NumByteToWrite / writeNumOneTime;
	WV_U32 writeEndNum = NumByteToWrite % writeNumOneTime;
	WV_U8 writeBuf[_W25QXX_WRITE_NUM_ + 8] = {0};
	WV_U32 addr;
	WV_S32 i;
	//写使能
	//W25QXX_Write_Enable();
	for (i = 0; i < writeTimes; i++)
	{
		W25QXX_Write_Enable();
		W25QXX_Wait_Busy();
		memset(writeBuf, 0xff, _W25QXX_WRITE_NUM_ + 8);
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
	 W25QXX_Wait_Busy();
	//写入剩余不足64字节的数据
	memset(writeBuf, 0xff, _W25QXX_WRITE_NUM_ + 8);
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
//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(WV_U8 *pBuffer, WV_U32 WriteAddr, WV_U16 NumByteToWrite)
{
	WV_U16 pageremain;
	pageremain = 256 - WriteAddr % 256; //单页剩余的字节数
	if (NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite; //不大于256个字节
	while (1)
	{
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break; //写入结束了
		else	   //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain; //减去已经写入了的字节数
			if (NumByteToWrite > 256)
				pageremain = 256; //一次可以写入256个字节
			else
				pageremain = NumByteToWrite; //不够256个字节了
		}
	};
}
//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
WV_U8 W25QXX_BUFFER[4096];
void W25QXX_Write(WV_U8 *pBuffer, WV_U32 WriteAddr, WV_U16 NumByteToWrite)
{
	WV_U32 secpos;
	WV_U16 secoff;
	WV_U16 secremain;
	WV_U16 i;
	WV_U8 *W25QXX_BUF;
	W25QXX_BUF = W25QXX_BUFFER;
	secpos = WriteAddr / 4096; //扇区地址
	secoff = WriteAddr % 4096; //在扇区内的偏移
	secremain = 4096 - secoff; //扇区剩余空间大小
	if (NumByteToWrite <= secremain)
		secremain = NumByteToWrite; //不大于4096个字节
	while (1)
	{
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); //读出整个扇区的内容
		for (i = 0; i < secremain; i++)				  //校验数据
		{
			if (W25QXX_BUF[secoff + i] != 0XFF)
				break; //需要擦除
		}
		if (i < secremain) //需要擦除
		{
			W25QXX_Erase_Sector(secpos);	//擦除这个扇区
			for (i = 0; i < secremain; i++) //复制
			{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); //写入整个扇区
		}
		else
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); //写已经擦除了的,直接写入扇区剩余区间.
		if (NumByteToWrite == secremain)
			break; //写入结束了
		else	   //写入未结束
		{
			secpos++;   //扇区地址增1
			secoff = 0; //偏移位置为0

			pBuffer += secremain;		 //指针偏移
			WriteAddr += secremain;		 //写地址偏移
			NumByteToWrite -= secremain; //字节数递减
			if (NumByteToWrite > 4096)
				secremain = 4096; //下一个扇区还是写不完
			else
				secremain = NumByteToWrite; //下一个扇区可以写完了
		}
	};
}
//擦除整个芯片
//等待时间超长...
void W25QXX_Erase_Chip(void)
{
	// W25QXX_Write_Enable();                  //SET WEL
	// W25QXX_Wait_Busy();
	// W25QXX_CS=0;                            //使能器件
	// SPI1_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令
	// W25QXX_CS=1;                            //取消片选
	// W25QXX_Wait_Busy();   				   //等待芯片擦除结束
	W25QXX_Write_Enable();
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_ChipErase;
	HIS_SPI_Write(writeBuf, 1);
	W25QXX_Wait_Busy();
}
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_Erase_Sector(WV_U32 Dst_Addr)
{
	//监视falsh擦除情况,测试用
	// printf("fe:%x\r\n",Dst_Addr);
	// Dst_Addr*=4096;
	// W25QXX_Write_Enable();                  //SET WEL
	// W25QXX_Wait_Busy();
	// W25QXX_CS=0;                            //使能器件
	// SPI1_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令
	// SPI1_ReadWriteByte((WV_U8)((Dst_Addr)>>16));  //发送24bit地址
	// SPI1_ReadWriteByte((WV_U8)((Dst_Addr)>>8));
	// SPI1_ReadWriteByte((WV_U8)Dst_Addr);
	// W25QXX_CS=1;                            //取消片选
	// W25QXX_Wait_Busy();   				   //等待擦除完成
	WV_U32 addr;
	WV_U8 writeBuf[4] = {0};
	addr = Dst_Addr * W25Q128_SectorSize;
	writeBuf[0] = W25X_SectorErase;
	writeBuf[1] = (WV_U8)((addr) >> 16);
	writeBuf[2] = (WV_U8)((addr) >> 8);
	writeBuf[3] = (WV_U8)addr;
	W25QXX_Write_Enable();
	W25QXX_Wait_Busy();
	//发送擦除命令
	HIS_SPI_Write(writeBuf, 4);
	W25QXX_Wait_Busy();
}

//擦除一个块
//Dst_Addr:块地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_Erase_Block(WV_U32 Dst_Addr)
{
	//监视falsh擦除情况,测试用
	// printf("fe:%x\r\n",Dst_Addr);
	// Dst_Addr*=4096;
	// W25QXX_Write_Enable();                  //SET WEL
	// W25QXX_Wait_Busy();
	// W25QXX_CS=0;                            //使能器件
	// SPI1_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令
	// SPI1_ReadWriteByte((WV_U8)((Dst_Addr)>>16));  //发送24bit地址
	// SPI1_ReadWriteByte((WV_U8)((Dst_Addr)>>8));
	// SPI1_ReadWriteByte((WV_U8)Dst_Addr);
	// W25QXX_CS=1;                            //取消片选
	// W25QXX_Wait_Busy();   				   //等待擦除完成
	WV_U32 addr;
	WV_U8 writeBuf[4] = {0};
	addr = Dst_Addr * W25Q128_BlockSize;
	writeBuf[0] = W25X_BlockErase;
	writeBuf[1] = (WV_U8)((addr) >> 16);
	writeBuf[2] = (WV_U8)((addr) >> 8);
	writeBuf[3] = (WV_U8)addr;
	W25QXX_Write_Enable();
	W25QXX_Wait_Busy();
	//发送擦除命令
	HIS_SPI_Write(writeBuf, 4);
	W25QXX_Wait_Busy();
}

//等待空闲
void W25QXX_Wait_Busy(void)
{
	while ((W25QXX_ReadSR() & 0x01) == 0x01){
		usleep(1000);
	}
		 // 等待BUSY位清空
}
//进入掉电模式
void W25QXX_PowerDown(void)
{
	// W25QXX_CS=0;                            //使能器件
	// SPI1_ReadWriteByte(W25X_PowerDown);        //发送掉电命令
	// W25QXX_CS=1;                            //取消片选
	// delay_us(3);                               //等待TPD
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_PowerDown;
	HIS_SPI_Write(writeBuf, 1);
	usleep(3000);
}
//唤醒
void W25QXX_WAKEUP(void)
{
	// W25QXX_CS=0;                            //使能器件
	// SPI1_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB
	// W25QXX_CS=1;                            //取消片选
	// delay_us(3);                               //等待TRES1
	WV_U8 writeBuf[4] = {0};
	writeBuf[0] = W25X_ReleasePowerDown;
	HIS_SPI_Write(writeBuf, 1);
	usleep(3000);
}
