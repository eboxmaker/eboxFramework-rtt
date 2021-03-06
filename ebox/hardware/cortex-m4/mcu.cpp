



#include "ebox_core.h"
#include "mcu.h"

#ifdef __cplusplus
extern "C" {
#endif
#define systick_no_interrupt()  SysTick->CTRL &=0xfffffffd
#define systick_interrupt()     SysTick->CTRL |=0x0002

__IO uint32_t millis_seconds;//提供一个mills()等效的全局变量。降低cpu调用开销
__IO uint16_t micro_para;

static void update_chip_info();
static void update_system_clock(CpuClock_t *clock);

void mcu_init(void)
{
    update_system_clock(&cpu.clock);
    SysTick_Config(cpu.clock.core / 1000); //  每隔 (nhz/168,000,000)s产生一次中断
    micro_para = cpu.clock.core / 1000000; //减少micros函数计算量
    NVIC_PriorityGroupConfig(NVIC_GROUP_CONFIG);

    update_chip_info();
}

void mcu_reset(void)
{
    NVIC_SystemReset();
}

uint32_t mcu_micros(void)
{
    uint32_t micro;
    micro = (millis_seconds * 1000 + (1000 - (SysTick->VAL) / (micro_para)));
    return  micro;
}


uint32_t mcu_millis( void )
{
    return millis_seconds;
}

void mcu_delay_ms(uint32_t ms)
{
    uint32_t end ;
    end = mcu_micros() + ms * 1000 - 3;
    while(mcu_micros() < end);
}

/**
  *@brief    us延时,使用systick计数器。48Mhz时钟时可以满足us(1.3)精度。8Mhz时最小6-7us,24Mhz时最小2.2us,16Mhz时最小3.5us
  *@param    uint16_t us  要延时的时长，最小1us
  *@retval   none
 */
void  mcu_delay_us(uint32_t us)
{
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

fun_noPara_t callBackFun;
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
        _multiple = multiple == 0 ? 1 : multiple;
        return EOK;
    }
    else
    {
        return ENG;
    }
}


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
void SysTick_Handler(void)//systick中断
{
    if (millis_seconds++ % _multiple == 0)
    {
        if(callBackFun != 0)
        {
            callBackFun();
        }
    }
}

static void update_chip_info()
{
    cpu.type = MCU_TYPE;
    cpu.pins = MCU_PINS;
    memcpy(cpu.company, "st\0", sizeof("st\0"));

    
    uint8_t *p = (uint8_t *)(0x1FFF7A10);
    for(int i = 0 ; i < 12; i++)
    {
        cpu.chip_id[i] = *p++;
    }


    cpu.flash.size = *(uint16_t *)(0x1FFF7A22);   //芯片flash容量
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
    
    millis_seconds = 0;
    SysTick->VAL = 0;
    //统计cpu计算能力//////////////////
    do
    {
        cpu.ability++;//统计cpu计算能力
    }
    while(millis_seconds < 1);
    cpu.ability = cpu.ability  * 1000 * 2;
    ////////////////////////////////
}

#ifdef __cplusplus
}
#endif
