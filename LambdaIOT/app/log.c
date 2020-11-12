/*
  log.c
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: Nov 07, 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "cpu_s.h"
#include "bsp.h"

#include "lpc_clkpwr.h"
#include "lpc_rtc.h"

#define MAX_LOG_SIZE	256
char g_sMsg[MAX_LOG_SIZE] __attribute__((aligned(8)));

extern void SendLog(int toPort, char* data, int len);

void LogPrint(int level, char* format, ... )
{
	unsigned long len;
	char sMsg[MAX_LOG_SIZE];
	RTC_TIME_Type FullTime;
	
	va_list ap;
	
	if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0)
		return; //if in interrupt mode

	RTC_GetFullTime(LPC_RTC, &FullTime);
	//CPU_CRITICAL_ENTER();

	va_start(ap, format);
		memset(sMsg, 0, sizeof(sMsg));
		if ((format[0] == '%') && (format[1] == 'T'))
		{
			snprintf(sMsg, 10, "%02d:%02d:%02d ", FullTime.HOUR, FullTime.MIN, FullTime.SEC);//tick/(3600000), (tick%3600000)/60000, (tick%60000)/1000);
			vsnprintf(sMsg + 9, sizeof(sMsg) - 12, format+2, ap);
		}
		else if((format[0] == '\r') && (format[1] == '\n'))
		{
			vsnprintf(sMsg, sizeof(sMsg) - 1, format, ap);
		}
		else if ((format[0] == '%') && (format[1] == 'C'))
		{ //command
			vsnprintf(sMsg, sizeof(sMsg) - 1, format+2, ap);
		}
		else
		{
			snprintf(sMsg, 10, "%02d:%02d:%02d ", FullTime.HOUR, FullTime.MIN, FullTime.SEC);//tick/(3600000), (tick%3600000)/60000, (tick%60000)/1000);
			vsnprintf(sMsg + 9, sizeof(sMsg) - 12, format, ap);
		}
		len = strlen(sMsg);
		if ((sMsg[len-1] != '\r') && (sMsg[len-1] != '\n'))
			strcat(sMsg, "\r\n");
	va_end(ap);

	SendLog(8899, sMsg, strlen(sMsg));
}
