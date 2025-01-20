#ifndef __ANO_PARAM_H
#define __ANO_PARAM_H

#include "stm32f10x.h"
#include "ANO_Drv_Flash.h"

struct param{
	uint8_t  NRF_Channel;
	uint8_t  OffSet_En;//��Ҫ����У׼ҡ��
	
	uint16_t OffSet_Rol;//�û�����Ư������(��ҡ��������ֵ�������) +/-128��Rc.AUX1��Χ +/-256
	uint16_t OffSet_Pit;//�û�����Ư������(��ҡ��������ֵ�������) +/-128��Rc.AUX3��Χ +/-256
	
	//uint16_t OffSet_Thr;// ȡ���û���������ƫ��
	uint16_t OffSet_Yaw;// �û�����ƫ����ƫ��
	
	uint16_t FirstInitFlag;
	
	uint16_t Z_OffSet_Rol;//ҡ��������ֵ����
	uint16_t Z_OffSet_Pit;//ҡ��������ֵ����
	
	uint16_t Z_OffSet_Thr;//ҡ��������ֵ�������û��޷��޸ģ�ͨ��ҡ��У׼���
	uint16_t Z_OffSet_Yaw;//ҡ��������ֵ����
	
	
	
	//�������ȼ���,��������,ת�Ƶ�ANO_Param�����ñ���
	//u8 force; //����   1��2��3��0, 0��ʾʵʱ���������,,��ʾֵΪ  8-ANO_Param.force
	
	//���ӱ���flag, �˳���ϸ��������ʱ���档�������˵������flagΪ1������ʾ"����"
	u8 SaveFlag; //����   1��2��3��0, 0��ʾʵʱ���������
	
};
extern struct param ANO_Param;

void ANO_Param_READ(void);
void ANO_Param_SAVE(void);

#endif

