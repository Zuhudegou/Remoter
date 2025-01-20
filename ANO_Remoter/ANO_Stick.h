#ifndef __ANO_STICK_H__
#define __ANO_STICK_H__

#include "sysconfig.h"

struct _Rc
{
	int16_t THR;	//油门
	int16_t YAW;//偏航
	int16_t PIT;//俯仰
	int16_t ROL;//翻滚
	
	int16_t AUX1; //读取ANO_Param中的漂移修正 ANO_Param.OffSet_Rol
	int16_t AUX2; //定高模式
	int16_t AUX3; //读取ANO_Param中的漂移修正 ANO_Param.OffSet_Pit
	
	int8_t AUX40; //按键状态（2个顶部，6个下部，除微调4个按键），低字节
	int8_t AUX41; //偏航修正，高字节
	
	int16_t AUX5; //遥控电池电压
	int16_t AUX6; //放接收到飞控数据的帧率 NRF_SSI
	
	int16_t AUX7; // 油门比例+方向力度+目标偏航
	int16_t AUX8; // 高字节：陀螺仪校准，低字节：一些列新增指令和状态
	
};

//新增
struct _Stick //当前摇杆值
{
	int16_t THR;	
	int16_t YAW;	
	int16_t ROL;
	int16_t PIT;	
	int16_t AUX5;	//遥控电压
};



//这个没用，在key.c有调用，但实际没有用处，属于老版保留下来的结构体
struct offset//
{
	int16_t BAT;		
	int16_t roll;
	int16_t pitch;	
	int16_t thr;	
	int16_t yaw;	
} ;



#define Filter_Num 10

struct _Filter
{
	uint32_t sum;
	uint16_t old[Filter_Num];
};
extern struct offset _offset;
extern struct _Filter Filter;
extern struct _Rc Rc;

extern  struct _Stick Stick_dis;  //仅用于显示（提前解析）

void ANO_Stick_Init(void);
void ANO_Stick_Scan(void);
void ANO_Rc_Data(void);   //准备发送数据

#endif
