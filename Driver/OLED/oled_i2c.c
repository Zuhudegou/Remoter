#include "oled_i2c.h"

#include "stdlib.h"
#include "oledfont.h"  	

#include "ANO_Drv_I2C.h"
	   
u8 OLED_GRAM[128][8];	 

void OLED_Refresh_Gram(void)
{
	static u8 i=0; //8页（8行*8=64行）
	u8 n; //128列 

	/*
	if(i==0){
				//开始传输第1页数据
				OLED_WrCmd(0xaf);// Disable Entire Display On (0xa4/0xa5)
	}
	*/
	
	
	//OLED_Start();//开始连续传输
	
	for(i=0;i<8;i++)  
	{  

		/* spi OLED
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址 
		for(n=0;n<128;n++) OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
		*/
		
		// i2c OLED，单次传输模式，总耗时48ms
		/*
		OLED_WrCmd(0xb0+i);//设置页地址（0~7）
		//OLED_WrCmd(0x01);//设置显示位置—列低地址;;;;从第1列开始
		OLED_WrCmd(0x00); //设置显示位置—列低地址;;;;从第0列开始
		OLED_WrCmd(0x10); //设置显示位置—列高地址，初始化时设置一次即可
		for(n=0;n<128;n++) OLED_WrDat(OLED_GRAM[n][i]);  //一次传8个像素，即8行（一页）里面的一列）
		*/
		
		
		
		//i2c OLED, 总耗时 24ms，连续传输模式，连续模式仅针对数据有效，命令不能用连续模式
		OLED_Start();//开始连续传输
		OLED_ctrl(0); //传输指令到寄存器, 传输指令并不能用连续模式，传输完一个指令就结束了，他内部需要处理指令并结束对话
		OLED_WrByte(0xb0+i);//设置页地址（0~7）
		//OLED_End();//结束连续传输
		
		
		OLED_Start();//开始连续传输，使用连续模式，实测耗时从48ms减少到24ms
		OLED_ctrl(1); //传输数据到显存
		for(n=0;n<128;n++) OLED_WrByte(OLED_GRAM[n][i]);  //一次传8个像素，即8行（一页）里面的一列）
		//OLED_End();//结束连续传输
		


	}
	OLED_End();//结束连续传输

	

	/*
	i++;
	if(i==8){
				i=0; //下次将传输第一页
				//8页数据传输完毕
				OLED_WrCmd(0xaf);// Disable Entire Display On (0xa4/0xa5)
	} 
	*/
	
}





//向OLED写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
/*
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_RS_Set();
	else 
	  OLED_RS_Clr();		  
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_RS_Set();   	  
} 
*/



/*
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 

*/


//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
//u8 tmp_cnt=0;
void OLED_Clear(void)  
{  
	
	//tmp_cnt ++;
	
	u8 i,n;  
	for(i=0;i<8;i++)
	{
		for(n=0;n<128;n++)
		{
			OLED_GRAM[n][i]=0X00;  
		}
	}
	
	//OLED_Refresh_Gram();//更新显示
	
}





//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}






//画斜线

//画线
//x1,y1:起点坐标
//x2,y2:结束坐标

/* 用法

		//display_wait=0;	//是否立即刷新显示，默认是0，不等待，立即刷新
		OLED_DrawLine(10,10,100,60,1)
		OLED_DrawLine(100,60,10,10,1)

*/
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	
	int incx,incy,uRow,uCol;
	
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	
	uRow=x1;//画线起点坐标
	
	uCol=y1;
	
	
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} //delta_x始终是正的，把incx变为负的
	
	
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	//else {incy=-1;delta_y=-delta_x;} //错误修正
	else {incy=-1;delta_y=-delta_y;} //delta_y始终是正的，把incy变为负的
	
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	
	
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,dot);//画点
		
		xerr+=delta_x;  //几个循环增加一次坐标值，基本增量轴，坐标+/-1的机会更大
		yerr+=delta_y;  //几个循环增加一次坐标值
		
		if(xerr>distance) //
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
	

	
}





//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(u8 x,u8 y,u8 r,u8 dot)
{
		int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,dot);//画点
        OLED_DrawPoint(x - a, y - b,dot);
        OLED_DrawPoint(x - a, y + b,dot);
        OLED_DrawPoint(x + a, y + b,dot);
 
        OLED_DrawPoint(x + b, y + a,dot);
        OLED_DrawPoint(x + b, y - a,dot);
        OLED_DrawPoint(x - b, y - a,dot);
        OLED_DrawPoint(x - b, y + a,dot);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}











//填字符空隙
void OLED_ShowGap(u8 x,u8 y,u8 gap,u8 size,u8 mode)
{      		
    for(u8 t=0; t<size; t++) //高度
    { 
					for(u8 t1=0; t1<gap; t1++) //宽度
					{
							OLED_DrawPoint(x+t1,y+t,!mode);//无字
					}
			
			
		}

}



/*
字符取模方式：
逐列从上而下扫描，字符高度12和16，都是每列占2个字节。

12高度：            16高度:
______             ________
||||||             |||||||| 
||||||             ||||||||  
______             ________
||||||             ||||||||
                   ||||||||   

*/

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++) //高度12像素，有6列，每列占2个字节，16高度的，有8列，每列也是占2个字节
    {   
					if(size==12)temp=oled_asc2_1206[chr][t];  //调用1206字体，temp为字符半列的8个像素，一共6列，注意下半列只有4个像素
					else temp=oled_asc2_1608[chr][t];		 //调用1608字体 ，temp为字符半列的8个像素，一共8列
			
					for(t1=0;t1<8;t1++) //半列的8个像素
					{
									if(temp&0x80)OLED_DrawPoint(x,y,mode); //有字
									else OLED_DrawPoint(x,y,!mode);//无字
						
						
									temp<<=1;
									y++;
									if((y-y0)==size) //12高度，下半列只有4个像素
									{
										y=y0;
										x++; //更换一列
										break;
									}
									
									
					}  	 
					
    }          
}
//显示汉字
void OLED_ShowCH(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{   			    
	u8 temp,t,t1;
	u8 y0=y;
		   
    for(t=0;t<size;t++)
    {   
			temp=oled_CH_1616[chr][t];		 //调用中文字库                          
			for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)OLED_DrawPoint(x,y,mode);
				else OLED_DrawPoint(x,y,!mode);
				temp<<=1;
				y++;
				if((y-y0)==size)
				{
					y=y0;
					x++;
					break;
				}
			}
    }          
}



//显示单个中文字符
//高度12的中文字符，宽度也是12
void OLED_Show_CH(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{
	OLED_ShowCH(x,y,chr*2,size,1);
	OLED_ShowCH(x+size/2,y,chr*2+1,size,1);
}



//显示一串中文字符
void OLED_Show_CH_String(u8 x,u8 y,const u8 *p,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u8 y0=y;
		   
	for(t=0;t<size;t++)
	{   
		temp=p[t];		 //调用中文字库                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}
	}
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 



//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体,没有间隙（字体宽8）； 用12高字体，有2像素间隙（字体宽6）
//高度12，宽度8（实际6像素，会加间隙2个像素）
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size,u8 mode)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
			
				if(size==12)OLED_ShowGap(x,y,1,size,mode); //新增,size为高度，2为gap
				x+=1;
			
        //if(x>MAX_CHAR_POSX){x=0;y+=size;}//换行,,,取消换行
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,mode);	
				
				if(size==12)OLED_ShowGap(x+6,y,1,size,mode); //新增,size为高度，2为gap
        x+=7; //12像素高的字符实际宽度6，所以字符之间空隙2像素
				
        p++;
    }  
}

//高度12，宽度6（实际6像素，会加间隙0个像素,用于显示数字）
void OLED_ShowString_num(u8 x,u8 y,const u8 *p,u8 size,u8 mode)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
			
				if(size==12)OLED_ShowGap(x,y,1,size,mode); //新增,size为高度，2为gap
				x+=0;
			
        //if(x>MAX_CHAR_POSX){x=0;y+=size;}//换行,,,取消换行
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,mode);	
				
				if(size==12)OLED_ShowGap(x+6,y,1,size,mode); //新增,size为高度，2为gap
        x+=6; //12像素高的字符实际宽度6，所以字符之间空隙2像素
				
        p++;
    }  
}



//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void OLED_ShowString_16(u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}//换行
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,16,1);	 
        x+=8;
        p++;
    }  
}	




u8 status;


//初始化OLED					    
void OLED_Init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
//	//改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG+SW-DP）
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
//	//改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
		
	
	/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化PA0,1  //B13, B15
 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_15); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_12;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化PA2   //B12, B14
 	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	*/
	
	ANO_I2C_Init();  //i2c端口初始化
	
	
	
	
	/*
	OLED_RST_Clr();
	Delay(1000);
	OLED_RST_Set(); 
					  
		OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC0,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD);
	
	
	*/
	
	
	
	
	Delay(500);//初始化之前的延时很重要！
	
	
	status = OLED_WrCmd(0xae);//--turn off oled panel
	
	OLED_WrCmd(0x00);//---set low column address  //从0列开始
	OLED_WrCmd(0x10);//---set high column address //从0列开始
	OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	
	OLED_WrCmd(0x81);//--set contrast control register 0x81下个就是亮度 0x00~0xff
	//OLED_WrCmd(0xCF); // Set SEG Output Current Brightness
	OLED_WrCmd(0x8F); // Set SEG Output Current Brightness，降低亮度
	
	OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WrCmd(0xc0);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WrCmd(0xa6);//--set normal display
	OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3f);//--1/64 duty
	OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00);//-not offset
	OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9);//--set pre-charge period
	OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda);//--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb);//--set vcomh
	OLED_WrCmd(0x40);//Set VCOM Deselect Level
	OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x02);//
	OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WrCmd(0x14);//--set(0x10) disable
	OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 

	
	
	//OLED_Fill(0x00); //初始清屏
	OLED_Clear();//
	OLED_WrCmd(0xaf);//--turn on oled panel
	
	
	//OLED_Set_Pos(0,0); //我们整页刷新，不需要
	

	
	
	
	
	Show.windows = 0;
	


}  





