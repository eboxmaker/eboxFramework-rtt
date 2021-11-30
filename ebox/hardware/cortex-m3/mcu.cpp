/**
  ******************************************************************************
  * @file    core.cpp
  * @author  shentq
  * @version V2.1
  * @date    2016/08/14
  * @brief
  		2019/9/28  �Ƴ����ô���,delay��������uint64_t��Ϊuint32_t		LQM
                   �޸�systemtick�жϻص�����������ע��
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

#define systick_no_interrupt()  SysTick->CTRL &=0xfffffffd
#define systick_interrupt()     SysTick->CTRL |=0x0002

extern "C" {
__IO uint32_t milli_seconds;//�ṩһ��mills()��Ч��ȫ�ֱ���������cpu���ÿ���
__IO uint16_t micro_para;

static void update_system_clock(CpuClock_t *clock);
static void update_chip_info(void);

/**
  *@brief    ���ֿգ�Ƶ����system_stm32f10x.c�ж������
  *@param    mcu
  *@retval   none
  */
__weak void SystemClock_Config()
{

}

/**
  *@brief    ��ʼ��������ʱ�ӣ�systemtick��systemtick callback������jtag
  *@param    mcu
  *@retval   none
  */
void ebox_init(void)
{
    SystemClock_Config();
    
    // update ʱ����Ϣ
    update_system_clock(&cpu.clock);

    // ����systemtick
    SysTick_Config(cpu.clock.core / 1000); //  ÿ�� 1ms����һ���ж�
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//systemticks clock��
    micro_para = cpu.clock.core / 1000000; //����micros����������

    //��pb4Ĭ������ΪIO�ڣ�����jtag
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_GROUP_CONFIG);
    
    attachSystickCallBack(nullFun,1);
    update_chip_info();
    #if RT_USING_CONSOLE
    
    #endif
}

void ebox_reset(void)
{
    NVIC_SystemReset();
}

/**
  *@brief    ��ȡus
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
  *@brief    ��ȡms
  *@param    mcu
  *@retval   none
  */
uint32_t millis( void )
{
    return milli_seconds;
}

/**
  *@brief    ms��ʱ
  *@param    uint32_t ms  Ҫ��ʱ��ʱ������С1ms
  *@retval   none
 */
void delay_ms(uint32_t ms)
{
    uint32_t end ;
    end = micros() + ms * 1000 ;
    while (micros() < end);
}
/**
  *@brief    us��ʱ,ʹ��systick��������48Mhz������ʱ��ʱ��������us(1.3)���ȡ�
  *          8Mhzʱ��С6-7us,24Mhzʱ��С2.2us,16Mhzʱ��С3.5us
  *@param    uint32_t us  Ҫ��ʱ��ʱ������С1us
  *@retval   none
 */
void  delay_us(uint32_t us)
{
    if(us == 0) return;
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;

    ticks = (us - 1) * micro_para;          /* �������� */
    tcnt = 0;
    told = SysTick->VAL;               /* ���浱ǰ����ֵ */

    while (1)
    {
        tnow = SysTick->VAL;
        tcnt += (tnow < told) ? (told - tnow) : (SysTick->LOAD - tnow + told);
        if (tcnt >= ticks)break;
        told = tnow;
    }
}

// systick �жϻص�����ָ�룬
fun_noPara_t  callBackFun;
static uint16_t _multiple = 1;

/**
*@brief    ע���жϻص�����������ָ���������� = �ж����� * multiple
*@param    SystickCallBack_T fun �ص��������޲Σ�, uint8_t multiple �����������趨��������
*@retval   E_OK  ע��ɹ��� E_NG ע��ʧ��
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
 *@brief    systick�жϴ�������
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
 *@brief    ��ȡϵͳʱ��
 *@param    *clock��  ʱ��ָ�룬����ϵͳʱ��
 *@retval   none
*/
static void update_system_clock(CpuClock_t *clock)
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
 *@brief    ��ȡϵͳ��Ϣ��CPU_ID,flashsize
 *@param    none
 *@retval   none
*/
static void update_chip_info()
{
    cpu.type = 1;
    cpu.pins = 1;
    memcpy(cpu.company, "123", sizeof("123"));

    uint8_t *p = (uint8_t *)(0X1FFFF7E8);
    for(int i = 0 ; i < 12; i++)
    {
        cpu.chip_id[i] = *p++;
    }


    cpu.flash.size = *(uint16_t *)(0x1FFFF7E0);   //оƬflash����
    switch(cpu.flash.size)
    {
        case 32:
        case 64:
        case 128:
            cpu.flash.page_size = 1024;
            break;
        default:
            cpu.flash.page_size = 2048;
            break;
    }
    cpu.flash.size = cpu.flash.size * 1024;
    cpu.flash.start = MCU_FLASH_BEGIN;
    cpu.flash.end = MCU_FLASH_BEGIN + cpu.flash.size - 1;
    cpu.flash.used = MCU_FLASH_USED;
    milli_seconds = 0;
    SysTick->VAL = SysTick->LOAD;

    }
/**
 *@brief    ����һ��ebox_printf��������������е�����豸�����ǿ��õ�
 *@param    NONE
 *@retval   NONE
*/
size_t ebox_printf(const char *fmt, ...)
{
#if USE_PRINTF
    int     size1 = 0;
    size_t  size2 = 64;
    char    *p;

    va_list va_params;
    va_start(va_params, fmt);


    do
    {
        p = (char *)rt_malloc(size2);
        if(p == NULL)
            return 0;

        size1 =  rt_vsnprintf(p, size2, fmt, va_params);

        if(size1 == -1  || size1 >= size2)
        {
            size2 += 64;
            size1 = -1;
            rt_free(p);
        }
    }
    while(size1 == -1);
    va_end(va_params);
    DBG_UART.write(p, size1);
    DBG_UART.flush();
    rt_free(p);
    return size1;
#endif
}


    int ebox_uart_putc(int ch)
    {
#if USE_PRINTF
        DBG_UART.write(ch);
        return ch;
#endif
    }

    size_t ebox_uart_write(const char *buffer, size_t size)
    {
#if USE_PRINTF
        DBG_UART.write(buffer, size);
        return size;
#endif
    }



}