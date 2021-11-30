///**
//  ******************************************************************************
//  * @file    ebox_i2c.cpp
//  * @author  cat_li
//  * @brief   ����������ģʽ
//		1  2017/5/30  ���ӳ�ʱ����ֹ������������д�������ӷ���״̬
//  ******************************************************************************
//  * @attention
//  *
//  * No part of this software may be used for any commercial activities by any form
//  * or means, without the prior written consent of shentq. This specification is
//  * preliminary and is subject to change at any time without notice. shentq assumes
//  * no responsibility for any errors contained herein.
//  * <h2><center>&copy; Copyright 2015 shentq. All Rights Reserved.</center></h2>
//  ******************************************************************************
//  */

///* Includes ------------------------------------------------------------------*/
#include "ebox_mcu_i2c.h"


#if EBOX_DEBUG
// �Ƿ��ӡ������Ϣ, 1��ӡ,0����ӡ
#define EBOX_DEBUG_MCUI2C_ENABLE       true
#define EBOX_DEBUG_MCUI2C_ENABLE_ERR   true
#endif


#if EBOX_DEBUG_MCUI2C_ENABLE
#define mcuI2cDebug(...)  ebox_printf("[mcuI2C DBG]:%d: ",__LINE__),ebox_printf(__VA_ARGS__ )
#else
#define mcuI2cDebug(...)
#endif

#if EBOX_DEBUG_MCUI2C_ENABLE_ERR
#define mcuI2cDebugErr(fmt, ...)  ebox_printf("[mcuI2C err]:%d: " fmt "\n", __LINE__, __VA_ARGS__)
#else
#define mcuI2cDebugErr(fmt, ...)
#endif

mcuI2c::mcuI2c(I2C_TypeDef *I2Cx,Gpio *sclPin, Gpio *sdaPin)
{
    _scl = sclPin;
    _sda = sdaPin;
    
    _i2cx = I2Cx;
    setTimeout(100);
}


/**
  *@brief    ����i2cʱ�Ӻ���������speed����timing,Ĭ��80K
  *@param    speed:  ���� 10,100,200,300,400 �ֱ����10k��100k��200k,300k,400k
  *@retval   None
  */

void mcuI2c::begin(uint8_t address)
{
    uint32_t rcc_src;

    _sda->mode(AF_OD_PU,GPIO_AF_I2C2);
    _scl->mode(AF_OD_PU,GPIO_AF_I2C2);

    rcc_clock_cmd((uint32_t)_i2cx, ENABLE);
    switch((uint32_t)_i2cx)    
    {
        case I2C1_BASE:
            rcc_src = RCC_APB1Periph_I2C1;break;
        case I2C2_BASE:
            rcc_src = RCC_APB1Periph_I2C2;break;
        case I2C3_BASE:
            rcc_src = RCC_APB1Periph_I2C3;break;
    }       
    /* Reset I2Cx IP */
    RCC_APB1PeriphResetCmd(rcc_src, ENABLE);
    /* Release reset signal of I2Cx IP */
    RCC_APB1PeriphResetCmd(rcc_src, DISABLE);

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = address;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    setClock();
 
    mcuI2cDebug("scl_pin: 0x%x ; sda_pin: 0x%x\n",_scl->id, _sda->id);

}
void mcuI2c::begin(int address)
{
    begin((uint8_t)address);
}
void mcuI2c::begin()
{
    begin((uint8_t)0);
}

/**
 * @brief ����i2cͨ���ٶ�.
 *
 * @param[in] speed i2c�ٶȣ�����ֵΪ400K��300K��200K��100Kʱ��i2cͨ��Ƶ��Ϊ��ֵ.
 *
 * @return 0.
 */
void mcuI2c::setClock(Speed_t speed)
{
   switch (speed)
    {
    case K10:
        _speed = (10000);
        break;
    case K100:
        _speed = (100000);
        break;
    case K200:
        _speed = (200000);
        break;
    case K400:
        _speed = (400000);
        break;
    case K1000:
        _speed = (1000000);
        break;
    default:
        _speed = (100000);
    }
    I2C_InitStructure.I2C_ClockSpeed = _speed;
    I2C_Init(_i2cx, &I2C_InitStructure);

    /* I2C Peripheral Enable */
    I2C_Cmd(_i2cx, ENABLE);
    /* Apply I2C configuration after enabling it */
    
    
    mcuI2cDebug("speed:%dKhz;\n",_speed/1000);
}

i2c_err_t mcuI2c::_write(const uint8_t *data, size_t quantity)
{
    i2c_err_t ret;
    for(int i = 0; i < quantity; i++) {
        ret = _sendByte(data[i]);
        if(ret != I2C_ERROR_OK)
        {
            return ret;
        }
    }
    return ret;
}

/*
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 *          5 .. timeout
 */
i2c_err_t mcuI2c::_write(uint8_t address,const uint8_t *data, size_t quantity, int sendStop)
{
    uint32_t cnt = 0;
    i2c_err_t ret;
    _start();
    ret = _send7bitsAddress(address, 0);
    if(ret) return ret;
    while (quantity--)
    {
        ret = _sendByte(*data);
        if(ret) return ret;
       data++;
    }
    ret = _stop();
    if(ret) return ret;
    return ret;
}
/**
  *@brief    ָ���Ĵ���������ȡ. start->WslaveAddr->regAddr->RSlaverAddr->data...->nRead==1->Nack->stop->data
  *@param    uint8_t slaveAddr:  �ӻ���ַ
  *          uint16_t regAddr: �Ĵ�����ַ
  *          uint8_t *data: ��ȡ��������
  *          uint16_t nRead��Ҫ��ȡ�����ݳ���
  *          : ��ʱ
  *@retval   EOK��EWAIT
  */
size_t mcuI2c::_read(uint8_t address,uint8_t *data, uint16_t length,uint8_t sendStop)
{
    uint32_t cnt = 0;
    i2c_err_t err = I2C_ERROR_OK;
    int ret = 0;
    ret = length;
    err = _start();
    if(err) return 0;
    err = _send7bitsAddress(address, 1);
    while (length--)
    {
        if (length == 0)
        {
            err = _sendNack();
            err = _stop();
        }
        err = _receiveByte(data);
        if(err) return 0;
        data++;
    }
    err = _sendAck();
    return ret;
    
}

i2c_err_t mcuI2c::_start()
{
    i2c_err_t ret;
    uint32_t cnt = 0;


    /* Send STRAT condition */
    I2C_GenerateSTART(_i2cx, ENABLE);
    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(_i2cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        cnt++;
        if (cnt > 0xfffe)
        {
            mcuI2cDebugErr("start fail, state reg SR2 = %d��SR1 = %d \r\n", _i2cx->SR2, _i2cx->SR1);
            return I2C_ERROR_TIMEOUT;
        }
    }
    _err_at = 0;
    return I2C_ERROR_OK;
}

i2c_err_t mcuI2c::_stop()
{
    I2C_GenerateSTOP(_i2cx, ENABLE);
    return I2C_ERROR_OK;
}


i2c_err_t mcuI2c::_sendByte(uint8_t data)
{
    uint32_t cnt = 0;
    /* Send the byte to be written */
    I2C_SendData(_i2cx, data);
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(_i2cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        cnt++;
        if (cnt > 0xfffe)
        {
            mcuI2cDebugErr("send data fail, state reg SR2 = %d��SR1 = %d \r\n", _i2cx->SR2, _i2cx->SR1);
            return I2C_ERROR_DATA_NACK_NO_RECV;
        }
    }

    return I2C_ERROR_OK;
}
i2c_err_t mcuI2c::_send7bitsAddress(uint8_t slaveAddr, uint8_t WR)
{
    uint32_t cnt = 0;

    if (WR) /* Send address for read */
    {
        //    mcuI2cDebug("send read address is %d \r\n",slaveAddr);
        I2C_Send7bitAddress(_i2cx, slaveAddr, I2C_Direction_Receiver);
        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(_i2cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
        cnt++;
        if (cnt > 0xfffe)
            {
                mcuI2cDebugErr("send read address fail, state reg SR2 = %d��SR1 = %d \r\n", _i2cx->SR2, _i2cx->SR1);
                return I2C_ERROR_ADDR_NACK_NO_RECV;
            }
        }
    }
    else   /* Send address for write */
    {
        //    mcuI2cDebug("send write address is %d \r\n",slaveAddr);
        I2C_Send7bitAddress(_i2cx, slaveAddr, I2C_Direction_Transmitter);
        /* Test on EV6 and clear it */
        while (!I2C_CheckEvent(_i2cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            cnt++;
            if (cnt > 0xfffe)
            {
                mcuI2cDebugErr("send write address fail, state reg SR2 = %d��SR1 = %d \r\n", _i2cx->SR2, _i2cx->SR1);
                return I2C_ERROR_ADDR_NACK_NO_RECV;
            }
        }
    }
    return I2C_ERROR_OK;
}
i2c_err_t mcuI2c::_receiveByte(uint8_t *data)
{
    i2c_err_t ret = I2C_ERROR_OK;
    uint32_t cnt = 0;
    while (!I2C_CheckEvent(_i2cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
        cnt++;
        if (cnt > 0xfffe)
        {
            mcuI2cDebugErr("read data fail, state reg SR2 = %d��SR1 = %d \r\n", _i2cx->SR2, _i2cx->SR1);
            return ret;
        }

    }
    *data = I2C_ReceiveData(_i2cx);//�����Ĵ�������
    return ret;
}

i2c_err_t mcuI2c::_sendAck()
{
    i2c_err_t err = I2C_ERROR_OK;
    I2C_AcknowledgeConfig(_i2cx, ENABLE);
    return err;
}
i2c_err_t mcuI2c::_sendNack()
{
    i2c_err_t err = I2C_ERROR_OK;
    I2C_AcknowledgeConfig(_i2cx, DISABLE);
    return err;
}