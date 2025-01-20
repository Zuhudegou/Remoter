#ifndef __SHOW_H
#define __SHOW_H
#include "sysconfig.h"

#define REVERSE(DATA,BIT) (DATA)^(1<<(BIT))//按位取反
#define BIT0	0x0001
#define BIT1	0x0002
#define BIT2	0x0004
#define BIT3	0x0008
#define BIT4	0x0010
#define BIT5	0x0020
#define BIT6	0x0040
#define BIT7	0x0080
#define BIT8	0x0100
#define BIT9	0x0200
#define BIT10 0x0400
#define BIT11 0x0800



struct _Show
{
	u8 hardware_type;//飞控类型，1为四轴飞行器
	u8 oled_delay;  //1,需要刷新，在main.c调用刷新函数
	u8 windows; //0主界面，1飞控设置，2传感器状态
	u8 low_power;  //遥控电量低
	u8 NRF_Err;
	u8 Rc_num;		//信号强度
	u8 Connect_Succeed;
	u16 Battery_Rc;
	u16 Battery_Fly;
	

	
	u16 test_flag; //下传传感器状态，从低位到高位，M6050(0)，气压计(1)，无线模块(2)，超声波模块(3)，视觉定位模块(4)，电压低(5)，光流模块(6)
	u16 set_flag; //下传飞控设置参数，定高模式(0)，抛飞模式(1)，无头模式(2)，切换机头(3)，视觉定位(4)，光流定点(5)
	              //只有一个参数可以上传，定高模式，通过AUX2上传。
	
	int X,Y,Z,H;  //飞控返回的姿态角(*10)，以及光流高度, X:roll(其实坐标是y), Y：pitch（其实坐标是x），Z：YAW，H：高度
	u8 unLock; //是否已解锁
	
	u8 position_old; //恢复之前的悬停模式
	
	int16_t loc_x,loc_y; //飞控返回的光流数据
	
	u8 qual;//光流质量
	
	int16_t x_desired,y_desired,x_out,y_out;//光流外环位移pid期望(位移)，和内环速度pid输出(姿态角)
	
	//u16 Control_high;//高度
	//u16 high_desired;//期望高度
	s16 Control_high;//高度
	s16 high_desired;//期望高度
	
	int16_t roll_desired,pitch_desired;//期望姿态角(*10)
	
	//增加油门比例
	u8 ratio; //显示值为 Show.ratio+1
	//u8 thr_mask; //油门屏蔽,转移到AUX82
	
	u8 force; //力度   1，2，3，0, 0表示实时，最大力度,,显示值为  8-ANO_Param.force

	char str_oled[20]; //第一行显示字符
	
	float yaw_Gs; //mpu6050 yaw角速度, =gyroZ* Gyro_G ，这里单位是 度/秒，从mpu6050的原始值int16_t 变为 float
	
	int16_t debug1,debug2,debug3,debug4,debug5,debug6,debug7,debug8;//从飞控传过来的，调试数据 
};



extern struct _Show Show;
extern u16 set_temp;
extern u16 test_temp;
extern u8 send_flag;

void oled_show(void);
void OLED_Show_progress_bar(u8 temp,u8 chr_star,u8 chr_default,u8 x,u8 y,u8 size,u8 mode);
void OLED_Show_Seting(void);
void OLED_Show_Test(void);
void Gesture_Check0(u16 val,vs16 *set,vs16 max);
void Gesture_Check1(u16 val,u16 *set,vs16 bit);
void Gesture_Check(void);
void Show_Duty(void);
#endif
