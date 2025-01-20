#include "show.h"
#include "oledfont.h" 

//#include "oled.h" //spi，二选一，共用端口  在sysconfig.h 也要二选一
#include "oled_i2c.h" //i2c

#include "stdio.h"  //	sprintf()
#include <math.h>
#include <string.h> //比较字符串

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

#define Y0 0   		//标题
#define Y1 14			//1
#define Y2 Y1+13	//2
#define Y3 Y2+13	//3
#define Y4 Y3+13	//4
#define Y5 Y4+12  //没有

struct _Show Show={.position_old=1};

//Show.unLock=1;

unsigned char i;          //计数变量
unsigned char Send_Count; //串口需要发送的数据个数
float Vol;

/**************************************************************************
函数功能：OLED显示
入口参数：无
返回  值：无
**************************************************************************/
//主界面刷新
void oled_show(void)
{
	
	char str_tmp[20]; //显示字符
	
	//u16 temp;
	int16_t temp;
	
	int temp1;
	
	static u8 page,page_temp,flash_cnt,show_mode=1;
	
	if(page != page_temp)//切换页面先清屏
	{
		page_temp = page;
		OLED_Clear();
	}
	
	
	///////////////////////////////第一行///////////////////////////////////

	if(Show.low_power)//遥控低电量
	{
		flash_cnt ++;
		if(flash_cnt>3) 
		{
			flash_cnt=0;
			if(show_mode==0)show_mode=1;
			else show_mode = 0;
		}
		
		
		for(u8 i=0;i<12;i++) OLED_Show_CH_String(Line1_Begin+i*6,Y0,oled_CH16[i],12,show_mode);//遥控电压低
		
	}
	
	
	else if(Show.test_flag&BIT5)//飞机低电量
	{
		flash_cnt ++;
		if(flash_cnt>3) 
		{
			flash_cnt=0;
			if(show_mode==0)show_mode=1;
			else show_mode = 0;
		}
		
		for(u8 i=0;i<12;i++) OLED_Show_CH_String(Line1_Begin+i*6,Y0,oled_CH17[i],12,show_mode);//飞机电压低
	}
	
	
	else  //显示第一行字符串
	{
		OLED_Show_CH(Line1_Begin+00,Y0,0,12,1);
		OLED_Show_CH(Line1_Begin+12,Y0,1,12,1);
		OLED_Show_CH(Line1_Begin+24,Y0,2,12,1);
		OLED_Show_CH(Line1_Begin+36,Y0,3,12,1);
		OLED_Show_CH(Line1_Begin+48,Y0,4,12,1);
		OLED_Show_CH(Line1_Begin+60,Y0,5,12,1);  //高度12，宽度12
	}
	
	
	
	
	//sprintf(Show.str_oled, "S=%+3i    ", 12);
	//显示按键操作记录
	
	static char str_oled_old[20]; //第一行显示字符
	static uint32_t time_str_oled =0; //上次更新时间
	
	
	//按键操作有更新
	if(memcmp( str_oled_old, Show.str_oled, 9)!=0){
			time_str_oled = SysTick_count; //上次更新时间
			memcpy(str_oled_old, Show.str_oled, 20);
	};
	
	//显示按键操作
	if(SysTick_count-time_str_oled <300){ //只显示3秒，防止长时间挡住标题以及低电压等消息
				OLED_ShowString(Line1_Begin+00,Y0,(u8 *)Show.str_oled,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
	}
	else{
				//恢复标题时，清空按键提示字符串，方便再次按键动作时重新显示
				sprintf(Show.str_oled, "");
				sprintf(str_oled_old, "");
	
	}



	
	
	//////////////////////////////////////////////////////
	if(Show.NRF_Err)	OLED_Show_CH(2,Y0,6 ,12,1);//无线模块故障显示X
	else				OLED_ShowNumber(2,Y0,ANO_Param.NRF_Channel,3,12);//显示无线信道
	
	//显示信号强度
	temp = Show.Rc_num/20;
	switch(temp)
	{
		case 0:OLED_Show_CH(Line1_Begin+85,Y0,6 ,12,1);break;  //6为叉叉
		case 1:OLED_Show_CH(Line1_Begin+85,Y0,7 ,12,1);break;  //1竖
		case 2:OLED_Show_CH(Line1_Begin+85,Y0,8 ,12,1);break;//2竖
		case 3:OLED_Show_CH(Line1_Begin+85,Y0,9 ,12,1);break;//3竖
		case 4:OLED_Show_CH(Line1_Begin+85,Y0,10,12,1);break;//4竖
		default:OLED_Show_CH(Line1_Begin+85,Y0,11,12,1);break;//5竖
	}
	
	
	
	
	///////////////////////////////第二行///////////////////////////////////
	//显示电压	
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
	
	
	
	
	///////////////////////////////第三、四行/////////////////////////////////
	//显示遥控数据
	OLED_ShowString(Line3_Begin+00,Y2,"THR:",12,1);
	
	if(AUX82_isH()) OLED_DrawLine(Line3_Begin+00,Y2+6,Line3_Begin+3*8,Y2+6,1); //画线
	
	//模拟飞控解析值Stick_dis.THR
	//单纯摇杆值  Rc.THR
	temp = (Stick_dis.THR-1000)/41;//1500时，进度条的第一个字符应该是满格12像素 500/41=12
	OLED_Show_progress_bar(temp,12,24,Line4_Begin+6 ,Y2,12,1);
	temp = (Stick_dis.THR-1500)/41;//进度条第二个字符，默认为空(12)
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
	
	

	
	///////////////////////////////第五行///////////////////////////////////
	if(Show.Rc_num && Show.hardware_type == 1) //已连接飞控，有信号
	{
		page = 1;//第一页
		//显示姿态角度和高度
		
		
		//显示高度正负号
		/*
		if(Show.H<0) 			temp1 =-Show.H,  OLED_ShowString(Z_Begin,Y4,"-",12,1);
		else    			temp1 = Show.H,  OLED_ShowString(Z_Begin,Y4,"+",12,1);
		*/
		
		//最高显示999厘米,或者999米。
		temp1 = Show.H;
		if(temp1>99900) temp1 = 99900, OLED_ShowString(Z_Begin,Y4,">",12,1); //>999米时
		
		if(temp1<1000){ //大于10米时，单位切换为米
				sprintf(str_tmp, "%+dcm",temp1);//显示高度，单位厘米cm
		}
		else{
				sprintf(str_tmp, "%+dM",temp1/100);//显示高度，单位为米M
		}

		OLED_ShowString_num(Z_Begin,Y4,(u8 *)"      ", 12,1);
		OLED_ShowString_num(Z_Begin,Y4,(u8 *)str_tmp,12,1);//显示高度
		

		
		
		
		
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
	
	
	else //当未连上飞控
	{
		page = 2;//第二页
		
		//显示微调旋钮数据,,微调范围：
		OLED_ShowString(Line5_Begin+00,Y4,"Y:",12,1);
		
		//temp = (ANO_Param.OffSet_Yaw-1000)/41; //默认为填满
		temp = ((int8_t)ANO_Param.OffSet_Yaw+128)/10; //默认为填满
		OLED_Show_progress_bar(temp,12,24,Line5_Begin+15 ,Y4,12,1);
		
		//temp = (ANO_Param.OffSet_Yaw-1500)/41; //默认为空
		temp = (ANO_Param.OffSet_Yaw-0)/10; //默认为空
		OLED_Show_progress_bar(temp,12,12,Line5_Begin+26,Y4,12,1);

		
		
		//显示微调旋钮数据,,微调范围：
		OLED_ShowString(Line5_Begin+44,Y4,"P:",12,1);
		
		temp = ((int8_t)ANO_Param.OffSet_Pit+128)/10; //+/-128 
		OLED_Show_progress_bar(temp,12,24,Line5_Begin+59 ,Y4,12,1);
		
		temp = (ANO_Param.OffSet_Pit-0)/10;
		OLED_Show_progress_bar(temp,12,12,Line5_Begin+70,Y4,12,1);
		
		
		
		
		
		//显示微调旋钮数据,,微调范围：
		OLED_ShowString(Line5_Begin+88,Y4,"R:",12,1);
		
		//进度条24个像素，分两次显示，漂移修正值+/-128
		temp = ((int8_t)ANO_Param.OffSet_Rol+128)/10; //修正值+/-128 ,,0时，进度条的第一个字符应该是满格12像素 128/10=12
		OLED_Show_progress_bar(temp,12,24,Line5_Begin+103 ,Y4,12,1);
		
		temp = (ANO_Param.OffSet_Rol-0)/10;
		OLED_Show_progress_bar(temp,12,12,Line5_Begin+114,Y4,12,1);
		
		


	}
	
	OLED_Refresh_Gram();//开始显示
	
	
}



//进度条显示函数，
void OLED_Show_progress_bar(u8 temp,u8 chr_star,u8 chr_default,u8 x,u8 y,u8 size,u8 mode)
{
	switch(temp)
	{
		//这个是单个汉字字符宽度的进度条，12像素宽，字库从12到24，12为孔，24为填满
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





vs16 detail_bit=0; //第一页
vs16 detail_max=1; //第二页
//更多参数界面：
void OLED_Show_detail(void)
{
	u8 cnt=0;//当前屏幕y坐标
	char str_tmp[20]; //显示字符
	
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
		
		//第一页，主要显示遥控器相关的数据
		case 0:

						///////////////第一行////////////////////////////
						for(u8 i=0;i<8;i++) OLED_Show_CH_String(34+i*6,cnt,oled_CH21[i],12,1);//详细数据：


						//退出详细界面时，需要保存参数
						if(ANO_Param.SaveFlag){//如果更改了 力度或者漂移修正值
									for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH28[i],12,0);//保存
						}
						else{
									for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH27[i],12,0);//下页
						}


						
						///////////////第一行////////////////////////////
						//显示按键操作
						OLED_ShowString(10,Y0,(u8 *)Show.str_oled,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						
						
						//是否无头模式,
						if(AUX83_isH()){
								OLED_ShowString(00,Y0,(u8 *)"H",12,0);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						} 
						else{// 默认为0，无头模式
								OLED_ShowString(00,Y0,(u8 *)"L",12,0);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						}


						
						///////////////第二行////////////////////////////
						//sprintf(str_tmp, "THR%1i, YAW%3i",Rc.THR,Rc.YAW);
						sprintf(str_tmp, "T%1i,Y%3i,%+3i",Stick_dis.THR,Stick_dis.YAW,-AUX73());
						OLED_ShowString(10,Y1,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						
						//是否屏蔽油门
						if(AUX82_isH()) OLED_DrawLine(10,Y1+6,6*8,Y1+6,1); //画线
						
						//是否悬停模式
						if(Rc.AUX2==2000){
								OLED_ShowString(00,Y1,(u8 *)"O",12,0);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						} 
						else{//默认为低，低为悬停模式
								OLED_ShowString(00,Y1,(u8 *)"P",12,0);  //高度12，宽度8（实际6像素，会加间隙2个像素）
							
								if(!AUX833_isH()) OLED_DrawLine(4,Y1+8, 8,Y1+8,0); //画线
								if(!AUX833_isH()) OLED_DrawLine(6,Y1+6, 6,Y1+11,0); //画线

						}

						
								//角度，逆时针为正，以下为加速度方向，与xyz坐标方向不一致
								//          x+	
								//         /
								//  y+ ___/___ -
								//       /|
								//      / |
								//     -	|
								//         z+  
						
						

						
						///////////////第三行////////////////////////////
						
						//sprintf(str_tmp, ",P%3i",Rc.PIT);
						sprintf(str_tmp, "P%04i,",Stick_dis.PIT);
						OLED_ShowString(2,Y2,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						
						//sprintf(str_tmp, ",R%3i",Rc.ROL);
						sprintf(str_tmp, "R%04i,",Stick_dis.ROL);
						OLED_ShowString(50,Y2,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						
						sprintf(str_tmp, "Y%+03i", (int16_t)ANO_Param.OffSet_Yaw);
						OLED_ShowString(98,Y2,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						

						

						
						///////////////第四行////////////////////////////

						sprintf(str_tmp, "x%+03i%+03i,", (int16_t)ANO_Param.OffSet_Pit,(int16_t)ANO_Param.Z_OffSet_Pit);
						OLED_ShowString(2,Y3,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						
						
						sprintf(str_tmp, "y%+03i%+03i", (int16_t)ANO_Param.OffSet_Rol,(int16_t)ANO_Param.Z_OffSet_Rol);
						OLED_ShowString(74,Y3,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						

						
						

						
						///////////////第五行////////////////////////////
						sprintf(str_tmp, "R:%02i|F:%1i|x%02x|x%02x",Show.ratio+1,  8-Show.force,  (uint16_t)Rc.AUX7>>8,  Rc.AUX8); //aux7显示高字节，aux8只用了低字节
																												//油门比例,          力度，          目标偏航角，   aux8
						OLED_ShowString(02,Y4,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）

						

		
			
		break;
		
		//第二页，主要显示飞控相关数据
		case 1:
			
						//OLED_Clear();
		
		
						///////////////第一行////////////////////////////
						//右上角，退出按钮
						for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,Y0,oled_CH24[i],12,0);//退出

						
						
						//是否无头模式,
						if(AUX83_isH()){
								OLED_ShowString(00,Y0,(u8 *)"H",12,0);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						} 
						else{// 默认为0，无头模式
								OLED_ShowString(00,Y0,(u8 *)"L",12,0);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						}
						
						

						if(Show.test_flag&BIT5)//飞机低电量
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
						
						//按键操作
						sprintf(str_tmp, "key input|");
						OLED_ShowString_num(11,Y0,(u8 *)str_tmp,12,1); 
						
						//显示电量
						sprintf(str_tmp, "%.2fv",Show.Battery_Fly/100.0f);
						OLED_ShowString_num(11+6*10,Y0,(u8 *)str_tmp,12,show_mode); 
						
						
						
						
						
						//显示按键操作
						OLED_ShowString_num(11,Y0,(u8 *)Show.str_oled,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						
		
						
						
						
						
						
						///////////////第二行////////////////////////////
						//是否悬停模式
						if(Rc.AUX2==2000){
								OLED_ShowString(00,Y1,(u8 *)"O",12,0);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						} 
						else{//默认为低，低为悬停模式
								OLED_ShowString(00,Y1,(u8 *)"P",12,0);  //高度12，宽度8（实际6像素，会加间隙2个像素）
							
								if(!AUX833_isH()) OLED_DrawLine(4,Y1+8, 8,Y1+8,0); //画线
								if(!AUX833_isH()) OLED_DrawLine(6,Y1+6, 6,Y1+11,0); //画线

						}


						 
						OLED_ShowString(11,Y1,(u8 *)"                ",12,1); 
						
						//光流外环位移pid期望(位移)
						sprintf(str_tmp, "%+i,%+i",-Show.x_desired,-Show.y_desired);
						
						OLED_ShowString_num(11,Y1,(u8 *)str_tmp,12,1); 
						

						
						//光流当前位置
						sprintf(str_tmp, "|%+i,%+i", -Show.loc_x ,-Show.loc_y);//当前高度，单位cm，大于4m用气压计
						OLED_ShowString_num(65,Y1,(u8 *)str_tmp,12,1); 


						//目标偏航角 /45度
						//sprintf(str_tmp, "%+i",AUX73());//油门构成
						//OLED_ShowString_num(116,Y1,(u8 *)str_tmp,12,1); 
						 
						//光流质量, 0x20以下代表光流质量低
						sprintf(str_tmp, "%02x",Show.qual);//油门构成
						OLED_ShowString_num(116,Y1,(u8 *)str_tmp,12,1); 
						
						
						
						
						
						
						///////////////第三行////////////////////////////
						//是否已解锁
						if(Show.unLock){
								OLED_ShowString(00,Y2,(u8 *)"A",12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						} 
						else{//默认为低，低为悬停模式
								OLED_ShowString(00,Y2,(u8 *)"N",12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						}
						
						OLED_ShowString(11,Y2,(u8 *)"                ",12,1); 
						
						//期望姿态角，来自方向摇杆，或者光流内环输出
						sprintf(str_tmp, "%+.1f,%+.1f",Show.pitch_desired/10.0f,Show.roll_desired/10.0f);//姿态角*10度
						OLED_ShowString_num(11,Y2,(u8 *)str_tmp,12,1); 
						
						//当前姿态角
						sprintf(str_tmp, "|%+.1f,%+.1f",Show.Y/10.0f,Show.X/10.0f);//姿态角*10度, 这里的Y是值pitch，并非Y坐标
						OLED_ShowString_num(65,Y2,(u8 *)str_tmp,12,1); 
						
						
						
						
						
						
						
		
						///////////////第四行////////////////////////////
						sprintf(str_tmp, "%i", Show.debug1);//定高详细模式
						OLED_ShowString(0,Y3,(u8 *)str_tmp,12,0); 
						

						OLED_ShowString(11,Y3,(u8 *)"                ",12,1); 
		
					
						//期望高度，当前高度
						sprintf(str_tmp, "%+i,%+i", Show.high_desired, Show.Control_high);//当前高度，单位cm，大于4m用气压计
						OLED_ShowString_num(11,Y3,(u8 *)str_tmp,12,1); 

						
						
						//光流内环速度pid输出(期望姿态角相关)
						sprintf(str_tmp, "|%+i,%+i",Show.x_out,Show.y_out);
						
						OLED_ShowString_num(65,Y3,(u8 *)str_tmp,12,1); 
						






		
						///////////////第五行////////////////////////////
						/*
						Aux_Rc.debug1 = hight_mode;//详细定高模式，0为姿态
						Aux_Rc.debug2 = Remote.thr -1000;// 基础油门，摇杆值比例修正后。只有这个是正数，其他都是有正有负
						Aux_Rc.debug3 = thr_base_add;//一键定高起飞的基础油们补偿
						Aux_Rc.debug4 = thr_adjust;//			普通定高模式下，自动调整 平衡油门，平衡油门指用于抵消飞机重力的油门
						Aux_Rc.debug5 = thr_hight_modify;// 临时油门增减，用于高度上升或下降过程。
						*/


						
						//是否屏蔽油门
						if(!AUX82_isH()){
									OLED_DrawLine(0,Y4+6,6*9,Y4+6,0); //画线
						}

									
		
						//基础油门，基础油门补偿
						sprintf(str_tmp, "T=");//油门构成
						OLED_ShowString_num(2,Y4,(u8 *)str_tmp,12,1); 
						
						
						OLED_ShowString(17,Y4,(u8 *)"                ",12,1); 
						
						sprintf(str_tmp, "%i %+i",Show.debug2,Show.debug3);//油门构成   Remote.thr-1000, thr_base_add
						OLED_ShowString_num(17,Y4,(u8 *)str_tmp,12,1); 
						
						//平衡油门，临时油门增减
						sprintf(str_tmp, "%+i %+i",Show.debug4,Show.debug5);//油门构成  thr_adjust, thr_hight_modify
						OLED_ShowString_num(63,Y4,(u8 *)str_tmp,12,1); 
						
						
						//是否屏蔽油门
						if(AUX82_isH()){
									OLED_DrawLine(0,Y4+6,6*9,Y4+6,1); //画线
						}
		
						
						/*
						//偏航角小数位，用来查看是否会偏航角漂移
						if(Show.Z>0){
									OLED_ShowString_num(116,48,(u8 *)"-",12,1);  //飞控发过来时，把yaw反向了，我们把它恢复成逆时针为正。
						}
						else if(Show.Z<=0){
									OLED_ShowString_num(116,48,(u8 *)"+",12,1); 
						}
						
						OLED_ShowNumber(121,Y3,(Show.Z%100)/10,1,12); //倒数第二位，，，Show.Z=yaw*10
						OLED_ShowNumber(121,Y4,Show.Z%10,1,12); //最后一位，小数位，，，，Show.Z=yaw*10
						OLED_DrawPoint(118,62,1);// 画点
						*/
						
						
						
						//偏航角速度小数位，用来查看是否会偏航角漂移
						//静止时yaw_Gs不为零，请做mpu6050校准；飞行中方向摇杆回中时yaw_Gs过大，请做yaw零漂修正
						//Show.yaw_Gs=0.01; 0.06度/秒=1(mpu6050原始数据)
						if(Show.yaw_Gs>=0.1){
									OLED_ShowString_num(116,48+1,(u8 *)"+",12,1);  //yaw_s 角速度，逆时针为正。
						}
						else if(Show.yaw_Gs<=-0.1){
									OLED_ShowString_num(116,48+1,(u8 *)"-",12,1); 
						}
						else{
									OLED_ShowString_num(116,48+1,(u8 *)" ",12,1); 
						}

						if(Show.yaw_Gs>=10.0f || Show.yaw_Gs<=-10.0f) Show.yaw_Gs=9.9;//大于10时显示9.9
						OLED_ShowNumber(121,Y3,(int)(Show.yaw_Gs*1)%10,1,12); //整数最后1位，，，
						OLED_ShowNumber(121,Y4,(int)(Show.yaw_Gs*10)%10,1,12); //小数第1位，，，，，
						
						OLED_DrawPoint(118,62-1,1);// 画点
						
						
						
						//pixel_flow.err1_cnt;//pixel_flow.err2_cnt
						//显示光流复位次数
						//OLED_ShowNumber(121,Y3,(Show.debug6>>8)%10,1,12); //pixel_flow.err1_cnt
						//OLED_ShowNumber(121,Y4,(Show.debug6)%10,1,12); //pixel_flow.err2_cnt
						
						
						/*
						
						//一行16个字符， 0x11 22 33 44 55
						//Show.debug2=-1;
						if(Show.debug2<0){
								sprintf(str_tmp, "%02x|-%02x|%02x|%02x|   ",Show.debug1,-Show.debug2,Show.debug3,Show.debug4);
						}
						else{
								sprintf(str_tmp, "%02x|+%02x|%02x|%02x|   ",Show.debug1,Show.debug2,Show.debug3,Show.debug4);
						}
						OLED_ShowString(0,Y4,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						
						
						//Show.debug5=-1;
						if(Show.debug5<0){
								sprintf(str_tmp, "-%02x| ",-Show.debug5);
						}
						else{
								sprintf(str_tmp, "+%02x| ",Show.debug5);
						}
						OLED_ShowString(12*8,Y4,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
						
						*/
						
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		
			
		break;
		
		
		
		default:break;
		
		
	}
	
	
	

	

	OLED_Refresh_Gram();//开始显示
}



















vs16 set_bit=7; //当前选中行 set_max+1 ，选中标题栏
vs16 set_max=6; //最后一行的序号，不含标题，（0~5）可选

u16 set_temp=0x0000; //当前参数，这个会发送给飞控

//显示飞控设置。刷新界面，
void OLED_Show_Seting(void)
{
	u8 cnt=0,bit_cnt=0; //当前屏幕y坐标，当前参数的行数
	u8 mode,page;
	static u8 page_temp;
	
	
	
	page = set_bit/4;  //第一页0~3，第二页4~7
	
	if(set_bit>set_max) page=0; //标题行
	
	bit_cnt = 4*page; //当前参数的行数
	
	//oled_CH0：飞控设置，一个汉字2个字符宽度
	for(u8 i=0;i<8;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH0[i],12,1); //飞控设置
	
	
	if(set_bit==set_max+1) mode=0; else mode=1; //反白显示，选中行与当前行一致
	for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH23[i],12,mode);//返回
	
	
	
	if(page_temp!=page)
	{
		page_temp=page;
		OLED_Clear();
	}
	
	switch(page)
	{
		
		//第一页
		case 0:
			cnt+=13;//当前屏幕y坐标
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH1[i],12,1); //激光定高：
			if(set_bit==bit_cnt) mode=0;	else mode=1;  //0反白显示，1正常显示
			if( set_temp&BIT0 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode); //" 是 "
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode); //" 否 "
			bit_cnt++;//当前参数的行数+1

			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH2[i],12,1); //抛飞模式：
			if(set_bit==bit_cnt) mode=0;	else mode=1;
			if( set_temp&BIT1 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;

			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH3[i],12,1);//无头模式：
			if(set_bit==bit_cnt) mode=0; else mode=1;
			if( set_temp&BIT2 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;

			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH4[i],12,1);//切换机头：
			if(set_bit==bit_cnt)	mode=0; else mode=1;
			if( set_temp&BIT3 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;
		break;
		
		
		
		//第2页
		case 1:
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH14[i],12,1);//视觉定位：
			if(set_bit==bit_cnt)	mode=0; else mode=1;
			if( set_temp&BIT4 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;
		
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH18[i],12,1);//光流定点：
			if(set_bit==bit_cnt)	mode=0; else mode=1;
			if( set_temp&BIT5 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;
		
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH29[i],12,1);//气压定高：
			if(set_bit==bit_cnt)	mode=0; else mode=1;
			if( set_temp&BIT6 )	for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH12[i],12,mode);
			else								for(u8 i=0;i<6;i++) OLED_Show_CH_String(94+i*6,cnt,oled_CH13[i],12,mode);
			bit_cnt++;
		
		
		break;
		
		default:break;
	}
	
	OLED_Refresh_Gram();//开始显示
}






vs16 test_bit=6; //当前选中行序号 test_max +1 ，选中标题栏
vs16 test_max=5; //最后一行的序号 ，不含标题共6行可选（0~5），
u16  test_temp=0; //从飞控来的传感器状态
//传感器状态：刷新界面
void OLED_Show_Test(void)
{
	u8 cnt=0,bit_cnt=0;//当前屏幕y坐标，当前参数的行数
	u8 mode,page;
	static u8 page_temp;
	
	test_temp = Show.test_flag;

	page = test_bit/4;
	
	if(test_bit>test_max) page=0; //标题行
	
	bit_cnt = 4*page;
	
	
	for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH5[i],12,1);//传感器状态
	
	
	if(test_bit==test_max+1) mode=0; else mode=1; //反白显示，选中行与当前行一致
	for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH23[i],12,mode);//返回
	
	

	
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
			if(test_bit==bit_cnt++) mode=0; else mode=1; //反白显示，选中行与当前行一致
			if( test_temp&BIT0 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);//正常
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);//错误
			
			cnt+=13;
			for(u8 i=0;i<8;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH7[i],12,1);//气压计：
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT1 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
			
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH8[i],12,1);//无线模块：
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT2 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
			
			cnt+=13;
			for(u8 i=0;i<12;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH9[i],12,1);//超声波模块
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT3 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
		break;
		
		case 1:
			cnt+=13;
			for(u8 i=0;i<14;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH15[i],12,1);//视觉定位模块
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT4 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
		
			cnt+=13;
			for(u8 i=0;i<10;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH19[i],12,1);//光流模块：
			if(test_bit==bit_cnt++) mode=0; else mode=1;
			if( test_temp&BIT6 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
		break;
		
		default:break;
	}

	OLED_Refresh_Gram();//开始显示
}








vs16 menu_bit=0; //当前选中行的序号，第一行（0），标题行（menu_max+1）
vs16 menu_max=2; //最后一行的序号，不含标题共3行可选（0~2），
//u16  menu_temp=0;

//主菜单：
void OLED_Show_Menu(void)
{
	
	//menu_bit=3; //当前选中行，选中详细数据界面
	
	u8 cnt=0,bit_cnt=0;//当前屏幕y坐标，当前参数的行数
	u8 mode,page;
	static u8 page_temp;
	
	char str_tmp[20]; //显示字符
	
	page = menu_bit/4;
	if( menu_bit> menu_max) page=0; //标题行
	
	
	bit_cnt = 4*page;
	
	
	
	//test_temp = Show.test_flag;
	//第一行
	for(u8 i=0;i<8;i++) OLED_Show_CH_String(24+i*6,cnt,oled_CH20[i],12,1);//菜单页面：
	
	
	
	if(menu_bit==menu_max+1) mode=0; else mode=1; //反白显示，选中行与当前行一致
	for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH24[i],12,mode);//退出
	
	

	
	if(page_temp!=page)	
	{
		page_temp=page;
		OLED_Clear();
	}
	
	switch(page)
	{
		case 0:
			
			//第二行
			cnt+=13;
			for(u8 i=0;i<18;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH21[i],12,1);//详细数据：
		
			if(menu_bit==bit_cnt++) mode=0; else mode=1;

		
		
		
		
			//在主菜单，需要保存参数时，在详细数据界面后面，显示保存，而非进入
			if(ANO_Param.SaveFlag){//如果更改了 力度或者漂移修正值
						for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH28[i],12,mode);//保存
			}
			else{
						for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH25[i],12,mode);//进入
			}
				

		
		
		
		
		
			//if( test_temp&BIT2 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			//else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
		
			//第三行
			cnt+=13;
			for(u8 i=0;i<18;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH0[i],12,1);//飞控设置 - - -
			if(menu_bit==bit_cnt++) mode=0; else mode=1; //反白显示，选中行与当前行一致
			//for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);//进入  或  未连
			if( Show.Connect_Succeed )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH25[i],12,mode);//进入
			else										for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH26[i],12,mode);//未连接
		
		
			//第四行
			cnt+=13;
			for(u8 i=0;i<18;i++) OLED_Show_CH_String(0+i*6,cnt,oled_CH5[i],12,1);//传感器状态
			if(menu_bit==bit_cnt++) mode=0; else mode=1;
			//for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);//进入  或  未连
			if( Show.Connect_Succeed )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH25[i],12,mode);
			else										for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH26[i],12,mode);
			

			
			
			//最后一行
			cnt+=13;
			
			for(u8 i=0;i<20;i++) OLED_Show_CH_String(8+i*6,cnt,oled_CH22[i],12,1);//请使用方向键选择↓↑：
			
			
			/*
			//一行16个字符， 0x11 22 33 44 55
			//Show.debug2=-1;
			if(Show.debug2<0){
					sprintf(str_tmp, "%02x|-%02x|%02x|%02x|   ",Show.debug1,-Show.debug2,Show.debug3,Show.debug4);
			}
			else{
					sprintf(str_tmp, "%02x|+%02x|%02x|%02x|   ",Show.debug1,Show.debug2,Show.debug3,Show.debug4);
			}
			OLED_ShowString(0,cnt,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
			//Show.debug5=-1;
			if(Show.debug5<0){
					sprintf(str_tmp, "-%02x| ",-Show.debug5);
			}
			else{
					sprintf(str_tmp, "+%02x| ",Show.debug5);
			}
			OLED_ShowString(12*8,cnt,(u8 *)str_tmp,12,1);  //高度12，宽度8（实际6像素，会加间隙2个像素）
			*/
			
			
			
			//if(test_bit==bit_cnt++) mode=0; else mode=1;
			//if( test_temp&BIT3 )for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH10[i],12,mode);
			//else								for(u8 i=0;i<4;i++) OLED_Show_CH_String(104+i*6,cnt,oled_CH11[i],12,mode);
			
		break;
		

		
		default:break;
	}

	OLED_Refresh_Gram();//开始显示
}





//遥杆 方向键 上下检测：
void Gesture_Check0(u16 val,vs16 *set,vs16 max)  //摇杆摇杆上下移动选择定高定点控制
{
	static u8 cnt0,cnt1; //向上，向下
	
	
	//向上拨动1次
	if(val>1800) 
	{
		cnt0++;
	}
	else//拨动回中时才算1次
	{
		if(cnt0>2) (*set)--; //光标向上移动一行
		cnt0 = 0;
	}
	
	
	//向下拨动1次
	if(val<1200) 
	{
		cnt1++;
	}
	else//拨动回中时才算1次
	{
		if(cnt1>2) (*set)++; //光标向下移动一行
		cnt1 = 0;
	}
	
	if((*set)<0) 				(*set) = max+1;//0表示第1行，max是最后一行，max+1是标题
	else if((*set)>max+1) (*set) = 0; //
}








//摇杆方向键左右检测
//更改设置值，注意这里的设置值，并不能保存，所以发上去的值不会变。
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


//Aux1_Check(Rc.PIT,&menu_bit,Show.windows); //短按

//顶部右侧按键  key1 (=PB7) 被按下 
//10ms一次，与key_function()一样
void Key1_Check(u16 val,vs16 *set,vs16 max, u8 index)  //摇杆摇杆上下移动选择定高定点控制
{
	static u8 cnt0; //向上，向下
	
	
	//按下
	if(val && !(Rc.AUX40 & 0x80) ) //key1单独按下，set按键没有按下时；set+key1表示校准摇杆中值
	{
		cnt0++;
	}
	else//
	{
		//按下松开才算，在key里面也定义了一个最短时间，那里负责蜂鸣器声音
		if(cnt0>=4 && cnt0<50){ //要小于key.c定义key1长按的时间
			
			sprintf(Show.str_oled, ""); //恢复标题栏
			
			//主界面0的时候在按键key.c那边
			//注意，主界面那边也会检测并响应 aux1 事件，点击后会进入主菜单。
			switch(index)
			{
				
					case 0://当前界面为主界面
									//在主界面时，使用key.c默认按键的响应程序，要区分长按与短按
					break;
				
				
					case 2://飞控设置界面
						if(*set==max+1) {
								Show.windows = 4; //回主菜单
								menu_bit=menu_max+1;//主菜单默认反白选择，回主界面
							  
						}
					
					break;
					
					case 3://传感状态界面
						if(*set==max+1) {
								Show.windows = 4; //回主菜单
								menu_bit=menu_max+1;//主菜单默认返回选择，回主界面
						}
					
					break;		
					
					case 1:  //正在详细数据 界面，可以直接回到主界面，而不是主菜单

						if(detail_bit==0){
							
									
										//退出详细数据界面时，需要保存参数
										if(ANO_Param.SaveFlag){//如果更改了 力度或者漂移修正值
													ANO_Param_SAVE();         //Flash存储功能   除了这里，还有恢复默认值，以及摇杆校准，对频时，会保存参数到flash
													sprintf(Show.str_oled, "SaveParam"); //恢复标题栏
													ANO_Param.SaveFlag=0;
										}
										else{
													//要等保存完参数才能进入下一个界面
													detail_bit=1; //进入第二页
											
										}

							
						
						}
						
						//当前为第二页，直接退出到主界面
						else{
							
										detail_bit=0;
										Show.windows = 0;
							
							
						}
					

					
					
					break;		
					
					case 4: //主菜单

					
							Show.windows =*set+1; //0表示回到主界面，1表示飞控设置界面，2表示传感器状态界面，3表示更多参数界面
							
							//进入详细数据界面时，需要保存参数
							if(Show.windows==1 && ANO_Param.SaveFlag){
									ANO_Param_SAVE();         //Flash存储功能   除了这里，还有恢复默认值，以及摇杆校准，对频时，会保存参数到flash
									sprintf(Show.str_oled, "SaveParam"); //恢复标题栏
									ANO_Param.SaveFlag=0;
							}
					
					
							//回到主界面
							if(*set==max+1) {
									Show.windows =0;//回主界面
									menu_bit=0;//退出后，下次重新进来主菜单，还是默认选择详细界面
							}
							else if(Show.Connect_Succeed==0 && ( *set==1 || *set ==2 )){ //如果选择项时1或2（第一个是0，标题是3）
									Show.windows =4;//仍然停留在主菜单，不允许进入 菜单2和3
							}
					
					break;	
			
			}
					
		
			
			
		
		
		}
		
		cnt0 = 0;

	}
	

}











u8 send_flag=0;



//10ms一次，在计划任务中调用，
//在这里把 Show.set_flag 传递给 set_temp，
//然后通过设置标识位send_flag，传输给飞控
void Gesture_Check(void)
{
	static u8 temp;
	
	//如果上次已操作界面与显示界面不一致
	if(temp!=Show.windows)
	{
		//如果当前显示界面为飞控设置界面，第一次进入该界面，提取参数 到set_temp
		if(Show.windows==2) set_temp = Show.set_flag;
		//目前暂无在参数设置界面更改飞控设置的功能
		//在遥控上，只能更改定高模式，通过key2来更改，设置界面只是用来显示。
		
		//如果上次操作界面为飞控设置界面，说明显示界面已以来操作界面，则上传设置参数set_temp
		//但飞控那边，暂未解析这个数据。目前仍然通过通道来接收遥控数据
		if(temp==2 && set_temp!=Show.set_flag) send_flag = 1;
		
		//更新当前操作界面
		temp=Show.windows;
	}
	
	
	switch(Show.windows)
	{
		//飞控设置界面
		//set_bit表示当前选中行
		//set_max表示屏幕共有多少行
		case 2:
			Gesture_Check0(Rc.PIT,&set_bit,set_max); //上下
		
			Key1_Check(Rc.AUX40&0x01, &set_bit,set_max, Show.windows); //短按
		
			if(Show.Connect_Succeed)
			{
			  //更改参数，方向左右，更改的值，导致set_temp变化，离开设置界面时，导致 send_flag = 1，触发上传
				Gesture_Check1(Rc.ROL,&set_temp,set_bit); //左右
			}
		break;
		
			
		//传感状态界面，响应上下选择键
		//test_bit表示当前选中行
		//test_max表示屏幕共有多少行
		case 3:
			Gesture_Check0(Rc.PIT,&test_bit,test_max); //上下
		
			Key1_Check(Rc.AUX40&0x01, &test_bit, test_max, Show.windows); //短按
		
		break;
		
		
		//详细数据界面，响应右键短按
		case 1:
			//Rc.AUX40&0x01   //右边按键B7被按下
			Key1_Check(Rc.AUX40&0x01, 0, 0, Show.windows); //短按
		
		break;

		//主菜单界面
		//menu_bit表示当前选中行
		//menu_max表示屏幕共有多少行
		case 4:
			Gesture_Check0(Rc.PIT,&menu_bit,menu_max); //上下

			//Rc.AUX40&0x01   //右边按键B7被按下
			Key1_Check(Rc.AUX40&0x01,&menu_bit,menu_max,Show.windows); //短按
		
		break;
		
		
		
		
		default:break;
	}
	
	
	
}





u32 time_tmp=0;
u16 time_ms=0;
u16 time_ms1=0;

//50ms执行一次
void Show_Duty(void)
{
	static u8 cnt=0,mode=0;
	static u8 temp=0; //当前显示界面
	
	cnt++;
	
	//当前界面和目标界面不同
	if(Show.windows!=temp)
	{
		temp = Show.windows;
		OLED_Clear();//清屏
	}
	
	//左下方计时小圆点，1秒变化一次
	if(cnt>=20){
				cnt=0;
				mode=!mode;
				OLED_DrawPoint(0,63,mode);
		

	}

	
	//查看被调用周期
	time_ms = (SysTick_count_2ms - time_tmp)*2;// 50ms
	time_tmp = SysTick_count_2ms;

	
	//刷新
	switch(Show.windows)
	{ //0主界面，1飞控设置，2传感器状态
		case 0:	oled_show();	break; //主界面
		
		case 2:	OLED_Show_Seting();	break;  //飞控设置
		case 3:	OLED_Show_Test();	break; //传感器状态
		
		case 1:	OLED_Show_detail();	break; //添加更多参数页面
		case 4:	OLED_Show_Menu();	break; //添加主菜单页面
		
		
		default:break;
	}
	
	
	
	
	
	//查看屏幕刷新耗时
	time_ms1 = (SysTick_count_2ms - time_tmp)*2;//耗时46~49ms，数据改为连续模式传输耗时变为24ms
	//time_tmp = SysTick_count_2ms;

	// i2c速度要比spi慢很多。i2c只要2根线，spi需要4~6根线（rst,dc,sck,mosi,cs,miso）,
	// 在这里，主要是spi有d/c单独的控制线，而i2c每个数据都要发送额外的d/c指令
	// 另外没有cs控制线，导致每次数据都需要发送器件从地址，导致单个字节时长为spi的3倍
		
	// 模拟i2c屏幕，更新一次主界面，需要46ms；
	// 屏幕每50ms刷新一次的速度太快了。
	// 已经解决：现在改为连续传输模式，全屏幕更新一次总耗时已经减少为24ms

	
}

