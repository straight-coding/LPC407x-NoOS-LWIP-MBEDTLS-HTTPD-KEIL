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

static const char *privkey_pass = "";

#if 0
#if (LWIP_ALTCP_TLS > 0)
#define KEYSIZE		512

#if (KEYSIZE == 512)
static const char *privkey = "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"\
	"MIIBpjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQIGPULjavQsEgCAggA\n"\
	"MBQGCCqGSIb3DQMHBAhzhjxyhEJ7NgSCAWBihB0wi33LrSzyWYUjmBc2DmIo9FgB\n"\
	"AK51vUyljP7XbSUE9lfSenRCf2bCtHlOe/x1crRmnI8Tr8F5iw6mOBEfJ6orROdy\n"\
	"NAjNMNZXu76tIYjkblRuMSzXDNoQzH/dKxN1KMojtsKQbG2EcQVfG0GlQypXstgr\n"\
	"ajJp57MZy0BKGXaWYUc+9Vgda8SLxWNLRy5wNwSBmN02ZxjT5t5zpYaUaUB1WFY4\n"\
	"6pIMgmynn636boOF2VHXQdRTJbcn76hsJYkVQhERB/z8yYKIhl0Y8Jag3Wm151wt\n"\
	"gXwZivq7cVwgk7TWuE15NJQQKRNo1GMbkMLyaHLIQwvLeXYeul38ryxXm5CtvmG+\n"\
	"VmndzTSd6H1kJcgv2mTlH7deajEgNO2JMLSEYQRv0AJyVwdgJauWu3iz7+0W1iAD\n"\
	"r2q+8CrzT1HhNdRoDLs28gwQ/f48XG8Erfg1ZZfUuQCNrYL1wATTs+wy\n"\
	"-----END ENCRYPTED PRIVATE KEY-----\n";

static const char *cert = "-----BEGIN CERTIFICATE-----\n"\
	"MIIBhjCCATCgAwIBAgIQuY9Og0ylhpRIUZV9sJFi5DANBgkqhkiG9w0BAQsFADAR\n"\
	"MQ8wDQYDVQQDEwZDQVJvb3QwHhcNMjAxMDI0MDcwMDAwWhcNMzAxMDI0MDcwMDAw\n"\
	"WjAaMRgwFgYDVQQDEw9zZXJ2ZXIuc3RyYWlnaHQwXDANBgkqhkiG9w0BAQEFAANL\n"\
	"ADBIAkEA44T2OhYNX5CAVfP3aB7xC8XpIIRvST0JGeRptFWv7MiMsrI/Ewe/Lt+K\n"\
	"ImUPDZwSzhJ4YEqMfOjclKPV06bf3QIDAQABo1swWTATBgNVHSUEDDAKBggrBgEF\n"\
	"BQcDATBCBgNVHQEEOzA5gBB4pVILJFLqT8sdQXmk6FaWoRMwETEPMA0GA1UEAxMG\n"\
	"Q0FSb290ghASHfUmS8dmi0SWS8hDV/pFMA0GCSqGSIb3DQEBCwUAA0EAQfkZZ2l1\n"\
	"YCu7PHQBVwC6BFF8Pq5FfxpzFm2xFJ/hMI9U2xmSbLdWclCiy0fXNTW+yl4TKgbk\n"\
	"EMYAWHOUwlBsTA==\n"\
	"-----END CERTIFICATE-----\n";
#endif

#if (KEYSIZE == 1024)
static const char *privkey = "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"\
	"MIICxjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQICZi2OGpU2lkCAggA\n"\
	"MBQGCCqGSIb3DQMHBAgSQEOgHjn0zQSCAoDr9oboaJBu+dZbTFCauetynjG3djiX\n"\
	"inTaCQ+cJb/mAJqcIqxypz6DH/VU5blq3p6HSNwp8f3olxfuOMEnCJkHXaRZeh0f\n"\
	"Fib8eBoS8BPMFbqyI68Aua1rIBRrkkGARFGvGaFun4qYaV1mEEcBrox7HHrzE3cH\n"\
	"meT1aNK+ucXKeqAv0X9xw9zJhsf2e6x3uAbg1ZF63UbCN330W4ZLin7PdS81OLhf\n"\
	"wGU7H2YUrqDEMH7B/g4/VAymhIVHu2B5eEYuWc55KnsrqREA0mZuEwHGiMzyTek3\n"\
	"YMrhJDRDU1nOonq57tmek43anxsnnDw2t1D0SR4xbOFSW6kxzfhgpM5hU1w6fNkX\n"\
	"4HpZWNfYlCCmH3cfmVMWrkbQWyLANqsApKAQL8y2KdrmCXxjPEVQclTIRKGWMy6X\n"\
	"GLc2JxHekqU86pN6D+lo6E6XAsoGOm/1LexMzcNsEVhiWIWXDG0e9mdizwmouWl/\n"\
	"Gw18vUVqkSmgJuUCZ7FMfa6THB9CSnXFDkH2/8XvMX8QYQj+83JefCUkDhYgDGCa\n"\
	"NQATsnAg23YcYy6Jhd4Ft/sM+SBuzTWFeoeoSzTPQT+1Bxt/pWMl7kVf8JUUz0FV\n"\
	"wyBCUm52pjieqfCUTMcZDeouhnQdq+wE+ZV7XtLt2CJl0grc/JB9A3cwVy43XQ/i\n"\
	"f0UxRfKcgUI/UG83FpEzQaFcwoXi3/+S0qg3Nk4G0mAAIi2KfgFiIqStTyIWorWn\n"\
	"A+1Ibheu3wAJxRpUJEyq8YJN/VKz65LSGSZGntrXKJbvlCo7XTZMS8No3JxT5+id\n"\
	"OG0ttzydqRaCicQ4wbh+e5uQ6DCyNQfxbaZIwfeeVNC9aQ5GbiExbSH8\n"\
	"-----END ENCRYPTED PRIVATE KEY-----\n";

static const char *cert = "-----BEGIN CERTIFICATE-----\n"\
	"MIICCzCCAXSgAwIBAgIQph/1VIEBq6BPQba4fDUF0zANBgkqhkiG9w0BAQsFADAR\n"\
	"MQ8wDQYDVQQDEwZDQVJvb3QwHhcNMjAwNjA2MDcwMDAwWhcNMzAwNjA2MDcwMDAw\n"\
	"WjAaMRgwFgYDVQQDEw9zZXJ2ZXIuc3RyYWlnaHQwgZ8wDQYJKoZIhvcNAQEBBQAD\n"\
	"gY0AMIGJAoGBAMew5lAQoJ6RNsBgvM3JaSd4GEd7LUumVyVnI5Meyv0hAftC+Muo\n"\
	"fmjlyS9bvHBQEwF2MwRSL1uHXV47J8pIr8g6iN7a7augsIO79x5bOPr+GXO4mXCs\n"\
	"gRuWeYddcbKHx7IEAcUA78Kc70KAHSnrSNRQQRRv1GVQjgIhaBfjaYjNAgMBAAGj\n"\
	"WzBZMBMGA1UdJQQMMAoGCCsGAQUFBwMBMEIGA1UdAQQ7MDmAEIIlXCLg5+C1KClr\n"\
	"DaYFc7yhEzARMQ8wDQYDVQQDEwZDQVJvb3SCELtRJzGDgSqHS7iarn4k5vswDQYJ\n"\
	"KoZIhvcNAQELBQADgYEAdfty93O6syeOCjcRFVDW+WnVrzB+vRg1zeujUD1EHSNj\n"\
	"Mf+aMALrZ7NaHpgx3ifbpijuc1l7ZztiSyvz1/c9srcj3W6DHsV5KgmABGbL/bGR\n"\
	"P9jV/z8GctLSbsCRH9SZHktF2pqNq87cSTPUx5VPWoOiluMZtqjCO9g13yRv5qk=\n"\
	"-----END CERTIFICATE-----\n";
#endif
#endif
#endif

struct netif main_netif;
long g_ipIsReady = 0; //

ip_addr_t main_netif_ipaddr;
ip_addr_t main_netif_netmask;
ip_addr_t main_netif_gw;

extern void LwipLinkUp(void);
extern void LwipLinkDown(void);

#if (LWIP_ALTCP_TLS > 0)
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
