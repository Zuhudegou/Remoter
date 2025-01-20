#ifndef _KEY_H_
#define _KEY_H_
#include "stm32f10x.h"



//��ȡ AUX72 AUX71,,, AUX73 
#define AUX72()				((uint8_t)( (Rc.AUX7 & 0xf000)>>12 ))		//���ű����� ���ֽڸ�4λ
#define AUX71() 			((uint8_t)( (Rc.AUX7 & 0x0f00)>>8  ))		 //���ȣ�		���ֽڵ�4λ
#define AUX73()  			((int8_t) (  Rc.AUX7 & 0x00ff      ))		//Ŀ��ƫ���ǣ�		���ֽ�

//���� AUX7 = 0x mn ww,  m����(AUX72)��nΪ����(AUX71)��wwƫ����(AUX73)
#define AUX72_s(x)  (Rc.AUX7 = ((Rc.AUX7 & 0x0fff) | ( ((uint16_t)x<<12) & 0xf000) ))		//д����ֽڸ�4λ�����ű���
#define AUX71_s(x)  (Rc.AUX7 = ((Rc.AUX7 & 0xf0ff) | ( ((uint16_t)x<<8)  & 0x0f00) )) 	//д����ֽڵ�4λ������
#define AUX73_s(x)  (Rc.AUX7 = ((Rc.AUX7 & 0xff00) | (  (uint16_t)x      & 0x00ff) ))  	//д����ֽڣ�Ŀ��ƫ����
 
//���� AUX81~AUX88��ֻ���˵��ֽڵ�8λ��������� a���������� b�� ��ͷģʽ c�� ���� e�� LED���� f�� �߶ȼ�ָ�� g�� �߶ȼ�ָ�� h��
// AB00 0000 abcd efgh 

#define AUX811_isH()  (Rc.AUX8 & 0x8000)	//������У׼ ���ֽڵĵ�1λA
#define AUX822_isH()  (Rc.AUX8 & 0x4000)	//����Զ����ԣ����ֽڵĵ�2λB
#define AUX833_isH()  (Rc.AUX8 & 0x2000)	//1��ʾ��ѹ���ȡ�Ĭ��0��ʾ�������ȣ���ѹ��4�����ϲ���Ч��
#define AUX844_isH()   (Rc.AUX8 & 0x1000)	//����ֶ����ԣ����ֽڵĵ�1λD

//����Ϊaux8���ֽ�
#define AUX81_isH()   (Rc.AUX8 & 0x0080)	//������� a
#define AUX82_isH()   (Rc.AUX8 & 0x0040)	//�������� b
#define AUX83_isH()   (Rc.AUX8 & 0x0020)	//��ͷģʽ c�������ģ�0Ϊ��ͷģʽ
#define AUX84_isH()   (Rc.AUX8 & 0x0010)	//�߶ȵ�����¼ d�����ڶ���ģʽ�������������

#define AUX85_isH()   (Rc.AUX8 & 0x0008)	//���� e
#define AUX86_isH()   (Rc.AUX8 & 0x0004)	//LED���� f
#define AUX87_isH()   (Rc.AUX8 & 0x0002)	//�߶ȼ�ָ�� g
#define AUX88_isH()   (Rc.AUX8 & 0x0001)	//�߶ȼ�ָ�� h



#define AUX811_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x8000) ):( Rc.AUX8 = (Rc.AUX8 & ~0x8000) ))//������У׼ ���ֽڵĵ�һλA
#define AUX822_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x4000) ):( Rc.AUX8 = (Rc.AUX8 & ~0x4000) ))//����Զ����ԣ� ���ֽڵĵ�2λB
#define AUX833_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x2000) ):( Rc.AUX8 = (Rc.AUX8 & ~0x2000) ))//�رռ�����ѹ��Ч�� ���ֽڵĵ�3λC
#define AUX844_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x1000) ):( Rc.AUX8 = (Rc.AUX8 & ~0x1000) ))//����ֶ����ԣ� ���ֽڵĵ�4λD


//����Ϊaux8���ֽ�
#define AUX81_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0080) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0080) ))//������� a
#define AUX82_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0040) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0040) ))//�������� b
#define AUX83_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0020) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0020) ))//��ͷģʽ c�����ģ�0Ϊ��ͷģʽ
#define AUX84_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0010) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0010) ))//�߶ȵ�����¼ d

#define AUX85_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0008) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0008) ))//���� e
#define AUX86_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0004) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0004) ))//LED���� f
#define AUX87_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0002) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0002) ))//�߶ȼ�ָ�� g
#define AUX88_s(x)  (x>0?( Rc.AUX8 = (Rc.AUX8 | 0x0001) ):( Rc.AUX8 = (Rc.AUX8 & ~0x0001) ))//�߶ȼ�ָ�� h





/******************************************************************************
							ȫ�ֱ�������
*******************************************************************************/ 
extern uint8_t Mode,Fun;
//#define	Mode_Read	(GPIOB->IDR  & GPIO_Pin_8) //Mode������
//#define	Fun_Read	(GPIOB->IDR  & GPIO_Pin_7) //Fun������

//SCL(B14) �� K1(B8) �Ե�
//SDA(B15) �� K2(B7) �Ե�
#define	Mode_Read	(GPIOB->IDR  & GPIO_Pin_14) //Mode������,K1(�ұ�)
#define	Fun_Read	(GPIOB->IDR  & GPIO_Pin_15) //Fun������2(���)

/******************************************************************************
							ȫ�ֺ�������
*******************************************************************************/ 
void KEY_Init(void);
void key_function(void);
//void ANO_key(void);
void key_scan(void);

#endif
