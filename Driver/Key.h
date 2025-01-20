#ifndef _KEY_H_
#define _KEY_H_
#include "stm32f10x.h"



//读取 AUX72 AUX71,,, AUX73 
#define AUX72()				((uint8_t)( (Rc.AUX7 & 0xf000)>>12 ))		//油门比例， 高字节高4位
#define AUX71() 			((uint8_t)( (Rc.AUX7 & 0x0f00)>>8  ))		 //力度，		高字节低4位
#define AUX73()  			((int8_t) (  Rc.AUX7 & 0x00ff      ))		//目标偏航角，		低字节

//设置 AUX7 = 0x mn ww,  m比例(AUX72)，n为力度(AUX71)，ww偏航角(AUX73)
#define AUX72_s(x)  (Rc.AUX7 = ((Rc.AUX7 & 0x0fff) | ( ((uint16_t)x<<12) & 0xf000) ))		//写入高字节高4位，油门比例
#define AUX71_s(x)  (Rc.AUX7 = ((Rc.AUX7 & 0xf0ff) | ( ((uint16_t)x<<8)  & 0x0f00) )) 	//写入高字节低4位，力度
#define AUX73_s(x)  (Rc.AUX7 = ((Rc.AUX7 & 0xff00) | (  (uint16_t)x      & 0x00ff) ))  	//写入低字节，目标偏航角
 
//设置 AUX81~AUX88，只用了低字节的8位，定高起飞 a，屏蔽油门 b， 无头模式 c， 拍照 e， LED亮度 f， 高度加指令 g， 高度减指令 h，
// AB00 0000 abcd efgh 

#define AUX811_isH()  (Rc.AUX8 & 0x8000)	//陀螺仪校准 高字节的第1位A
#define AUX822_isH()  (Rc.AUX8 & 0x4000)	//马达自动测试，高字节的第2位B
#define AUX833_isH()  (Rc.AUX8 & 0x2000)	//1表示气压优先。默认0表示激光优先，气压在4米以上才有效。
#define AUX844_isH()   (Rc.AUX8 & 0x1000)	//马达手动测试，高字节的第1位D

//以下为aux8低字节
#define AUX81_isH()   (Rc.AUX8 & 0x0080)	//定高起飞 a
#define AUX82_isH()   (Rc.AUX8 & 0x0040)	//屏蔽油门 b
#define AUX83_isH()   (Rc.AUX8 & 0x0020)	//无头模式 c，，更改：0为无头模式
#define AUX84_isH()   (Rc.AUX8 & 0x0010)	//高度调整记录 d，用于定高模式，计算调整距离

#define AUX85_isH()   (Rc.AUX8 & 0x0008)	//拍照 e
#define AUX86_isH()   (Rc.AUX8 & 0x0004)	//LED亮度 f
#define AUX87_isH()   (Rc.AUX8 & 0x0002)	//高度加指令 g
#define AUX88_isH()   (Rc.AUX8 & 0x0001)	//高度减指令 h



#define AUX811_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x8000) ):( Rc.AUX8 = (Rc.AUX8 & ~0x8000) ))//陀螺仪校准 高字节的第一位A
#define AUX822_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x4000) ):( Rc.AUX8 = (Rc.AUX8 & ~0x4000) ))//马达自动测试， 高字节的第2位B
#define AUX833_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x2000) ):( Rc.AUX8 = (Rc.AUX8 & ~0x2000) ))//关闭激光气压生效， 高字节的第3位C
#define AUX844_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x1000) ):( Rc.AUX8 = (Rc.AUX8 & ~0x1000) ))//马达手动测试， 高字节的第4位D


//以下为aux8低字节
#define AUX81_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0080) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0080) ))//定高起飞 a
#define AUX82_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0040) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0040) ))//屏蔽油门 b
#define AUX83_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0020) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0020) ))//无头模式 c，更改：0为无头模式
#define AUX84_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0010) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0010) ))//高度调整记录 d

#define AUX85_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0008) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0008) ))//拍照 e
#define AUX86_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0004) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0004) ))//LED亮度 f
#define AUX87_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0002) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0002) ))//高度加指令 g
#define AUX88_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0001) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0001) ))//高度减指令 h





/******************************************************************************
							全局变量声明
*******************************************************************************/ 
extern uint8_t Mode,Fun;
//#define	Mode_Read	(GPIOB->IDR  & GPIO_Pin_8) //Mode读数据
//#define	Fun_Read	(GPIOB->IDR  & GPIO_Pin_7) //Fun读数据

//SCL(B14) 和 K1(B8) 对调
//SDA(B15) 和 K2(B7) 对调
#define	Mode_Read	(GPIOB->IDR  & GPIO_Pin_14) //Mode读数据,K1(右边)
#define	Fun_Read	(GPIOB->IDR  & GPIO_Pin_15) //Fun读数据2(左边)

/******************************************************************************
							全局函数声明
*******************************************************************************/ 
void KEY_Init(void);
void key_function(void);
//void ANO_key(void);
void key_scan(void);

#endif
