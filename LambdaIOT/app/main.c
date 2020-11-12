/*
  main.c
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: Sept 01, 2020
*/

#include "LPC407x_8x_177x_8x.h"
#include "lpc_types.h"

#include "lpc_gpio.h"

#include "cpu_s.h"
#include "bsp.h"
#include "log.h"

#include "lwip/apps/netbiosns.h"
#include "ssdp.h"

#include "http_fs.h"
#include "http_session.h"
#include "http_core.h"

#include "arch/port.h"

extern void poll_network(void);
void Beacon(long long tick, long interval);

extern long g_ipIsReady;

extern struct netif main_netif;

struct altcp_pcb *g_pcbListen80 = NULL;
struct altcp_pcb *g_pcbListen443 = NULL;

extern unsigned long GetLongIP(void);
extern unsigned long GetSubnet(void);
extern unsigned long GetGateway(void);

extern void tcp_kill_all(void);
extern void PrintLwipStatus(void);
extern struct altcp_pcb* HttpdInit(int tls, unsigned long port);
extern int HttpdStop(struct altcp_pcb *pcbListen);

extern void LogInit(int port);
extern void LogDown(void);

unsigned long g_nLastMacStatus = 0xFFFFFFFF;
long long tLastBeaconTick = 0;
long long tLastPollTick = 0;

int main(void)
{
#ifdef _DEBUG
	SCB->VTOR = (unsigned long)0x00000000; //burn form 0x28000
#else
	SCB->VTOR = (unsigned long)(WORKCODE_BASE+0x1000); //burn from 0x30000, but run from 0x31000
#endif

	CPU_IntDis(); //disable all inturrupts
	
	BSP_SysTickInit(); //setup the System Tick Timer
	
	BSP_GPIO_Init(); //Configure GPIO power/clock control bit
	
	LogPrint(0, "\r\n");
	LogPrint(0, "Initial...\r\n");
	
#ifdef _SSP
	BSP_SSP_Init();
#endif
	
	BSP_RTC_Init();			//RTC
	BSP_IAP_Init();			//IAP
	
	InitDevInfo(); //initiate device settings

// initialization done
	CPU_IntEn();

	GPIO_SetDir(4, (1<<23), 1); //LED1: P4.23
	//GPIO_SetDir(4, (1<<26), 1); //LED2: P4.26
	//GPIO_SetDir(5, (1<< 1), 1); //BEEP: P5.1
	
	WEB_fs_init();	//initiate file system in memory
	
	SetupHttpContext(); //initiate http receiving context

	LwipInit(); //initiate lwip stack and start dhcp
	
	while(TRUE)
	{
		SessionClearAll();	//clear http session context

		while (g_ipIsReady == 0)
		{
			long long tick = BSP_GetTickCount();
			Beacon(tick, 250);
			poll_network();
		}
		
		LogInit(8899);

		netbiosns_set_name("lwipSimu");// GetDeviceName());
		netbiosns_init();

		ssdpInit();
		LWIP_DEBUGF(REST_DEBUG, ("ip: %u.%u.%u.%u, gateway: %u.%u.%u.%u, subnet: %u.%u.%u.%u",
			((GetLongIP() >> 24) & 0xFF), ((GetLongIP() >> 16) & 0xFF),
			((GetLongIP() >> 8) & 0xFF), ((GetLongIP() & 0xFF)),
			((GetGateway() >> 24) & 0xFF), ((GetGateway() >> 16) & 0xFF),
			((GetGateway() >> 8) & 0xFF), ((GetGateway() & 0xFF)),
			((GetSubnet() >> 24) & 0xFF), ((GetSubnet() >> 16) & 0xFF),
			((GetSubnet() >> 8) & 0xFF), ((GetSubnet() & 0xFF))));
		
		g_pcbListen80  = HttpdInit(0, 80);
		g_pcbListen443 = HttpdInit(1, 443);

		LogPrint(0, "App Service started\r\n");

		while(g_ipIsReady > 0)
		{
			long long tick = BSP_GetTickCount();
			if ((tick - tLastPollTick) > 100)
			{
				tLastPollTick = tick;
				
				ssdpCheckTimeout(tick);
				SessionCheck(tick);
				Beacon(tick, 500);
			}
			poll_network();
		}
		
		if (g_pcbListen80 != NULL)
			HttpdStop(g_pcbListen80);

		if (g_pcbListen443 != NULL)
			HttpdStop(g_pcbListen443);

		LogPrint(0, "App Service stopped\r\n");

		ssdpDown();
		netbiosns_stop();
		
		LogDown();

		PrintLwipStatus();

		tcp_kill_all();

		LogPrint(0, "Initial...\r\n");
        CPU_Nop();
	}
}

void Beacon(long long tick, long interval)
{
	if ((tick - tLastBeaconTick) > interval)
	{
		unsigned long pin;
		tLastBeaconTick = tick;
		
		pin = GPIO_ReadValue(4);
		if ((pin & (1<<23)) == 0)
		{
			GPIO_SetValue(4, (1<<23));		//off
			//LogPrint(0, "Light OFF\r\n");
		}
		else
		{
			GPIO_ClearValue(4, (1<<23));	//on
			//LogPrint(0, "Light ON\r\n");
		}
	}
}
