#ifndef __ANO_PARAM_H
#define __ANO_PARAM_H

#include "stm32f10x.h"
#include "ANO_Drv_Flash.h"

struct param{
	uint8_t  NRF_Channel;
	uint8_t  OffSet_En;//需要立即校准摇杆
	
	uint16_t OffSet_Rol;//用户设置漂移修正(含摇杆自身中值误差修正) +/-128，Rc.AUX1范围 +/-256
	uint16_t OffSet_Pit;//用户设置漂移修正(含摇杆自身中值误差修正) +/-128，Rc.AUX3范围 +/-256
	
	//uint16_t OffSet_Thr;// 取消用户设置油门偏移
	uint16_t OffSet_Yaw;// 用户设置偏航角偏移
	
	uint16_t FirstInitFlag;
	
	uint16_t Z_OffSet_Rol;//摇杆自身中值修正
	uint16_t Z_OffSet_Pit;//摇杆自身中值修正
	
	uint16_t Z_OffSet_Thr;//摇杆自身中值修正，用户无法修改，通过摇杆校准获得
	uint16_t Z_OffSet_Yaw;//摇杆自身中值修正
	
	
	
	//增加力度记忆,保存起来,转移到ANO_Param，不用保存
	//u8 force; //力度   1，2，3，0, 0表示实时，最大力度,,显示值为  8-ANO_Param.force
	
	//增加保存flag, 退出详细参数界面时保存。进入主菜单，如果flag为1，会显示"保存"
	u8 SaveFlag; //力度   1，2，3，0, 0表示实时，最大力度
	
};
extern struct param ANO_Param;

void ANO_Param_READ(void);
void ANO_Param_SAVE(void);

#endif

