#include "ANO_Stick.h"

#define American_Mode

struct offset _offset;  //这个没用，在key.c有调用，但实际没有用处
struct _Rc Rc;



struct _Stick Stick = {0}; //摇杆原始数据，2ms一次
struct _Stick Stick_dis = {0};  //仅用于显示（提前解析）


struct _Filter Filter_THR,Filter_PIT,Filter_ROL,Filter_YAW,
							 Filter_AUX1,Filter_AUX2,Filter_AUX3,Filter_AUX5;
 
//消除中值漂移误差（油门位最低位零漂）
void Mid_Limit(struct _Rc *rc)
{
	if(rc->THR>995  && rc->THR<1005) rc->THR = 1000;
	if(rc->PIT>1490 && rc->PIT<1510) rc->PIT = 1500;
	if(rc->ROL>1490 && rc->ROL<1510) rc->ROL = 1500;
	if(rc->YAW>1490 && rc->YAW<1510) rc->YAW = 1500;
}



//通道值 限幅
void RC_Limit(struct _Rc *rc)
{
	rc->THR = (rc->THR<=1000)?1000:rc->THR; 
	rc->THR = (rc->THR>=2000)?2000:rc->THR; 
	rc->PIT = (rc->PIT<=1000)?1000:rc->PIT; 
	rc->PIT = (rc->PIT>=2000)?2000:rc->PIT; 
	rc->ROL = (rc->ROL<=1000)?1000:rc->ROL; 
	rc->ROL = (rc->ROL>=2000)?2000:rc->ROL; 
	rc->YAW  = (rc->YAW<=1000)?1000:rc->YAW; 
	rc->YAW  = (rc->YAW>=2000)?2000:rc->YAW; 
	rc->AUX1 = (rc->AUX1<=1000)?1000:rc->AUX1; 
	rc->AUX1 = (rc->AUX1>=2000)?2000:rc->AUX1; 
	rc->AUX2 = (rc->AUX2<=1000)?1000:rc->AUX2; 
	rc->AUX2 = (rc->AUX2>=2000)?2000:rc->AUX2; 
	rc->AUX3 = (rc->AUX3<=1000)?1000:rc->AUX3; 
	rc->AUX3 = (rc->AUX3>=2000)?2000:rc->AUX3; 
}



//摇杆原始数据滤波
void Window_Filter(struct _Stick *rc)
{
	static uint8_t 	Filter_Count = 0;
	
	//丢数据
	Filter_THR.sum  -= Filter_THR.old[Filter_Count];
	Filter_YAW.sum  -= Filter_YAW.old[Filter_Count];
	Filter_ROL.sum  -= Filter_ROL.old[Filter_Count];
	Filter_PIT.sum  -= Filter_PIT.old[Filter_Count];
	//Filter_AUX1.sum -= Filter_AUX1.old[Filter_Count];
	//Filter_AUX2.sum -= Filter_AUX2.old[Filter_Count];
	//Filter_AUX3.sum -= Filter_AUX3.old[Filter_Count];
	Filter_AUX5.sum -= Filter_AUX5.old[Filter_Count];
	
	Filter_THR.old[Filter_Count]  = rc->THR;
	Filter_YAW.old[Filter_Count]  = rc->YAW;
	Filter_ROL.old[Filter_Count]  = rc->ROL;
	Filter_PIT.old[Filter_Count]  = rc->PIT;
	//Filter_AUX1.old[Filter_Count] = rc->AUX1;
	//Filter_AUX2.old[Filter_Count] = rc->AUX2;
	//Filter_AUX3.old[Filter_Count] = rc->AUX3;
	Filter_AUX5.old[Filter_Count] = rc->AUX5;
	
	//更新数据
	Filter_THR.sum  += Filter_THR.old[Filter_Count];
	Filter_YAW.sum  += Filter_YAW.old[Filter_Count];
	Filter_ROL.sum  += Filter_ROL.old[Filter_Count];
	Filter_PIT.sum  += Filter_PIT.old[Filter_Count];
	//Filter_AUX1.sum += Filter_AUX1.old[Filter_Count];
	//Filter_AUX2.sum += Filter_AUX2.old[Filter_Count];
	//Filter_AUX3.sum += Filter_AUX3.old[Filter_Count];
	Filter_AUX5.sum += Filter_AUX5.old[Filter_Count];
	
	//输出数据
	rc->THR = Filter_THR.sum / Filter_Num;
	rc->YAW = Filter_YAW.sum / Filter_Num;
	rc->ROL = Filter_ROL.sum / Filter_Num;
	rc->PIT = Filter_PIT.sum / Filter_Num;
	//rc->AUX1 = Filter_AUX1.sum / Filter_Num;
	//rc->AUX2 = Filter_AUX2.sum / Filter_Num;
	//rc->AUX3 = Filter_AUX3.sum / Filter_Num;
	rc->AUX5 = Filter_AUX5.sum / Filter_Num;
	
	Filter_Count++;
	if(Filter_Count == Filter_Num)	Filter_Count=0;
}




//摇杆中值校准
void Mid_Offset(struct _Stick *rc)
{
	
	
	//重设参数，摇杆校准
	
	//校准时，油门放在最低位置，其他在中心位置。
	//漂移修正值=摇杆自身修正值ANO_Param.Z_OffSet=中心位置距离1500的偏移值
	//油门修正值=放底部时，距离1000的偏移值
	if(ANO_Param.OffSet_En)
	{		
		static uint8_t count=0;  //次数
		static int32_t count0,count1,count2,count3;
		if(count==0)
		{
			//用户数据清零
			//ANO_Param.OffSet_Thr = 0;
			//ANO_Param.OffSet_Yaw = 0;
			ANO_Param.OffSet_Pit = 0;
			ANO_Param.OffSet_Rol = 0;
			
			//摇杆自身误差清零
			ANO_Param.Z_OffSet_Thr = 0;
			ANO_Param.Z_OffSet_Yaw = 0;
			ANO_Param.Z_OffSet_Pit = 0;
			ANO_Param.Z_OffSet_Rol = 0;
			count  = 1;
			count0 = 0;
			count1 = 0;
			count2 = 0;
			count3 = 0;
			return;
		}
		else
		{
			count++;  //加50次
			count0 += rc->THR;  //读50次 求和
			count1 += rc->YAW;	//读50次 求和
			count2 += rc->PIT;	//读50次 求和
			count3 += rc->ROL;	//读50次 求和
		}
		if(count==51)
		{
			count--;
			ANO_Param.Z_OffSet_Thr = 1000 - count0 / count; //油门平均值与最下方1000的差异
			ANO_Param.Z_OffSet_Yaw = 1500 - count1 / count; //与中值1500的差异
			ANO_Param.Z_OffSet_Pit = 1500 - count2 / count; //与中值1500的差异
			ANO_Param.Z_OffSet_Rol = 1500 - count3 / count; //与中值1500的差异
			count = 0;
			
			
			Show.force = 5; //  右摇杆：默认力度8-5=3级，有保存到eeprom，如果不想被重设，可以注释这句。
			Show.ratio = 2; //	左摇杆：默认油门比例：2+1=3级
			
			
			ANO_Param.OffSet_En = 0;
			ANO_Param_SAVE();         //Flash存储功能
		}
	}
}

//2ms调用一次，获取摇杆原始数据（含自身误差修正以及滤波），不含用户漂移修正值，
void ANO_Stick_Scan(void)   //左油门, 采集摇杆数据
{

		/*
		
		ADC_ConvertedValue[0]---ADC_Channel_0---PA0---rol---方向：左右
		ADC_ConvertedValue[1]---ADC_Channel_1---PA1---pit---方向：前后
		ADC_ConvertedValue[2]---ADC_Channel_2---PA2---yaw---偏航：旋转
		ADC_ConvertedValue[3]---ADC_Channel_3---PA3---thr---油门
		
		ADC_ConvertedValue[7]---ADC_Channel_8---PB0--电池电量
		ADC_ConvertedValue[8]---ADC_Channel_17---PB0--内部1.2v参考电压
		
		*/

	
		//1.0  摇杆中位校准，仅当 ANO_Param.OffSet_En==1时（长按key2）才重新校准，校准后漂移修正就是摇杆自身中值误差
		Mid_Offset(&Stick);		//摇杆中位校准，此时不要引入 用户漂移修正值，只是摇杆自身校准
		
	
	
	
		//2.0 采集 原始数据先消除摇杆自身的中值误差，先不要引入 用户漂移修正值，
	#ifdef American_Mode //跑这里的程序                                                     
		Stick.THR = 1000 + (uint16_t)(0.25f*ADC_ConvertedValue[3]) + ANO_Param.Z_OffSet_Thr;  //油门, A3
		Stick.PIT = 1000 + (uint16_t)(0.25f*ADC_ConvertedValue[1]) + ANO_Param.Z_OffSet_Pit;  //方向：前后, A1
	#else//这里不工作																																				 
		Stick.PIT = 1000 + (uint16_t)(0.25f*ADC_ConvertedValue[1]) + ANO_Param.Z_OffSet_Pit;
		Stick.THR = 2000 - (uint16_t)(0.25f*ADC_ConvertedValue[3]) + ANO_Param.Z_OffSet_Thr;
	#endif

		Stick.YAW = 1000 + (uint16_t)(0.25f*ADC_ConvertedValue[2]) + ANO_Param.Z_OffSet_Yaw;//偏航：旋转, A2
		Stick.ROL = 2000 - (uint16_t)(0.25f*ADC_ConvertedValue[0]) + ANO_Param.Z_OffSet_Rol;//方向：左右, A0
		
		Stick.AUX5 =(uint16_t)(2.0f*ADC_ConvertedValue[7]/ADC_ConvertedValue[8]*1.2f*100)+44;     //测量电池电压 +44是防反接二极管的压价通过万用表测量得出    
		//Stick.AUX5 =(uint16_t)(2.0f*ADC_ConvertedValue[7]/ADC_ConvertedValue[8]*1.2f*100)+74; 
 
		//系数1.2代表1.2v内部基准电压，2.0表示1/2测量分压，100表示1v显示100
		//不同规格二极管压降不同，不同电流下以及不同电池电压下都有些微小差异，若实测肖特基压降为0.44v，就写44;实测压降0.69v就写69
		
		
		
		//3.0 原始数据滤波,摇杆值以及电池
		Window_Filter(&Stick);	//滑动窗口滤波
		

		
}

//10ms调用一次
void f_Stick_display(void){
		//Stick -> Rc -> Stick_display
		//Rc为上传值，Stick_display为显示值（模拟飞控解析数据）
	
		Stick_dis.THR = Rc.THR;
		Stick_dis.YAW = Rc.YAW;
	
		Stick_dis.ROL = Rc.ROL;
		Stick_dis.PIT = Rc.PIT;

		

					
										//1. 取出各种参数
										//1.1 取出力度
										int8_t lidu=(8 - AUX71());  //( 8 - AUX71);  //0~7变成8~1, 同时也是显示值
								
										//1.2 取出Z增量方向，用户计算油门临时增加值
										int move_fx_z=0;
										if(AUX88_isH()){
												move_fx_z=-1;
										}
										if(AUX87_isH()){
												move_fx_z=1;
										}
										
										//1.3 取出油门比例，用于计算基础油门
										double thr_ratio=0;
										thr_ratio =  1 + 0.05*(AUX72()-2);//  1 + 0.05*(AUX72-2); (0~15)分别为(90%~165%) 油门比例显示值 AUX72()+1,,1~16
										
										
										
										//2.更新油门摇杆值，修改好后，control.c里面电机控制会用到油门摇杆值
										//2.1 更新油门基础值,
										Stick_dis.THR = (Stick_dis.THR-1000) * thr_ratio +1000;
										
										//2.2 计算油门临时增加值, 当力度为8时，移动量为当前值的50%
										//int move_thr = ( (Stick_dis.THR-1000) * lidu * move_fx_z )/8/2; //油门临时移动量与当前油门有关
										int move_thr = ( (Stick_dis.THR-1000) * 3 * move_fx_z )/8/2; //油门临时移动量与当前油门有关
										
										//修改临时增加值，防止油门最终出现负数
										if((Stick_dis.THR-1000) + move_thr <=0) move_thr = -(Stick_dis.THR-1000);
										
										//2.3 最终修改油门值，如果收到的油门为1000，则不会做任何更改
										Stick_dis.THR = Stick_dis.THR + move_thr;
										
										
										//3.更新xy姿态角摇杆值	
										
										//3.1 摇杆值中值误差修正：现在的摇杆值 - 摇杆在解锁时的中值误差
										//这里的_offset修正，是由于摇杆自身每次回中误差导致的差异。这个差异在解锁无人机时会记录。
										//Remote.pitch -= pitch_offset;
										//Remote.roll -= roll_offset;

										//3.2.姿态角摇杆值力度修正
										Stick_dis.ROL=(Stick_dis.ROL-1500)*lidu/8 + 1500; // 1/8
										Stick_dis.PIT=(Stick_dis.PIT-1500)*lidu/8 + 1500; // 1/8

										//3.3.坐标转换，如果是无头模式	
										/*										
										if(AUX83_isH){
												int16_t x1 = (Remote.roll-1500)*cos(Angle.yaw*PI/180) + (Remote.pitch-1500)*sin(Angle.yaw*PI/180);
												int16_t y1 =-(Remote.roll-1500)*sin(Angle.yaw*PI/180) + (Remote.pitch-1500)*cos(Angle.yaw*PI/180);
												Remote.roll = x1+1500;
												Remote.pitch= y1+1500;
										}
										
										*/
												
										//3.4 零飘修正，需要在力度与无头转换之后（永远是修正飞机自身xy，不应坐标转换，与摇杆中值误差不同）
										Stick_dis.ROL = Stick_dis.ROL + (Rc.AUX1-1500);
										Stick_dis.PIT = Stick_dis.PIT + (Rc.AUX3-1500);
										Stick_dis.YAW = Stick_dis.YAW; //yaw修正的是陀螺仪，不是yaw期望值， + Rc.AUX41; 
										
										
										//float roll_pitch_ratio = 0.04f;
										//3.5 取消PID里面的offset修正摇杆中值误差 ，
										//pidRoll.offset  = 0;   
										//pidPitch.offset = 0; 


										//3.6 更新xy姿态期望角度，，，姿态摇杆值，转为姿态角。
										//摇杆中值误差已经在3.1里面提前修正了，漂移修正已经在3.4里面提前修正了
										//pidRoll.desired  = -(Remote.roll-1500) * roll_pitch_ratio;	//将遥杆值作为飞行角度的期望值
										//pidPitch.desired = -(Remote.pitch-1500) * roll_pitch_ratio;	 
										
		

}   





//10ms调用一次
void ANO_Rc_Data(void)   //整理发送数据
{
		//ANO_Stick_Scan()会提前多次调用摇杆数据采集
		//4.1 提取摇杆原始值（滤波后）
		Rc.THR=Stick.THR;
		Rc.YAW=Stick.YAW;
		Rc.PIT=Stick.PIT;
		Rc.ROL=Stick.ROL;

	
		//4.2 按键扫描，解锁，锁定等等
		key_function();//按键扫面(顶部2个按键，以及左边6个按键)
	
	
		//零漂修正值 设置+/-125，实际 +/-250
		//4.3 漂移修正值Rol，，这个不包含摇杆自身修正，纯用户漂移修正值; 注意：  .Z_OffSet_Rol 为摇杆自身中值误差
		Rc.AUX1 = 1500 + ANO_Param.OffSet_Rol * 2 ; // 2000 - (uint16_t)(0.25f*_offset.roll);
	
		//AUX2:定高模式   在key.c更新
	
		//4.3 漂移修正值Pit;  注意：  .Z_OffSet_Pit 为摇杆自身中值误差
		Rc.AUX3 = 1500 + ANO_Param.OffSet_Pit * 2; // 2000 - (uint16_t)(0.25f*_offset.pitch);
	
	
	  //4.3 漂移修正值yaw,,AUX4高字节, 注意：不是  .Z_OffSet_Yaw （  .Z_* 为摇杆自身中值误差   ）
		Rc.AUX41 = ANO_Param.OffSet_Yaw; //AUX41上传到飞控为AUX4高字节
	
		//AUX40:按键状态	 在key.c更新，AUX40上传到飞控为AUX4低字节
	
		//4.4提取电池电压值（滤波后）
		Rc.AUX5 = Stick.AUX5; //上传电池电压
	
		//AUX6: 帧率	NRF_SSI,在scheduler.c更新
	
		//4.5 提取摇杆力度和油门系数
		AUX72_s(Show.ratio);//改由stick.c统一处理
		AUX71_s(Show.force); //改为stick.c统一处理
		//AUX73 在key.c更新
		
		//AUX8 在key.c更新


		//5 摇杆中点误差消除
		Mid_Limit(&Rc); //中点误差消除（消除中点抖动），，，自动回中摇杆在+/-10以内，认为是误差，用户并未拨动

		//对读取到的Rc进行修正,要到飞控端进行修正，要考虑无头模式
		//现在改为单纯摇杆值（含自身中值修正），漂移修正使用 Rc.AUX1 和 Rc.AUX3发送。
		//Rc.ROL += ANO_Param.OffSet_Rol; //加上用户漂移修正
		//Rc.PIT += ANO_Param.OffSet_Pit;

		//6 摇杆值限幅
		RC_Limit(&Rc);			//摇杆值和AUX1~3 输出限幅 [1000,2000],注意AUX4~6和AUX7~8并没有限幅
		
		if(AUX82_isH()) Rc.THR=1000; //油门屏蔽状态，直接输出1000
	
		//7 模拟飞控解析数据
		f_Stick_display(); //显示飞控解析后的数据，会加上漂移修正值

		
		//以下在2ms任务中
		//1.摇杆中位校准（如果用户长按key2，自身误差校准）
		//2.采集原始值（含摇杆自身误差修正）  在2ms任务中
		//3.原始值滤波  在2ms任务中

	
		//以下在10ms任务中
		//4.提取数据
		//5.中点误差消除（中点误差消除，应该放到用户漂移修正之前，不然很小的用户修正会被消除，而且依旧不能消除中点抖动）
		//6.输出限幅
		
		//7.生成显示摇杆值，与飞控那边的计算方法一致，会用到用户漂移修正值，通过 Rc.AUX1 和 Rc.AUX3传递。
		
		
		//除了向飞控传输摇杆值以及AUX1~AUX6外，
		//还有飞控设置菜单界面里面的参数，比如定高模式，无头模式等等。
		//在函数 Gesture_Check()   把Show.set_flag 传递给 set_temp，再传递给飞控，

		//Rc为上传值，Stick_display为显示值（提前解析）
		

}



