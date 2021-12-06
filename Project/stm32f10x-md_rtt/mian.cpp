/**
 ******************************************************************************
 * @file    main.cpp
 * @author  cat_li
 * @version V1.0
 * @date    2017/07/13
 * @brief   ebox exti example, ����stm32nucleo(072)ƽ̨��֤
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

#include "ebox.h"
#include "bsp_ebox.h"
#include "ebox_exti.h"
#include "rtthread.h"
#include "cxx_Thread.h"

using namespace rtthread;
#include "rtcMillis.h"

/**
	*	1	��������ʾ��GPIO�ж�
    *   2   ����userbt1�����û����������º͵���󶨲�ͬ�Ļص�����
            ���𴮿ڴ�ӡ��Ϣ�����·�תled���
    *   3   ex����PA0���½��غ������ذ�ͬһ���ص�������x++���Ӵ������
	*/

/* ���������������̷������� */
#define EXAMPLE_NAME	"STM32F0 GPIO_EXTI example"
#define EXAMPLE_DATE	"2017-09-10"
RtcMillis rtc;
DateTime dt(__DATE__,__TIME__,8);//������

void setup()
{
    console_uart_init();
    print_log(EXAMPLE_NAME, EXAMPLE_DATE);
    LED1.mode(OUTPUT_PP);
    rtc.begin(dt);
}


Mutex *mutex;
//��̬�߳�����
static void dynamic_entry(void *param)
{
    static int cnt = 0;
	
    while (++cnt)
    {
//		rt_kprintf("dynamic_thread is run:%d\n",cnt);
        rt_thread_mdelay(500);
    }
}
int thread_sample(void)
{
    static rt_thread_t thread_id = RT_NULL;
    thread_id = rt_thread_create("dynamic_th",    //����
                                 dynamic_entry,   //�̴߳���
                                 RT_NULL,         //����
                                 1024,            //ջ��С
                                 15,              //���ȼ�
                                 20);             //ʱ��Ƭ

    if (thread_id != RT_NULL)
        rt_thread_startup(thread_id);			  //�߳̽������̬
    else
        rt_kprintf("dynamic_thread create failure\n");
					//�߳̽������̬		
									 
    return RT_EOK;									 
}
void myTask(void *p)
{
    while(1)
    {
//        rtc.update();
//        rtc.dateTime.print(UART);
        Thread::sleep(1000);
    }
}
Thread *task1;

class ThreadA:public Thread
{
public:
    ThreadA(String &nm)
    {
        name = nm;
    }
    virtual void run(void *parameter)
    {
        while(1)
        {
//            uart1.printf("ThreadA:%s run\n",name.c_str());
            Thread::sleep(1000);

        }
    }
private:
    String name;

};
int main(void)
{
    setup();
    thread_sample();
    task1 = new Thread(myTask);
    task1->start();
    String name = "name1";
    ThreadA ta(name);
    name = "name2";
    ThreadA tb(name);
    ta.start();
    tb.start();
    while(1)
    {
        LED1.toggle();
        rt_thread_mdelay(1000);
        
    }
}
