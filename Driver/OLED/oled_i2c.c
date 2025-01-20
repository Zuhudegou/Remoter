#include "oled_i2c.h"

#include "stdlib.h"
#include "oledfont.h"  	

#include "ANO_Drv_I2C.h"
	   
u8 OLED_GRAM[128][8];	 

void OLED_Refresh_Gram(void)
{
	static u8 i=0; //8ҳ��8��*8=64�У�
	u8 n; //128�� 

	/*
	if(i==0){
				//��ʼ�����1ҳ����
				OLED_WrCmd(0xaf);// Disable Entire Display On (0xa4/0xa5)
	}
	*/
	
	
	//OLED_Start();//��ʼ��������
	
	for(i=0;i<8;i++)  
	{  

		/* spi OLED
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ 
		for(n=0;n<128;n++) OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
		*/
		
		// i2c OLED�����δ���ģʽ���ܺ�ʱ48ms
		/*
		OLED_WrCmd(0xb0+i);//����ҳ��ַ��0~7��
		//OLED_WrCmd(0x01);//������ʾλ�á��е͵�ַ;;;;�ӵ�1�п�ʼ
		OLED_WrCmd(0x00); //������ʾλ�á��е͵�ַ;;;;�ӵ�0�п�ʼ
		OLED_WrCmd(0x10); //������ʾλ�á��иߵ�ַ����ʼ��ʱ����һ�μ���
		for(n=0;n<128;n++) OLED_WrDat(OLED_GRAM[n][i]);  //һ�δ�8�����أ���8�У�һҳ�������һ�У�
		*/
		
		
		
		//i2c OLED, �ܺ�ʱ 24ms����������ģʽ������ģʽ�����������Ч�������������ģʽ
		OLED_Start();//��ʼ��������
		OLED_ctrl(0); //����ָ��Ĵ���, ����ָ�����������ģʽ��������һ��ָ��ͽ����ˣ����ڲ���Ҫ����ָ������Ի�
		OLED_WrByte(0xb0+i);//����ҳ��ַ��0~7��
		//OLED_End();//������������
		
		
		OLED_Start();//��ʼ�������䣬ʹ������ģʽ��ʵ���ʱ��48ms���ٵ�24ms
		OLED_ctrl(1); //�������ݵ��Դ�
		for(n=0;n<128;n++) OLED_WrByte(OLED_GRAM[n][i]);  //һ�δ�8�����أ���8�У�һҳ�������һ�У�
		//OLED_End();//������������
		


	}
	OLED_End();//������������

	

	/*
	i++;
	if(i==8){
				i=0; //�´ν������һҳ
				//8ҳ���ݴ������
				OLED_WrCmd(0xaf);// Disable Entire Display On (0xa4/0xa5)
	} 
	*/
	
}





//��OLEDд��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
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
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 

*/


//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
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
	
	//OLED_Refresh_Gram();//������ʾ
	
}





//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}






//��б��

//����
//x1,y1:�������
//x2,y2:��������

/* �÷�

		//display_wait=0;	//�Ƿ�����ˢ����ʾ��Ĭ����0�����ȴ�������ˢ��
		OLED_DrawLine(10,10,100,60,1)
		OLED_DrawLine(100,60,10,10,1)

*/
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	
	int incx,incy,uRow,uCol;
	
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	
	uRow=x1;//�����������
	
	uCol=y1;
	
	
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} //delta_xʼ�������ģ���incx��Ϊ����
	
	
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	//else {incy=-1;delta_y=-delta_x;} //��������
	else {incy=-1;delta_y=-delta_y;} //delta_yʼ�������ģ���incy��Ϊ����
	
	if(delta_x>delta_y)distance=delta_x; //ѡȡ��������������
	else distance=delta_y;
	
	
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,dot);//����
		
		xerr+=delta_x;  //����ѭ������һ������ֵ�����������ᣬ����+/-1�Ļ������
		yerr+=delta_y;  //����ѭ������һ������ֵ
		
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





//x,y:Բ������
//r:Բ�İ뾶
void OLED_DrawCircle(u8 x,u8 y,u8 r,u8 dot)
{
		int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,dot);//����
        OLED_DrawPoint(x - a, y - b,dot);
        OLED_DrawPoint(x - a, y + b,dot);
        OLED_DrawPoint(x + a, y + b,dot);
 
        OLED_DrawPoint(x + b, y + a,dot);
        OLED_DrawPoint(x + b, y - a,dot);
        OLED_DrawPoint(x - b, y - a,dot);
        OLED_DrawPoint(x - b, y + a,dot);
        
        a++;
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}











//���ַ���϶
void OLED_ShowGap(u8 x,u8 y,u8 gap,u8 size,u8 mode)
{      		
    for(u8 t=0; t<size; t++) //�߶�
    { 
					for(u8 t1=0; t1<gap; t1++) //���
					{
							OLED_DrawPoint(x+t1,y+t,!mode);//����
					}
			
			
		}

}



/*
�ַ�ȡģ��ʽ��
���д��϶���ɨ�裬�ַ��߶�12��16������ÿ��ռ2���ֽڡ�

12�߶ȣ�            16�߶�:
______             ________
||||||             |||||||| 
||||||             ||||||||  
______             ________
||||||             ||||||||
                   ||||||||   

*/

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ				   
    for(t=0;t<size;t++) //�߶�12���أ���6�У�ÿ��ռ2���ֽڣ�16�߶ȵģ���8�У�ÿ��Ҳ��ռ2���ֽ�
    {   
					if(size==12)temp=oled_asc2_1206[chr][t];  //����1206���壬tempΪ�ַ����е�8�����أ�һ��6�У�ע���°���ֻ��4������
					else temp=oled_asc2_1608[chr][t];		 //����1608���� ��tempΪ�ַ����е�8�����أ�һ��8��
			
					for(t1=0;t1<8;t1++) //���е�8������
					{
									if(temp&0x80)OLED_DrawPoint(x,y,mode); //����
									else OLED_DrawPoint(x,y,!mode);//����
						
						
									temp<<=1;
									y++;
									if((y-y0)==size) //12�߶ȣ��°���ֻ��4������
									{
										y=y0;
										x++; //����һ��
										break;
									}
									
									
					}  	 
					
    }          
}
//��ʾ����
void OLED_ShowCH(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{   			    
	u8 temp,t,t1;
	u8 y0=y;
		   
    for(t=0;t<size;t++)
    {   
			temp=oled_CH_1616[chr][t];		 //���������ֿ�                          
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



//��ʾ���������ַ�
//�߶�12�������ַ������Ҳ��12
void OLED_Show_CH(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{
	OLED_ShowCH(x,y,chr*2,size,1);
	OLED_ShowCH(x+size/2,y,chr*2+1,size,1);
}



//��ʾһ�������ַ�
void OLED_Show_CH_String(u8 x,u8 y,const u8 *p,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u8 y0=y;
		   
	for(t=0;t<size;t++)
	{   
		temp=p[t];		 //���������ֿ�                          
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
//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
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



//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//��16����,û�м�϶�������8���� ��12�����壬��2���ؼ�϶�������6��
//�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size,u8 mode)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
			
				if(size==12)OLED_ShowGap(x,y,1,size,mode); //����,sizeΪ�߶ȣ�2Ϊgap
				x+=1;
			
        //if(x>MAX_CHAR_POSX){x=0;y+=size;}//����,,,ȡ������
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,mode);	
				
				if(size==12)OLED_ShowGap(x+6,y,1,size,mode); //����,sizeΪ�߶ȣ�2Ϊgap
        x+=7; //12���ظߵ��ַ�ʵ�ʿ��6�������ַ�֮���϶2����
				
        p++;
    }  
}

//�߶�12�����6��ʵ��6���أ���Ӽ�϶0������,������ʾ���֣�
void OLED_ShowString_num(u8 x,u8 y,const u8 *p,u8 size,u8 mode)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
			
				if(size==12)OLED_ShowGap(x,y,1,size,mode); //����,sizeΪ�߶ȣ�2Ϊgap
				x+=0;
			
        //if(x>MAX_CHAR_POSX){x=0;y+=size;}//����,,,ȡ������
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,mode);	
				
				if(size==12)OLED_ShowGap(x+6,y,1,size,mode); //����,sizeΪ�߶ȣ�2Ϊgap
        x+=6; //12���ظߵ��ַ�ʵ�ʿ��6�������ַ�֮���϶2����
				
        p++;
    }  
}



//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//��16����
void OLED_ShowString_16(u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}//����
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,16,1);	 
        x+=8;
        p++;
    }  
}	




u8 status;


//��ʼ��OLED					    
void OLED_Init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��PB�˿�ʱ��
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
//	//�ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
//	//�ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ��
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
		
	
	/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��PA0,1  //B13, B15
 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_15); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_12;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��PA2   //B12, B14
 	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	*/
	
	ANO_I2C_Init();  //i2c�˿ڳ�ʼ��
	
	
	
	
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
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC0,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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
	
	
	
	
	Delay(500);//��ʼ��֮ǰ����ʱ����Ҫ��
	
	
	status = OLED_WrCmd(0xae);//--turn off oled panel
	
	OLED_WrCmd(0x00);//---set low column address  //��0�п�ʼ
	OLED_WrCmd(0x10);//---set high column address //��0�п�ʼ
	OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	
	OLED_WrCmd(0x81);//--set contrast control register 0x81�¸��������� 0x00~0xff
	//OLED_WrCmd(0xCF); // Set SEG Output Current Brightness
	OLED_WrCmd(0x8F); // Set SEG Output Current Brightness����������
	
	OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WrCmd(0xc0);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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

	
	
	//OLED_Fill(0x00); //��ʼ����
	OLED_Clear();//
	OLED_WrCmd(0xaf);//--turn on oled panel
	
	
	//OLED_Set_Pos(0,0); //������ҳˢ�£�����Ҫ
	

	
	
	
	
	Show.windows = 0;
	


}  





