/*
  lwip_api.c
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: April 12, 2020
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <time.h>
#endif

#include "lwip/sys.h"
#include "lwip/arch.h"

#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/timeouts.h"

#include "lwip/dhcp.h"
#include "lwip/priv/tcpip_priv.h"

#include "netif/ethernetif.h"
#include "arch/port.h"

#include "lwip/altcp.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"

#include "mbedtls/config.h"


extern unsigned long GetMyIP(void);
extern unsigned long GetLongIP(void);
extern unsigned long GetSubnet(void);
extern unsigned long GetGateway(void);

extern void SetLogReady(int bReady);

extern void SetLongIP(u32_t addr);
extern void SetGateway(u32_t addr);
extern void SetSubnet(u32_t addr);

extern const char *privkey;
extern const char *cert;

struct netif main_netif;
long g_ipIsReady = 0; //

ip_addr_t main_netif_ipaddr;
ip_addr_t main_netif_netmask;
ip_addr_t main_netif_gw;

extern void LwipLinkUp(void);
extern void LwipLinkDown(void);

#if (LWIP_ALTCP_TLS > 0)
static const char *privkey_pass = "";

extern void mbedtls_debug_set_threshold(int threshold);
struct altcp_tls_config* getTlsConfig(void)
{
	struct altcp_tls_config* conf;
	
	size_t privkey_len = strlen(privkey) + 1;
	size_t privkey_pass_len = strlen(privkey_pass) + 1;
	size_t cert_len = strlen(cert) + 1;

	conf = altcp_tls_create_config_server_privkey_cert((u8_t*)privkey, privkey_len, (u8_t*)privkey_pass, privkey_pass_len, (u8_t*)cert, cert_len);
	mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL); //0 No debug,1 Error,2 State change,3 Informational,4 Verbose
	
	return conf;
}

#endif

#ifdef MBEDTLS_ENTROPY_NV_SEED
unsigned char g_bySeed[256];
int mbedtls_platform_std_nv_seed_read(unsigned char *buf, size_t buf_len)
{
	LWIP_DEBUGF(REST_DEBUG, ("RNG read len=%d, data: 0x%02X, 0x%02X, 0x%02X, 0x%02X, ..., 0x%02X",
							buf_len, g_bySeed[0], g_bySeed[1], g_bySeed[2], g_bySeed[3], g_bySeed[buf_len-1]));

	memcpy(buf, g_bySeed, buf_len);
	return (buf_len);
}

int mbedtls_platform_std_nv_seed_write(unsigned char *buf, size_t buf_len)
{
	if (memcmp(g_bySeed, buf, buf_len) != 0)
	{
		LWIP_DEBUGF(REST_DEBUG, ("RNG update len=%d, data: 0x%02X, 0x%02X, 0x%02X, 0x%02X, ..., 0x%02X",
								buf_len, buf[0], buf[1], buf[2], buf[3], buf[buf_len-1]));
	}

	memcpy(g_bySeed, buf, buf_len);
	return (buf_len);
}
#endif

void LwipInit(void)
{
#ifdef MBEDTLS_ENTROPY_NV_SEED
	{
		memset(g_bySeed, 0xAA, sizeof(g_bySeed));
#ifdef _WIN32
		{
			int i;
			srand((unsigned int)time(0));
			for(i = 0; i < sizeof(g_bySeed); i ++)
			{
				g_bySeed[i] = (unsigned char)rand();
			}
		}
#endif
	}
#endif
	
	lwip_init();

	main_netif_ipaddr.addr = PP_HTONL(GetMyIP());
	main_netif_netmask.addr = PP_HTONL(GetSubnet());
	main_netif_gw.addr = PP_HTONL(GetGateway());

	netif_set_flags(&main_netif, NETIF_FLAG_IGMP);
	
	netif_add(&main_netif,
				&main_netif_ipaddr, 
				&main_netif_netmask, 
				&main_netif_gw, 
				NULL, 
				ethernetif_init, 
				netif_input);
	netif_set_default(&main_netif);
	
	LwipLinkUp();
}

extern int GetFrameCount(void);
void poll_network(void)
{
	//if (GetFrameCount() > 0)
	ethernetif_input(&main_netif);
	
	sys_check_timeouts();
}

void LwipStop(void)
{
	LwipLinkDown();

	sys_mbox_free(&tcpip_mbox);
	sys_mutex_free(&lock_tcpip_core);
}

unsigned long g_nTimerLastCheck = 0;

extern u32_t g_nNetIsUp;

void LwipLinkUp(void)
{
	netif_set_link_up(&main_netif);	
	netif_set_up(&main_netif);
	
	main_netif_ipaddr.addr = PP_HTONL(GetMyIP());
	main_netif_netmask.addr = PP_HTONL(GetSubnet());
	main_netif_gw.addr = PP_HTONL(GetGateway());
	
	netif_set_ipaddr(&main_netif, &main_netif_ipaddr);
	netif_set_netmask(&main_netif, &main_netif_netmask);
	netif_set_gw(&main_netif, &main_netif_gw);

	g_nNetIsUp = 1;
	SetLogReady(1);
	
	if (IsDhcpEnabled())
	{
		if (dhcp_start(&main_netif) == ERR_OK)
			return;
		LWIP_DEBUGF(LWIP_DBG_ON, ("DHCP failed"));
	}
	
	OnDhcpFinished();
}

void OnDhcpFinished(void)
{
	SetLongIP(PP_HTONL(main_netif.ip_addr.addr));
	SetGateway(PP_HTONL(main_netif.gw.addr));
	SetSubnet(PP_HTONL(main_netif.netmask.addr));

	netif_set_flags(&main_netif, NETIF_FLAG_IGMP);

	LWIP_DEBUGF(REST_DEBUG, ("ip: %u.%u.%u.%u, gateway: %u.%u.%u.%u, subnet: %u.%u.%u.%u",
		((GetLongIP() >> 24) & 0xFF), ((GetLongIP() >> 16) & 0xFF),
		((GetLongIP() >> 8) & 0xFF), ((GetLongIP() & 0xFF)),
		((GetGateway() >> 24) & 0xFF), ((GetGateway() >> 16) & 0xFF),
		((GetGateway() >> 8) & 0xFF), ((GetGateway() & 0xFF)),
		((GetSubnet() >> 24) & 0xFF), ((GetSubnet() >> 16) & 0xFF),
		((GetSubnet() >> 8) & 0xFF), ((GetSubnet() & 0xFF))));

	g_ipIsReady = 1;
}

void LwipLinkDown(void)
{
	g_ipIsReady = 0;
	
	dhcp_stop(&main_netif);
	
	netif_set_down(&main_netif);
	netif_set_link_down(&main_netif);	
}

struct udp_pcb* pcbLogUdp = NULL;
void LogInit(int port)
{
	pcbLogUdp = udp_new_ip_type(IPADDR_TYPE_ANY);
	if (pcbLogUdp == NULL) 
	{
		LWIP_DEBUGF(UDP_DEBUG, ("udp_new failed!\n"));
		return;
	}

	if (udp_bind(pcbLogUdp, IP_ADDR_ANY, port) != ERR_OK) 
	{
		LWIP_DEBUGF(UDP_DEBUG, ("udp_bind failed!\n"));
		return;
	}
}

void SendLog(int toPort, char* data, int len)
{
	struct pbuf *p, *q;
	char* ptr = data;
	
	if (pcbLogUdp != NULL)
	{
		/* We allocate a pbuf chain of pbufs from the pool. */
		p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
		if (p != NULL) 
		{
			for (q = p; q != 0; q = q->next) 
			{
				memcpy(q->payload, ptr, q->len);
				ptr += q->len;
			}
			udp_sendto(pcbLogUdp, p, IP_ADDR_BROADCAST, toPort);
			pbuf_free(p);
		}
	}
}

void LogDown(void)
{
	if (pcbLogUdp != NULL)
		udp_remove(pcbLogUdp);
	pcbLogUdp = NULL;
}


int UdpFilter(struct pbuf *p, ip_addr_t* src_ip, ip_addr_t* dest_ip)
{
	struct udp_hdr* udphdr = (struct udp_hdr*)p->payload;
	u16_t src = lwip_ntohs(udphdr->src);
	u16_t dest = lwip_ntohs(udphdr->dest);
	
	if (dest == 12345)
	{
		return 1; //eaten
	}
	
	return 0; //pass through
}
