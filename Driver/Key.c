#include "sysconfig.h"
#include "stdio.h"  //	sprintf()

uint8_t Mode=1,Fun=0;//默认锁尾模式、非夜间模式

//AUX1 零飘修正值Rol   ...stick.c更新
//AUX2 定高模式				                 ---key.c更新
//AUX3 零飘修正值Pit   ...stick.c更新
//AUX4 按键状态				                 ---key.c更新
//AUX5 电池电压				...stick.c更新
//AUX6 帧率 NRF_SSI 	...ANO_Loop_1Hz()更新
//AUX7 力度/油门比例/偏航角			               ---key.c更新
//AUX8 定高起飞/屏蔽油门/无头模式                ---key.c更新
//AUX8 拍照/LED亮度/高度调整记录/高度加指令/高度减指令          ---key.c更新

u8 xy_set=0; //记录按下set按键后，有没有按 漂移修正/key2/lock



/******************************************************************************
函数原型:	void KEY_Init(void)
功　　能:	按键初始化
*******************************************************************************/ 
void KEY_Init(void)
{
	
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |  RCC_APB2Periph_AFIO,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |  RCC_APB2Periph_AFIO,ENABLE);
		
		//改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG+SW-DP）
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
		//改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
		
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11;    
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //上拉输入，，，lock按键(B2)，最好再加外部4.7k上拉
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(GPIOB, &GPIO_InitStructure);   //B3,B4,B5,B6,    B7,B8,    B2(12),B11,B9,
	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_13; 
		GPIO_Init(GPIOC, &GPIO_InitStructure);	 //C13,C14,C15
	
		//没有用到
		_offset.pitch=_offset.roll=2048;  //微调按键值L R
		_offset.thr=2048; //微调按键值M
	
	
}






//10ms检测一次，
void key_function(void)  //顶部2个按键的程序，可以区分长按，短按
{
		
	
	
	/*
		
		k2(B7)																	k1(B8)
		
		
														 set(B9)
																						k3/front(B3)
	k7/up(C13)	  k9/p(C14)                 
																	k6/left(B4)       k5/right(B6)
	k8/down(B11)  k10/m(C15)
																						k4/back(B5)
				 lock(B2)改为B12
		
	
	
//SCL(B14) 和 K1(B8) 对调
//SDA(B15) 和 K2(B7) 对调
	*/
	

		
		
		
		
		static u8 count0=0,count1=0;
		static u8 count2=0,count3=0;
		static u8 count4=0,count5=0;
		static u8 count6=0,count7=0;
	
		static u8 beet_falg=0;
		static int16_t beet_temp;//有负数
		
		Rc.AUX40 = 0; //安装状态全部复位
		if(!Mode_Read)	Rc.AUX40 |= 0x01;	//右边按键KEY1   PB14 被按下（低电平）
		if(!Fun_Read)		Rc.AUX40 |= 0x02;	//左边按键KEY2	  PB15 被按下（低电平）
		
//SCL(B14) 和 K1(B8) 对调
//SDA(B15) 和 K2(B7) 对调
		
		
		//其余6个按键,算上顶部2个，一共8个按键
		//另外还有4个，方向摇杆下面，在另一个函数
	
	
		if(!(GPIOC->IDR  & GPIO_Pin_13)) Rc.AUX40 |= 0x04;	//KEY_UP(k7)   = C13   //系统板的C13有LED
		if(!(GPIOB->IDR  & GPIO_Pin_11)) Rc.AUX40 |= 0x08;	//KEY_DOWN(k8) = B11
	
		if(!(GPIOC->IDR  & GPIO_Pin_14)) Rc.AUX40 |= 0x10;	//KEY_RATIO_P(k9) = C14
		if(!(GPIOC->IDR  & GPIO_Pin_15)) Rc.AUX40 |= 0x20;	//KEY_RATIO_M(k10) = C15
	
		if(!(GPIOB->IDR  & GPIO_Pin_12))  Rc.AUX40 |= 0x40;	//KEY_LOCK = B2, 改为B12, 原来的DC，spi OLED 
  	if(!(GPIOB->IDR  & GPIO_Pin_9))  Rc.AUX40 |= 0x80;	//KEY_SET  = B9，原来红色led
	
		//是否被按下
		#define KEY1()  (Rc.AUX40 & 0x01)
		#define KEY2()  (Rc.AUX40 & 0x02)
	
		#define KEY_UP()  (Rc.AUX40 & 0x04)
		#define KEY_DOWN()  (Rc.AUX40 & 0x08)

		#define KEY_RATIO_P()  (Rc.AUX40 & 0x10)
		#define KEY_RATIO_M()  (Rc.AUX40 & 0x20)
	
		#define KEY_LOCK()  (Rc.AUX40 & 0x40)
		#define KEY_SET()  (Rc.AUX40 & 0x80)

	//0.5s后取消临时移动, 相应通道变为低数值
#define MOVE_TIME 30 //300ms

	//为了让移动增量持续0.5s后自动回落
 	static u8 move_time3=0;  //Z+
	static u8 move_time4=0;  //Z-



	//Z+
	//油门按键增加动作时长已到，并且不在加速起飞中。如果加速则稍微等等
	//if( move_time3>MOVE_TIME  && aux2_long!=1){  
	if( move_time3>MOVE_TIME){  //qux2取消加速起飞功能
		
		move_time3=0;
		AUX87_s(0); // 结束油门增加动作，至少0.5s
		
	}
	else if(move_time3){
		
		move_time3++; //正常计时
		
		//如果没有放开，则计时重设，注意这里不管方向，方向是按下瞬间决定的
		//快结束的时候看下用户有没有持续按，持续按的话，稍稍延长，不要延长0.5s
		//if( move_time3==MOVE_TIME && (!(GPIOC->IDR & aux3) || !(GPIOA->IDR & aux4)) ){
		if( move_time3==MOVE_TIME && KEY_UP() ){	
				move_time3=MOVE_TIME-1; //不要直接给1，长按结束要尽快停止临时移动
		}
		
	}
	
	//Z-
	//油门按键增加动作时长已到，并且不在加速起飞中。如果加速则稍微等等
	//if( move_time3>MOVE_TIME  && aux2_long!=1){  
	if( move_time4>MOVE_TIME){ //qux2取消加速起飞功能
		
		move_time4=0;
		AUX88_s(0); // 结束油门增加动作，至少0.5s
		
	}
	else if(move_time4){
		
		move_time4++; //正常计时
		
		//如果没有放开，则计时重设，注意这里不管方向，方向是按下瞬间决定的
		//快结束的时候看下用户有没有持续按，持续按的话，稍稍延长，不要延长0.5s
		//if( move_time3==MOVE_TIME && (!(GPIOC->IDR & aux3) || !(GPIOA->IDR & aux4)) ){
		if( move_time4==MOVE_TIME && KEY_DOWN() ){
				move_time4=MOVE_TIME-1; //不要直接给1，长按结束要尽快停止临时移动
		}
		
	}
	
	


	///////////////////////////顶部2个功能键////////////////////////////////////////////
	
	
		// 右边顶部 key1 /////////////////////////////////////////////////////
		if(KEY1())//右边按键B7按下状态，按下计时
		{
				if(count0!=255)//尚未触发长按
				{		
						count0++;
					
						if(count0>=100)   //长按1s，只在主界面有效,其他界面在show.c定义（长按=短按）
						{
							count0=255; //已触发长按
							//beet_falg=2;
							
							//key1长按只在主界面有效。主界面才会区分长按与短按，
							//在其他子界面，key1使用另外的检测程序，不区分长按与短按
							
							sprintf(Show.str_oled, "K1: long ");
							Check_Ch = 1;   //对频变量
							ANO_LED_0_FLASH();		//led闪烁
							Show.windows=0; //进入主界面
							
							
							
						}
				}
			
		}
		
		
		//更换屏幕界面
		else   //右边按键key1=PB7，已经松开
		{
			if( count0>=4 && count0!=255 ) //短按松开, 进入菜单
			{ //在show.c里面也会检测key1,  Key1_Check()
				//if(Show.hardware_type==1) //飞控返回的硬件类型为四轴飞行器
				//{
					
					//Show.windows++;
					//Show.set_flag&BIT6，当无人机失去联系后，会自动返回主界面
					//if(Show.windows>2 || Show.set_flag&BIT6) Show.windows=0;//回主界面
					
				
				
				
					if(KEY_SET()){ //set+key1，只在主界面和详细界面（含2页）有效（可以显示操作提示），其他界面不响应set+key1
					
						if(Show.windows==0 || Show.windows==1 ){
							
								xy_set=1;//已经按下left，屏幕不要刷新成set键松开
											
							
								if( Rc.THR > 1050.0f){ //摇杆校准时，油门必须在底部
										sprintf(Show.str_oled, "K1:error3");
										beet_falg=2;
									
									
								}
								else{
										//count0=0;
										//beet_falg=1;
										sprintf(Show.str_oled, "MidOffset");
										ANO_Param.OffSet_En = 1; //触发 Mid_Offset() 
										ANO_LED_0_FLASH();//蜂鸣器和LED闪烁3次, 这里有两次delay0.6s，导致连续两次触发按键
								
								}
							

							
							
						
						}
						else{
								//没有响声表示，，所在显示界面不支持 set+key1
						
						
						}
						

						
						
						
						

					
					}
					else{ //短按，以下代码只在主界面有效；其他界面的短按在show.c定义
						//注意：在show.c也有key1的检测！！！负责主界面以外的界面1，2，3，4的key1响应

							beet_falg=1;//所有界面单独按key1响声都在这里
						
							if(Show.windows==0){//在主界面时，右键只响应声音，具体动作由具体界面负责
									
									sprintf(Show.str_oled, "K1: down ");  
									Show.windows=4; //进入主菜单界面
							}
						

					
					}

			}
			
			count0=0;
			
		}
		
		
		static uint32_t time_key2 =0; //上次按下时间
		static u8 key2_click=0;//连续点击次数
		
		
		

		
		
		
		
		
		// 左边顶部 key2 /////////////////////////////////////////////////////		
		//if(Rc.AUX4&0x02)//左边按键 K2 按下状态，按下计时
		if(KEY2())
		{
				if(count1!=255)//尚未触发长按
				{
						count1++;
					
						//校准摇杆中值，已经转移到 set+key1
						if(count1>=100)   //长按1s，定高起飞，//摇杆中位校准
						{
							
							

							
								count1=255; //已触发长按
							
							
								//检测油门是否在中位靠下位置（1400~+/-100）（1300~1500）上限扩到1600，，，1300~1600
							
								//非油门屏蔽下（长按代表陀螺仪校准）
								if(!AUX82_isH()){ 
									
													if( Rc.THR > 1100.0f){ //陀螺仪校准时，油门必须在底部
															sprintf(Show.str_oled, "K2:error1");
															beet_falg=2;
													}
													else{
															AUX811_s(1); //陀螺仪校准

															sprintf(Show.str_oled, "K:MPU6050");
															beet_falg=4; //响3声
														
													
													}
								
								}
								
								//在非油门屏蔽下（才能保证飞机已解锁）
								else if( Rc.THR < 1300.0f || Rc.THR > 1700.0f){ //油门必须在中位附近
													sprintf(Show.str_oled, "K2:error2");
													beet_falg=2;
									
								}

								else{	
													sprintf(Show.str_oled, "K2:keyFly");
													beet_falg=2;

													AUX81_s(1);//会立即进入定高起飞，飞多高有用户按多久来决定。松开时，会把当前高度作为悬停高度；
													AUX82_s(0); //取消油门屏蔽
													Rc.AUX2 =1000;//自动改为定高模式
													AUX73_s(0);//偏航角预期角设为0

								}


							
							

						}
				}
		}
		
		
		else //左边按键 K2 已经松开，松开响一次，50ms
		{
			if( count1>=4 && count1!=255 ) //短按=切换定高模式
			{
				
					if(KEY_SET()){ //有按着SET按键，松开时

									//在油门有效状态，进入屏蔽状态（ready）
									if(!AUX82_isH()){
										
												beet_falg=2; //长1声
										
												//进入待飞状态
												AUX82_s(1); //屏蔽油门，油门输出1000
												//AUX82(1); //油门图标打叉
											
												//当收到油门输出1000，偏航输出2000，并且在油门屏蔽状态 AUX82_isH时，
												//飞控会解锁飞机，进入油门屏蔽下的readay状态
												Rc.YAW = 2000; //当油门为1000，yaw为2000 飞控会自动解锁
											
												sprintf(Show.str_oled, "K2: ready");
										
												AUX73_s(0);//偏航角预期角设为0
												Rc.AUX2 =2000;//自动改为姿态模式
										
												xy_set=1;//已经按下aux2，屏幕不要刷新成set键松开
										

											

									}
									//在待飞状态，则取消待飞，回到紧急锁定
									else{
										
												beet_falg=1; //短1声
										
												//油门被屏蔽时，短按松开才紧急锁定
												sprintf(Show.str_oled, "K2:cancel");
												//不按aux9表示油门失效的同时，锁定机器。
												Rc.YAW =1000;//
												Rc.THR=1000; //油门无效时，直接输出1000
									
												//并且油门输出正常
												AUX82_s(0);//取消油门屏蔽
												//AUX82(0); //油门图标恢复
												AUX73_s(0);//偏航角预期角设为0
											
										
												//飞机从解锁状态变更为 紧急锁定，要恢复之前保存的定高/姿态模式
												if(Show.position_old==1){
															Rc.AUX2=1000;
												}
												else{
															Rc.AUX2=2000;
												}
													
										
												xy_set=1;//已经按下aux2，屏幕不要刷新成set键松开
										
									}	
					}
					
					
					else{ //单独按key2，松开时
						
									

						
						
						
									
									if(!AUX82_isH()){//非油门屏蔽下
										
										
												beet_falg=1; //响1声
										
												sprintf(Show.str_oled, "K2: Click");
										
												/*
												beet_falg=3; //响2声
																
												Rc.AUX2 ^= (2000^1000); //切换悬停模式
										
												//是否悬停模式
												if(Rc.AUX2==2000){
														sprintf(Show.str_oled, "K2:Po= no");
														Show.position_old =0;  //通过按键明确更改定高定点时，需要保存，以便解锁时恢复
														
												} 
												else{
														sprintf(Show.str_oled, "K2:Po=yes");
														Show.position_old =1;
												}
												*/
										

									}
									
									else{//取消油门屏蔽，并紧急锁定
										
												beet_falg=1; //响1声
										
												sprintf(Show.str_oled, "K2: LOCK ");
												//不按aux9表示油门失效的同时，锁定机器。
												Rc.YAW =1000;//
												Rc.THR=1000; //油门无效时，直接输出1000
									
												//并且油门输出正常
												AUX82_s(0);//取消油门屏蔽
												//AUX82(0); //油门图标恢复
												AUX73_s(0);//偏航角预期角设为0
										
									
												//飞机从解锁状态变更为 紧急锁定，要恢复之前保存的定高/姿态模式
												if(Show.position_old==1){
															Rc.AUX2=1000;
												}
												else{
															Rc.AUX2=2000;
												}
													
									
									
									}
									
									
									
									//连按两次
									
									
									//单独按3次，马达测试
									//if(!KEY_SET()){//按下后，只触发一次短按

									if( SysTick_count-time_key2>55){ //间隔时间大于1s，清空连续点击记录
											key2_click=0;//连续点击次数
									}
									
									time_key2	= SysTick_count; //上次单独按下时间
										
								
									key2_click++;//连续点击次数
									
									
									if(key2_click==2 && KEY_LOCK()){//连续点击次数2次,并且按着 LOCK按键
										
												key2_click=0;
												beet_falg=3; //响2声
										
										
												//按着lock键，关闭激光
												//光流悬停模式下，切换激光优先与气压优先，用于强阳光直射下屏蔽激光。
												if(AUX833_isH()){//1表示已关闭激光
														AUX833_s(0); //开启激光，4米以下激光测高
														sprintf(Show.str_oled, "Laser:yes"); 
														xy_set=1;//已经按下left，屏幕不要刷新成set键松开
												
												}
												else{//0表示未关闭激光
														AUX833_s(1); //强制关闭激光，气压测高
														sprintf(Show.str_oled, "Laser: no");
														xy_set=1;//已经按下left，屏幕不要刷新成set键松开
												}
												

									}
									//切换定高模式，如果有光流，定高即悬停模式，如果没有光流，定高即气压定高模式
									else if(key2_click>=3){//连续点击次数3次

												key2_click=0;
												beet_falg=3; //响2声
										

												Rc.AUX2 ^= (2000^1000); //切换悬停模式
										
												//是否悬停模式
												if(Rc.AUX2==2000){
														sprintf(Show.str_oled, "Posit: no");
														Show.position_old =0;  //通过按键明确更改定高定点时，需要保存，以便解锁时恢复
														
												} 
												else{
														sprintf(Show.str_oled, "Posit:yes");
														Show.position_old =1;
														AUX833_s(0); //开启激光，4米以下激光测高

												}

										
										
									}

									//}
									
														
									
									
									
					
					}
		
						
						
						
						
						
						
				
			}
			else if( count1==255 ) { //长按松开=结束
			
						sprintf(Show.str_oled, "K2:LongUp");
						AUX81_s(0); //定高起飞结束
						AUX811_s(0); //陀螺仪校准结束
			
			}
			
			count1=0;
		}
		

		
	///////////////////////////其他按键////////////////////////////////////////////
		
		// 按键  H+/////////////////////////////////////////////////////		
		if(KEY_UP())//按下就响应，不区分长按短按，松开后0.5s内持续输出
		{
			if(count2!=255){
					count2++;

					if(count2>=2) //短按按下
					{
							count2=255;
							beet_falg=1;//按下响一次，50ms
						
						
									//在屏蔽状态（ready），
									//打开油门,姿态模式一键起飞
									if(AUX82_isH()){
											AUX82_s(0);//改变油门屏蔽状态
											//LCD_str_h16(0,0,"K:thr_ok+"); 
											//sprintf(Show.str_oled, "K:thr_ok+");
											sprintf(Show.str_oled, "K: thrFly");
										
									}
									else{
											sprintf(Show.str_oled, "K: hight+");
									
									}
						


									AUX84_s(!AUX84_isH()); //定高模式时，调整高度，每按一次翻转一次，


									AUX87_s(1); // 姿态模式时增加油门，至少0.5s
									move_time3 = 1; //计时重设, 0.5s后，数值自动变低
				

					}
			
			}

			
		}
		else //按键已松开，松开响一次，50ms
		{
			if( count2==255 ) //短按松开
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "key_up up");
				
				
			}
			count2=0;
		}
		
		
		
		
		
		
		// 按键 H- /////////////////////////////////////////////////////		
		if(KEY_DOWN())//按下就响应，不区分长按短按，松开后0.5s内持续输出
		{
			if(count3!=255){
					count3++;

					if(count3>=2) //短按按下
					{
							count3=255;
							beet_falg=1;//按下响一次，50ms
						
									if(!KEY_SET()){
											sprintf(Show.str_oled, "K: hight-");

											AUX84_s(!AUX84_isH()); //定高模式时，调整高度，每按一次翻转一次，


											AUX88_s(1); // 姿态模式时减少油门，至少0.5s
											move_time4 = 1; //计时重设, 0.5s后，数值自动变低
											
									}
									//SET + 下 表示马达手动测试
									else{
											//sprintf(Show.str_oled, "K: hight-");
											sprintf(Show.str_oled, "key:test2");
											AUX844_s(1);//进入马达手动测试状态，不影响飞行。锁定时按H+电机转动，H-切换电机
											xy_set=1; //set松开时不要更新屏幕
									}

						
						
					}
			
			}

			
		}
		else //按键已松开
		{
			if( count3==255 ) //短按松开
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "key_down up");
				

				
			}
			count3=0;
		}
		
		
			
		
		#define RATIO_TIME 30	//触发连续调节
		
	
		// 按键 /////////////////////////////////////////////////////		
		if(KEY_RATIO_P())//按下就响应，不区分长按短按，但会触发连续调节
		{

			

					if(count4<255) count4++;


					if(count4>=4 && count4<100) //触发短按按下事件
					{
							count4=100;//短按触发成功
							beet_falg=1;//按下响一次，50ms

						
						
							//按下就响应
							if(!KEY_SET()){
										if(Show.ratio<0x0f) {
												//Show.ratio++; //默认取消调整功能（油门比例）
										}
										//油门比例
										sprintf(Show.str_oled, "K+ratio%02i", Show.ratio+1);
							}
							
					}
					
					
					else if(count4>100+RATIO_TIME){//间隔30个周期后，触发连续调节
					
							count4=100+RATIO_TIME-3; //下次连续调节间隔3个周期
						
					
							//按下就响应
							if(!KEY_SET()){
						
										if(Show.ratio<0x0f) {
												//Show.ratio++; //默认取消调整功能（油门比例）
										}
										//油门比例
										sprintf(Show.str_oled, "K+ratio%02i", Show.ratio+1);
							}

					}

					
					


		}
		else //按键已松开，松开响一次，50ms
		{
			//if( count4==255 && count4>100) //短按
			if( count4>=4 ) //松开，不管短按长按
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "RATIO_P up");
				
					//松开才响应
					if(KEY_SET()){
						
									if((int8_t)ANO_Param.OffSet_Yaw <= 127-5){
											ANO_Param.OffSet_Yaw = ANO_Param.OffSet_Yaw + 1;
											//ANO_Param.SaveFlag=1;			//显示待保存，，，，，，注意：如果需要保存 YAW偏移，还 需要 在 ANO_Param_READ() 中注释掉   YAW偏移复位
										
									}

									sprintf(Show.str_oled, "Set: Y+1 ");
								
									xy_set=1;
								
				
					}
				
				
				

			}
			count4=0;
		}
				
		


		// 按键 /////////////////////////////////////////////////////		
		if(KEY_RATIO_M())//按下就响应，不区分长按短按，但会触发连续调节
		{
			



					if(count5<255) count5++;


					if(count5>=4 && count5<100) //触发短按按下事件
					{
							count5=100;//短按触发成功
							beet_falg=1;//按下响一次，50ms


							//按下就响应
							if(!KEY_SET()){
											if(Show.ratio>0) {
													//Show.ratio--; //默认取消调整功能（油门比例）
											}

											//油门比例
											sprintf(Show.str_oled, "K-ratio%02i",Show.ratio+1);
											
							}
							
							
							
					}
					else if(count5>100+RATIO_TIME){//间隔30个周期后，触发连续调节
					
							count5=100+RATIO_TIME-3; //下次连续调节间隔3个周期
						
					
						
							//按下就响应
							if(!KEY_SET()){

											if(Show.ratio>0) {
													//Show.ratio--; //默认取消调整功能（油门比例）
											}
											

											//油门比例
											sprintf(Show.str_oled, "K-ratio%02i", Show.ratio+1);
											
							}
	
							
							
							
					}
					
					
						

			
		}
		else //按键已松开，松开响一次，50ms
		{
			//if( count5>=2 && count5<=100 ) //短按
			if( count5>=4 ) //松开，不管短按长按
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "RATIO_M up");
				
				
					//松开才响应
					if(KEY_SET()){
						
									if((int8_t)ANO_Param.OffSet_Yaw >= -127+5){
											ANO_Param.OffSet_Yaw = ANO_Param.OffSet_Yaw-1;
											//ANO_Param.SaveFlag=1;			//显示待保存，，，注意：如果需要保存 YAW偏移，还 需要 在 ANO_Param_READ() 中注释掉   YAW偏移复位
										
									}

									sprintf(Show.str_oled, "Set: Y-1 ");
								
									xy_set=1;
								
				
					}

				
				
			}
			count5=0;
		}
				
				
		
		static uint32_t time_key_lock =0; //上次按下时间
		static u8 key_locks_click=0;//连续点击次数
		
		

		// 按键 /////////////////////////////////////////////////////	
		if(KEY_LOCK())//按下就响应，不区分长按短按
		{



					if(KEY_LOCK() && !KEY_SET())//单独按下LOCK键，持续输出解锁信号
					{

									//sprintf(Show.str_oled, "Key:LOCK "); //显示紧急锁定
									//if( count6!=255 )beet_falg=2;//按下响一次，响200ms，这里要响久一些,飞机会掉下来
					
									Rc.YAW = 1000;//
									Rc.THR = 1000; //油门无效时，直接输出1000
						
									AUX82_s(0);//取消油门屏蔽
						
									AUX73_s(0);//偏航角预期角设为0
						
									//飞机从解锁状态变更为 紧急锁定，要恢复之前保存的定高/姿态模式
									if(Show.position_old==1){
												Rc.AUX2=1000;
									}
									else{
												Rc.AUX2=2000;
									}
									
						
					}

					
					if(count6!=255){ 

							count6++;

							if(count6>=2) //短按按下
							{
									count6=255;//只响应一次，松开后清零
								

									//单独按3次，马达测试
									if(!KEY_SET()){//按下后，只触发一次短按

											if( SysTick_count-time_key_lock>55){ //间隔时间大于1s，清空连续点击记录
													key_locks_click=0;//连续点击次数
											}
											
											time_key_lock	= SysTick_count; //上次单独按下时间
												
										
											key_locks_click++;//连续点击次数
									
											if(key_locks_click>=3){//连续点击次数3次
												
													AUX822_s(1);//马达测试开始
													key_locks_click=0;
												
													beet_falg=3; 
											}
											else{
													beet_falg=2; 	//按下响一次，响200ms，这里要响久一些,飞机会掉下来
													sprintf(Show.str_oled, "Key:LOCK "); //显示紧急锁定
												
											
											}

									}
										

									

									//修正，同时按着aux9，表示修正偏移
									if(KEY_SET())
									{
										
											beet_falg=1;//按下响一次，50ms
										
										
											if(AUX83_isH()){//1表示有头模式
													//切换到无头模式
													AUX83_s(0);
													sprintf(Show.str_oled, "HLess:yes");
													xy_set=1;//已经按下left，屏幕不要刷新成set键松开
											
											}
											else{//0表示无头模式
													//切换到有头模式
													AUX83_s(1);
													sprintf(Show.str_oled, "HLess: no");
													xy_set=1;//已经按下left，屏幕不要刷新成set键松开
											}
											
											
									}
									else{
													//轮流显示
													//if(*Show.str_oled=='\0'){
													//}
													//else if(!AUX822_isH()){
																	//sprintf(Show.str_oled, ""); //恢复标题栏
													//}
									}



							}
							

					
					
					}

			
		}
		else //按键已松开
		{
			if( count6==255 ) //短按松开
			{
					//beet_falg=1;
					//sprintf(Show.str_oled, "key_lock up");

			}
			
			
			if(AUX822_isH()){
					sprintf(Show.str_oled, "Key:test1"); //开始测试电机
			}
			
			
			AUX822_s(0); //马达测试结束
			
			count6=0;
		}
						
		
		
		
		
		
		
		
		// 按键 /////////////////////////////////////////////////////		
		if(KEY_SET()) //按下就响应，不区分长按短按
		{
			
			if(count7!=255){
					count7++;

					if(count7>=2) //短按按下
					{
							count7=255;
							beet_falg=1;//按下响一次，50ms
							sprintf(Show.str_oled, "key: set ");
							xy_set=0;
					}
			
			}


			
		}
		else //按键已松开，
		{

			if( count7==255) //短按松开
			{
					//beet_falg=1;
					if(xy_set==0) sprintf(Show.str_oled, "key:setUp");

			}
			
			
			count7=0;
			
		}
						
		

		
		
		
		
		//10ms检测一次
		if(beet_falg==1 || beet_falg==2 || beet_falg==3 || beet_falg==4 ) //控制蜂鸣器响
		{
				if(beet_temp>=0){
						BEEP_L; //开蜂鸣器
						LED_Red_On;  //开LED
				} 
				
				beet_temp++;
			
				if( (beet_temp>5 && ( beet_falg==1 || beet_falg==3 ||  beet_falg==4 )) || (beet_temp>20 && beet_falg==2)) //响50ms,或者响200ms
				{
						
					
						if(beet_falg==4){ //响3声
								beet_falg=3; 
								beet_temp=-10;//间隔时间
						}
						else if(beet_falg==3){ //响2声
								beet_falg=1; 
								beet_temp=-15;//间隔时间
						}
						else{
								beet_falg=0;
								beet_temp=0;
						}
						
					
						BEEP_H;  //关蜂鸣器
						LED_Red_OFF;//关闭LED
				}

		}

}





//4个微调按键程序，20ms一次
void key_scan(void)  //扫描4个微调按键程序，可以区分长按，短按
{

		//是否被按下
		#define  KEY_FRONT() !(GPIOB->IDR & GPIO_Pin_3) //front=B3
		#define  KEY_LEFT()  !(GPIOB->IDR & GPIO_Pin_4) //left=B4
		#define  KEY_BACK()  !(GPIOB->IDR & GPIO_Pin_5) //back=B5
		#define  KEY_RIGHT() !(GPIOB->IDR & GPIO_Pin_6) //right=B6
	
		static u8 count0=0,count1=0;
		static u8 count2=0,count3=0;

		static u8 beet_falg=0,beet_temp;
	
	
	

	///////////////////////////front///////////////////////////////////////////
		if(KEY_FRONT())//front 被按下
		{
			if(count0!=255)//尚未触发长按
			{		
					count0++;
				
					if(count0>=25)   //长按0.5s
					{
						count0=255; //已触发长按
						beet_falg=2;
						sprintf(Show.str_oled, "K3: long ");
				
					}
			}

		}
		else//front已松开
		{
			if( count0>=2 && count0!=255) //短按松开
			{
					beet_falg=1;

				
								if(KEY_SET()){
									
									
									if((int8_t)ANO_Param.OffSet_Pit<=127-5){
											ANO_Param.OffSet_Pit=ANO_Param.OffSet_Pit+5;
											ANO_Param.SaveFlag=1;			//待保存
										
									}

										sprintf(Show.str_oled, "Set: x+5 ");
									
										xy_set=1;
									
								
								}
								else{
										//sprintf(Show.str_oled, "key:front   ");
										if(Show.force>0) {
												//Show.force--; //默认取消调整功能（力度比例）如果希望快速移動，可以启用力度修改，把力度调最大
											
										}

										sprintf(Show.str_oled, "K+:force%i", 8-Show.force);

								}
								
				
				
				
				

			}
			count0=0;
		}
		
		
		
		
		
		
		
	/////////////////////////// left ///////////////////////////////////////////
		if(KEY_LEFT())//left 被按下
		{
			if(count1!=255)//尚未触发长按
			{		
					count1++;
				
					if(count1>=25)   //长按0.5s
					{
						count1=255; //已触发长按
						beet_falg=2;
				
				
										sprintf(Show.str_oled, "K6:camera");
										//，从坐标系的角度看，逆时针=正,，但从普通的习惯逆时针为负，所以把显示的反一下
										AUX85_s(!AUX85_isH());

				
				

					}
			}
					

		}
		
		else//left 已松开
		{
			if( count1>=2 && count1!=255) //短按松开
			{
					beet_falg=1;

				
								if(KEY_SET()){
										//ANO_Param.OffSet_Rol=ANO_Param.OffSet_Rol-10;
									
										if( (int8_t)ANO_Param.OffSet_Rol>=-127+5 ){
												ANO_Param.OffSet_Rol=ANO_Param.OffSet_Rol-5;
												ANO_Param.SaveFlag=1;			//待保存
											
										}
									
									
									
										sprintf(Show.str_oled, "Set: y-5 ");
									
										xy_set=1;
								
								}
								else{
										//sprintf(Show.str_oled, "key:left   ");
										//，从坐标系的角度看，逆时针=正,，但从普通的习惯逆时针为负，所以把显示的反一下
										if(AUX73()<96) AUX73_s(AUX73()+1);

										sprintf(Show.str_oled, "K6:yaw%+3i", -AUX73());

								}
				
				
				
				
			}
			count1=0;
		}
		
		
		
		
		
	/////////////////////////// back ///////////////////////////////////////////
		if(KEY_BACK())//back 被按下
		{
			if(count2!=255)//尚未触发长按
			{		
					count2++;
				
					if(count2>=25)   //长按0.5s
					{
						count2=255; //已触发长按
						beet_falg=2;
						
						sprintf(Show.str_oled, "K4: long ");
						
					}

			}
			
		}
		
		else//back 已松开
		{
			if( count2>=2 && count2!=255) //短按松开
			{
					beet_falg=1;
				
				
								if(KEY_SET()){
										//ANO_Param.OffSet_Pit=ANO_Param.OffSet_Pit-10;
									
									
										if( (int8_t)ANO_Param.OffSet_Pit>=-127+5 ){
												ANO_Param.OffSet_Pit=ANO_Param.OffSet_Pit-5;
												ANO_Param.SaveFlag=1;			//待保存
											
										}
									
									
										sprintf(Show.str_oled, "Set: x-5 ");
									
										xy_set=1;
								
								}
								else{
										//sprintf(Show.str_oled, "key:back   ");

										if(Show.force<7) {
												//Show.force++; //默认取消调整功能（力度比例） 如果希望快速移動，可以启用力度修改，把力度调最大
										}


										sprintf(Show.str_oled, "K-:force%i", 8-Show.force);

								}
								

				
			}
			count2=0;
		}
		
		
		
		
	/////////////////////////// right ///////////////////////////////////////////
		if(KEY_RIGHT())//right 被按下
		{
			if(count3!=255){//尚未触发长按
					count3++;
				
					if(count3>=25)   //长按0.5s
					{
						count3=255; //已触发长按
						beet_falg=2;

												sprintf(Show.str_oled, "K5: LED  ");
												//，从坐标系的角度看，逆时针=正,，但从普通的习惯逆时针为负，所以把显示的反一下
												AUX86_s(!AUX86_isH());

					}
			
			}

		}
		
		else//right 已松开
		{
			if( count3>=2 && count3!=255) //短按松开
			{
					beet_falg=1;

				
				
								if(KEY_SET()){
										//ANO_Param.OffSet_Rol=ANO_Param.OffSet_Rol+10;
									
									
										if( (int8_t)ANO_Param.OffSet_Rol<=127-5 ){
												ANO_Param.OffSet_Rol=ANO_Param.OffSet_Rol+5;
												ANO_Param.SaveFlag=1;			//待保存
											
										}
									
									
									
									
									
										sprintf(Show.str_oled, "Set: y+5 ");
									
										xy_set=1;
								
								}
								else{
										//sprintf(Show.str_oled, "key:right   ");
										//从坐标系的角度看，顺时针=负,，但从普通的习惯顺时针为正，所以把显示的反一下
										if(AUX73()>-96) AUX73_s(AUX73()-1);

										sprintf(Show.str_oled, "K5:yaw%+3i", -AUX73());

								}
				
				
				
				
				
				
			}
			count3=0;
		}
		
		

		
	
		
		//10ms检测一次
		if(beet_falg==1 || beet_falg==2) //控制蜂鸣器响
		{
				BEEP_L; //开蜂鸣器
				LED_Red_On;  //开LED
				beet_temp++;
			
				if( (beet_temp>5 && beet_falg==1) ||(beet_temp>20 && beet_falg==2)) //响50ms,或者响200ms
				{
						beet_temp=0;
						beet_falg=0;
						BEEP_H;  //关蜂鸣器
						LED_Red_OFF;//关闭LED
				}

		}
	
	
}






/******************************************************************************
4个微调按键程序，20ms一次,这个已经废弃，key_scan()替代
*******************************************************************************/ 
void ANO_key(void)//只能响应按下动作，不能区分长短按
{
		#define KEY3 GPIO_Pin_3			//front B3
		#define KEY4 GPIO_Pin_4			//left  B4
		#define KEY5 GPIO_Pin_5 		//back  B5
		#define KEY6 GPIO_Pin_6 		//right B6
	
		//#define KEY2 GPIO_Pin_2 		//偏航角修正，B2未启用
		//#define KEY11 GPIO_Pin_11 	//偏航角修正，B11未启用
	
	
		volatile static uint8_t status = 0;	
		volatile static uint8_t BEET_flag=0;
		static uint32_t temp,BUT_mun;
		
	
		switch(status)
		{
			
				//检测何时所有按键都松开（全都是高电平）
				case 0:
					if(SysTick_count - temp >10) //经过了10*20=200ms才判断是否松开，才关闭蜂鸣器，响200ms
					{
						//如果按键被按下
						//if(	(GPIOB->IDR & (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_2|GPIO_Pin_11)) 
						//			== (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_2|GPIO_Pin_11)	)
						if(	(GPIOB->IDR & (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6))
									== (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)	)
						
							status = 1;  //检测何时按下
						
						BEEP_H;  //关闭蜂鸣器
						LED_Red_OFF;//关闭LED
					}
					break;
					
					
				//检测何时有按键被按下（至少1个低电平）
				case 1:
					
						//if((GPIOB->IDR & (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_2|GPIO_Pin_11)) 
						//			!= (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_2|GPIO_Pin_11))
						if((GPIOB->IDR & (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)) 
									!= (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6))
						status = 2; //进入按下后的业务处理
						
					break;
						
						
						
						
						
						
						
						
						
						
						
						
				
				//按下后的业务处理
				case 2:	
					
				
					if(!(GPIOB->IDR & KEY6))   //右
					{
						
		//					Rc.AUX1 = 2000 - (uint16_t)(0.25f*_offset.roll);	
							//if(ANO_Param.OffSet_Rol == ANO_Param.Z_OffSet_Rol)      //微调回原始值判断
							//{	BEET_flag=1;  }				
					}
					
					
					else if(!(GPIOB->IDR & KEY4)) //左
					{

		//					Rc.AUX1 ^= 2000 - (uint16_t)(0.25f*_offset.roll);		
							//if(ANO_Param.OffSet_Rol == ANO_Param.Z_OffSet_Rol)    //微调回原始值判断
						  //{	BEET_flag=1;  }	
					}
					
					
					
					else if(!(GPIOB->IDR & KEY5))//后
					{

							//if(ANO_Param.OffSet_Pit == ANO_Param.Z_OffSet_Pit)    //微调回原始值判断
						  //{	BEET_flag=1;  }	
					}
					
					
					
					
					else if(!(GPIOB->IDR & KEY3)) //前
					{

							//if(ANO_Param.OffSet_Pit == ANO_Param.Z_OffSet_Pit)   //微调回原始值判断
						  //{	BEET_flag=1;  }
					}
					

					
					/*
					else if(!(GPIOB->IDR & KEY2))
					{
							ANO_Param.OffSet_Yaw=ANO_Param.OffSet_Yaw+10; 
						  if(ANO_Param.OffSet_Yaw == ANO_Param.Z_OffSet_Yaw)   //微调回原始值判断
						  {	BEET_flag=1;  }
					}
					else if(!(GPIOB->IDR & KEY11))
					{
							ANO_Param.OffSet_Yaw=ANO_Param.OffSet_Yaw-10; //
							if(ANO_Param.OffSet_Yaw == ANO_Param.Z_OffSet_Yaw)   //微调回原始值判断
						  {	BEET_flag=1;  }
					}	
					*/
					
					status = 0;		//业务处理完毕，重新回到松开检测。
					
					BEEP_L;  //打开蜂鸣器
					LED_Red_On;  //开LED
					temp = SysTick_count; //记录本次时间，下次循环需要等20*10=200ms后，同时也是蜂鸣器的时长
					
					
					break;
					
					
					
		}
		
			
			
		//20ms调用一次，
		//每次响10*20=0.2s
		if(BEET_flag!=0)	//微调回原始值就响2声蜂鸣器
		{
					BUT_mun++;								     //声音延时用
					
					//   1......... 11......... 21......... 31......... 41......... 51........60
				  //       响          关           响           关         响          关
			
					if(BUT_mun==1||BUT_mun==20||BUT_mun==40)   //延时判断
					{																								
							BEEP_L;
							LED_Red_On;//开LED
																			
					}
					
					else if((BUT_mun>10 && BUT_mun<20) || (BUT_mun>30 && BUT_mun<40) || (BUT_mun>50 && BUT_mun<61))  //延时判断
					{
							BEEP_H;  						  
							LED_Red_OFF;  //关LED
						 if(BUT_mun==60){BEET_flag=0;BUT_mun=0;}
					}
 
		}
		
		
		
}











