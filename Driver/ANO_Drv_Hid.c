#include "ano_drv_hid.h"
#include "board.h"

u8 Hid_RxData[64];
u8 HID_SEND_TIMEOUT = 3;			//调用次数，hid发送不足一帧时，等待HID_SEND_TIMEOUT周期进行发送
u8 hid_datatemp[256];					//hid环形缓冲区
u8 hid_datatemp_begin = 0;		//环形缓冲区数据指针，指向应当发送的数据
u8 hid_datatemp_end = 0;			//环形缓冲区数据结尾

void USB_HID_Init(void)
{
	USB_GPIO_Configuration();
	USB_Interrupts_Config();
	Set_USBClock();    
	USB_Init();
	USB_ReceiveFlg = FALSE;
	USB_Connect(FALSE);//软件或者硬件重启usb
	Delay(100);
	USB_Connect(TRUE);
}

u8 no_delay=0;

void Usb_Hid_Adddata(u8 *dataToSend , u8 length)
{
	for(u8 i=0; i<length; i++)
	{
		hid_datatemp[hid_datatemp_end++] = dataToSend[i];
	}
	
	//发送校验码（特征字为0xEF）,不需要满63字节一起发送。
	if(dataToSend[2]==0xEF) no_delay=1; //不需要等满63个字节，就了立即发送
	//else no_delay=0;
	
	//飞控 最后一条发送：AA AA EF 01 00 44 可以实现立即发送，遥控遇到特征字 0xEF需要立即发送
	
}

void Usb_Hid_Send (void)
{
	static u8 notfull_timeout=0;
	
	if(hid_datatemp_end > hid_datatemp_begin)
	{
		if((hid_datatemp_end - hid_datatemp_begin) >= 63)
		{
			notfull_timeout = 0;
			Transi_Buffer[0] = 63;
			for(u8 i=0; i<63; i++)
			{
				Transi_Buffer[i+1] = hid_datatemp[hid_datatemp_begin++];
			}
			UserToPMABufferCopy(Transi_Buffer, ENDP2_TXADDR, 64);
			SetEPTxValid(ENDP2);
		}
		
		else
		{
			notfull_timeout++;
			//if(notfull_timeout == HID_SEND_TIMEOUT) //超时
			//如果是设置pid返回校验码，可以马上返回，不要等63个字节再一起发给上位机
			if(notfull_timeout == HID_SEND_TIMEOUT || no_delay==1) //超时或者发送校验码（特征字为0xEF）
			{
				notfull_timeout = 0;
				Transi_Buffer[0] = hid_datatemp_end - hid_datatemp_begin;
				for(u8 i=0; i<63; i++)
				{
					if(i<hid_datatemp_end - hid_datatemp_begin)
						Transi_Buffer[i+1] = hid_datatemp[hid_datatemp_begin+i];
					else
						Transi_Buffer[i+1] = 0;
				}
				hid_datatemp_begin = hid_datatemp_end;
				UserToPMABufferCopy(Transi_Buffer, ENDP2_TXADDR, 64);
				SetEPTxValid(ENDP2);
			}
			
			no_delay=0; //发送完毕恢复正常
		}
	}
	else if(hid_datatemp_end < hid_datatemp_begin)
	{
		if((256 - hid_datatemp_begin + hid_datatemp_end) >= 63)
		{
			notfull_timeout = 0;
			Transi_Buffer[0] = 63;
			for(u8 i=0; i<63; i++)
			{
				Transi_Buffer[i+1] = hid_datatemp[hid_datatemp_begin++];
			}
			UserToPMABufferCopy(Transi_Buffer, ENDP2_TXADDR, 64);
			SetEPTxValid(ENDP2);
		}
		else
		{
			notfull_timeout++;
			if(notfull_timeout == HID_SEND_TIMEOUT)
			{
				notfull_timeout = 0;
				Transi_Buffer[0] = 256 - hid_datatemp_begin + hid_datatemp_end;
				for(u8 i=0; i<63; i++)
				{
					if(i<256 - hid_datatemp_begin + hid_datatemp_end)
						Transi_Buffer[i+1] = hid_datatemp[(u8)(hid_datatemp_begin+i)];
					else
						Transi_Buffer[i+1] = 0;
				}
				hid_datatemp_begin = hid_datatemp_end;
				UserToPMABufferCopy(Transi_Buffer, ENDP2_TXADDR, 64);
				SetEPTxValid(ENDP2);
			}
		}
	}
}
