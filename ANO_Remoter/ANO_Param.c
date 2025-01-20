#include "ANO_Param.h"
#include "ANO_Stick.h"
#include "ANO_Drv_MPU6050.h"

#define FIRSTINITFLAH  			0x44 //68

struct param ANO_Param;

void ANO_Param_Init(void)//恢复默认参数
{
	ANO_Param.NRF_Channel= 0xC8;  //新单片机未初始化读出为 0xff, 改为0xC8(200) //信道有效范围 0~125，一共126个
	ANO_Param.OffSet_En  = 0; 
	ANO_Param.OffSet_Rol = 0; //用户漂移修正
	ANO_Param.OffSet_Pit = 0; //用户漂移修正
	
	ANO_Param.OffSet_Yaw = 0; //用户漂移修正
	//ANO_Param.OffSet_Thr = 0;//已取消
	
	//遥感自身修正值也要复位，新单片机读取flash值是随机的或者是0xff
	ANO_Param.Z_OffSet_Rol=0;//摇杆自身中值修正
	ANO_Param.Z_OffSet_Pit=0;//摇杆自身中值修正
	
	ANO_Param.Z_OffSet_Thr=0;//摇杆自身中值修正，用户无法修改，通过摇杆校准获得
	ANO_Param.Z_OffSet_Yaw=0;//摇杆自身中值修正
	
	
	
	//ANO_Param.force = 5; //  右摇杆：默认力度8-5=3级，有保存到eeprom，如果不想被重设，可以注释这句。
	
	//ANO_Param.ratio = 2; //	左摇杆：默认油门比例：2+1=3级

	
	
	ANO_Param.FirstInitFlag = FIRSTINITFLAH;
	
	ANO_Param_SAVE();						//存储数据
}


//Flash存储功能   1.恢复默认值，2.摇杆校准，3.对频时，4.退出详细数据界面（如果右更改力度和漂移修正）
void ANO_Param_SAVE(void)   //存储数据
{
	ANO_Flash_Write((u8 *)(&ANO_Param),sizeof(ANO_Param));  //写进Flash
}


//只是上电时读取一下，用于初始化一些用户参数
void ANO_Param_READ(void)
{
		
	ANO_Flash_Read((u8 *)(&ANO_Param),sizeof(ANO_Param));   //上电读出存储数据
	
	//ANO_Param.FirstInitFlag = 0; //每次开机都强制复位零漂，信道等参数
	
	if(ANO_Param.FirstInitFlag != FIRSTINITFLAH)//板子从未初始化
	{
		ANO_Param_Init();  //复位数据
	}
	
	//ANO_Param.force = 5; //  右摇杆：默认力度8-5=3级，有保存到eeprom，如果不想被重设，可以注释这句。
	//油门比例，不要用保存值，每次开机恢复默认值
	Show.ratio = 2; //	左摇杆：上电时默认油门比例：2+1=3级
	Show.force = 5; //  右摇杆：默认力度8-5=3级，
	
	ANO_Param.OffSet_Yaw =0; //复位偏航角偏移，如果需要保存断电保存偏航角偏移修正，把这句注释，并且 在 key.c 764/858行，  ANO_Param.SaveFlag=1;			//显示待保存
	
	ANO_Param.SaveFlag=0; //默认为0，防止开机被写成1
}










