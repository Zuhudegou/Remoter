#ifndef __ANO_DRV_FLASH_H__
#define __ANO_DRV_FLASH_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported constants --------------------------------------------------------*/
/* Define the STM32F10Xxx Flash page size depending on the used STM32 device */
#if defined (STM32F10X_LD) || defined (STM32F10X_MD)
  #define PAGE_SIZE  (uint16_t)0x400  /* Page size = 1KByte */
#elif defined (STM32F10X_HD) || defined (STM32F10X_CL)
  #define PAGE_SIZE  (uint16_t)0x800  /* Page size = 2KByte */
#endif
/* EEPROM start address in Flash */
//#define EEPROM_START_ADDRESS    ((uint32_t)0x08010000) /* EEPROM emulation start address: after 64KByte of used Flash memory ,ע�⣬���ʵ�����ֻ��64K�������ַ�Ѿ�û����ȡ��*/
#define EEPROM_START_ADDRESS    ((uint32_t)0x0800F400) 	//Ҫ��д����ʼ��ַ   λ��61K�ֽڴ�, оƬflashһ��64K�ֽ�
#define PARAMFLASH_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
#define PARAMFLASH_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1))) //����λ��62K�ֽڴ���1K�ֽڣ�1��������

//оƬflash��ʼ��ַ 0x08000000
//оƬflash������ַ 0x0800FFFF  //��С64K, ��0x10000(65536)/0x400(1024)
//оƬflash������ַ 0x0801FFFF  //��С128K,��0x20000(131072)/0x400(1024)
//stm32f103c8t6оƬ�����Ϊ64k�����е�оƬʵ����128k falsh



u8 ANO_Flash_Write(u8 *addr, u16 len);
u8 ANO_Flash_Read(u8 *addr, u16 len);

#endif
