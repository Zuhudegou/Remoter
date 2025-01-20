#include "ANO_Drv_I2C.h"

//模拟IIC初始化
void ANO_I2C_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  RCC_APB2PeriphClockCmd(ANO_RCC_I2C , ENABLE );
  GPIO_InitStructure.GPIO_Pin =  I2C_Pin_SCL;//b14
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(ANO_GPIO_I2C, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  I2C_Pin_SDA;//b15 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //当sda设置为开漏，读取时只要把sda设为高电平即可，无需更改为输入模式。
  GPIO_Init(ANO_GPIO_I2C, &GPIO_InitStructure);		
}


void ANO_I2C_delay(void)
{
   /*u8 i=0; 
   while(i) 
   { 
     i--; 
   } 
		*/ 	
	//不需要延迟也可以，节省软件i2c占用的时间。
	/*
    volatile unsigned char i = 5;\
    while (i)\
        i--;\
	*/
		
	
}

int ANO_I2C_Start(void)
{
	SDA_H;
	SCL_H;
	ANO_I2C_delay();
	if(!SDA_read)return 0;	//SDA线为低电平则总线忙,退出
	SDA_L;
	ANO_I2C_delay();
	if(SDA_read) return 0;	//SDA线为高电平则总线出错,退出
	//SDA_L; //bug
	SCL_L;
	ANO_I2C_delay();
	return 1;	
}

void ANO_I2C_Stop(void)
{
	SCL_L;
	ANO_I2C_delay();
	SDA_L;
	ANO_I2C_delay();
	SCL_H;
	ANO_I2C_delay();
	SDA_H;
	ANO_I2C_delay();
} 

void ANO_I2C_Ack(void)
{	
	SCL_L;
	ANO_I2C_delay();
	SDA_L;
	ANO_I2C_delay();
	SCL_H;
	ANO_I2C_delay();
	SCL_L;
	ANO_I2C_delay();
}   

void ANO_I2C_NoAck(void)
{	
	SCL_L;
	ANO_I2C_delay();
	SDA_H;
	ANO_I2C_delay();
	SCL_H;
	ANO_I2C_delay();
	SCL_L;
	ANO_I2C_delay();
} 

int ANO_I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	SCL_L;
	ANO_I2C_delay();
	SDA_H;			
	ANO_I2C_delay();
	SCL_H;
	ANO_I2C_delay();
	if(SDA_read)
	{
      SCL_L;
			ANO_I2C_delay();
      return 0;
	}
	SCL_L;
	ANO_I2C_delay();
	return 1;
}



/*
void ANO_I2C_SendByte_ack(u8 IIC_Byte) //数据从高位到低位//
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		
		
		if(IIC_Byte & 0x80)
			SDA_H;
		else
			SDA_L;
		
		
		ANO_I2C_delay();
		SCL_H;
		ANO_I2C_delay();
		SCL_L;
		ANO_I2C_delay();
		
		
		IIC_Byte<<=1;
	}
	
	
	SDA_H;
	ANO_I2C_delay();
	
	
	SCL_H;
	ANO_I2C_delay();
	SCL_L;
	ANO_I2C_delay();

}  
*/

//_____                                           ______________
//     *             *             *               
//      ______        ______        ______        _______
//_____|      |______|      |______|      |______|       |______


//     *             *             *     
//      ______        ______        ______
//_____|      |______|      |______|      |_____________________



void ANO_I2C_SendByte(u8 SendByte) //数据从高位到低位//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        ANO_I2C_delay();

				if(SendByte&0x80)
					SDA_H;  
				else 
					SDA_L;   

        SendByte<<=1;
        ANO_I2C_delay();
				
				SCL_H;
				ANO_I2C_delay();

    }

    SCL_L;
		
}  



void ANO_I2C_SendByte_ack(u8 SendByte) //数据从高位到低位//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        ANO_I2C_delay();

				if(SendByte&0x80)
					SDA_H;  
				else 
					SDA_L;   

        SendByte<<=1;
        ANO_I2C_delay();
				
				SCL_H;
				ANO_I2C_delay();

    }

		
		{ //新增，相当于给一个应答时钟
				SCL_L;
				ANO_I2C_delay();
				SCL_H;
				ANO_I2C_delay();
		}
		
		
    SCL_L;
		//ANO_I2C_delay();

}  





//    _____                                         ________________
//         *             *             *       
//          ______        ______        ______
//    _____|      |______|      |______|      |_____________________





u8 ANO_I2C_ReadByte(void)  //数据从高位到低位//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      ANO_I2C_delay();
			SCL_H;
      ANO_I2C_delay();	
      if(SDA_read)		//当sda设置为开漏，读取时只要把sda设为高电平即可，无需更改为输入模式。
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
} 

/*********************OLED，连续模式，开始传输（含i2c地址）************************************/
int OLED_Start(void)
{
		// i2c速度要比spi慢很多。但i2c只要2根线，spi需要4~6根线（rst,dc,sck,mosi,cs,miso）,
		// 在这里，主要是spi有d/c单独的控制线，而i2c每个数据都要发送额外的d/c指令
		// 另外没有cs控制线，导致每次数据都需要发送器件从地址，导致单个字节时长为spi的3倍
  	if(!ANO_I2C_Start())return 0;
		ANO_I2C_SendByte_ack(0x78);   //OLED屏幕的i2c地址  Write_IIC_Byte(0x78);  //Slave address,SA0=0
    return 1;
}
/*********************OLED，连续模式，结束传输************************************/
int OLED_End(void)
{
    ANO_I2C_Stop(); 
    return 1;
}

/*********************OLED，连续模式，切换数据与命令************************************/
//连续模式仅仅针对数据，写到驱动ic的GDDRAM，写命令到OLED寄存器不能用连续模式。
int OLED_ctrl(u8 mode) //0：写命令，1：写数据到GDDRAM
{
		if(mode==0)	//0：写命令
				ANO_I2C_SendByte_ack(0x00);   //Write_IIC_Byte(0x00);			//write command
		else				//1：写数据到GDDRAM
				ANO_I2C_SendByte_ack(0x40);   //Write_IIC_Byte(0x40);			//write data
		
    return 1;
}
/*********************OLED，连续模式，写一个字节的数据 ************************************/ 
int OLED_WrByte(unsigned char IIC_Byte) // OLED_ctrl() 后面可以跟随多个OLED_WrByte()
{
		ANO_I2C_SendByte_ack(IIC_Byte);   //发送一个或者多个字节，具体是命令还是数据，由 OLED_ctrl()决定
    return 1;
}





/*********************OLED，普通模式，写命令************************************/
int OLED_WrCmd(unsigned char IIC_Command)
{
		// i2c速度要比spi慢很多。但i2c只要2根线，spi需要4~6根线（rst,dc,sck,mosi,cs,miso）,
		// 在这里，主要是spi有d/c单独的控制线，而i2c每个数据都要发送额外的d/c指令
		// 另外没有cs控制线，导致每次数据都需要发送器件从地址，导致单个字节时长为spi的3倍
  	if(!ANO_I2C_Start())return 0;
		ANO_I2C_SendByte_ack(0x78);   //OLED屏幕的i2c地址  Write_IIC_Byte(0x78);  //Slave address,SA0=0
		ANO_I2C_SendByte_ack(0x00);   //Write_IIC_Byte(0x00);			//write command
		ANO_I2C_SendByte_ack(IIC_Command);   //Write_IIC_Byte(IIC_Command);
		//这里的应答时钟放到 SendByte()里面了
    ANO_I2C_Stop(); 
    return 1;
}




/*********************OLED，普通模式，写数据************************************/ 
int OLED_WrDat(unsigned char IIC_Data)
{
  	if(!ANO_I2C_Start())return 0;
	  ANO_I2C_SendByte_ack(0x78);   //OLED屏幕的i2c地址   Write_IIC_Byte(0x78);
		ANO_I2C_SendByte_ack(0x40);   //Write_IIC_Byte(0x40);			//write data
		ANO_I2C_SendByte_ack(IIC_Data);   //Write_IIC_Byte(IIC_Data);
		//这里的应答时钟放到 SendByte()里面了
    ANO_I2C_Stop(); 
    return 1;
}






//单字节写入*******************************************
int ANO_I2C_Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data)		
{
  	if(!ANO_I2C_Start())return 0;
    ANO_I2C_SendByte(SlaveAddress);   //发送设备地址+写信号//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址 
    if(!ANO_I2C_WaitAck()){ANO_I2C_Stop(); return 0;}
    ANO_I2C_SendByte(REG_Address );   //设置低起始地址      
    ANO_I2C_WaitAck();	
    ANO_I2C_SendByte(REG_data);
    ANO_I2C_WaitAck();   
    ANO_I2C_Stop(); 
    return 1;
}

//单字节读取*****************************************
int ANO_I2C_Single_Read(u8 SlaveAddress,u8 REG_Address)
{   
		unsigned char REG_data;     	
		if(!ANO_I2C_Start())return 0;
    ANO_I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址 
    if(!ANO_I2C_WaitAck())
		{
			ANO_I2C_Stop();
			return 0;
		}
    ANO_I2C_SendByte((u8) REG_Address);   //设置低起始地址      
    ANO_I2C_WaitAck();
    ANO_I2C_Start();
    ANO_I2C_SendByte(SlaveAddress+1);
    ANO_I2C_WaitAck();

		REG_data= ANO_I2C_ReadByte();
    ANO_I2C_NoAck();
    ANO_I2C_Stop();
    //return TRUE;
	return REG_data;

}	

/******************************************************************************
多字节读取
******************************************************************************/
int ANO_I2C_Mult_Read(u8 SlaveAddress,u8 REG_Address,u8 * ptChar,u8 size)
{
    uint8_t i;
    
    if(size < 1)
			return 0;
    if(!ANO_I2C_Start())
			return 0;
    ANO_I2C_SendByte(SlaveAddress);
    if(!ANO_I2C_WaitAck())
		{
			ANO_I2C_Stop();
			return 0;
		}
    ANO_I2C_SendByte(REG_Address);    
    ANO_I2C_WaitAck();
    
    ANO_I2C_Start();
    ANO_I2C_SendByte(SlaveAddress+1);
    ANO_I2C_WaitAck();
    
    for(i=1;i<size; i++)
    {
        *ptChar++ = ANO_I2C_ReadByte();
        ANO_I2C_Ack();
    }
    *ptChar++ = ANO_I2C_ReadByte();
    ANO_I2C_NoAck();
    ANO_I2C_Stop();
    return 1;    
}	

