/**
  ******************************************************************************
  * @file    ebox_config.h
  * @author  cat_li
  * @version V2.0
  * @date    2016/10/21
  * @brief   ebox配置裁剪文件
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

#ifndef __EBOX_CONFIG_H
#define __EBOX_CONFIG_H


#define EBOX_DEBUG true

#include "ebox_core.h"
extern "C" {

    void console_uart_init();
    void console_uart_putc(char ch);
    void console_uart_write(const char *p,uint8_t len);
    int console_uart_getc();
}
#endif

