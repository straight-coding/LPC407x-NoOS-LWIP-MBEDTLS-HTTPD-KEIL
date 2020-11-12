/*
  ssdp.h
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: April 12, 2020
*/

#ifndef _SSDP_H
#define _SSDP_H

void ssdpInit(void);
void ssdpDown(void);
void ssdpCheckTimeout(long long tick);

#define LWIP_GetTickCount 	BSP_GetTickCount
extern long long BSP_GetTickCount(void);

#endif
