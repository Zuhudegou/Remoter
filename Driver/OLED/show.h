#ifndef __SHOW_H
#define __SHOW_H
#include "sysconfig.h"

#define REVERSE(DATA,BIT) (DATA)^(1<<(BIT))//��λȡ��
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
	u8 hardware_type;//�ɿ����ͣ�1Ϊ���������
	u8 oled_delay;  //1,��Ҫˢ�£���main.c����ˢ�º���
	u8 windows; //0�����棬1�ɿ����ã�2������״̬
	u8 low_power;  //ң�ص�����
	u8 NRF_Err;
	u8 Rc_num;		//�ź�ǿ��
	u8 Connect_Succeed;
	u16 Battery_Rc;
	u16 Battery_Fly;
	

	
	u16 test_flag; //�´�������״̬���ӵ�λ����λ��M6050(0)����ѹ��(1)������ģ��(2)��������ģ��(3)���Ӿ���λģ��(4)����ѹ��(5)������ģ��(6)
	u16 set_flag; //�´��ɿ����ò���������ģʽ(0)���׷�ģʽ(1)����ͷģʽ(2)���л���ͷ(3)���Ӿ���λ(4)����������(5)
	              //ֻ��һ�����������ϴ�������ģʽ��ͨ��AUX2�ϴ���
	
	int X,Y,Z,H;  //�ɿط��ص���̬��(*10)���Լ������߶�, X:roll(��ʵ������y), Y��pitch����ʵ������x����Z��YAW��H���߶�
	u8 unLock; //�Ƿ��ѽ���
	
	u8 position_old; //�ָ�֮ǰ����ͣģʽ
	
	int16_t loc_x,loc_y; //�ɿط��صĹ�������
	
	u8 qual;//��������
	
	int16_t x_desired,y_desired,x_out,y_out;//�����⻷λ��pid����(λ��)�����ڻ��ٶ�pid���(��̬��)
	
	//u16 Control_high;//�߶�
	//u16 high_desired;//�����߶�
	s16 Control_high;//�߶�
	s16 high_desired;//�����߶�
	
	int16_t roll_desired,pitch_desired;//������̬��(*10)
	
	//�������ű���
	u8 ratio; //��ʾֵΪ Show.ratio+1
	//u8 thr_mask; //��������,ת�Ƶ�AUX82
	
	u8 force; //����   1��2��3��0, 0��ʾʵʱ���������,,��ʾֵΪ  8-ANO_Param.force

	char str_oled[20]; //��һ����ʾ�ַ�
	
	float yaw_Gs; //mpu6050 yaw���ٶ�, =gyroZ* Gyro_G �����ﵥλ�� ��/�룬��mpu6050��ԭʼֵint16_t ��Ϊ float
	
	int16_t debug1,debug2,debug3,debug4,debug5,debug6,debug7,debug8;//�ӷɿش������ģ��������� 
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
