/*
  bsp.h
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: Sept 01, 2020
*/

#ifndef _BSP_H_
#define _BSP_H_

void BSP_SysTickInit(void);
void BSP_RTC_Init(void);
void BSP_GPIO_Init(void);

unsigned long BSP_IAP_Init(void);
long long BSP_GetTickCount(void);

unsigned long BSP_EMAC_Init(unsigned char* mac);

#endif
