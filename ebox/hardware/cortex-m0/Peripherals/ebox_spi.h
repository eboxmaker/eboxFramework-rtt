/**
  ******************************************************************************
  * @file    spi.h
  * @author  shentq
  * @version V2.1
  * @date    2016/08/14
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

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __SPI_H
#define __SPI_H

#include "ebox_core.h"
#include "spi.h"
#include "mcu.h"

/*
	1.目前只测试了SPI1、SPI2，spi3望网友测试
	2.该spi功能强大，总线支持同时挂载不同MODE ,SPEED,bit_oder的设备
	3.每一个spi设备应有一个自己的SPI_CONFIG的配置，以支持该设备的的读写，
		在读写前需要获得SPI的控制权，如果获取不到则一直等待！主要是为了兼容操作系统，
		如果不使用操作系统也必须加上获得控制权的代码，在是用完SPI后一定要释放SPI总线，
		如果不释放总线会导致别的SPI设备一直处于等待的状态
    4.得益于f0系列灵活的pin配置，可以使用不同的io组合来使用spi资源，默认spi1为PA5，PA6，PA7
      也可以使用PB3，PA6，PA7作为spi1，在stm32f072_define中做好定义即可
*/
//默认配置 空，只依靠结构体SPICONFIG来初始化
class mcuSpi: public Spi
{
public:
    mcuSpi(SPI_TypeDef *SPIx, Gpio *sck, Gpio *miso, Gpio *mosi);

    virtual void    begin (Config_t *spi_config);
    virtual void    config(Config_t *spi_config);
    virtual uint8_t read_config(void);

    virtual uint8_t transfer(uint8_t data);

    virtual int8_t  write(uint8_t data);
    virtual uint8_t read();
    virtual int8_t  read(uint8_t  *recv_data);

    virtual int8_t  write_buf(uint8_t *data, uint16_t len);
    virtual int8_t  read_buf(uint8_t *recv_data, uint16_t len);
public:
    virtual int8_t  take(Config_t *spi_config);
    virtual int8_t  release(void);

private:
    SPI_TypeDef *_spi;
    Gpio        *_sck;
    Gpio        *_miso;
    Gpio        *_mosi;

    uint8_t     _busy;
};

/*
	注意：1.该类的SPI_CLOCK_DIV是由delay_us延时函数控制。略有不准，比硬件SPI会慢很多
				2.speed设置只能为SPI_CLOCK_DIVx。如果不是此值，则会将SPI_CLOCK_DIV的值直接传递给delay_us.即delay_us(SPI_CONFIG_TYPE->prescaler);
				3.初期调试I2C设备建议使用SPI_CLOCK_DIV256。
				4.函数接口和硬件SPI完全一样可以互相替换。
*/
class SoftSpi: public Spi
{
public:
    SoftSpi(Gpio *sck, Gpio *miso, Gpio *mosi);

    virtual void    begin (Config_t *spi_config);
    virtual void    config(Config_t *spi_config);
    virtual uint8_t read_config(void);

    virtual uint8_t transfer(uint8_t data);

    virtual int8_t  write(uint8_t data);
    virtual uint8_t read();
    virtual int8_t  read(uint8_t  *recv_data);

    virtual int8_t  write_buf(uint8_t *data, uint16_t len);
    virtual int8_t  read_buf(uint8_t *recv_data, uint16_t len);

public:
    virtual int8_t take(Config_t *spi_config);
    virtual int8_t release(void);

private:
    Gpio    *_sck;
    Gpio    *_miso;
    Gpio    *_mosi;

    uint8_t mode;
    uint8_t bit_order;
    uint8_t spi_delay;

    uint8_t busy;

    uint8_t transfer0(uint8_t data);
    uint8_t transfer1(uint8_t data);
    uint8_t transfer2(uint8_t data);
    uint8_t transfer3(uint8_t data);
};




#endif
