#include "show.h"
#include "oledfont.h" 

//#include "oled.h" //spi����ѡһ�����ö˿�  ��sysconfig.h ҲҪ��ѡһ
#include "oled_i2c.h" //i2c

#include "stdio.h"  //	sprintf()
#include <math.h>
#include <string.h> //�Ƚ��ַ���

#define Line1_Begin 29+4
#define Line2_Begin 5
#define Line3_Begin 5
#define Line4_Begin 30
#define Line5_Begin 2


#define Z_Begin 0+5 //yaw
#define Y_Begin 51 //pitch
#define X_Begin 103-3//roll


#define Line1_Begin1 0
#define Line2_Begin1 0
#define Line3_Begin1 40
#define Line4_Begin1 0
#define Line5_Begin1 0

#define Y0 0   		//����
#define Y1 14			//1
#define Y2 Y1+13	//2
#define Y3 Y2+13	//3
#define Y4 Y3+13	//4
#define Y5 Y4+12  //û��

struct _Show Show={.position_old=1};

//Show.unLock=1;

unsigned char i;          //��������
unsigned char Send_Count; //������Ҫ���͵����ݸ���
float Vol;

/**************************************************************************
�������ܣ�OLED��ʾ
��ڲ�������
����  ֵ����
**************************************************************************/
//������ˢ��
void oled_show(void)
{
	
	char str_tmp[20]; //��ʾ�ַ�
	
	//u16 temp;
	int16_t temp;
	
	int temp1;
	
	static u8 page,page_temp,flash_cnt,show_mode=1;
	
	if(page != page_temp)//�л�ҳ��������
	{
		page_temp = page;
		OLED_Clear();
	}
	
	
	///////////////////////////////��һ��///////////////////////////////////

	if(Show.low_power)//ң�ص͵���
	{
		flash_cnt ++;
		if(flash_cnt>3) 
		{
			flash_cnt=0;
			if(show_mode==0)show_mode=1;
			else show_mode = 0;
		}
		
		
		for(u8 i=0;i<12;i++) OLED_Show_CH_String(Line1_Begin+i*6,Y0,oled_CH16[i],12,show_mode);//ң�ص�ѹ��
		
	}
	
	
	else if(Show.test_flag&BIT5)//�ɻ��͵���
	{
		flash_cnt ++;
		if(flash_cnt>3) 
		{
			flash_cnt=0;
			if(show_mode==0)show_mode=1;
			else show_mode = 0;
		}
		
		for(u8 i=0;i<12;i++) OLED_Show_CH_String(Line1_Begin+i*6,Y0,oled_CH17[i],12,show_mode);//�ɻ���ѹ��
	}
	
	
	else  //��ʾ��һ���ַ���
	{
		OLED_Show_CH(Line1_Begin+00,Y0,0,12,1);
		OLED_Show_CH(Line1_Begin+12,Y0,1,12,1);
		OLED_Show_CH(Line1_Begin+24,Y0,2,12,1);
		OLED_Show_CH(Line1_Begin+36,Y0,3,12,1);
		OLED_Show_CH(Line1_Begin+48,Y0,4,12,1);
		OLED_Show_CH(Line1_Begin+60,Y0,5,12,1);  //�߶�12�����12
	}
	
	
	
	
	//sprintf(Show.str_oled, "S=%+3i    ", 12);
	//��ʾ����������¼
	
	static char str_oled_old[20]; //��һ����ʾ�ַ�
	static uint32_t time_str_oled =0; //�ϴθ���ʱ��
	
	
	//���������и���
	if(memcmp( str_oled_old, Show.str_oled, 9)!=0){
			time_str_oled = SysTick_count; //�ϴθ���ʱ��
			memcpy(str_oled_old, Show.str_oled, 20);
	};
	
	//��ʾ��������
	if(SysTick_count-time_str_oled <300){ //ֻ��ʾ3�룬��ֹ��ʱ�䵲ס�����Լ��͵�ѹ����Ϣ
				OLED_ShowString(Line1_Begin+00,Y0,(u8 *)Show.str_oled,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
	}
	else{
				//�ָ�����ʱ����հ�����ʾ�ַ����������ٴΰ�������ʱ������ʾ
				sprintf(Show.str_oled, "");
				sprintf(str_oled_old, "");
	
	}



	
	
	//////////////////////////////////////////////////////
	if(Show.NRF_Err)	OLED_Show_CH(2,Y0,6 ,12,1);//����ģ�������ʾX
	else				OLED_ShowNumber(2,Y0,ANO_Param.NRF_Channel,3,12);//��ʾ�����ŵ�
	
	//��ʾ�ź�ǿ��
	temp = Show.Rc_num/20;
	switch(temp)
	{
		case 0:OLED_Show_CH(Line1_Begin+85,Y0,6 ,12,1);break;  //6Ϊ���
		case 1:OLED_Show_CH(Line1_Begin+85,Y0,7 ,12,1);break;  //1��
		case 2:OLED_Show_CH(Line1_Begin+85,Y0,8 ,12,1);break;//2��
		case 3:OLED_Show_CH(Line1_Begin+85,Y0,9 ,12,1);break;//3��
		case 4:OLED_Show_CH(Line1_Begin+85,Y0,10,12,1);break;//4��
		default:OLED_Show_CH(Line1_Begin+85,Y0,11,12,1);break;//5��
	}
	
	
	
	
	///////////////////////////////�ڶ���///////////////////////////////////
	//��ʾ��ѹ	
	OLED_ShowString(Line2_Begin+00,Y1,"R-V:",12,1);
	OLED_ShowString(Line2_Begin+36,Y1,".",12,1);
	OLED_ShowNumber(Line2_Begin+30,Y1,Show.Battery_Rc/100,1,12);
	OLED_ShowNumber(Line2_Begin+42,Y1,Show.Battery_Rc%100,2,12);
	if(Show.Battery_Rc%100<10)	OLED_ShowNumber(Line2_Begin+42,Y1,0,1,12);
	
	OLED_ShowString(Line2_Begin+64,Y1,"F-V:",12,1);
	OLED_ShowString(Line2_Begin+100,Y1,".",12,1);
	OLED_ShowNumber(Line2_Begin+94,Y1,Show.Battery_Fly/100,1,12);
	OLED_ShowNumber(Line2_Begin+106,Y1,Show.Battery_Fly%100,2,12);
	if(Show.Battery_Fly%100<10)	OLED_ShowNumber(Line2_Begin+106,Y1,0,1,12);
	
	
	
	
	///////////////////////////////����������/////////////////////////////////
	//��ʾң������
	OLED_ShowString(Line3_Begin+00,Y2,"THR:",12,1);
	
	if(AUX82_isH()) OLED_DrawLine(Line3_Begin+00,Y2+6,Line3_Begin+3*8,Y2+6,1); //����
	
	//ģ��ɿؽ���ֵStick_dis.THR
	//����ҡ��ֵ  Rc.THR
	temp = (Stick_dis.THR-1000)/41;//1500ʱ���������ĵ�һ���ַ�Ӧ��������12���� 500/41=12
	OLED_Show_progress_bar(temp,12,24,Line4_Begin+6 ,Y2,12,1);
	temp = (Stick_dis.THR-1500)/41;//�������ڶ����ַ���Ĭ��Ϊ��(12)
	OLED_Show_progress_bar(temp,12,12,Line4_Begin+17,Y2,12,1);
	
	OLED_ShowString(Line3_Begin+00,Y3,"YAW:",12,1);
	temp = (Stick_dis.YAW-1000)/41;
	OLED_Show_progress_bar(temp,12,24,Line4_Begin+6 ,Y3,12,1);
	temp = (Stick_dis.YAW-1500)/41;
	OLED_Show_progress_bar(temp,12,12,Line4_Begin+17,Y3,12,1);
	
	
	OLED_ShowString(Line3_Begin+64,Y2,"PIT:",12,1);
	temp = (Stick_dis.PIT-1000)/41; 
	OLED_Show_progress_bar(temp,12,24,Line4_Begin+70,Y2,12,1);
	temp = (Stick_dis.PIT-1500)/41; if(temp>12) temp=12;
	OLED_Show_progress_bar(temp,12,12,Line4_Begin+81,Y2,12,1);
	
	
	OLED_ShowString(Line3_Begin+64,Y3,"ROL:",12,1);
	temp = (Stick_dis.ROL-1000)/41;
	OLED_Show_progress_bar(temp,12,24,Line4_Begin+70,Y3,12,1);
	temp = (Stick_dis.ROL-1500)/41; if(temp>12) temp=12;
	OLED_Show_progress_bar(temp,12,12,Line4_Begin+81,Y3,12,1);
	
	

	
	///////////////////////////////������///////////////////////////////////
	if(Show.Rc_num && Show.hardware_type == 1) //�����ӷɿأ����ź�
	{
		page = 1;//��һҳ
		//��ʾ��̬�ǶȺ͸߶�
		
		
		//��ʾ�߶�������
		/*
		if(Show.H<0) 			temp1 =-Show.H,  OLED_ShowString(Z_Begin,Y4,"-",12,1);
		else    			temp1 = Show.H,  OLED_ShowString(Z_Begin,Y4,"+",12,1);
		*/
		
		//�����ʾ999����,����999�ס�
		temp1 = Show.H;
		if(temp1>99900) temp1 = 99900, OLED_ShowString(Z_Begin,Y4,">",12,1); //>999��ʱ
		
		if(temp1<1000){ //����10��ʱ����λ�л�Ϊ��
				sprintf(str_tmp, "%+dcm",temp1);//��ʾ�߶ȣ���λ����cm
		}
		else{
				sprintf(str_tmp, "%+dM",temp1/100);//��ʾ�߶ȣ���λΪ��M
		}

		OLED_ShowString_num(Z_Begin,Y4,(u8 *)"      ", 12,1);
		OLED_ShowString_num(Z_Begin,Y4,(u8 *)str_tmp,12,1);//��ʾ�߶�
		

		
		
		
		
		/*
		if(Show.X<0) temp1 = -Show.X,OLED_ShowString(X_Begin,Y4,"-",12,1);
		else         temp1 = Show.X, OLED_ShowString(X_Begin,Y4,"+",12,1);
		OLED_ShowNumber(X_Begin+8,Y4,temp1/10,3,12);
		OLED_ShowString(X_Begin+28,Y4,".",12,1);
		OLED_ShowNumber(X_Begin+32,Y4,temp1%10,1,12);
		*/
		
		//pitch
		OLED_ShowString_num(Y_Begin,Y4, (u8 *)"          ", 12,1);
		sprintf(str_tmp, "%+.1f",Show.Y/10.0f);
		OLED_ShowString_num(Y_Begin,Y4, (u8 *)str_tmp, 12,1);
		
		
		/*
		if(Show.Y<0) temp1 = -Show.Y,OLED_ShowString(Y_Begin,Y4,"-",12,1);
		else    temp1 = Show.Y, OLED_ShowString(Y_Begin,Y4,"+",12,1);
		OLED_ShowNumber(Y_Begin+8,Y4,temp1/10,3,12);
		OLED_ShowString(Y_Begin+28,Y4,".",12,1);
		OLED_ShowNumber(Y_Begin+32,Y4,temp1%10,1,12);
		*/
		

		
		
		OLED_ShowString_num(X_Begin,Y4, (u8 *)"      ", 12,1);
		sprintf(str_tmp, "%+.1f",Show.X/10.0f);
		u8 begin_offset =0;
		if(abs(Show.X)>=100) begin_offset=1;
		if(abs(Show.X)>=1000) begin_offset=2;
		
		OLED_ShowString_num(X_Begin - 6*begin_offset ,Y4, (u8 *)str_tmp, 12,1);
		


	}
	
	
	else //��δ���Ϸɿ�
	{
		page = 2;//�ڶ�ҳ
		
		//��ʾ΢����ť����,,΢����Χ��
		OLED_ShowString(Line5_Begin+00,Y4,"Y:",12,1);
		
		//temp = (ANO_Param.OffSet_Yaw-1000)/41; //Ĭ��Ϊ����
		temp = ((int8_t)ANO_Param.OffSet_Yaw+128)/10; //Ĭ��Ϊ����
		OLED_Show_progress_bar(temp,12,24,Line5_Begin+15 ,Y4,12,1);
		
		//temp = (ANO_Param.OffSet_Yaw-1500)/41; //Ĭ��Ϊ��
		temp = (ANO_Param.OffSet_Yaw-0)/10; //Ĭ��Ϊ��
		OLED_Show_progress_bar(temp,12,12,Line5_Begin+26,Y4,12,1);

		
		
		//��ʾ΢����ť����,,΢����Χ��
		OLED_ShowString(Line5_Begin+44,Y4,"P:",12,1);
		
		temp = ((int8_t)ANO_Param.OffSet_Pit+128)/10; //+/-128 
		OLED_Show_progress_bar(temp,12,24,Line5_Begin+59 ,Y4,12,1);
		
		temp = (ANO_Param.OffSet_Pit-0)/10;
		OLED_Show_progress_bar(temp,12,12,Line5_Begin+70,Y4,12,1);
		
		
		
		
		
		//��ʾ΢����ť����,,΢����Χ��
		OLED_ShowString(Line5_Begin+88,Y4,"R:",12,1);
		
		//������24�����أ���������ʾ��Ư������ֵ+/-128
		temp = ((int8_t)ANO_Param.OffSet_Rol+128)/10; //����ֵ+/-128 ,,0ʱ���������ĵ�һ���ַ�Ӧ��������12���� 128/10=12
		OLED_Show_progress_bar(temp,12,24,Line5_Begin+103 ,Y4,12,1);
		
		temp = (ANO_Param.OffSet_Rol-0)/10;
		OLED_Show_progress_bar(temp,12,12,Line5_Begin+114,Y4,12,1);
		
		


	}
	
	OLED_Refresh_Gram();//��ʼ��ʾ
	
	
}



//��������ʾ������
void OLED_Show_progress_bar(u8 temp,u8 chr_star,u8 chr_default,u8 x,u8 y,u8 size,u8 mode)
{
	switch(temp)
	{
		//����ǵ��������ַ���ȵĽ�������12���ؿ��ֿ��12��24��12Ϊ�ף�24Ϊ����
		case  0:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case  1:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case  2:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case  3:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case  4:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case  5:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case  6:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case  7:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case  8:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case  9:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case 10:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case 11:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		case 12:OLED_Show_CH(x,y,chr_star+temp,size,size);break;
		
		default:OLED_Show_CH(x,y,chr_default,size,size);break;
	}
}





vs16 detail_bit=0; //��һҳ
vs16 detail_max=1; //�ڶ�ҳ
//����������棺
void OLED_Show_detail(void)
{
	u8 cnt=0;//��ǰ��Ļy����
	char str_tmp[20]; //��ʾ�ַ�
	
	u8 page;
	static u8 page_temp;
	
	static u8 flash_cnt,show_mode=1;
	
	page=detail_bit;
	
	if(page_temp!=page)
	{
		page_temp=page;
		OLED_Clear();
	}
	
	switch(page)
	{
		
		//��һҳ����Ҫ��ʾң������ص�����
		case 0:

						///////////////��һ��////////////////////////////
						for(u8 i=0;i<8;i++) OLED_Show_CH_String(34+i*6,cnt,oled_CH21[i],12,1);//��ϸ���ݣ�


						//�˳���ϸ����ʱ����Ҫ�������
						if(ANO_Param.SaveFlag){//��������� ���Ȼ���Ư������ֵ
									for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH28[i],12,0);//����
						}
						else{
									for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH27[i],12,0);//��ҳ
						}


						
						///////////////��һ��////////////////////////////
						//��ʾ��������
						OLED_ShowString(10,Y0,(u8 *)Show.str_oled,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						
						
						//�Ƿ���ͷģʽ,
						if(AUX83_isH()){
								OLED_ShowString(00,Y0,(u8 *)"H",12,0);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						} 
						else{// Ĭ��Ϊ0����ͷģʽ
								OLED_ShowString(00,Y0,(u8 *)"L",12,0);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						}


						
						///////////////�ڶ���////////////////////////////
						//sprintf(str_tmp, "THR%1i, YAW%3i",Rc.THR,Rc.YAW);
						sprintf(str_tmp, "T%1i,Y%3i,%+3i",Stick_dis.THR,Stick_dis.YAW,-AUX73());
						OLED_ShowString(10,Y1,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						
						//�Ƿ���������
						if(AUX82_isH()) OLED_DrawLine(10,Y1+6,6*8,Y1+6,1); //����
						
						//�Ƿ���ͣģʽ
						if(Rc.AUX2==2000){
								OLED_ShowString(00,Y1,(u8 *)"O",12,0);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						} 
						else{//Ĭ��Ϊ�ͣ���Ϊ��ͣģʽ
								OLED_ShowString(00,Y1,(u8 *)"P",12,0);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
							
								if(!AUX833_isH()) OLED_DrawLine(4,Y1+8, 8,Y1+8,0); //����
								if(!AUX833_isH()) OLED_DrawLine(6,Y1+6, 6,Y1+11,0); //����

						}

						
								//�Ƕȣ���ʱ��Ϊ��������Ϊ���ٶȷ�����xyz���귽��һ��
								//          x+	
								//         /
								//  y+ ___/___ -
								//       /|
								//      / |
								//     -	|
								//         z+  
						
						

						
						///////////////������////////////////////////////
						
						//sprintf(str_tmp, ",P%3i",Rc.PIT);
						sprintf(str_tmp, "P%04i,",Stick_dis.PIT);
						OLED_ShowString(2,Y2,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						
						//sprintf(str_tmp, ",R%3i",Rc.ROL);
						sprintf(str_tmp, "R%04i,",Stick_dis.ROL);
						OLED_ShowString(50,Y2,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						
						sprintf(str_tmp, "Y%+03i", (int16_t)ANO_Param.OffSet_Yaw);
						OLED_ShowString(98,Y2,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						

						

						
						///////////////������////////////////////////////

						sprintf(str_tmp, "x%+03i%+03i,", (int16_t)ANO_Param.OffSet_Pit,(int16_t)ANO_Param.Z_OffSet_Pit);
						OLED_ShowString(2,Y3,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						
						
						sprintf(str_tmp, "y%+03i%+03i", (int16_t)ANO_Param.OffSet_Rol,(int16_t)ANO_Param.Z_OffSet_Rol);
						OLED_ShowString(74,Y3,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						

						
						

						
						///////////////������////////////////////////////
						sprintf(str_tmp, "R:%02i|F:%1i|x%02x|x%02x",Show.ratio+1,  8-Show.force,  (uint16_t)Rc.AUX7>>8,  Rc.AUX8); //aux7��ʾ���ֽڣ�aux8ֻ���˵��ֽ�
																												//���ű���,          ���ȣ�          Ŀ��ƫ���ǣ�   aux8
						OLED_ShowString(02,Y4,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�

						

		
			
		break;
		
		//�ڶ�ҳ����Ҫ��ʾ�ɿ��������
		case 1:
			
						//OLED_Clear();
		
		
						///////////////��һ��////////////////////////////
						//���Ͻǣ��˳���ť
						for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,Y0,oled_CH24[i],12,0);//�˳�

						
						
						//�Ƿ���ͷģʽ,
						if(AUX83_isH()){
								OLED_ShowString(00,Y0,(u8 *)"H",12,0);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						} 
						else{// Ĭ��Ϊ0����ͷģʽ
								OLED_ShowString(00,Y0,(u8 *)"L",12,0);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						}
						
						

						if(Show.test_flag&BIT5)//�ɻ��͵���
						{
									flash_cnt ++;
									if(flash_cnt>3) 
									{
										flash_cnt=0;
										if(show_mode==0)show_mode=1;
										else show_mode = 0;
									}
							
						}
						else{
									show_mode=1;
						}
						
						//��������
						sprintf(str_tmp, "key input|");
						OLED_ShowString_num(11,Y0,(u8 *)str_tmp,12,1); 
						
						//��ʾ����
						sprintf(str_tmp, "%.2fv",Show.Battery_Fly/100.0f);
						OLED_ShowString_num(11+6*10,Y0,(u8 *)str_tmp,12,show_mode); 
						
						
						
						
						
						//��ʾ��������
						OLED_ShowString_num(11,Y0,(u8 *)Show.str_oled,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						
		
						
						
						
						
						
						///////////////�ڶ���////////////////////////////
						//�Ƿ���ͣģʽ
						if(Rc.AUX2==2000){
								OLED_ShowString(00,Y1,(u8 *)"O",12,0);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						} 
						else{//Ĭ��Ϊ�ͣ���Ϊ��ͣģʽ
								OLED_ShowString(00,Y1,(u8 *)"P",12,0);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
							
								if(!AUX833_isH()) OLED_DrawLine(4,Y1+8, 8,Y1+8,0); //����
								if(!AUX833_isH()) OLED_DrawLine(6,Y1+6, 6,Y1+11,0); //����

						}


						 
						OLED_ShowString(11,Y1,(u8 *)"                ",12,1); 
						
						//�����⻷λ��pid����(λ��)
						sprintf(str_tmp, "%+i,%+i",-Show.x_desired,-Show.y_desired);
						
						OLED_ShowString_num(11,Y1,(u8 *)str_tmp,12,1); 
						

						
						//������ǰλ��
						sprintf(str_tmp, "|%+i,%+i", -Show.loc_x ,-Show.loc_y);//��ǰ�߶ȣ���λcm������4m����ѹ��
						OLED_ShowString_num(65,Y1,(u8 *)str_tmp,12,1); 


						//Ŀ��ƫ���� /45��
						//sprintf(str_tmp, "%+i",AUX73());//���Ź���
						//OLED_ShowString_num(116,Y1,(u8 *)str_tmp,12,1); 
						 
						//��������, 0x20���´������������
						sprintf(str_tmp, "%02x",Show.qual);//���Ź���
						OLED_ShowString_num(116,Y1,(u8 *)str_tmp,12,1); 
						
						
						
						
						
						
						///////////////������////////////////////////////
						//�Ƿ��ѽ���
						if(Show.unLock){
								OLED_ShowString(00,Y2,(u8 *)"A",12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						} 
						else{//Ĭ��Ϊ�ͣ���Ϊ��ͣģʽ
								OLED_ShowString(00,Y2,(u8 *)"N",12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						}
						
						OLED_ShowString(11,Y2,(u8 *)"                ",12,1); 
						
						//������̬�ǣ����Է���ҡ�ˣ����߹����ڻ����
						sprintf(str_tmp, "%+.1f,%+.1f",Show.pitch_desired/10.0f,Show.roll_desired/10.0f);//��̬��*10��
						OLED_ShowString_num(11,Y2,(u8 *)str_tmp,12,1); 
						
						//��ǰ��̬��
						sprintf(str_tmp, "|%+.1f,%+.1f",Show.Y/10.0f,Show.X/10.0f);//��̬��*10��, �����Y��ֵpitch������Y����
						OLED_ShowString_num(65,Y2,(u8 *)str_tmp,12,1); 
						
						
						
						
						
						
						
		
						///////////////������////////////////////////////
						sprintf(str_tmp, "%i", Show.debug1);//������ϸģʽ
						OLED_ShowString(0,Y3,(u8 *)str_tmp,12,0); 
						

						OLED_ShowString(11,Y3,(u8 *)"                ",12,1); 
		
					
						//�����߶ȣ���ǰ�߶�
						sprintf(str_tmp, "%+i,%+i", Show.high_desired, Show.Control_high);//��ǰ�߶ȣ���λcm������4m����ѹ��
						OLED_ShowString_num(11,Y3,(u8 *)str_tmp,12,1); 

						
						
						//�����ڻ��ٶ�pid���(������̬�����)
						sprintf(str_tmp, "|%+i,%+i",Show.x_out,Show.y_out);
						
						OLED_ShowString_num(65,Y3,(u8 *)str_tmp,12,1); 
						






		
						///////////////������////////////////////////////
						/*
						Aux_Rc.debug1 = hight_mode;//��ϸ����ģʽ��0Ϊ��̬
						Aux_Rc.debug2 = Remote.thr -1000;// �������ţ�ҡ��ֵ����������ֻ��������������������������и�
						Aux_Rc.debug3 = thr_base_add;//һ��������ɵĻ������ǲ���
						Aux_Rc.debug4 = thr_adjust;//			��ͨ����ģʽ�£��Զ����� ƽ�����ţ�ƽ������ָ���ڵ����ɻ�����������
						Aux_Rc.debug5 = thr_hight_modify;// ��ʱ�������������ڸ߶��������½����̡�
						*/


						
						//�Ƿ���������
						if(!AUX82_isH()){
									OLED_DrawLine(0,Y4+6,6*9,Y4+6,0); //����
						}

									
		
						//�������ţ��������Ų���
						sprintf(str_tmp, "T=");//���Ź���
						OLED_ShowString_num(2,Y4,(u8 *)str_tmp,12,1); 
						
						
						OLED_ShowString(17,Y4,(u8 *)"                ",12,1); 
						
						sprintf(str_tmp, "%i %+i",Show.debug2,Show.debug3);//���Ź���   Remote.thr-1000, thr_base_add
						OLED_ShowString_num(17,Y4,(u8 *)str_tmp,12,1); 
						
						//ƽ�����ţ���ʱ��������
						sprintf(str_tmp, "%+i %+i",Show.debug4,Show.debug5);//���Ź���  thr_adjust, thr_hight_modify
						OLED_ShowString_num(63,Y4,(u8 *)str_tmp,12,1); 
						
						
						//�Ƿ���������
						if(AUX82_isH()){
									OLED_DrawLine(0,Y4+6,6*9,Y4+6,1); //����
						}
		
						
						/*
						//ƫ����С��λ�������鿴�Ƿ��ƫ����Ư��
						if(Show.Z>0){
									OLED_ShowString_num(116,48,(u8 *)"-",12,1);  //�ɿط�����ʱ����yaw�����ˣ����ǰ����ָ�����ʱ��Ϊ����
						}
						else if(Show.Z<=0){
									OLED_ShowString_num(116,48,(u8 *)"+",12,1); 
						}
						
						OLED_ShowNumber(121,Y3,(Show.Z%100)/10,1,12); //�����ڶ�λ������Show.Z=yaw*10
						OLED_ShowNumber(121,Y4,Show.Z%10,1,12); //���һλ��С��λ��������Show.Z=yaw*10
						OLED_DrawPoint(118,62,1);// ����
						*/
						
						
						
						//ƫ�����ٶ�С��λ�������鿴�Ƿ��ƫ����Ư��
						//��ֹʱyaw_Gs��Ϊ�㣬����mpu6050У׼�������з���ҡ�˻���ʱyaw_Gs��������yaw��Ư����
						//Show.yaw_Gs=0.01; 0.06��/��=1(mpu6050ԭʼ����)
						if(Show.yaw_Gs>=0.1){
									OLED_ShowString_num(116,48+1,(u8 *)"+",12,1);  //yaw_s ���ٶȣ���ʱ��Ϊ����
						}
						else if(Show.yaw_Gs<=-0.1){
									OLED_ShowString_num(116,48+1,(u8 *)"-",12,1); 
						}
						else{
									OLED_ShowString_num(116,48+1,(u8 *)" ",12,1); 
						}

						if(Show.yaw_Gs>=10.0f || Show.yaw_Gs<=-10.0f) Show.yaw_Gs=9.9;//����10ʱ��ʾ9.9
						OLED_ShowNumber(121,Y3,(int)(Show.yaw_Gs*1)%10,1,12); //�������1λ������
						OLED_ShowNumber(121,Y4,(int)(Show.yaw_Gs*10)%10,1,12); //С����1λ����������
						
						OLED_DrawPoint(118,62-1,1);// ����
						
						
						
						//pixel_flow.err1_cnt;//pixel_flow.err2_cnt
						//��ʾ������λ����
						//OLED_ShowNumber(121,Y3,(Show.debug6>>8)%10,1,12); //pixel_flow.err1_cnt
						//OLED_ShowNumber(121,Y4,(Show.debug6)%10,1,12); //pixel_flow.err2_cnt
						
						
						/*
						
						//һ��16���ַ��� 0x11 22 33 44 55
						//Show.debug2=-1;
						if(Show.debug2<0){
								sprintf(str_tmp, "%02x|-%02x|%02x|%02x|   ",Show.debug1,-Show.debug2,Show.debug3,Show.debug4);
						}
						else{
								sprintf(str_tmp, "%02x|+%02x|%02x|%02x|   ",Show.debug1,Show.debug2,Show.debug3,Show.debug4);
						}
						OLED_ShowString(0,Y4,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						
						
						//Show.debug5=-1;
						if(Show.debug5<0){
								sprintf(str_tmp, "-%02x| ",-Show.debug5);
						}
						else{
								sprintf(str_tmp, "+%02x| ",Show.debug5);
						}
						OLED_ShowString(12*8,Y4,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
						
						*/
						
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		
			
		break;
		
		
		
		default:break;
		
		
	}
	
	
	

	

	OLED_Refresh_Gram();//��ʼ��ʾ
}



















vs16 set_bit=7; //��ǰѡ���� set_max+1 ��ѡ�б�����
vs16 set_max=6; //���һ�е���ţ��������⣬��0~5����ѡ

u16 set_temp=0x0000; //��ǰ����������ᷢ�͸��ɿ�

//��ʾ�ɿ����á�ˢ�½��棬
void OLED_Show_Seting(void)
{
	u8 cnt=0,bit_cnt=0; //��ǰ��Ļy���꣬��ǰ����������
	u8 mode,page;
	static u8 page_temp;
	
	
	
	page = set_bit/4;  //��һҳ0~3���ڶ�ҳ4~7
	
	if(set_bit>set_max) page=0; //������
	
	bit_cnt = 4*page; //��ǰ����������
	
	//oled_CH0���ɿ����ã�һ������2���ַ����
	for(u8 i=0;i<8;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH0[i],12,1); //�ɿ�����
	
	
	if(set_bit==set_max+1) mode=0; else mode=1; //������ʾ��ѡ�����뵱ǰ��һ��
	for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH23[i],12,mode);//����
	
	
	
	if(page_temp!=page)
	{
		page_temp=page;
		OLED_Clear();
	}
	
	switch(page)
	{
		
		//��һҳ
		case 0:
			cnt+=13;//��ǰ��Ļy����
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH1[i],12,1); //���ⶨ�ߣ�
			if(set_bit==bit_cnt) mode=0;	else mode=1;  //0������ʾ��1������ʾ
			if( set_temp&BIT0 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode); //" �� "
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode); //" �� "
			bit_cnt++;//��ǰ����������+1

			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH2[i],12,1); //�׷�ģʽ��
			if(set_bit==bit_cnt) mode=0;	else mode=1;
			if( set_temp&BIT1 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;

			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH3[i],12,1);//��ͷģʽ��
			if(set_bit==bit_cnt) mode=0; else mode=1;
			if( set_temp&BIT2 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;

			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH4[i],12,1);//�л���ͷ��
			if(set_bit==bit_cnt)	mode=0; else mode=1;
			if( set_temp&BIT3 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;
		break;
		
		
		
		//��2ҳ
		case 1:
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH14[i],12,1);//�Ӿ���λ��
			if(set_bit==bit_cnt)	mode=0; else mode=1;
			if( set_temp&BIT4 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;
		
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH18[i],12,1);//�������㣺
			if(set_bit==bit_cnt)	mode=0; else mode=1;
			if( set_temp&BIT5 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;
		
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH29[i],12,1);//��ѹ���ߣ�
			if(set_bit==bit_cnt)	mode=0; else mode=1;
			if( set_temp&BIT6 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;
		
		
		break;
		
		default:break;
	}
	
	OLED_Refresh_Gram();//��ʼ��ʾ
}






vs16 test_bit=6; //��ǰѡ������� test_max +1 ��ѡ�б�����
vs16 test_max=5; //���һ�е���� ���������⹲6�п�ѡ��0~5����
u16  test_temp=0; //�ӷɿ����Ĵ�����״̬
//������״̬��ˢ�½���
void OLED_Show_Test(void)
{
	u8 cnt=0,bit_cnt=0;//��ǰ��Ļy���꣬��ǰ����������
	u8 mode,page;
	static u8 page_temp;
	
	test_temp = Show.test_flag;

	page = test_bit/4;
	
	if(test_bit>test_max) page=0; //������
	
	bit_cnt = 4*page;
	
	
	for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH5[i],12,1);//������״̬
	
	
	if(test_bit==test_max+1) mode=0; else mode=1; //������ʾ��ѡ�����뵱ǰ��һ��
	for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH23[i],12,mode);//����
	
	

	
	if(page_temp!=page)	
	{
		page_temp=page;
		OLED_Clear();
	}
	
	
	
	switch(page)
	{
		case 0:
			cnt+=13;
			for(u8 i=0;i<8;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH6[i],12,1);//MPU5060:
			if(test_bit==bit_cnt++) mode=0; else mode=1; //������ʾ��ѡ�����뵱ǰ��һ��
			if( test_temp&BIT0 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);//����
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);//����
			
			cnt+=13;
			for(u8 i=0;i<8;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH7[i],12,1);//��ѹ�ƣ�
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT1 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
			
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH8[i],12,1);//����ģ�飺
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT2 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
			
			cnt+=13;
			for(u8 i=0;i<12;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH9[i],12,1);//������ģ��
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT3 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
		break;
		
		case 1:
			cnt+=13;
			for(u8 i=0;i<14;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH15[i],12,1);//�Ӿ���λģ��
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT4 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
		
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH19[i],12,1);//����ģ�飺
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT6 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
		break;
		
		default:break;
	}

	OLED_Refresh_Gram();//��ʼ��ʾ
}








vs16 menu_bit=0; //��ǰѡ���е���ţ���һ�У�0���������У�menu_max+1��
vs16 menu_max=2; //���һ�е���ţ��������⹲3�п�ѡ��0~2����
//u16  menu_temp=0;

//���˵���
void OLED_Show_Menu(void)
{
	
	//menu_bit=3; //��ǰѡ���У�ѡ����ϸ���ݽ���
	
	u8 cnt=0,bit_cnt=0;//��ǰ��Ļy���꣬��ǰ����������
	u8 mode,page;
	static u8 page_temp;
	
	char str_tmp[20]; //��ʾ�ַ�
	
	page = menu_bit/4;
	if( menu_bit> menu_max) page=0; //������
	
	
	bit_cnt = 4*page;
	
	
	
	//test_temp = Show.test_flag;
	//��һ��
	for(u8 i=0;i<8;i++) OLED_Show_CH_String(24+i*6,cnt,oled_CH20[i],12,1);//�˵�ҳ�棺
	
	
	
	if(menu_bit==menu_max+1) mode=0; else mode=1; //������ʾ��ѡ�����뵱ǰ��һ��
	for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH24[i],12,mode);//�˳�
	
	

	
	if(page_temp!=page)	
	{
		page_temp=page;
		OLED_Clear();
	}
	
	switch(page)
	{
		case 0:
			
			//�ڶ���
			cnt+=13;
			for(u8 i=0;i<18;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH21[i],12,1);//��ϸ���ݣ�
		
			if(menu_bit==bit_cnt++) mode=0; else mode=1;

		
		
		
		
			//�����˵�����Ҫ�������ʱ������ϸ���ݽ�����棬��ʾ���棬���ǽ���
			if(ANO_Param.SaveFlag){//��������� ���Ȼ���Ư������ֵ
						for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH28[i],12,mode);//����
			}
			else{
						for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH25[i],12,mode);//����
			}
				

		
		
		
		
		
			//if( test_temp&BIT2 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			//else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
		
			//������
			cnt+=13;
			for(u8 i=0;i<18;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH0[i],12,1);//�ɿ����� - - -
			if(menu_bit==bit_cnt++) mode=0; else mode=1; //������ʾ��ѡ�����뵱ǰ��һ��
			//for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);//����  ��  δ��
			if( Show.Connect_Succeed )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH25[i],12,mode);//����
			else										for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH26[i],12,mode);//δ����
		
		
			//������
			cnt+=13;
			for(u8 i=0;i<18;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH5[i],12,1);//������״̬
			if(menu_bit==bit_cnt++) mode=0; else mode=1;
			//for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);//����  ��  δ��
			if( Show.Connect_Succeed )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH25[i],12,mode);
			else										for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH26[i],12,mode);
			

			
			
			//���һ��
			cnt+=13;
			
			for(u8 i=0;i<20;i++) OLED_Show_CH_String(8+i*6,cnt,oled_CH22[i],12,1);//��ʹ�÷����ѡ�������
			
			
			/*
			//һ��16���ַ��� 0x11 22 33 44 55
			//Show.debug2=-1;
			if(Show.debug2<0){
					sprintf(str_tmp, "%02x|-%02x|%02x|%02x|   ",Show.debug1,-Show.debug2,Show.debug3,Show.debug4);
			}
			else{
					sprintf(str_tmp, "%02x|+%02x|%02x|%02x|   ",Show.debug1,Show.debug2,Show.debug3,Show.debug4);
			}
			OLED_ShowString(0,cnt,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
			//Show.debug5=-1;
			if(Show.debug5<0){
					sprintf(str_tmp, "-%02x| ",-Show.debug5);
			}
			else{
					sprintf(str_tmp, "+%02x| ",Show.debug5);
			}
			OLED_ShowString(12*8,cnt,(u8 *)str_tmp,12,1);  //�߶�12�����8��ʵ��6���أ���Ӽ�϶2�����أ�
			*/
			
			
			
			//if(test_bit==bit_cnt++) mode=0; else mode=1;
			//if( test_temp&BIT3 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			//else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
			
		break;
		

		
		default:break;
	}

	OLED_Refresh_Gram();//��ʼ��ʾ
}





//ң�� ����� ���¼�⣺
void Gesture_Check0(u16 val,vs16 *set,vs16 max)  //ҡ��ҡ�������ƶ�ѡ�񶨸߶������
{
	static u8 cnt0,cnt1; //���ϣ�����
	
	
	//���ϲ���1��
	if(val>1800) 
	{
		cnt0++;
	}
	else//��������ʱ����1��
	{
		if(cnt0>2) (*set)--; //��������ƶ�һ��
		cnt0 = 0;
	}
	
	
	//���²���1��
	if(val<1200) 
	{
		cnt1++;
	}
	else//��������ʱ����1��
	{
		if(cnt1>2) (*set)++; //��������ƶ�һ��
		cnt1 = 0;
	}
	
	if((*set)<0) 				(*set) = max+1;//0��ʾ��1�У�max�����һ�У�max+1�Ǳ���
	else if((*set)>max+1) (*set) = 0; //
}








//ҡ�˷�������Ҽ��
//��������ֵ��ע�����������ֵ�������ܱ��棬���Է���ȥ��ֵ����䡣
void Gesture_Check1(u16 val,u16 *set,vs16 bit)
{
	static u8 cnt0;
	
	if(val>1800 || val<1200) 
	{
		cnt0++;
	}
	else
	{
		if(cnt0>2) 
			*set = REVERSE(*set,bit);
		cnt0 = 0;
	}
}


//Aux1_Check(Rc.PIT,&menu_bit,Show.windows); //�̰�

//�����Ҳఴ��  key1 (=PB7) ������ 
//10msһ�Σ���key_function()һ��
void Key1_Check(u16 val,vs16 *set,vs16 max, u8 index)  //ҡ��ҡ�������ƶ�ѡ�񶨸߶������
{
	static u8 cnt0; //���ϣ�����
	
	
	//����
	if(val && !(Rc.AUX40 & 0x80) ) //key1�������£�set����û�а���ʱ��set+key1��ʾУ׼ҡ����ֵ
	{
		cnt0++;
	}
	else//
	{
		//�����ɿ����㣬��key����Ҳ������һ�����ʱ�䣬���︺�����������
		if(cnt0>=4 && cnt0<50){ //ҪС��key.c����key1������ʱ��
			
			sprintf(Show.str_oled, ""); //�ָ�������
			
			//������0��ʱ���ڰ���key.c�Ǳ�
			//ע�⣬�������Ǳ�Ҳ���Ⲣ��Ӧ aux1 �¼���������������˵���
			switch(index)
			{
				
					case 0://��ǰ����Ϊ������
									//��������ʱ��ʹ��key.cĬ�ϰ�������Ӧ����Ҫ���ֳ�����̰�
					break;
				
				
					case 2://�ɿ����ý���
						if(*set==max+1) {
								Show.windows = 4; //�����˵�
								menu_bit=menu_max+1;//���˵�Ĭ�Ϸ���ѡ�񣬻�������
							  
						}
					
					break;
					
					case 3://����״̬����
						if(*set==max+1) {
								Show.windows = 4; //�����˵�
								menu_bit=menu_max+1;//���˵�Ĭ�Ϸ���ѡ�񣬻�������
						}
					
					break;		
					
					case 1:  //������ϸ���� ���棬����ֱ�ӻص������棬���������˵�

						if(detail_bit==0){
							
									
										//�˳���ϸ���ݽ���ʱ����Ҫ�������
										if(ANO_Param.SaveFlag){//��������� ���Ȼ���Ư������ֵ
													ANO_Param_SAVE();         //Flash�洢����   ����������лָ�Ĭ��ֵ���Լ�ҡ��У׼����Ƶʱ���ᱣ�������flash
													sprintf(Show.str_oled, "SaveParam"); //�ָ�������
													ANO_Param.SaveFlag=0;
										}
										else{
													//Ҫ�ȱ�����������ܽ�����һ������
													detail_bit=1; //����ڶ�ҳ
											
										}

							
						
						}
						
						//��ǰΪ�ڶ�ҳ��ֱ���˳���������
						else{
							
										detail_bit=0;
										Show.windows = 0;
							
							
						}
					

					
					
					break;		
					
					case 4: //���˵�

					
							Show.windows =*set+1; //0��ʾ�ص������棬1��ʾ�ɿ����ý��棬2��ʾ������״̬���棬3��ʾ�����������
							
							//������ϸ���ݽ���ʱ����Ҫ�������
							if(Show.windows==1 && ANO_Param.SaveFlag){
									ANO_Param_SAVE();         //Flash�洢����   ����������лָ�Ĭ��ֵ���Լ�ҡ��У׼����Ƶʱ���ᱣ�������flash
									sprintf(Show.str_oled, "SaveParam"); //�ָ�������
									ANO_Param.SaveFlag=0;
							}
					
					
							//�ص�������
							if(*set==max+1) {
									Show.windows =0;//��������
									menu_bit=0;//�˳����´����½������˵�������Ĭ��ѡ����ϸ����
							}
							else if(Show.Connect_Succeed==0 && ( *set==1 || *set ==2 )){ //���ѡ����ʱ1��2����һ����0��������3��
									Show.windows =4;//��Ȼͣ�������˵������������ �˵�2��3
							}
					
					break;	
			
			}
					
		
			
			
		
		
		}
		
		cnt0 = 0;

	}
	

}











u8 send_flag=0;



//10msһ�Σ��ڼƻ������е��ã�
//������� Show.set_flag ���ݸ� set_temp��
//Ȼ��ͨ�����ñ�ʶλsend_flag��������ɿ�
void Gesture_Check(void)
{
	static u8 temp;
	
	//����ϴ��Ѳ�����������ʾ���治һ��
	if(temp!=Show.windows)
	{
		//�����ǰ��ʾ����Ϊ�ɿ����ý��棬��һ�ν���ý��棬��ȡ���� ��set_temp
		if(Show.windows==2) set_temp = Show.set_flag;
		//Ŀǰ�����ڲ������ý�����ķɿ����õĹ���
		//��ң���ϣ�ֻ�ܸ��Ķ���ģʽ��ͨ��key2�����ģ����ý���ֻ��������ʾ��
		
		//����ϴβ�������Ϊ�ɿ����ý��棬˵����ʾ�����������������棬���ϴ����ò���set_temp
		//���ɿ��Ǳߣ���δ����������ݡ�Ŀǰ��Ȼͨ��ͨ��������ң������
		if(temp==2 && set_temp!=Show.set_flag) send_flag = 1;
		
		//���µ�ǰ��������
		temp=Show.windows;
	}
	
	
	switch(Show.windows)
	{
		//�ɿ����ý���
		//set_bit��ʾ��ǰѡ����
		//set_max��ʾ��Ļ���ж�����
		case 2:
			Gesture_Check0(Rc.PIT,&set_bit,set_max); //����
		
			Key1_Check(Rc.AUX40&0x01, &set_bit,set_max, Show.windows); //�̰�
		
			if(Show.Connect_Succeed)
			{
			  //���Ĳ������������ң����ĵ�ֵ������set_temp�仯���뿪���ý���ʱ������ send_flag = 1�������ϴ�
				Gesture_Check1(Rc.ROL,&set_temp,set_bit); //����
			}
		break;
		
			
		//����״̬���棬��Ӧ����ѡ���
		//test_bit��ʾ��ǰѡ����
		//test_max��ʾ��Ļ���ж�����
		case 3:
			Gesture_Check0(Rc.PIT,&test_bit,test_max); //����
		
			Key1_Check(Rc.AUX40&0x01, &test_bit, test_max, Show.windows); //�̰�
		
		break;
		
		
		//��ϸ���ݽ��棬��Ӧ�Ҽ��̰�
		case 1:
			//Rc.AUX40&0x01   //�ұ߰���B7������
			Key1_Check(Rc.AUX40&0x01, 0, 0, Show.windows); //�̰�
		
		break;

		//���˵�����
		//menu_bit��ʾ��ǰѡ����
		//menu_max��ʾ��Ļ���ж�����
		case 4:
			Gesture_Check0(Rc.PIT,&menu_bit,menu_max); //����

			//Rc.AUX40&0x01   //�ұ߰���B7������
			Key1_Check(Rc.AUX40&0x01,&menu_bit,menu_max,Show.windows); //�̰�
		
		break;
		
		
		
		
		default:break;
	}
	
	
	
}





u32 time_tmp=0;
u16 time_ms=0;
u16 time_ms1=0;

//50msִ��һ��
void Show_Duty(void)
{
	static u8 cnt=0,mode=0;
	static u8 temp=0; //��ǰ��ʾ����
	
	cnt++;
	
	//��ǰ�����Ŀ����治ͬ
	if(Show.windows!=temp)
	{
		temp = Show.windows;
		OLED_Clear();//����
	}
	
	//���·���ʱСԲ�㣬1��仯һ��
	if(cnt>=20){
				cnt=0;
				mode=!mode;
				OLED_DrawPoint(0,63,mode);
		

	}

	
	//�鿴����������
	time_ms = (SysTick_count_2ms - time_tmp)*2;// 50ms
	time_tmp = SysTick_count_2ms;

	
	//ˢ��
	switch(Show.windows)
	{ //0�����棬1�ɿ����ã�2������״̬
		case 0:	oled_show();	break; //������
		
		case 2:	OLED_Show_Seting();	break;  //�ɿ�����
		case 3:	OLED_Show_Test();	break; //������״̬
		
		case 1:	OLED_Show_detail();	break; //��Ӹ������ҳ��
		case 4:	OLED_Show_Menu();	break; //������˵�ҳ��
		
		
		default:break;
	}
	
	
	
	
	
	//�鿴��Ļˢ�º�ʱ
	time_ms1 = (SysTick_count_2ms - time_tmp)*2;//��ʱ46~49ms�����ݸ�Ϊ����ģʽ�����ʱ��Ϊ24ms
	//time_tmp = SysTick_count_2ms;

	// i2c�ٶ�Ҫ��spi���ܶࡣi2cֻҪ2���ߣ�spi��Ҫ4~6���ߣ�rst,dc,sck,mosi,cs,miso��,
	// �������Ҫ��spi��d/c�����Ŀ����ߣ���i2cÿ�����ݶ�Ҫ���Ͷ����d/cָ��
	// ����û��cs�����ߣ�����ÿ�����ݶ���Ҫ���������ӵ�ַ�����µ����ֽ�ʱ��Ϊspi��3��
		
	// ģ��i2c��Ļ������һ�������棬��Ҫ46ms��
	// ��Ļÿ50msˢ��һ�ε��ٶ�̫���ˡ�
	// �Ѿ���������ڸ�Ϊ��������ģʽ��ȫ��Ļ����һ���ܺ�ʱ�Ѿ�����Ϊ24ms

	
}

