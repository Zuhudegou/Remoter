#include "ANO_DT.h"

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	
u8 data_to_send[50];
int16_t PLANE_YAW,PLANE_ROL,PLANE_PIT;

void ANO_DT_Send_Data(u8 *dataToSend , u8 length)
{
	ANO_NRF_TxPacket(dataToSend,length);
}

//发送遥控数据给飞控
void ANO_DT_Send_RCData(void) //由ANO_Scheduler.c 的    ANO_Loop_100Hz() 调用
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAF;
	data_to_send[_cnt++]=0x03;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Rc.THR);
	data_to_send[_cnt++]=BYTE0(Rc.THR);
	data_to_send[_cnt++]=BYTE1(Rc.YAW);
	data_to_send[_cnt++]=BYTE0(Rc.YAW);
	data_to_send[_cnt++]=BYTE1(Rc.ROL);
	data_to_send[_cnt++]=BYTE0(Rc.ROL);
	data_to_send[_cnt++]=BYTE1(Rc.PIT);
	data_to_send[_cnt++]=BYTE0(Rc.PIT);

	data_to_send[_cnt++]=BYTE1(Rc.AUX1);
	data_to_send[_cnt++]=BYTE0(Rc.AUX1);
	data_to_send[_cnt++]=BYTE1(Rc.AUX2);
	data_to_send[_cnt++]=BYTE0(Rc.AUX2);
	data_to_send[_cnt++]=BYTE1(Rc.AUX3);
	data_to_send[_cnt++]=BYTE0(Rc.AUX3);
	data_to_send[_cnt++]=Rc.AUX41;
	data_to_send[_cnt++]=Rc.AUX40;
	data_to_send[_cnt++]=BYTE1(Rc.AUX5);
	data_to_send[_cnt++]=BYTE0(Rc.AUX5);
	data_to_send[_cnt++]=BYTE1(Rc.AUX6);
	data_to_send[_cnt++]=BYTE0(Rc.AUX6);
	
	data_to_send[_cnt++]=BYTE1(Rc.AUX7);
	data_to_send[_cnt++]=BYTE0(Rc.AUX7);
	data_to_send[_cnt++]=BYTE1(Rc.AUX8);
	data_to_send[_cnt++]=BYTE0(Rc.AUX8);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	ANO_DT_Send_Data(data_to_send, _cnt);
}



//发送遥控数据给上位机, 这是未连接飞控的状态下。
//已连接飞控时，飞控会把收到的遥控值，回传回来，这时再转发回传回来的值给上位机

void ANO_DT_Send_RCData_To_Pc(void)//由ANO_Scheduler.c 的    ANO_Loop_100Hz() 调用
{
	u8 _cnt=0;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x03;
	data_to_send[_cnt++]=0;
	
	data_to_send[_cnt++]=BYTE1(Rc.THR);
	data_to_send[_cnt++]=BYTE0(Rc.THR);
	data_to_send[_cnt++]=BYTE1(Rc.YAW);
	data_to_send[_cnt++]=BYTE0(Rc.YAW);
	data_to_send[_cnt++]=BYTE1(Rc.ROL);
	data_to_send[_cnt++]=BYTE0(Rc.ROL);
	data_to_send[_cnt++]=BYTE1(Rc.PIT);
	data_to_send[_cnt++]=BYTE0(Rc.PIT);

	data_to_send[_cnt++]=BYTE1(Rc.AUX1);
	data_to_send[_cnt++]=BYTE0(Rc.AUX1);
	data_to_send[_cnt++]=BYTE1(Rc.AUX2);
	data_to_send[_cnt++]=BYTE0(Rc.AUX2);
	data_to_send[_cnt++]=BYTE1(Rc.AUX3);
	data_to_send[_cnt++]=BYTE0(Rc.AUX3);
	data_to_send[_cnt++]=Rc.AUX41;
	data_to_send[_cnt++]=Rc.AUX40;
	data_to_send[_cnt++]=BYTE1(Rc.AUX5);
	data_to_send[_cnt++]=BYTE0(Rc.AUX5);
	data_to_send[_cnt++]=BYTE1(Rc.AUX6);
	data_to_send[_cnt++]=BYTE0(Rc.AUX6);
	
	data_to_send[3] = _cnt-4;
	
	u8 sum = 0;
	for(u8 i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	Usb_Hid_Adddata(data_to_send, _cnt);
	Usb_Hid_Send();
}


//这里解析的飞机数据，仅用于屏幕显示飞控飞行数据。
//另外，解析完成后，在ANO_Scheduler.c 的 ANO_Loop_500Hz()里面，
//飞控传回的所有数据还会原封不动的转发给上位机。数据需要符合匿名上位机的协议
//当设置 NRF_Evnet=0，这条数据就不会转发给上位机
void ANO_DT_NrfData_Anl(u8 *data_buf , u8 num)    //解析飞机数据
{
	u8 sum = 0;
	
	for(u8 i=0;i<(num-1);i++)
		sum += *(data_buf+i);
	if(!(sum==*(data_buf+num-1)))		return;		//判断sum
	if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAA))		return;		//判断帧头
	
	

		//如果本次收到的数据不需要上传给上位机，则 NRF_Evnet = 0;
		u8 byte_feature  = *(data_buf+2); //功能字 
	
		//0x10, 0x11, 0x12, 0x13, 0xEF	pid相关

	if(//不在这里的都会上传给上位机, 方便使用上位机调试，
				 //注意如果屏蔽了日常数据上传，要注意usb要满63个字节才给上位机发送一次，
	       //请飞控最后发送一条 6字节的数据，AA AA EF 01 00 44 //遥控遇到特征字 0xEF需要立即发送
					  byte_feature==0x09 			// 屏幕调试数据（详细数据第二页）
				 //|| byte_feature==0x01 ||  byte_feature==0x02   //状态，加速度
				 //|| byte_feature==0x03 ||  byte_feature==0x05  //返回收到的遥控值，电压信号强度等
				 //|| byte_feature==0x07  //气压激光高度等，
				 //|| byte_feature==0x0B ||  byte_feature==0xf1 //光流模块调试数据, 用户数据（帧1）
				){ 
			
					NRF_Evnet=0; //不上传给上位机
		}

	
	
	
	
		if(*(data_buf+2)==0x01)//读取姿态角
		{
			//飞控发过来时，*100，1表示0.01度
			Show.X = ( (int16_t)(*(data_buf+4)<<8)|*(data_buf+5) )*0.1; //飞控发送过来时故意反向了，已经调整
			Show.Y = ( (int16_t)(*(data_buf+6)<<8)|*(data_buf+7) )*0.1;
			Show.Z = ( (int16_t)(*(data_buf+8)<<8)|*(data_buf+9) )*0.1;
			//Show.H = ( (int32_t)(*(data_buf+10)<<24)|(*(data_buf+11)<<16)|(*(data_buf+12)<<8)|*(data_buf+13) )*0.01;
			Show.H = ( (int32_t)(*(data_buf+10)<<24)|(*(data_buf+11)<<16)|(*(data_buf+12)<<8)|*(data_buf+13) )*1.0;//接收时cm，显示时按照大小切换单位
		
			u8 _unlock=*(data_buf+15); //已解锁消息

			//飞机从解锁状态变更为 紧急锁定，要恢复之前保存的定高/姿态模式
			if(Show.unLock==1 && _unlock==0){
						if(Show.position_old==1){
									Rc.AUX2=1000;
						}
						else{
									Rc.AUX2=2000;
						}
			}
			
			Show.unLock = _unlock; //已解锁消息

		}
		
		
		const float Gyro_G = 0.03051756f*2;	  	//陀螺仪初始化量程+-2000度每秒于1 / (65536 / 4000) = 0.03051756*2		
		
		
		if(*(data_buf+2)==0x02)//读取陀螺仪数据
		{
			Show.yaw_Gs = ( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) )* Gyro_G;   //yaw角速度，不是yaw姿态角
		
		}
		
		
		
		
		
		if(*(data_buf+2)==0x05)//读取飞控电压//传感器状态//调试数据等等等
		{
			Show.Battery_Fly = ( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );   //飞机电压
			//Show.Battery_Rc  = ( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
			Show.hardware_type = *(data_buf+8);//硬件类型 1、四轴 2、万向轮
			Show.Rc_num = *(data_buf+9);  
			Show.test_flag = ( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );  //传感器状态

			
			//下传飞控设置参数，定高模式(0)，抛飞模式(1)，无头模式(2)，切换机头(3)，
		  //                 视觉定位(4)，光流定点(5)
			//悬停 0x0021, 姿态 0x0000
			Show.set_flag  = ( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );  //飞行模式设置
			
			//Show.debug_flag = 
			Show.qual = *(data_buf+14); //光流质量

			//发送的数据set_temp，接收的数据Show.set_flag
			//如果一致了，就不需要继续发送 set_temp 了
			if(Show.set_flag==set_temp) send_flag = 0; 
		}
		
		
		
		if(*(data_buf+2)==0x07)//读取飞控发来的高度，激光高度+气压高度
		{

			Show.Control_high = ( (s16)((*(data_buf+8)<<8)|*(data_buf+9)) );   //
			Show.high_desired = ( (s16)((*(data_buf+10)<<8)|*(data_buf+11)) );   //

			//发送的数据set_temp，接收的数据Show.set_flag
			//如果一致了，就不需要继续发送 set_temp 了
			//if(Show.set_flag==set_temp) send_flag = 0; 
			
			
		}
		
		
		
		
		if(*(data_buf+2)==0x09)//读取飞控发来的调试信息
		{

			Show.debug1 = ( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );   //
			Show.debug2 = ( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );   //	
			Show.debug3 = ( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );   //
			Show.debug4 = ( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );   //		
			Show.debug5 = ( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );   //
			Show.debug6 = ( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );   //	pixel_flow.err1_cnt,pixel_flow.err2_cnt
			
			Show.roll_desired = ( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );   //
		  Show.pitch_desired = ( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );   //	
			
			
			
			//发送的数据set_temp，接收的数据Show.set_flag
			//如果一致了，就不需要继续发送 set_temp 了
			if(Show.set_flag==set_temp) send_flag = 0; 
		}
		
		
		
		
		if(*(data_buf+2)==0x0B)//读取飞控发来的光流信息
		{

			Show.loc_x = ( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );   //
			Show.loc_y = ( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );   //	

			//光流pid外环期望值
			Show.x_desired = ( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );   //
			Show.y_desired = ( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );   //	
			
			//光流pid内环输出值
			Show.x_out = ( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );   //
			Show.y_out = ( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );   //	


			//发送的数据set_temp，接收的数据Show.set_flag
			//如果一致了，就不需要继续发送 set_temp 了
			if(Show.set_flag==set_temp) send_flag = 0; 
		}
		
						
		
		
		
}

void ANO_DT_Send_Flag_To_Fly(u16 flag0, u16 flag1)
{
	u8 _cnt=0,i;
	u8 sum = 0;
	u16 temp;
	
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAF;
	data_to_send[_cnt++]=0xF1;
	data_to_send[_cnt++]=0;
	
	temp = flag0;
	data_to_send[_cnt++]=BYTE1(temp);
	data_to_send[_cnt++]=BYTE0(temp);
	temp = flag1;
	data_to_send[_cnt++]=BYTE1(temp);
	data_to_send[_cnt++]=BYTE0(temp);
	
	data_to_send[3] = _cnt-4;
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
	ANO_DT_Send_Data(data_to_send, 32);
}
