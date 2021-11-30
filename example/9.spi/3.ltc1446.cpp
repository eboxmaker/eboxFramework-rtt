/*
file   : *.cpp
author : shentq
version: V1.0
date   : 2015/7/5

Copyright 2015 shentq. All Rights Reserved.
*/

//STM32 RUN IN eBox


#include "ebox.h"
#include "bsp_ebox.h"

#include "ltc1446.h"

/**
	*	1	��������Ҫ����eDriveĿ¼�µ�LTCX����
	*	2	��������ʾ���ⲿDAC��LTCϵ�У��Ļ�������
    *   3   DACA,��������ӣ�DACB����𽥼�С
    *   4   PAO����DACA����ӡ�����PA1����DACB����ӡ���
	*/

/* ���������������̷������� */
#define EXAMPLE_NAME	"LTCX example"
#define EXAMPLE_DATE	"2018-08-11"

#define ADCA	PA0
#define ADCB	PA1

//LTCX ltc1(&PA6, &spi1);
LTCX ltc1(&PA6, &sspi1);
Adc adc(ADC1);

void setup()
{
    ebox_init();
    UART.begin(115200);
    print_log(EXAMPLE_NAME, EXAMPLE_DATE);
    ltc1.begin();


    adc.add_ch(&ADCA);
    adc.add_ch(&ADCB);
    adc.begin();
}
int main(void)
{
    uint32_t i = 0, t = 4000;
    setup();

    while(1)
    {
        if(i != t)
        {
            i+=100;
            ltc1.write(i, i);
            UART.printf("ͨ��A��ѹ = %.01f mv; ͨ��B��ѹ = %.01f mv \r\n", (float)(4096 - i),(float) i);
//					            UART.printf("ͨ��A��ѹ = %.01f mv; ͨ��B��ѹ = %.01f mv \r\n", adc.read_voltage(&ADCA), adc.read_voltage(&ADCB));
            delay_ms(10000);
        }
        else
        {
            i = 2048;
        }
    }
}
