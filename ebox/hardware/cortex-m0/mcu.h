/**
  ******************************************************************************
  * @file    mcu.h
  * @author  shentq
  * @version V2.0
  * @date    2018/11/6
  * @brief   此文件为ebox核心文件,提供了诸多非常重要的接口,可以被标准的c文件调用。
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
/**
 * Modification History:
 * -shentq                  -version 1.2(2016/08/17)
 *      *修复了芯片主频只能限制为72Mhz的限制，支持任意主频
 *      *增加了cpu的信息获取，包括各个总线时钟，芯片容量，芯片计算加加的能力
 */


/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __CORE_H
#define __CORE_H


#ifdef __cplusplus
extern "C" {
#endif
#include "mcu_config.h"
#include "rcc.h"
#include "nvic.h"


// 空函数，不做任何操作，用来给函数指针赋值，防止函数指针 = null时被调用，出错
__STATIC_INLINE void nullFun(void) {}
typedef void (*fun_noPara_t)(void);           //无参函数指针
typedef void (*fun_onePara_t)(unsigned int);  //单参数函数指针


///////全局变量、函数///////////////////////////////////////////////
extern __IO uint32_t millis_seconds;     //提供一个mills()等效的全局变量。降低cpu调用开销
// 初始化
extern void mcu_init(void);              //ebox系统初始化
extern void SystemClock_Config(void);    //外部实现该函数，可以修改时钟源以及时钟频率。默认工作在48Mhz
extern void mcu_reset(void);
//  uint32_t get_cpu_calculate_per_sec(void);//获取cpu在不跑操作系统的情况下的计算能力。用于统计cpu绝对使用率
// 时间相关接口
extern uint32_t mcu_micros(void);               //获取系统当前运行了多长时间。单位：us
extern uint32_t mcu_millis(void);               //获取系统当前运行了多长时间。单位：ms
extern void mcu_delay_ms(uint32_t ms);//延时n毫秒。可在任意情况下使用，包括禁止所有中断的情况。精度正负1us。
extern void mcu_delay_us(uint32_t us);//延时n微秒。可在任意情况下使用，包括禁止所有中断的情况。精度正负1us。
// systick中断回调函数
extern uint16_t attachSystickCallBack(fun_noPara_t fun, uint16_t multiple = 1); // 绑定中断回调函数，允许则定义周期 = multiple * 1ms; 最大65535

#define GetEndTime(timeOut)					(millis_seconds + timeOut)
// 超时,返回1 否则返回0   这里采用millis()获取millis_seconds,可以防止因为关闭中断导致程序死在延时函数里
#define IsTimeOut(endTime,delay)		    ((uint32_t)(endTime - millis())>delay)

#ifdef __cplusplus
}
#endif
#endif
