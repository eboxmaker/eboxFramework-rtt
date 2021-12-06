/**
  ******************************************************************************
  * @file    core.cpp
  * @author  shentq
  * @version V2.1
  * @date    2016/08/14
  * @brief
  		2019/9/28  移除无用代码,delay函数参数uint64_t改为uint32_t		LQM
                   修改systemtick中断回调函数，完善注释
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

#include "ebox_core.h"
#include "mcu.h"


extern "C" {
__IO uint32_t milli_seconds;//提供一个mills()等效的全局变量。降低cpu调用开销
__IO uint16_t micro_para;


/**
  *@brief    保持空，频率由system_stm32f10x.c中定义决定
  *@param    mcu
  *@retval   none
  */
__weak void SystemClock_Config()
{

}

/**
  *@brief    初始化，设置时钟，systemtick，systemtick callback，禁用jtag
  *@param    mcu
  *@retval   none
  */
void ebox_init(void)
{
    SystemClock_Config();
    
    // update 时钟信息
    SystemCoreClockUpdate();
    CpuClock_t clock;
    get_system_clock(&clock);

    // 配置systemtick
    SysTick_Config(clock.core / 1000); //  每隔 1ms产生一次中断
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//systemticks clock；
    micro_para = clock.core / 1000000; //减少micros函数计算量

    //将pb4默认设置为IO口，禁用jtag
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_GROUP_CONFIG);
    
    attachSystickCallBack(nullFun,1);
    #ifdef RT_USING_CONSOLE
    
    #endif
}

void ebox_reset(void)
{
    NVIC_SystemReset();
}

/**
  *@brief    获取us
  *@param    mcu
  *@retval   none
  */
uint32_t micros(void)
{
    uint32_t micro;
    micro = (milli_seconds * 1000 + (1000 - (SysTick->VAL) / (micro_para)));
    return  micro;
}

/**
  *@brief    获取ms
  *@param    mcu
  *@retval   none
  */
uint32_t millis( void )
{
    return milli_seconds;
}

/**
  *@brief    ms延时
  *@param    uint32_t ms  要延时的时长，最小1ms
  *@retval   none
 */
void delay_ms(uint32_t ms)
{
    uint32_t end ;
    end = micros() + ms * 1000 ;
    while (micros() < end);
}
/**
  *@brief    us延时,使用systick计数器。48Mhz及以上时钟时可以满足us(1.3)精度。
  *          8Mhz时最小6-7us,24Mhz时最小2.2us,16Mhz时最小3.5us
  *@param    uint32_t us  要延时的时长，最小1us
  *@retval   none
 */
void  delay_us(uint32_t us)
{
    if(us == 0) return;
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;

    ticks = (us - 1) * micro_para;          /* 计数周期 */
    tcnt = 0;
    told = SysTick->VAL;               /* 保存当前计数值 */

    while (1)
    {
        tnow = SysTick->VAL;
        tcnt += (tnow < told) ? (told - tnow) : (SysTick->LOAD - tnow + told);
        if (tcnt >= ticks)break;
        told = tnow;
    }
}

// systick 中断回调函数指针，
fun_noPara_t  callBackFun;
static uint16_t _multiple = 1;

/**
*@brief    注册中断回调函数，可以指定调用周期 = 中断周期 * multiple
*@param    SystickCallBack_T fun 回调函数（无参）, uint8_t multiple 倍数。用来设定调用周期
*@retval   E_OK  注册成功， E_NG 注册失败
*/
uint16_t attachSystickCallBack(fun_noPara_t fun, uint16_t multiple)
{

    if (callBackFun == NULL || callBackFun == nullFun)
    {
        callBackFun = fun;
        _multiple = ( multiple == 0 ) ? 1 : multiple;
        return EOK;
    }
    else
    {
        return ENG;
    }
}

/**
 *@brief    systick中断处理函数
 *@param    none
 *@retval   none
*/
void SysTick_Handler(void)
{
    if (milli_seconds++ % _multiple == 0)
    {
        callBackFun();
    }
}

/**
 *@brief    获取系统时钟
 *@param    *clock：  时钟指针，返回系统时钟
 *@retval   none
*/
 void get_system_clock(CpuClock_t *clock)
{
    RCC_ClocksTypeDef RCC_ClocksStatus;

    SystemCoreClockUpdate();
    RCC_GetClocksFreq(&RCC_ClocksStatus);

    clock->core = RCC_ClocksStatus.SYSCLK_Frequency;
    clock->hclk = RCC_ClocksStatus.HCLK_Frequency;
    clock->pclk2 = RCC_ClocksStatus.PCLK2_Frequency;
    clock->pclk1 = RCC_ClocksStatus.PCLK1_Frequency;
}

/**
 *@brief    获取系统信息，CPU_ID,flashsize
 *@param    none
 *@retval   none
*/
 void get_chip_info(Cpu_t *cpu)
{
    uint8_t *p = (uint8_t *)(0X1FFFF7E8);
    for(int i = 0 ; i < 12; i++)
    {
        cpu->chip_id[i] = *p++;
    }


    cpu->flash.size = *(uint16_t *)(0x1FFFF7E0);   //芯片flash容量
    switch(cpu->flash.size)
    {
        case 32:
        case 64:
        case 128:
            cpu->flash.page_size = 1024;
            break;
        default:
            cpu->flash.page_size = 2048;
            break;
    }
    cpu->flash.size = cpu->flash.size * 1024;
    cpu->flash.start = MCU_FLASH_BEGIN;
    cpu->flash.used = MCU_FLASH_USED;
    SysTick->VAL = SysTick->LOAD;
}
}
