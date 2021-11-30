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
#include "ebox_mem.h"


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
    UART.print("\n");
    UART.print("*************************************************************\n");
    UART.print("* \r\n");	                /* 打印一行空格 */
    UART.print("* 例程名称      : ");	    /* 打印例程名称 */
    UART.println(name);
    UART.print("* 发布日期      : ");	    /* 打印例程日期 */
    UART.println(date);

    UART.print("* 硬件平台      : ");       /* 硬件平台 */
    UART.println(HARDWARE);
    UART.print("* EBOX库版本    : ebox_V" );/* 版本信息 */
    UART.println(EBOX_VERSION);
    UART.print("*                     CPU 信息\n");	/* CPU信息 */
    UART.println();
    UART.print("* CPU PINS      : ");	    /* 打印CPU引脚数量 */
    UART.println(cpu.pins);
    UART.print("* CPU COMPANY   : ");	    /* 打印CPU公司 */
    UART.println(cpu.company);


    UART.print("* CPUID         : ");       /* 打印CPU唯一ID */
    for(int i = 0 ; i <11; i++)
    {
        UART.printf("%02X-",cpu.chip_id[i]);
    }
    UART.printf("%02X",cpu.chip_id[11]);
    UART.printf("\n");



    UART.print("* core          : ");
    UART.print(cpu.clock.core / 1000000.0);	/* 打印时钟树信息 */
    UART.println("Mhz");
    UART.print("* hclk          : ");
    UART.print(cpu.clock.hclk / 1000000.0);
    UART.println("Mhz");
    UART.print("* pclk1         : ");
    UART.print(cpu.clock.pclk1 / 1000000.0);
    UART.println("Mhz");
    UART.print("* pclk2         : ");
    UART.print(cpu.clock.pclk2 / 1000000.0);
    UART.println("Mhz");


    UART.print("* flash size    : ");       /* 打印flash大小 */
    UART.print(cpu.flash.size/1024);
    UART.println("KB");
    
    UART.print("* flash page size    : ");  /* 打印flash page大小 */
    UART.print(cpu.flash.page_size);
    UART.println("B");


    UART.print("* flash used    : ");	    /* 打印flash使用了多少KB */
    UART.print(cpu.flash.used / 1024.0);
    UART.println("KB");

    UART.print("* flash remaind : ");	    /* 打印flash剩余了多少KB  */
    UART.print((cpu.flash.size - cpu.flash.used) / 1024.0);
    UART.println("KB");


    rt_uint32_t total,used,remained;
    
    rt_memory_info(&total,&used,&remained);
    
    UART.print("* mem size      : ");       /* 打印SRAM大小 */
    UART.print(total / 1024.0);
    UART.println("KB");

    UART.print("* heap used     : ");	    /* 打印heap使用率 */
    UART.print(used / 1024.0);
    UART.println("KB");

    UART.print("* heap free     : ");	    /* 打印heap使用了多少KB */
    UART.print(remained / 1024.0);
    UART.println("KB");

    UART.print("*\n");	                /* 打印一行空格 */
    UART.print("*************************************************************\n");
    UART.flush();
}


