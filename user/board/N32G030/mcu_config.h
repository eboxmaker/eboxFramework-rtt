#ifndef __MCU_CONFIG_H
#define __MCU_CONFIG_H


#include "stdint.h"
#include "ebox_type.h"

//用户配置区域
//请查阅ebox_cpu_type.h寻找特定信号CPU的详细配置


#define HSE_VALUE    ((uint32_t)8000000) 


#define MCU_SRAM1_BEGIN  0x20000000
#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define MCU_SRAM1_USER_END 	((uint32_t)&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#else
extern int __bss_end;
#endif
#define MCU_SRAM1_USED       (MCU_SRAM1_USER_END - MCU_SRAM1_BEGIN)



//FLASH 区域定义
#ifdef __CC_ARM
extern int SHT$$INIT_ARRAY$$Limit;
#define MCU_FLASH_PRG_END 	((uint32_t)&SHT$$INIT_ARRAY$$Limit)
#endif

#define MCU_FLASH_BEGIN         0x8000000
#define MCU_FLASH_USED          (MCU_FLASH_PRG_END - MCU_FLASH_BEGIN)
//--------------------------------------------------------



#include "n32g030.h"




#endif
