#include "sysconfig.h"
#include "stdio.h"  //	sprintf()

uint8_t Mode=1,Fun=0;//Ĭ����βģʽ����ҹ��ģʽ

//AUX1 ��Ʈ����ֵRol   ...stick.c����
//AUX2 ����ģʽ				                 ---key.c����
//AUX3 ��Ʈ����ֵPit   ...stick.c����
//AUX4 ����״̬				                 ---key.c����
//AUX5 ��ص�ѹ				...stick.c����
//AUX6 ֡�� NRF_SSI 	...ANO_Loop_1Hz()����
//AUX7 ����/���ű���/ƫ����			               ---key.c����
//AUX8 �������/��������/��ͷģʽ                ---key.c����
//AUX8 ����/LED����/�߶ȵ�����¼/�߶ȼ�ָ��/�߶ȼ�ָ��          ---key.c����

u8 xy_set=0; //��¼����set��������û�а� Ư������/key2/lock



/******************************************************************************
����ԭ��:	void KEY_Init(void)
��������:	������ʼ��
*******************************************************************************/ 
void KEY_Init(void)
{
	
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |  RCC_APB2Periph_AFIO,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |  RCC_APB2Periph_AFIO,ENABLE);
		
		//�ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
		//�ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ��
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
		
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11;    
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //�������룬����lock����(B2)������ټ��ⲿ4.7k����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(GPIOB, &GPIO_InitStructure);   //B3,B4,B5,B6,    B7,B8,    B2(12),B11,B9,
	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_13; 
		GPIO_Init(GPIOC, &GPIO_InitStructure);	 //C13,C14,C15
	
		//û���õ�
		_offset.pitch=_offset.roll=2048;  //΢������ֵL R
		_offset.thr=2048; //΢������ֵM
	
	
}






//10ms���һ�Σ�
void key_function(void)  //����2�������ĳ��򣬿������ֳ������̰�
{
		
	
	
	/*
		
		k2(B7)																	k1(B8)
		
		
														 set(B9)
																						k3/front(B3)
	k7/up(C13)	  k9/p(C14)                 
																	k6/left(B4)       k5/right(B6)
	k8/down(B11)  k10/m(C15)
																						k4/back(B5)
				 lock(B2)��ΪB12
		
	
	
//SCL(B14) �� K1(B8) �Ե�
//SDA(B15) �� K2(B7) �Ե�
	*/
	

		
		
		
		
		static u8 count0=0,count1=0;
		static u8 count2=0,count3=0;
		static u8 count4=0,count5=0;
		static u8 count6=0,count7=0;
	
		static u8 beet_falg=0;
		static int16_t beet_temp;//�и���
		
		Rc.AUX40 = 0; //��װ״̬ȫ����λ
		if(!Mode_Read)	Rc.AUX40 |= 0x01;	//�ұ߰���KEY1   PB14 �����£��͵�ƽ��
		if(!Fun_Read)		Rc.AUX40 |= 0x02;	//��߰���KEY2	  PB15 �����£��͵�ƽ��
		
//SCL(B14) �� K1(B8) �Ե�
//SDA(B15) �� K2(B7) �Ե�
		
		
		//����6������,���϶���2����һ��8������
		//���⻹��4��������ҡ�����棬����һ������
	
	
		if(!(GPIOC->IDR  & GPIO_Pin_13)) Rc.AUX40 |= 0x04;	//KEY_UP(k7)   = C13   //ϵͳ���C13��LED
		if(!(GPIOB->IDR  & GPIO_Pin_11)) Rc.AUX40 |= 0x08;	//KEY_DOWN(k8) = B11
	
		if(!(GPIOC->IDR  & GPIO_Pin_14)) Rc.AUX40 |= 0x10;	//KEY_RATIO_P(k9) = C14
		if(!(GPIOC->IDR  & GPIO_Pin_15)) Rc.AUX40 |= 0x20;	//KEY_RATIO_M(k10) = C15
	
		if(!(GPIOB->IDR  & GPIO_Pin_12))  Rc.AUX40 |= 0x40;	//KEY_LOCK = B2, ��ΪB12, ԭ����DC��spi OLED 
  	if(!(GPIOB->IDR  & GPIO_Pin_9))  Rc.AUX40 |= 0x80;	//KEY_SET  = B9��ԭ����ɫled
	
		//�Ƿ񱻰���
		#define KEY1()  (Rc.AUX40 & 0x01)
		#define KEY2()  (Rc.AUX40 & 0x02)
	
		#define KEY_UP()  (Rc.AUX40 & 0x04)
		#define KEY_DOWN()  (Rc.AUX40 & 0x08)

		#define KEY_RATIO_P()  (Rc.AUX40 & 0x10)
		#define KEY_RATIO_M()  (Rc.AUX40 & 0x20)
	
		#define KEY_LOCK()  (Rc.AUX40 & 0x40)
		#define KEY_SET()  (Rc.AUX40 & 0x80)

	//0.5s��ȡ����ʱ�ƶ�, ��Ӧͨ����Ϊ����ֵ
#define MOVE_TIME 30 //300ms

	//Ϊ�����ƶ���������0.5s���Զ�����
 	static u8 move_time3=0;  //Z+
	static u8 move_time4=0;  //Z-



	//Z+
	//���Ű������Ӷ���ʱ���ѵ������Ҳ��ڼ�������С������������΢�ȵ�
	//if( move_time3>MOVE_TIME  && aux2_long!=1){  
	if( move_time3>MOVE_TIME){  //qux2ȡ��������ɹ���
		
		move_time3=0;
		AUX87_s(0); // �����������Ӷ���������0.5s
		
	}
	else if(move_time3){
		
		move_time3++; //������ʱ
		
		//���û�зſ������ʱ���裬ע�����ﲻ�ܷ��򣬷����ǰ���˲�������
		//�������ʱ�����û���û�г��������������Ļ��������ӳ�����Ҫ�ӳ�0.5s
		//if( move_time3==MOVE_TIME && (!(GPIOC->IDR & aux3) || !(GPIOA->IDR & aux4)) ){
		if( move_time3==MOVE_TIME && KEY_UP() ){	
				move_time3=MOVE_TIME-1; //��Ҫֱ�Ӹ�1����������Ҫ����ֹͣ��ʱ�ƶ�
		}
		
	}
	
	//Z-
	//���Ű������Ӷ���ʱ���ѵ������Ҳ��ڼ�������С������������΢�ȵ�
	//if( move_time3>MOVE_TIME  && aux2_long!=1){  
	if( move_time4>MOVE_TIME){ //qux2ȡ��������ɹ���
		
		move_time4=0;
		AUX88_s(0); // �����������Ӷ���������0.5s
		
	}
	else if(move_time4){
		
		move_time4++; //������ʱ
		
		//���û�зſ������ʱ���裬ע�����ﲻ�ܷ��򣬷����ǰ���˲�������
		//�������ʱ�����û���û�г��������������Ļ��������ӳ�����Ҫ�ӳ�0.5s
		//if( move_time3==MOVE_TIME && (!(GPIOC->IDR & aux3) || !(GPIOA->IDR & aux4)) ){
		if( move_time4==MOVE_TIME && KEY_DOWN() ){
				move_time4=MOVE_TIME-1; //��Ҫֱ�Ӹ�1����������Ҫ����ֹͣ��ʱ�ƶ�
		}
		
	}
	
	


	///////////////////////////����2�����ܼ�////////////////////////////////////////////
	
	
		// �ұ߶��� key1 /////////////////////////////////////////////////////
		if(KEY1())//�ұ߰���B7����״̬�����¼�ʱ
		{
				if(count0!=255)//��δ��������
				{		
						count0++;
					
						if(count0>=100)   //����1s��ֻ����������Ч,����������show.c���壨����=�̰���
						{
							count0=255; //�Ѵ�������
							//beet_falg=2;
							
							//key1����ֻ����������Ч��������Ż����ֳ�����̰���
							//�������ӽ��棬key1ʹ������ļ����򣬲����ֳ�����̰�
							
							sprintf(Show.str_oled, "K1: long ");
							Check_Ch = 1;   //��Ƶ����
							ANO_LED_0_FLASH();		//led��˸
							Show.windows=0; //����������
							
							
							
						}
				}
			
		}
		
		
		//������Ļ����
		else   //�ұ߰���key1=PB7���Ѿ��ɿ�
		{
			if( count0>=4 && count0!=255 ) //�̰��ɿ�, ����˵�
			{ //��show.c����Ҳ����key1,  Key1_Check()
				//if(Show.hardware_type==1) //�ɿط��ص�Ӳ������Ϊ���������
				//{
					
					//Show.windows++;
					//Show.set_flag&BIT6�������˻�ʧȥ��ϵ�󣬻��Զ�����������
					//if(Show.windows>2 || Show.set_flag&BIT6) Show.windows=0;//��������
					
				
				
				
					if(KEY_SET()){ //set+key1��ֻ�����������ϸ���棨��2ҳ����Ч��������ʾ������ʾ�����������治��Ӧset+key1
					
						if(Show.windows==0 || Show.windows==1 ){
							
								xy_set=1;//�Ѿ�����left����Ļ��Ҫˢ�³�set���ɿ�
											
							
								if( Rc.THR > 1050.0f){ //ҡ��У׼ʱ�����ű����ڵײ�
										sprintf(Show.str_oled, "K1:error3");
										beet_falg=2;
									
									
								}
								else{
										//count0=0;
										//beet_falg=1;
										sprintf(Show.str_oled, "MidOffset");
										ANO_Param.OffSet_En = 1; //���� Mid_Offset() 
										ANO_LED_0_FLASH();//��������LED��˸3��, ����������delay0.6s�������������δ�������
								
								}
							

							
							
						
						}
						else{
								//û��������ʾ����������ʾ���治֧�� set+key1
						
						
						}
						

						
						
						
						

					
					}
					else{ //�̰������´���ֻ����������Ч����������Ķ̰���show.c����
						//ע�⣺��show.cҲ��key1�ļ�⣡������������������Ľ���1��2��3��4��key1��Ӧ

							beet_falg=1;//���н��浥����key1������������
						
							if(Show.windows==0){//��������ʱ���Ҽ�ֻ��Ӧ���������嶯���ɾ�����渺��
									
									sprintf(Show.str_oled, "K1: down ");  
									Show.windows=4; //�������˵�����
							}
						

					
					}

			}
			
			count0=0;
			
		}
		
		
		static uint32_t time_key2 =0; //�ϴΰ���ʱ��
		static u8 key2_click=0;//�����������
		
		
		

		
		
		
		
		
		// ��߶��� key2 /////////////////////////////////////////////////////		
		//if(Rc.AUX4&0x02)//��߰��� K2 ����״̬�����¼�ʱ
		if(KEY2())
		{
				if(count1!=255)//��δ��������
				{
						count1++;
					
						//У׼ҡ����ֵ���Ѿ�ת�Ƶ� set+key1
						if(count1>=100)   //����1s��������ɣ�//ҡ����λУ׼
						{
							
							

							
								count1=255; //�Ѵ�������
							
							
								//��������Ƿ�����λ����λ�ã�1400~+/-100����1300~1500����������1600������1300~1600
							
								//�����������£���������������У׼��
								if(!AUX82_isH()){ 
									
													if( Rc.THR > 1100.0f){ //������У׼ʱ�����ű����ڵײ�
															sprintf(Show.str_oled, "K2:error1");
															beet_falg=2;
													}
													else{
															AUX811_s(1); //������У׼

															sprintf(Show.str_oled, "K:MPU6050");
															beet_falg=4; //��3��
														
													
													}
								
								}
								
								//�ڷ����������£����ܱ�֤�ɻ��ѽ�����
								else if( Rc.THR < 1300.0f || Rc.THR > 1700.0f){ //���ű�������λ����
													sprintf(Show.str_oled, "K2:error2");
													beet_falg=2;
									
								}

								else{	
													sprintf(Show.str_oled, "K2:keyFly");
													beet_falg=2;

													AUX81_s(1);//���������붨����ɣ��ɶ�����û���������������ɿ�ʱ����ѵ�ǰ�߶���Ϊ��ͣ�߶ȣ�
													AUX82_s(0); //ȡ����������
													Rc.AUX2 =1000;//�Զ���Ϊ����ģʽ
													AUX73_s(0);//ƫ����Ԥ�ڽ���Ϊ0

								}


							
							

						}
				}
		}
		
		
		else //��߰��� K2 �Ѿ��ɿ����ɿ���һ�Σ�50ms
		{
			if( count1>=4 && count1!=255 ) //�̰�=�л�����ģʽ
			{
				
					if(KEY_SET()){ //�а���SET�������ɿ�ʱ

									//��������Ч״̬����������״̬��ready��
									if(!AUX82_isH()){
										
												beet_falg=2; //��1��
										
												//�������״̬
												AUX82_s(1); //�������ţ��������1000
												//AUX82(1); //����ͼ����
											
												//���յ��������1000��ƫ�����2000����������������״̬ AUX82_isHʱ��
												//�ɿػ�����ɻ����������������µ�readay״̬
												Rc.YAW = 2000; //������Ϊ1000��yawΪ2000 �ɿػ��Զ�����
											
												sprintf(Show.str_oled, "K2: ready");
										
												AUX73_s(0);//ƫ����Ԥ�ڽ���Ϊ0
												Rc.AUX2 =2000;//�Զ���Ϊ��̬ģʽ
										
												xy_set=1;//�Ѿ�����aux2����Ļ��Ҫˢ�³�set���ɿ�
										

											

									}
									//�ڴ���״̬����ȡ�����ɣ��ص���������
									else{
										
												beet_falg=1; //��1��
										
												//���ű�����ʱ���̰��ɿ��Ž�������
												sprintf(Show.str_oled, "K2:cancel");
												//����aux9��ʾ����ʧЧ��ͬʱ������������
												Rc.YAW =1000;//
												Rc.THR=1000; //������Чʱ��ֱ�����1000
									
												//���������������
												AUX82_s(0);//ȡ����������
												//AUX82(0); //����ͼ��ָ�
												AUX73_s(0);//ƫ����Ԥ�ڽ���Ϊ0
											
										
												//�ɻ��ӽ���״̬���Ϊ ����������Ҫ�ָ�֮ǰ����Ķ���/��̬ģʽ
												if(Show.position_old==1){
															Rc.AUX2=1000;
												}
												else{
															Rc.AUX2=2000;
												}
													
										
												xy_set=1;//�Ѿ�����aux2����Ļ��Ҫˢ�³�set���ɿ�
										
									}	
					}
					
					
					else{ //������key2���ɿ�ʱ
						
									

						
						
						
									
									if(!AUX82_isH()){//������������
										
										
												beet_falg=1; //��1��
										
												sprintf(Show.str_oled, "K2: Click");
										
												/*
												beet_falg=3; //��2��
																
												Rc.AUX2 ^= (2000^1000); //�л���ͣģʽ
										
												//�Ƿ���ͣģʽ
												if(Rc.AUX2==2000){
														sprintf(Show.str_oled, "K2:Po= no");
														Show.position_old =0;  //ͨ��������ȷ���Ķ��߶���ʱ����Ҫ���棬�Ա����ʱ�ָ�
														
												} 
												else{
														sprintf(Show.str_oled, "K2:Po=yes");
														Show.position_old =1;
												}
												*/
										

									}
									
									else{//ȡ���������Σ�����������
										
												beet_falg=1; //��1��
										
												sprintf(Show.str_oled, "K2: LOCK ");
												//����aux9��ʾ����ʧЧ��ͬʱ������������
												Rc.YAW =1000;//
												Rc.THR=1000; //������Чʱ��ֱ�����1000
									
												//���������������
												AUX82_s(0);//ȡ����������
												//AUX82(0); //����ͼ��ָ�
												AUX73_s(0);//ƫ����Ԥ�ڽ���Ϊ0
										
									
												//�ɻ��ӽ���״̬���Ϊ ����������Ҫ�ָ�֮ǰ����Ķ���/��̬ģʽ
												if(Show.position_old==1){
															Rc.AUX2=1000;
												}
												else{
															Rc.AUX2=2000;
												}
													
									
									
									}
									
									
									
									//��������
									
									
									//������3�Σ�������
									//if(!KEY_SET()){//���º�ֻ����һ�ζ̰�

									if( SysTick_count-time_key2>55){ //���ʱ�����1s��������������¼
											key2_click=0;//�����������
									}
									
									time_key2	= SysTick_count; //�ϴε�������ʱ��
										
								
									key2_click++;//�����������
									
									
									if(key2_click==2 && KEY_LOCK()){//�����������2��,���Ұ��� LOCK����
										
												key2_click=0;
												beet_falg=3; //��2��
										
										
												//����lock�����رռ���
												//������ͣģʽ�£��л�������������ѹ���ȣ�����ǿ����ֱ�������μ��⡣
												if(AUX833_isH()){//1��ʾ�ѹرռ���
														AUX833_s(0); //�������⣬4�����¼�����
														sprintf(Show.str_oled, "Laser:yes"); 
														xy_set=1;//�Ѿ�����left����Ļ��Ҫˢ�³�set���ɿ�
												
												}
												else{//0��ʾδ�رռ���
														AUX833_s(1); //ǿ�ƹرռ��⣬��ѹ���
														sprintf(Show.str_oled, "Laser: no");
														xy_set=1;//�Ѿ�����left����Ļ��Ҫˢ�³�set���ɿ�
												}
												

									}
									//�л�����ģʽ������й��������߼���ͣģʽ�����û�й��������߼���ѹ����ģʽ
									else if(key2_click>=3){//�����������3��

												key2_click=0;
												beet_falg=3; //��2��
										

												Rc.AUX2 ^= (2000^1000); //�л���ͣģʽ
										
												//�Ƿ���ͣģʽ
												if(Rc.AUX2==2000){
														sprintf(Show.str_oled, "Posit: no");
														Show.position_old =0;  //ͨ��������ȷ���Ķ��߶���ʱ����Ҫ���棬�Ա����ʱ�ָ�
														
												} 
												else{
														sprintf(Show.str_oled, "Posit:yes");
														Show.position_old =1;
														AUX833_s(0); //�������⣬4�����¼�����

												}

										
										
									}

									//}
									
														
									
									
									
					
					}
		
						
						
						
						
						
						
				
			}
			else if( count1==255 ) { //�����ɿ�=����
			
						sprintf(Show.str_oled, "K2:LongUp");
						AUX81_s(0); //������ɽ���
						AUX811_s(0); //������У׼����
			
			}
			
			count1=0;
		}
		

		
	///////////////////////////��������////////////////////////////////////////////
		
		// ����  H+/////////////////////////////////////////////////////		
		if(KEY_UP())//���¾���Ӧ�������ֳ����̰����ɿ���0.5s�ڳ������
		{
			if(count2!=255){
					count2++;

					if(count2>=2) //�̰�����
					{
							count2=255;
							beet_falg=1;//������һ�Σ�50ms
						
						
									//������״̬��ready����
									//������,��̬ģʽһ�����
									if(AUX82_isH()){
											AUX82_s(0);//�ı���������״̬
											//LCD_str_h16(0,0,"K:thr_ok+"); 
											//sprintf(Show.str_oled, "K:thr_ok+");
											sprintf(Show.str_oled, "K: thrFly");
										
									}
									else{
											sprintf(Show.str_oled, "K: hight+");
									
									}
						


									AUX84_s(!AUX84_isH()); //����ģʽʱ�������߶ȣ�ÿ��һ�η�תһ�Σ�


									AUX87_s(1); // ��̬ģʽʱ�������ţ�����0.5s
									move_time3 = 1; //��ʱ����, 0.5s����ֵ�Զ����
				

					}
			
			}

			
		}
		else //�������ɿ����ɿ���һ�Σ�50ms
		{
			if( count2==255 ) //�̰��ɿ�
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "key_up up");
				
				
			}
			count2=0;
		}
		
		
		
		
		
		
		// ���� H- /////////////////////////////////////////////////////		
		if(KEY_DOWN())//���¾���Ӧ�������ֳ����̰����ɿ���0.5s�ڳ������
		{
			if(count3!=255){
					count3++;

					if(count3>=2) //�̰�����
					{
							count3=255;
							beet_falg=1;//������һ�Σ�50ms
						
									if(!KEY_SET()){
											sprintf(Show.str_oled, "K: hight-");

											AUX84_s(!AUX84_isH()); //����ģʽʱ�������߶ȣ�ÿ��һ�η�תһ�Σ�


											AUX88_s(1); // ��̬ģʽʱ�������ţ�����0.5s
											move_time4 = 1; //��ʱ����, 0.5s����ֵ�Զ����
											
									}
									//SET + �� ��ʾ����ֶ�����
									else{
											//sprintf(Show.str_oled, "K: hight-");
											sprintf(Show.str_oled, "key:test2");
											AUX844_s(1);//��������ֶ�����״̬����Ӱ����С�����ʱ��H+���ת����H-�л����
											xy_set=1; //set�ɿ�ʱ��Ҫ������Ļ
									}

						
						
					}
			
			}

			
		}
		else //�������ɿ�
		{
			if( count3==255 ) //�̰��ɿ�
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "key_down up");
				

				
			}
			count3=0;
		}
		
		
			
		
		#define RATIO_TIME 30	//������������
		
	
		// ���� /////////////////////////////////////////////////////		
		if(KEY_RATIO_P())//���¾���Ӧ�������ֳ����̰������ᴥ����������
		{

			

					if(count4<255) count4++;


					if(count4>=4 && count4<100) //�����̰������¼�
					{
							count4=100;//�̰������ɹ�
							beet_falg=1;//������һ�Σ�50ms

						
						
							//���¾���Ӧ
							if(!KEY_SET()){
										if(Show.ratio<0x0f) {
												//Show.ratio++; //Ĭ��ȡ���������ܣ����ű�����
										}
										//���ű���
										sprintf(Show.str_oled, "K+ratio%02i", Show.ratio+1);
							}
							
					}
					
					
					else if(count4>100+RATIO_TIME){//���30�����ں󣬴�����������
					
							count4=100+RATIO_TIME-3; //�´��������ڼ��3������
						
					
							//���¾���Ӧ
							if(!KEY_SET()){
						
										if(Show.ratio<0x0f) {
												//Show.ratio++; //Ĭ��ȡ���������ܣ����ű�����
										}
										//���ű���
										sprintf(Show.str_oled, "K+ratio%02i", Show.ratio+1);
							}

					}

					
					


		}
		else //�������ɿ����ɿ���һ�Σ�50ms
		{
			//if( count4==255 && count4>100) //�̰�
			if( count4>=4 ) //�ɿ������̰ܶ�����
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "RATIO_P up");
				
					//�ɿ�����Ӧ
					if(KEY_SET()){
						
									if((int8_t)ANO_Param.OffSet_Yaw <= 127-5){
											ANO_Param.OffSet_Yaw = ANO_Param.OffSet_Yaw + 1;
											//ANO_Param.SaveFlag=1;			//��ʾ�����棬����������ע�⣺�����Ҫ���� YAWƫ�ƣ��� ��Ҫ �� ANO_Param_READ() ��ע�͵�   YAWƫ�Ƹ�λ
										
									}

									sprintf(Show.str_oled, "Set: Y+1 ");
								
									xy_set=1;
								
				
					}
				
				
				

			}
			count4=0;
		}
				
		


		// ���� /////////////////////////////////////////////////////		
		if(KEY_RATIO_M())//���¾���Ӧ�������ֳ����̰������ᴥ����������
		{
			



					if(count5<255) count5++;


					if(count5>=4 && count5<100) //�����̰������¼�
					{
							count5=100;//�̰������ɹ�
							beet_falg=1;//������һ�Σ�50ms


							//���¾���Ӧ
							if(!KEY_SET()){
											if(Show.ratio>0) {
													//Show.ratio--; //Ĭ��ȡ���������ܣ����ű�����
											}

											//���ű���
											sprintf(Show.str_oled, "K-ratio%02i",Show.ratio+1);
											
							}
							
							
							
					}
					else if(count5>100+RATIO_TIME){//���30�����ں󣬴�����������
					
							count5=100+RATIO_TIME-3; //�´��������ڼ��3������
						
					
						
							//���¾���Ӧ
							if(!KEY_SET()){

											if(Show.ratio>0) {
													//Show.ratio--; //Ĭ��ȡ���������ܣ����ű�����
											}
											

											//���ű���
											sprintf(Show.str_oled, "K-ratio%02i", Show.ratio+1);
											
							}
	
							
							
							
					}
					
					
						

			
		}
		else //�������ɿ����ɿ���һ�Σ�50ms
		{
			//if( count5>=2 && count5<=100 ) //�̰�
			if( count5>=4 ) //�ɿ������̰ܶ�����
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "RATIO_M up");
				
				
					//�ɿ�����Ӧ
					if(KEY_SET()){
						
									if((int8_t)ANO_Param.OffSet_Yaw >= -127+5){
											ANO_Param.OffSet_Yaw = ANO_Param.OffSet_Yaw-1;
											//ANO_Param.SaveFlag=1;			//��ʾ�����棬����ע�⣺�����Ҫ���� YAWƫ�ƣ��� ��Ҫ �� ANO_Param_READ() ��ע�͵�   YAWƫ�Ƹ�λ
										
									}

									sprintf(Show.str_oled, "Set: Y-1 ");
								
									xy_set=1;
								
				
					}

				
				
			}
			count5=0;
		}
				
				
		
		static uint32_t time_key_lock =0; //�ϴΰ���ʱ��
		static u8 key_locks_click=0;//�����������
		
		

		// ���� /////////////////////////////////////////////////////	
		if(KEY_LOCK())//���¾���Ӧ�������ֳ����̰�
		{



					if(KEY_LOCK() && !KEY_SET())//��������LOCK����������������ź�
					{

									//sprintf(Show.str_oled, "Key:LOCK "); //��ʾ��������
									//if( count6!=255 )beet_falg=2;//������һ�Σ���200ms������Ҫ���һЩ,�ɻ��������
					
									Rc.YAW = 1000;//
									Rc.THR = 1000; //������Чʱ��ֱ�����1000
						
									AUX82_s(0);//ȡ����������
						
									AUX73_s(0);//ƫ����Ԥ�ڽ���Ϊ0
						
									//�ɻ��ӽ���״̬���Ϊ ����������Ҫ�ָ�֮ǰ����Ķ���/��̬ģʽ
									if(Show.position_old==1){
												Rc.AUX2=1000;
									}
									else{
												Rc.AUX2=2000;
									}
									
						
					}

					
					if(count6!=255){ 

							count6++;

							if(count6>=2) //�̰�����
							{
									count6=255;//ֻ��Ӧһ�Σ��ɿ�������
								

									//������3�Σ�������
									if(!KEY_SET()){//���º�ֻ����һ�ζ̰�

											if( SysTick_count-time_key_lock>55){ //���ʱ�����1s��������������¼
													key_locks_click=0;//�����������
											}
											
											time_key_lock	= SysTick_count; //�ϴε�������ʱ��
												
										
											key_locks_click++;//�����������
									
											if(key_locks_click>=3){//�����������3��
												
													AUX822_s(1);//�����Կ�ʼ
													key_locks_click=0;
												
													beet_falg=3; 
											}
											else{
													beet_falg=2; 	//������һ�Σ���200ms������Ҫ���һЩ,�ɻ��������
													sprintf(Show.str_oled, "Key:LOCK "); //��ʾ��������
												
											
											}

									}
										

									

									//������ͬʱ����aux9����ʾ����ƫ��
									if(KEY_SET())
									{
										
											beet_falg=1;//������һ�Σ�50ms
										
										
											if(AUX83_isH()){//1��ʾ��ͷģʽ
													//�л�����ͷģʽ
													AUX83_s(0);
													sprintf(Show.str_oled, "HLess:yes");
													xy_set=1;//�Ѿ�����left����Ļ��Ҫˢ�³�set���ɿ�
											
											}
											else{//0��ʾ��ͷģʽ
													//�л�����ͷģʽ
													AUX83_s(1);
													sprintf(Show.str_oled, "HLess: no");
													xy_set=1;//�Ѿ�����left����Ļ��Ҫˢ�³�set���ɿ�
											}
											
											
									}
									else{
													//������ʾ
													//if(*Show.str_oled=='\0'){
													//}
													//else if(!AUX822_isH()){
																	//sprintf(Show.str_oled, ""); //�ָ�������
													//}
									}



							}
							

					
					
					}

			
		}
		else //�������ɿ�
		{
			if( count6==255 ) //�̰��ɿ�
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "key_lock up");

			}
			
			
			if(AUX822_isH()){
					sprintf(Show.str_oled, "Key:test1"); //��ʼ���Ե��
			}
			
			
			AUX822_s(0); //�����Խ���
			
			count6=0;
		}
						
		
		
		
		
		
		
		
		// ���� /////////////////////////////////////////////////////		
		if(KEY_SET()) //���¾���Ӧ�������ֳ����̰�
		{
			
			if(count7!=255){
					count7++;

					if(count7>=2) //�̰�����
					{
							count7=255;
							beet_falg=1;//������һ�Σ�50ms
							sprintf(Show.str_oled, "key: set ");
							xy_set=0;
					}
			
			}


			
		}
		else //�������ɿ���
		{

			if( count7==255) //�̰��ɿ�
			{
					//beet_falg=1;
					if(xy_set==0) sprintf(Show.str_oled, "key:setUp");

			}
			
			
			count7=0;
			
		}
						
		

		
		
		
		
		//10ms���һ��
		if(beet_falg==1 || beet_falg==2 || beet_falg==3 || beet_falg==4 ) //���Ʒ�������
		{
				if(beet_temp>=0){
						BEEP_L; //��������
						LED_Red_On;  //��LED
				} 
				
				beet_temp++;
			
				if( (beet_temp>5 && ( beet_falg==1 || beet_falg==3 ||  beet_falg==4 )) || (beet_temp>20 && beet_falg==2)) //��50ms,������200ms
				{
						
					
						if(beet_falg==4){ //��3��
								beet_falg=3; 
								beet_temp=-10;//���ʱ��
						}
						else if(beet_falg==3){ //��2��
								beet_falg=1; 
								beet_temp=-15;//���ʱ��
						}
						else{
								beet_falg=0;
								beet_temp=0;
						}
						
					
						BEEP_H;  //�ط�����
						LED_Red_OFF;//�ر�LED
				}

		}

}





//4��΢����������20msһ��
void key_scan(void)  //ɨ��4��΢���������򣬿������ֳ������̰�
{

		//�Ƿ񱻰���
		#define  KEY_FRONT() !(GPIOB->IDR & GPIO_Pin_3) //front=B3
		#define  KEY_LEFT()  !(GPIOB->IDR & GPIO_Pin_4) //left=B4
		#define  KEY_BACK()  !(GPIOB->IDR & GPIO_Pin_5) //back=B5
		#define  KEY_RIGHT() !(GPIOB->IDR & GPIO_Pin_6) //right=B6
	
		static u8 count0=0,count1=0;
		static u8 count2=0,count3=0;

		static u8 beet_falg=0,beet_temp;
	
	
	

	///////////////////////////front///////////////////////////////////////////
		if(KEY_FRONT())//front ������
		{
			if(count0!=255)//��δ��������
			{		
					count0++;
				
					if(count0>=25)   //����0.5s
					{
						count0=255; //�Ѵ�������
						beet_falg=2;
						sprintf(Show.str_oled, "K3: long ");
				
					}
			}

		}
		else//front���ɿ�
		{
			if( count0>=2 && count0!=255) //�̰��ɿ�
			{
					beet_falg=1;

				
								if(KEY_SET()){
									
									
									if((int8_t)ANO_Param.OffSet_Pit<=127-5){
											ANO_Param.OffSet_Pit=ANO_Param.OffSet_Pit+5;
											ANO_Param.SaveFlag=1;			//������
										
									}

										sprintf(Show.str_oled, "Set: x+5 ");
									
										xy_set=1;
									
								
								}
								else{
										//sprintf(Show.str_oled, "key:front   ");
										if(Show.force>0) {
												//Show.force--; //Ĭ��ȡ���������ܣ����ȱ��������ϣ�������Ƅӣ��������������޸ģ������ȵ����
											
										}

										sprintf(Show.str_oled, "K+:force%i", 8-Show.force);

								}
								
				
				
				
				

			}
			count0=0;
		}
		
		
		
		
		
		
		
	/////////////////////////// left ///////////////////////////////////////////
		if(KEY_LEFT())//left ������
		{
			if(count1!=255)//��δ��������
			{		
					count1++;
				
					if(count1>=25)   //����0.5s
					{
						count1=255; //�Ѵ�������
						beet_falg=2;
				
				
										sprintf(Show.str_oled, "K6:camera");
										//��������ϵ�ĽǶȿ�����ʱ��=��,��������ͨ��ϰ����ʱ��Ϊ�������԰���ʾ�ķ�һ��
										AUX85_s(!AUX85_isH());

				
				

					}
			}
					

		}
		
		else//left ���ɿ�
		{
			if( count1>=2 && count1!=255) //�̰��ɿ�
			{
					beet_falg=1;

				
								if(KEY_SET()){
										//ANO_Param.OffSet_Rol=ANO_Param.OffSet_Rol-10;
									
										if( (int8_t)ANO_Param.OffSet_Rol>=-127+5 ){
												ANO_Param.OffSet_Rol=ANO_Param.OffSet_Rol-5;
												ANO_Param.SaveFlag=1;			//������
											
										}
									
									
									
										sprintf(Show.str_oled, "Set: y-5 ");
									
										xy_set=1;
								
								}
								else{
										//sprintf(Show.str_oled, "key:left   ");
										//��������ϵ�ĽǶȿ�����ʱ��=��,��������ͨ��ϰ����ʱ��Ϊ�������԰���ʾ�ķ�һ��
										if(AUX73()<96) AUX73_s(AUX73()+1);

										sprintf(Show.str_oled, "K6:yaw%+3i", -AUX73());

								}
				
				
				
				
			}
			count1=0;
		}
		
		
		
		
		
	/////////////////////////// back ///////////////////////////////////////////
		if(KEY_BACK())//back ������
		{
			if(count2!=255)//��δ��������
			{		
					count2++;
				
					if(count2>=25)   //����0.5s
					{
						count2=255; //�Ѵ�������
						beet_falg=2;
						
						sprintf(Show.str_oled, "K4: long ");
						
					}

			}
			
		}
		
		else//back ���ɿ�
		{
			if( count2>=2 && count2!=255) //�̰��ɿ�
			{
					beet_falg=1;
				
				
								if(KEY_SET()){
										//ANO_Param.OffSet_Pit=ANO_Param.OffSet_Pit-10;
									
									
										if( (int8_t)ANO_Param.OffSet_Pit>=-127+5 ){
												ANO_Param.OffSet_Pit=ANO_Param.OffSet_Pit-5;
												ANO_Param.SaveFlag=1;			//������
											
										}
									
									
										sprintf(Show.str_oled, "Set: x-5 ");
									
										xy_set=1;
								
								}
								else{
										//sprintf(Show.str_oled, "key:back   ");

										if(Show.force<7) {
												//Show.force++; //Ĭ��ȡ���������ܣ����ȱ����� ���ϣ�������Ƅӣ��������������޸ģ������ȵ����
										}


										sprintf(Show.str_oled, "K-:force%i", 8-Show.force);

								}
								

				
			}
			count2=0;
		}
		
		
		
		
	/////////////////////////// right ///////////////////////////////////////////
		if(KEY_RIGHT())//right ������
		{
			if(count3!=255){//��δ��������
					count3++;
				
					if(count3>=25)   //����0.5s
					{
						count3=255; //�Ѵ�������
						beet_falg=2;

												sprintf(Show.str_oled, "K5: LED  ");
												//��������ϵ�ĽǶȿ�����ʱ��=��,��������ͨ��ϰ����ʱ��Ϊ�������԰���ʾ�ķ�һ��
												AUX86_s(!AUX86_isH());

					}
			
			}

		}
		
		else//right ���ɿ�
		{
			if( count3>=2 && count3!=255) //�̰��ɿ�
			{
					beet_falg=1;

				
				
								if(KEY_SET()){
										//ANO_Param.OffSet_Rol=ANO_Param.OffSet_Rol+10;
									
									
										if( (int8_t)ANO_Param.OffSet_Rol<=127-5 ){
												ANO_Param.OffSet_Rol=ANO_Param.OffSet_Rol+5;
												ANO_Param.SaveFlag=1;			//������
											
										}
									
									
									
									
									
										sprintf(Show.str_oled, "Set: y+5 ");
									
										xy_set=1;
								
								}
								else{
										//sprintf(Show.str_oled, "key:right   ");
										//������ϵ�ĽǶȿ���˳ʱ��=��,��������ͨ��ϰ��˳ʱ��Ϊ�������԰���ʾ�ķ�һ��
										if(AUX73()>-96) AUX73_s(AUX73()-1);

										sprintf(Show.str_oled, "K5:yaw%+3i", -AUX73());

								}
				
				
				
				
				
				
			}
			count3=0;
		}
		
		

		
	
		
		//10ms���һ��
		if(beet_falg==1 || beet_falg==2) //���Ʒ�������
		{
				BEEP_L; //��������
				LED_Red_On;  //��LED
				beet_temp++;
			
				if( (beet_temp>5 && beet_falg==1) ||(beet_temp>20 && beet_falg==2)) //��50ms,������200ms
				{
						beet_temp=0;
						beet_falg=0;
						BEEP_H;  //�ط�����
						LED_Red_OFF;//�ر�LED
				}

		}
	
	
}






/******************************************************************************
4��΢����������20msһ��,����Ѿ�������key_scan()���
*******************************************************************************/ 
void ANO_key(void)//ֻ����Ӧ���¶������������ֳ��̰�
{
		#define KEY3 GPIO_Pin_3			//front B3
		#define KEY4 GPIO_Pin_4			//left  B4
		#define KEY5 GPIO_Pin_5 		//back  B5
		#define KEY6 GPIO_Pin_6 		//right B6
	
		//#define KEY2 GPIO_Pin_2 		//ƫ����������B2δ����
		//#define KEY11 GPIO_Pin_11 	//ƫ����������B11δ����
	
	
		volatile static uint8_t status = 0;	
		volatile static uint8_t BEET_flag=0;
		static uint32_t temp,BUT_mun;
		
	
		switch(status)
		{
			
				//����ʱ���а������ɿ���ȫ���Ǹߵ�ƽ��
				case 0:
					if(SysTick_count - temp >10) //������10*20=200ms���ж��Ƿ��ɿ����Źرշ���������200ms
					{
						//�������������
						//if(	(GPIOB->IDR & (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_2|GPIO_Pin_11)) 
						//			== (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_2|GPIO_Pin_11)	)
						if(	(GPIOB->IDR & (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6))
									== (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)	)
						
							status = 1;  //����ʱ����
						
						BEEP_H;  //�رշ�����
						LED_Red_OFF;//�ر�LED
					}
					break;
					
					
				//����ʱ�а��������£�����1���͵�ƽ��
				case 1:
					
						//if((GPIOB->IDR & (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_2|GPIO_Pin_11)) 
						//			!= (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_2|GPIO_Pin_11))
						if((GPIOB->IDR & (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)) 
									!= (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6))
						status = 2; //���밴�º��ҵ����
						
					break;
						
						
						
						
						
						
						
						
						
						
						
						
				
				//���º��ҵ����
				case 2:	
					
				
					if(!(GPIOB->IDR & KEY6))   //��
					{
						
		//					Rc.AUX1 = 2000 - (uint16_t)(0.25f*_offset.roll);	
							//if(ANO_Param.OffSet_Rol == ANO_Param.Z_OffSet_Rol)      //΢����ԭʼֵ�ж�
							//{	BEET_flag=1;  }				
					}
					
					
					else if(!(GPIOB->IDR & KEY4)) //��
					{

		//					Rc.AUX1 ^= 2000 - (uint16_t)(0.25f*_offset.roll);		
							//if(ANO_Param.OffSet_Rol == ANO_Param.Z_OffSet_Rol)    //΢����ԭʼֵ�ж�
						  //{	BEET_flag=1;  }	
					}
					
					
					
					else if(!(GPIOB->IDR & KEY5))//��
					{

							//if(ANO_Param.OffSet_Pit == ANO_Param.Z_OffSet_Pit)    //΢����ԭʼֵ�ж�
						  //{	BEET_flag=1;  }	
					}
					
					
					
					
					else if(!(GPIOB->IDR & KEY3)) //ǰ
					{

							//if(ANO_Param.OffSet_Pit == ANO_Param.Z_OffSet_Pit)   //΢����ԭʼֵ�ж�
						  //{	BEET_flag=1;  }
					}
					

					
					/*
					else if(!(GPIOB->IDR & KEY2))
					{
							ANO_Param.OffSet_Yaw=ANO_Param.OffSet_Yaw+10; 
						  if(ANO_Param.OffSet_Yaw == ANO_Param.Z_OffSet_Yaw)   //΢����ԭʼֵ�ж�
						  {	BEET_flag=1;  }
					}
					else if(!(GPIOB->IDR & KEY11))
					{
							ANO_Param.OffSet_Yaw=ANO_Param.OffSet_Yaw-10; //
							if(ANO_Param.OffSet_Yaw == ANO_Param.Z_OffSet_Yaw)   //΢����ԭʼֵ�ж�
						  {	BEET_flag=1;  }
					}	
					*/
					
					status = 0;		//ҵ������ϣ����»ص��ɿ���⡣
					
					BEEP_L;  //�򿪷�����
					LED_Red_On;  //��LED
					temp = SysTick_count; //��¼����ʱ�䣬�´�ѭ����Ҫ��20*10=200ms��ͬʱҲ�Ƿ�������ʱ��
					
					
					break;
					
					
					
		}
		
			
			
		//20ms����һ�Σ�
		//ÿ����10*20=0.2s
		if(BEET_flag!=0)	//΢����ԭʼֵ����2��������
		{
					BUT_mun++;								     //������ʱ��
					
					//   1......... 11......... 21......... 31......... 41......... 51........60
				  //       ��          ��           ��           ��         ��          ��
			
					if(BUT_mun==1||BUT_mun==20||BUT_mun==40)   //��ʱ�ж�
					{																								
							BEEP_L;
							LED_Red_On;//��LED
																			
					}
					
					else if((BUT_mun>10 && BUT_mun<20) || (BUT_mun>30 && BUT_mun<40) || (BUT_mun>50 && BUT_mun<61))  //��ʱ�ж�
					{
							BEEP_H;  						  
							LED_Red_OFF;  //��LED
						 if(BUT_mun==60){BEET_flag=0;BUT_mun=0;}
					}
 
		}
		
		
		
}











