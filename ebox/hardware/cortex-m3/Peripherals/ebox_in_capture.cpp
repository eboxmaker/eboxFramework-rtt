/**
  ******************************************************************************
  * @file    incapture.cpp
  * @author  shentq
  * @version V2.1
  * @date    2017/07/23
  * @brief  
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
#include "ebox_in_capture.h"
#include "ebox_timer_it.h"
extern uint16_t t1_overflow_times ;
extern uint16_t t2_overflow_times ;
extern uint16_t t3_overflow_times ;
extern uint16_t t4_overflow_times ;


InCapture::InCapture(Gpio *capture_pin)
{
    this->capture_pin = capture_pin;
    this->period = 0xffff;
    this->prescaler = 1;
    this->polarity = Falling;
    high_capture = 0;
    low_capture = 0;
    _capture = 0;
}
void InCapture::begin(uint16_t prescaler ,ICMode_t mode)
{
	  uint8_t index;
    capture_pin->mode(INPUT_PU);
    init_info(this->capture_pin);

    this->prescaler = prescaler;
    base_init(this->period, this->prescaler);
    timer_clock = get_timer_clock();
   
    switch((uint32_t)TIMx)
    {
    case (uint32_t)TIM1_BASE:
        master_counter = &(master_count[0]);
        if(ch== TIM_Channel_1) {
            _set_polarity = TIM_OC1PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR1);
            index = TIM1_IT_CC1;
        }
        if(ch== TIM_Channel_2) {
            _set_polarity = TIM_OC2PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR2);
            index = TIM1_IT_CC2;
        }
        if(ch== TIM_Channel_3) {
            _set_polarity = TIM_OC3PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR3); 
            index = TIM1_IT_CC3;
        }
        if(ch== TIM_Channel_4) {
            _set_polarity = TIM_OC4PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR4); 
            index = TIM1_IT_CC4;
        }
        break;
    case (uint32_t)TIM2_BASE:
        master_counter = &(master_count[1]);

        if(ch== TIM_Channel_1) {
            _set_polarity = TIM_OC1PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR1);
            index = TIM2_IT_CC1;
        }
        if(ch== TIM_Channel_2) {
            _set_polarity = TIM_OC2PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR2);
            index = TIM2_IT_CC2;
        }
        if(ch== TIM_Channel_3) {
            _set_polarity = TIM_OC3PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR3); 
            index = TIM2_IT_CC3;
        }
        if(ch== TIM_Channel_4) {
            _set_polarity = TIM_OC4PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR4); 
            index = TIM2_IT_CC4;
        }
        break;
    case (uint32_t)TIM3_BASE:
        master_counter = &(master_count[2]);

        if(ch== TIM_Channel_1) {
            _set_polarity = TIM_OC1PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR1);
            index = TIM3_IT_CC1;
        }
        if(ch== TIM_Channel_2) {
            _set_polarity = TIM_OC2PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR2);
            index = TIM3_IT_CC2;
        }
        if(ch== TIM_Channel_3) {
            _set_polarity = TIM_OC3PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR3); 
            index = TIM3_IT_CC3;
        }
        if(ch== TIM_Channel_4) {
            _set_polarity = TIM_OC4PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR4); 
            index = TIM3_IT_CC4;
        }
        break;
    case (uint32_t)TIM4_BASE:
        master_counter = &(master_count[3]);

        if(ch== TIM_Channel_1) {
            _set_polarity = TIM_OC1PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR1);
            index = TIM4_IT_CC1;
        }
        if(ch== TIM_Channel_2) {
            _set_polarity = TIM_OC2PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR2);
            index = TIM4_IT_CC2;
        }
        if(ch== TIM_Channel_3) {
            _set_polarity = TIM_OC3PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR3); 
            index = TIM4_IT_CC3;
        }
        if(ch== TIM_Channel_4) {
            _set_polarity = TIM_OC4PolarityConfig;
            ccr = (uint16_t *)&(TIMx->CCR4); 
            index = TIM4_IT_CC4;
        }
        break;

    }
    tim_irq_init(index,(&InCapture::_irq_handler),(uint32_t)this);
    if(mode == SIMPLE)
        attach(this,&InCapture::simple_event);
    else
        attach(this,&InCapture::complex_event);

}

void InCapture::close()
{
    rcc_clock_cmd((uint32_t)TIMx,DISABLE);
    rt_kprintf("CLOSE\n");
}

void InCapture::base_init(uint16_t period, uint16_t prescaler)
{
    this->period = period;//????period
    this->prescaler = prescaler;//????prescaler

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIMx_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


    switch((uint32_t)this->TIMx)
    {
    case (uint32_t)TIM1_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;//
        break;
    case (uint32_t)TIM2_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//
        break;
    case (uint32_t)TIM3_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;//
        break;
    case (uint32_t)TIM4_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
        NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;//
        break;
    }

    TIM_TimeBaseStructure.TIM_Period = this->period ; //ARR
    TIM_TimeBaseStructure.TIM_Prescaler = this->prescaler - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

    TIM_ARRPreloadConfig(TIMx, ENABLE);


    //??????TIMx???????????? ????2
    TIMx_ICInitStructure.TIM_Channel = ch; //CC1S=01 	?????????? IC1??????TI1??
    TIMx_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//??????????
    TIMx_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //??????TI1??
    TIMx_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //????????????,??????
    TIMx_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 ?????????????? ??????
    TIM_ICInit(this->TIMx, &TIMx_ICInitStructure);
    TIM_ClearFlag(this->TIMx, TIM_FLAG_Update);


    //??????????????
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //??????????1??
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;  //????????0??
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ??????????
    NVIC_Init(&NVIC_InitStructure);   //????NVIC_InitStruct??????????????????????NVIC??????

    switch(ch)
    {
    case TIM_Channel_1:
        TIM_ClearFlag(this->TIMx, TIM_FLAG_CC1);
        TIM_ITConfig(this->TIMx,  TIM_IT_CC1 | TIM_IT_Update, ENABLE);   //????TIMx??????2????????
        break;
    case TIM_Channel_2:
        TIM_ClearFlag(this->TIMx, TIM_FLAG_CC2);
        TIM_ITConfig(this->TIMx,  TIM_IT_CC2 | TIM_IT_Update, ENABLE);   //????TIMx??????2????????
        break;
    case TIM_Channel_3:
        TIM_ClearFlag(this->TIMx, TIM_FLAG_CC3);
        TIM_ITConfig(this->TIMx,  TIM_IT_CC3 | TIM_IT_Update, ENABLE);   //????TIMx??????2????????
        break;
    case TIM_Channel_4:
        TIM_ClearFlag(this->TIMx, TIM_FLAG_CC4);
        TIM_ITConfig(this->TIMx,  TIM_IT_CC4 | TIM_IT_Update, ENABLE);   //????TIMx??????2????????
        break;
    }

    TIM_Cmd(TIMx, ENABLE); //

}
void InCapture::init_info(Gpio *capture_pin)
{
    switch((uint8_t)capture_pin->id)
    {
        case PA0_ID :
            TIMx = TIM2;
            ch = TIM_Channel_1;//irq = TIM2_IRQn;
            break;
        case PA1_ID:
            TIMx = TIM2;
            ch = TIM_Channel_2;//irq = TIM2_IRQn;
            break;
        case PA2_ID:
            TIMx = TIM2;
            ch = TIM_Channel_3;//irq = TIM2_IRQn;
            break;
        case PA3_ID:
            TIMx = TIM2;
            ch = TIM_Channel_4;//irq = TIM2_IRQn;
            break;        
    
        case PA6_ID:
            TIMx = TIM3;
            ch = TIM_Channel_1;//irq = TIM3_IRQn;
            break;
        case PA7_ID:
            TIMx = TIM3;
            ch = TIM_Channel_2;//irq = TIM3_IRQn;
            break;
        
        
        case PB0_ID:
            TIMx = TIM3;
            ch = TIM_Channel_3;//irq = TIM3_IRQn;
            break;
        case PB1_ID:
            TIMx = TIM3;
            ch = TIM_Channel_4;//irq = TIM3_IRQn;
            break;

        case PB6_ID:
            TIMx = TIM4;
            ch = TIM_Channel_1;//irq = TIM4_IRQn;
            break;
        case PB7_ID:
            TIMx = TIM4;
            ch = TIM_Channel_2;//irq = TIM4_IRQn;
            break;
        case PB8_ID:
            TIMx = TIM4;
            ch = TIM_Channel_3;//irq = TIM4_IRQn;
            break;
        case PB9_ID:
            TIMx = TIM4;
            ch = TIM_Channel_4;//irq = TIM4_IRQn;
            break;
    }
    

}

void InCapture::set_polarity_falling()
{
    this->polarity = Falling;
    _set_polarity(TIMx,this->polarity);

}
void InCapture::set_polarity_rising()
{
    this->polarity = Rising;
    _set_polarity(TIMx,this->polarity);
}



void InCapture::complex_event()
{
    uint32_t    capture = 0;
    uint32_t    now = 0;
    now = *ccr + *master_counter;  //get capture value
    capture = now - last_value;
    last_value = now;
    
    if(polarity == Falling)//????????????????????????????????
    {
        high_capture = capture;
        set_polarity_rising();//??????????????
    }
    else
    {
        low_capture = capture;
        set_polarity_falling();//??????????????
    }
//    if((high_capture!= 0) && (low_capture != 0))
        _available = true;
    
}
void InCapture::simple_event()
{
    uint32_t    now = 0;
    now = *ccr + *master_counter;  //get capture value
    _capture = now - last_value;
    master_count[1] = 0;
    last_value = *ccr ;

    _available = true;  
}

bool InCapture::update_resault()
{
    if(_available == false)
        return false;
    else
    {
        if(_capture != 0)//????????
        {
            res.frq = ((float)timer_clock/(float)_capture);
            res.peroid = (_capture*1000000.0/(timer_clock));
        }
        else//????????
        {
            res.frq = (timer_clock/((high_capture + low_capture)));
            res.peroid = ((high_capture + low_capture)*1000000.0/(timer_clock));
            res.high_duty = (high_capture*100.0/(high_capture + low_capture));
            res.low_duty =  (low_capture*100.0/(high_capture + low_capture));
            res.high_time = ((high_capture )*1000000.0/(timer_clock));
            res.low_time = ((low_capture)*1000000.0/(timer_clock));
        }
        _available = false;
    }
    return true;
}

bool InCapture::available()
{
    return _available;
}


uint32_t InCapture::get_timer_clock()
{
    return get_timer_source_clock()/this->prescaler;
}

uint32_t InCapture::get_timer_source_clock()
{
    uint32_t temp = 0;
    uint32_t timer_clock = 0x00;
    CpuClock_t clk;
    get_system_clock(&clk);

    if ((uint32_t)this->TIMx == TIM1_BASE)
    {
        timer_clock = clk.pclk2;
    }
    else
    {
        temp = RCC->CFGR;
        if(temp & 0x00000400)//????PCLK??????????????????????????????????????????????PCLK1??????
            timer_clock = clk.pclk1 * 2;
        else
            timer_clock = clk.pclk1 ;
    }
    return timer_clock;

}


void InCapture::_irq_handler( uint32_t id)
    { 
		InCapture *handler = (InCapture*)id;
		handler->_irq.call();

}


void InCapture::attach(void (*fptr)(void))
    {
    if (fptr) 
        {
        _irq.attach(fptr);
		}
}

