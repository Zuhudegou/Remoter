#ifndef __ANO_STICK_H__
#define __ANO_STICK_H__

#include "sysconfig.h"

struct _Rc
{
	int16_t THR;	//����
	int16_t YAW;//ƫ��
	int16_t PIT;//����
	int16_t ROL;//����
	
	int16_t AUX1; //��ȡANO_Param�е�Ư������ ANO_Param.OffSet_Rol
	int16_t AUX2; //����ģʽ
	int16_t AUX3; //��ȡANO_Param�е�Ư������ ANO_Param.OffSet_Pit
	
	int8_t AUX40; //����״̬��2��������6���²�����΢��4�������������ֽ�
	int8_t AUX41; //ƫ�����������ֽ�
	
	int16_t AUX5; //ң�ص�ص�ѹ
	int16_t AUX6; //�Ž��յ��ɿ����ݵ�֡�� NRF_SSI
	
	int16_t AUX7; // ���ű���+��������+Ŀ��ƫ��
	int16_t AUX8; // ���ֽڣ�������У׼�����ֽڣ�һЩ������ָ���״̬
	
};

//����
struct _Stick //��ǰҡ��ֵ
{
	int16_t THR;	
	int16_t YAW;	
	int16_t ROL;
	int16_t PIT;	
	int16_t AUX5;	//ң�ص�ѹ
};



//���û�ã���key.c�е��ã���ʵ��û���ô��������ϰ汣�������Ľṹ��
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

extern  struct _Stick Stick_dis;  //��������ʾ����ǰ������

void ANO_Stick_Init(void);
void ANO_Stick_Scan(void);
void ANO_Rc_Data(void);   //׼����������

#endif
