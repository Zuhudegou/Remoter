#include "ANO_Scheduler.h"
#include "sysconfig.h"

uint16_t S_cnt_2ms=0,S_cnt_4ms=0,S_cnt_10ms=0,S_cnt_20ms=0,S_cnt_50ms=0,S_cnt_1000ms=0;

uint16_t S_cnt; //新增秒计数，方便测试系统时钟是否准确，在ANO_Loop()中累加

//2ms一次
void Loop_check(void)
{
	S_cnt_2ms++;
	S_cnt_4ms++;
	S_cnt_10ms++;
	S_cnt_20ms++;
	S_cnt_50ms++;
	S_cnt_1000ms++;
	
	ANO_Loop();
}

/*
1ms   1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
2ms    . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
4ms      .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .
10ms           .         .         .         .         .         .         .         .         .         .
20ms                     .                   .                   .                   .                   . 
50ms                                                   .                                                 .
1000ms                                                                                                    

2ms: 射频接收，usb转发飞控数据，摇杆读取(刷新Rc结构体)
4ms: usb接收，转发上位机数据给飞控 | 发送遥控数据给飞控
10ms: 按键检测(顶部2个功能键)，屏幕飞控设置检测
20ms：按键扫描(不含顶部功能键)
50ms：发送设置数据到飞机（屏幕设置）
100ms：电压检测，判断射频连接状态
*/



static void ANO_Loop_500Hz(void)	//2ms执行一次
{	
	ANO_NRF_Check_Event();//检查并接收飞机数据，同时调用解析程序，用于屏幕显示飞控参数，在这里会更改NRF_Evnet，1表示，数据需要转发给上位机
	if(NRF_Evnet)//如果收到飞控数据还要把它原封不动的通过usb端口转发给上位机
	{
		Usb_Hid_Adddata(NRF24L01_2_RXDATA , RX_LEN); //发送给上位机的不用解析，只是转发
		Usb_Hid_Send();   //数据发到上位机
		NRF_Evnet = 0; //数据已经发送给上位机
	}
	
	ANO_Stick_Scan();  //遥杆等数据 采集以及滤波
	
}

static void ANO_Loop_250Hz(void)	//4ms执行一次
{
	if (USB_ReceiveFlg == TRUE)  //如果收到上位机数据优先把上位机数据发送到飞机，否则发送遥控数据到飞机
	{
		if(Hid_RxData[0] < 33)
			ANO_NRF_TxPacket(&(Hid_RxData[1]),Hid_RxData[0]);
		
		USB_ReceiveFlg = 0x00;	
	}
	else
	{
		//ANO_DT_Send_RCData();  //发送摇杆等控制数据给飞机，转移到10ms任务，放到准备发送数据后面
	}
}


//10ms执行一次
static void ANO_Loop_100Hz(void)	//10ms执行一次
{
	

	//key_function();			//按键扫面(顶部2个按键，以及左边6个按键),移动到 ANO_Rc_Data()函数里面
	ANO_Rc_Data();		//准备发送数据 摇杆以及 Aux1~Aux8
	
	ANO_DT_Send_RCData();  //发送摇杆等控制数据给飞机
	
	Gesture_Check();		//显示屏摇杆上下选择操作检测(屏幕飞控设置检测)，也包括key1检测
	SysTick_count++; 		 //10ms一次的计数器，并不是1ms一次；在按键扫描用到
	
	if(!Show.Connect_Succeed)//如果与飞机连接失败就发送遥控数据到电脑
	{
		ANO_DT_Send_RCData_To_Pc();
	}
}

static void ANO_Loop_50Hz(void)	//20ms执行一次
{
	//ANO_key();				//用key_scan()替代
	key_scan();				//按键检测(4个微调按键)
}

static void ANO_Loop_20Hz(void)	//50ms执行一次
{
	Show.oled_delay = 1;//屏幕刷新标识符
	NRF_Check_Ch();//自动对频检测

	if(send_flag)//发送设置数据到飞机（飞控设置菜单里面的参数）
	ANO_DT_Send_Flag_To_Fly(set_temp,0); //set_temp来自show.c的Show.set_flag
}

//增加一个100ms一次的，给i2c的屏幕用


static void ANO_Loop_1Hz(void) // 1000ms执行一次
{
	//更新电压值
	Show.Battery_Rc = Rc.AUX5;
	if(Show.Battery_Rc<345 && Show.Battery_Rc>320)  //判断电压提示电池电量低
		Show.low_power = 1;
	else                    
		Show.low_power = 0;
	
	//计算收到飞机数据的帧率
	NRF_SSI = NRF_SSI_CNT;
	Rc.AUX6 = NRF_SSI;
	NRF_SSI_CNT = 0;
	
	/*如果帧率为0标记为失联状态*/
	if(NRF_SSI==0)
	{
		ANO_LED_blue_OFF;
		Show.Rc_num = 0;
		Show.Connect_Succeed = 0;
		Show.hardware_type = 0;
		Show.test_flag = 0;
		if(Show.windows==2 || Show.windows==3 ) Show.windows = 0;	//与飞控失联后自动回到主界面
	}
	else
	{
		ANO_LED_blue_ON;
		Show.Connect_Succeed = 1;
	}
	
	

	
}

void ANO_Loop(void)
{
	if(S_cnt_2ms >= 1){
		ANO_Loop_500Hz();
		S_cnt_2ms = 0;
	}		
	if(S_cnt_4ms >= 2){	
		ANO_Loop_250Hz();
		S_cnt_4ms = 0;
	}
	if(S_cnt_10ms >= 5){		
		ANO_Loop_100Hz();
		S_cnt_10ms = 0;
	}
	if(S_cnt_20ms >= 10){		
		ANO_Loop_50Hz();
		S_cnt_20ms = 0;
	}	
	if(S_cnt_50ms >= 25){		
		ANO_Loop_20Hz();
		S_cnt_50ms = 0;
	}	
	if(S_cnt_1000ms >= 500){		
		S_cnt++; //新增秒计数
		ANO_Loop_1Hz();
		S_cnt_1000ms = 0;
	}
}
/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/
