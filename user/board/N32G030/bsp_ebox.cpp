/**
  ******************************************************************************
  * @file    bsp_ebox.cpp
  * @author  cat_li
  * @version V1.0
  * @date    2018/07/31
  * @brief   硬件相关信息声明
  ******************************************************************************
  * @attention
  *
  * No part of this software may be used for any commercial activities by any form
  * or means, without the prior written consent of shentq. This specification is
  * preliminary and is subject to change at any time without notice. shentq assumes
  * no responsibility for any errors contained herein.
  * <h2><center>&copy; Copyright 2015 shentq. All Rights Reserved.</center></h2>
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "bsp_ebox.h"
















extern "C" {

    void console_uart_init()
    {
        ConsoleUart.begin(115200);
    }
    void console_uart_putc(char ch)
    {
        ConsoleUart.write(ch);
    }
    void console_uart_write(const char *p,uint8_t len)
    {
        ConsoleUart.write(p,len);
        ConsoleUart.flush();
    }
}
/*
*********************************************************************************************************
*	函 数 名: PrintfLog
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void print_log(const char *name, const char *date)
{
    Cpu_t cpu;
    CpuClock_t clock;
    get_chip_info(&cpu);

    ConsoleUart.print("\n");
    ConsoleUart.print("*************************************************************\n");
    ConsoleUart.print("* \r\n");	                /* 打印一行空格 */
    ConsoleUart.print("* 例程名称      : ");	    /* 打印例程名称 */
    ConsoleUart.println(name);
    ConsoleUart.print("* 发布日期      : ");	    /* 打印例程日期 */
    ConsoleUart.println(date);

    ConsoleUart.print("* 硬件平台      : ");       /* 硬件平台 */
    ConsoleUart.println(HARDWARE);
    ConsoleUart.print("* EBOX库版本    : ebox_V" );/* 版本信息 */
    ConsoleUart.println(EBOX_VERSION);
    ConsoleUart.print("*                     CPU 信息\n");	/* CPU信息 */
    ConsoleUart.println();
    ConsoleUart.print("* CPU COMPANY   : ");	    /* 打印CPU公司 */
    ConsoleUart.println(cpu.company);


    ConsoleUart.print("* CPUID         : ");       /* 打印CPU唯一ID */
    for(int i = 0 ; i <11; i++)
    {
        ConsoleUart.printf("%02X-",cpu.chip_id[i]);
    }
    ConsoleUart.printf("%02X",cpu.chip_id[11]);
    ConsoleUart.printf("\n");



    ConsoleUart.print("* core          : ");
    ConsoleUart.print(clock.core / 1000000.0);	/* 打印时钟树信息 */
    ConsoleUart.println("Mhz");
    ConsoleUart.print("* hclk          : ");
    ConsoleUart.print(clock.hclk / 1000000.0);
    ConsoleUart.println("Mhz");
    ConsoleUart.print("* pclk1         : ");
    ConsoleUart.print(clock.pclk1 / 1000000.0);
    ConsoleUart.println("Mhz");
    ConsoleUart.print("* pclk2         : ");
    ConsoleUart.print(clock.pclk2 / 1000000.0);
    ConsoleUart.println("Mhz");


    ConsoleUart.print("* flash size    : ");       /* 打印flash大小 */
    ConsoleUart.print(cpu.flash.size/1024);
    ConsoleUart.println("KB");
    
    ConsoleUart.print("* flash page size    : ");  /* 打印flash page大小 */
    ConsoleUart.print(cpu.flash.page_size);
    ConsoleUart.println("B");


    ConsoleUart.print("* flash used    : ");	    /* 打印flash使用了多少KB */
    ConsoleUart.print(cpu.flash.used / 1024.0);
    ConsoleUart.println("KB");

    ConsoleUart.print("* flash remaind : ");	    /* 打印flash剩余了多少KB  */
    ConsoleUart.print((cpu.flash.size - cpu.flash.used) / 1024.0);
    ConsoleUart.println("KB");


    rt_uint32_t total,used,remained;
    
    rt_memory_info(&total,&used,&remained);
    
    ConsoleUart.print("* mem used      : ");       /* 打印SRAM大小 */
    ConsoleUart.print(MCU_SRAM1_USED / 1024.0);
    ConsoleUart.println("KB");

    
    
    ConsoleUart.print("* heap size      : ");       /* 打印SRAM大小 */
    ConsoleUart.print(total / 1024.0);
    ConsoleUart.println("KB");

    ConsoleUart.print("* heap used     : ");	    /* 打印heap使用率 */
    ConsoleUart.print(used / 1024.0);
    ConsoleUart.println("KB");

    ConsoleUart.print("* heap free     : ");	    /* 打印heap使用了多少KB */
    ConsoleUart.print(remained / 1024.0);
    ConsoleUart.println("KB");

    ConsoleUart.print("*\n");	                /* 打印一行空格 */
    ConsoleUart.print("*************************************************************\n");
    ConsoleUart.flush();
}


