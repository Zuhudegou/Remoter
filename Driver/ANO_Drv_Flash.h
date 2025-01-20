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
//#define EEPROM_START_ADDRESS    ((uint32_t)0x08010000) /* EEPROM emulation start address: after 64KByte of used Flash memory ,注意，如果实际真的只有64K，这个地址已经没法读取了*/
#define EEPROM_START_ADDRESS    ((uint32_t)0x0800F400) 	//要编写的起始地址   位于61K字节处, 芯片flash一共64K字节
#define PARAMFLASH_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
#define PARAMFLASH_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1))) //结束位于62K字节处，1K字节（1个扇区）

//芯片flash开始地址 0x08000000
//芯片flash结束地址 0x0800FFFF  //大小64K, 即0x10000(65536)/0x400(1024)
//芯片flash结束地址 0x0801FFFF  //大小128K,即0x20000(131072)/0x400(1024)
//stm32f103c8t6芯片规格书为64k，但有的芯片实际有128k falsh



u8 ANO_Flash_Write(u8 *addr, u16 len);
u8 ANO_Flash_Read(u8 *addr, u16 len);

#endif
