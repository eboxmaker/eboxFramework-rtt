/**
  ******************************************************************************
  * @file    bsp_ebox.cpp
  * @author  cat_li
  * @version V1.0
  * @date    2018/07/31
  * @brief   Ӳ�������Ϣ����
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
*	�� �� ��: PrintfLog
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void print_log(const char *name, const char *date)
{
    UART.print("\n");
    UART.print("*************************************************************\n");
    UART.print("* \r\n");	                /* ��ӡһ�пո� */
    UART.print("* ��������      : ");	    /* ��ӡ�������� */
    UART.println(name);
    UART.print("* ��������      : ");	    /* ��ӡ�������� */
    UART.println(date);

    UART.print("* Ӳ��ƽ̨      : ");       /* Ӳ��ƽ̨ */
    UART.println(HARDWARE);
    UART.print("* EBOX��汾    : ebox_V" );/* �汾��Ϣ */
    UART.println(EBOX_VERSION);
    UART.print("*                     CPU ��Ϣ\n");	/* CPU��Ϣ */
    UART.println();
    UART.print("* CPU PINS      : ");	    /* ��ӡCPU�������� */
    UART.println(cpu.pins);
    UART.print("* CPU COMPANY   : ");	    /* ��ӡCPU��˾ */
    UART.println(cpu.company);


    UART.print("* CPUID         : ");       /* ��ӡCPUΨһID */
    for(int i = 0 ; i <11; i++)
    {
        UART.printf("%02X-",cpu.chip_id[i]);
    }
    UART.printf("%02X",cpu.chip_id[11]);
    UART.printf("\n");



    UART.print("* core          : ");
    UART.print(cpu.clock.core / 1000000.0);	/* ��ӡʱ������Ϣ */
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


    UART.print("* flash size    : ");       /* ��ӡflash��С */
    UART.print(cpu.flash.size/1024);
    UART.println("KB");
    
    UART.print("* flash page size    : ");  /* ��ӡflash page��С */
    UART.print(cpu.flash.page_size);
    UART.println("B");


    UART.print("* flash used    : ");	    /* ��ӡflashʹ���˶���KB */
    UART.print(cpu.flash.used / 1024.0);
    UART.println("KB");

    UART.print("* flash remaind : ");	    /* ��ӡflashʣ���˶���KB  */
    UART.print((cpu.flash.size - cpu.flash.used) / 1024.0);
    UART.println("KB");


    rt_uint32_t total,used,remained;
    
    rt_memory_info(&total,&used,&remained);
    
    UART.print("* mem size      : ");       /* ��ӡSRAM��С */
    UART.print(total / 1024.0);
    UART.println("KB");

    UART.print("* heap used     : ");	    /* ��ӡheapʹ���� */
    UART.print(used / 1024.0);
    UART.println("KB");

    UART.print("* heap free     : ");	    /* ��ӡheapʹ���˶���KB */
    UART.print(remained / 1024.0);
    UART.println("KB");

    UART.print("*\n");	                /* ��ӡһ�пո� */
    UART.print("*************************************************************\n");
    UART.flush();
}


