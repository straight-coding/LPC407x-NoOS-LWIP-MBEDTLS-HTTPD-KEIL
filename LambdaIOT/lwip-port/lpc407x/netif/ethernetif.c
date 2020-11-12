/*
  ethernetif.c
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: April 12, 2020
*/
#include <string.h>

#include "cpu_s.h"

#include "lwip/opt.h"
#include "lwip/sys.h"

#include "netif/etharp.h"
#include "ethernetif.h"

#include "arch/port.h" //NIC interfaces

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

extern unsigned int BSP_EMAC_Init(u8_t* mac);
extern struct pbuf* EMAC_ReceivePBuf(void);
extern void EMAC_SendPBuf(struct pbuf *p);

static err_t low_level_init(struct netif *netif)
{
	u8_t* pMac;

	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	pMac = GetMyMAC();

	netif->hwaddr[0] = pMac[0];
	netif->hwaddr[1] = pMac[1];
	netif->hwaddr[2] = pMac[2];
	netif->hwaddr[3] = pMac[3];
	netif->hwaddr[4] = pMac[4];
	netif->hwaddr[5] = pMac[5];

	netif->mtu = 1500;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP | NETIF_FLAG_LINK_UP;

	//return ERR_OK; //ERR_IF
	return BSP_EMAC_Init((unsigned char*)netif->hwaddr);
}

err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
	
#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
	
	EMAC_SendPBuf(p);

#if ETH_PAD_SIZE
	pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
	
	CPU_CRITICAL_EXIT();
  
	return ERR_OK;
}

pbuf_t* low_level_input(struct netif *netif)
{
	return EMAC_ReceivePBuf();
}

err_t ethernetif_input(struct netif *netif) //called by poll_network()
{
	struct eth_hdr *ethhdr;
	struct pbuf *p;
	
	/* move received packet into a new pbuf */
	p = low_level_input(netif);
	/* no packet could be read, silently ignore this */
	if (p == NULL) 
		return 0;//ERR_MEM;

	/* points to packet payload, which starts with an Ethernet header */
	ethhdr = p->payload;

	switch (htons(ethhdr->type)) 
	{ /* IP or ARP packet? */
	case ETHTYPE_IP:
	case ETHTYPE_IPV6:
	case ETHTYPE_ARP:
		/* full packet send to tcpip_thread to process */
		if (netif->input(p, netif) != ERR_OK)
		{ 
			LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
			pbuf_free(p);
			p = NULL;
		}
		break;
	default:
		pbuf_free(p);
		p = NULL;
		break;
	}

	return 0;
}

err_t ethernetif_init(struct netif *netif) //called by netif_add(...)
{
	netif->output = etharp_output; //defined: lwip\src\core\ipv4\etharp.c
	netif->linkoutput = low_level_output;

	return low_level_init(netif);
}
