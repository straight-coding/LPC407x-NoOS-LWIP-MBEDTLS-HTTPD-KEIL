#ifndef _BSP_PHYLAN_H_
#define _BSP_PHYLAN_H_

#include "mytypes.h"
#include "lpc_emac.h"

#define PHY_DP83848C			0
#define PHY_KSZ8041NL			1
#define PHY_DM9161A				2

#define DP83848C_ID         	0x20005C90  /* PHY Identifier                    */
#define DM9161A_ID				0x0181B8A0  /* PHY Identifier                    */
//#define KSZ8041NL_ID			0x00221512  /* PHY Identifier                    */

#define KSZ8041_ID1             (0x0022)
#define KSZ8041_ID2             (0x1400)
#define KSZ8041NL_ID			(KSZ8041_ID1<<16) | KSZ8041_ID2 	/* PHY Identifier  */

//Phy general registers
#define PHY_REG_BMCR        	0x00        /* Basic Mode Control Register       */
#define PHY_REG_BMSR        	0x01        /* Basic Mode Status Register        */
#define PHY_REG_IDR1        	0x02        /* PHY Identifier 1                  */
#define PHY_REG_IDR2        	0x03        /* PHY Identifier 2                  */
#define PHY_REG_ANAR        	0x04        /* Auto-Negotiation Advertisement    */
#define PHY_REG_ANLPAR      	0x05        /* Auto-Neg. Link Partner Abitily    */
#define PHY_REG_ANER        	0x06        /* Auto-Neg. Expansion Register      */
#define PHY_REG_ANNPTR      	0x07        /* Auto-Neg. Next Page TX            */

#define EMAC_PHY_REG_BMCR    		PHY_REG_BMCR   
#define EMAC_PHY_REG_BMSR           PHY_REG_BMSR   
#define EMAC_PHY_REG_IDR1           PHY_REG_IDR1   
#define EMAC_PHY_REG_IDR2           PHY_REG_IDR2   
#define EMAC_PHY_REG_ANAR   	    PHY_REG_ANAR   
#define EMAC_PHY_REG_ANLPAR 	    PHY_REG_ANLPAR 
#define EMAC_PHY_REG_ANER   	    PHY_REG_ANER   
#define EMAC_PHY_REG_ANNPTR 	    PHY_REG_ANNPTR 

/* KSZ8041 PHY Extended Registers */
#define KSZ8041_REG_PHYCTL1     	0x1E        /* PHY Control 1            */
#define KSZ8041_REG_PHYCTL2     	0x1F        /* PHY Control 2            */
#define PHYCTL2_AUTO_NEG_COMPLETE	0x80		//1 = Auto-negotiation process completed
#define PHYCTL2_FLOW_CTL_CAPABLE	0x40		//1 = Flow control capable
#define PHYCTL2_100TX_FULL			(6<<2)
#define PHYCTL2_100TX_HALF			(2<<2)
#define PHYCTL2_10BT_FULL			(5<<2)
#define PHYCTL2_10BT_HALF			(1<<2)

/* DP83848C PHY Extended Registers */
#define DP83848C_REG_STS         	0x10        /* Status Register                   */
#define DP83848C_REG_MICR        	0x11        /* MII Interrupt Control Register    */
#define DP83848C_REG_MISR        	0x12        /* MII Interrupt Status Register     */
#define DP83848C_REG_FCSCR       	0x14        /* False Carrier Sense Counter       */
#define DP83848C_REG_RECR        	0x15        /* Receive Error Counter             */
#define DP83848C_REG_PCSR        	0x16        /* PCS Sublayer Config. and Status   */
#define DP83848C_REG_RBR         	0x17        /* RMII and Bypass Register          */
#define DP83848C_REG_LEDCR       	0x18        /* LED Direct Control Register       */
#define DP83848C_REG_PHYCR       	0x19        /* PHY Control Register              */
#define DP83848C_REG_10BTSCR     	0x1A        /* 10Base-T Status/Control Register  */
#define DP83848C_REG_CDCTRL1     	0x1B        /* CD Test Control and BIST Extens.  */
#define DP83848C_REG_EDCR        	0x1D        /* Energy Detect Control Register    */
/* KSZ8041 PHY Extended Registers */
#define DM9161A_REG_SPEC_CFG		0x10
#define SPEC_CFG_REPEATER			(1<<11)
#define DM9161A_REG_SPEC_CFG_STAT	0x11
#define SPEC_CFG_STAT_AN_COMPLETE	0x08
//PHY_REG_BMCR bit definition
#define BMCR_FULLD_100M      		0x2100      /* Full Duplex 100Mbit               */
#define BMCR_HALFD_100M      		0x2000      /* Half Duplex 100Mbit               */
#define BMCR_FULLD_10M       		0x0100      /* Full Duplex 10Mbit                */
#define BMCR_HALFD_10M       		0x0000      /* Half Duplex 10MBit                */
#define BMCR_AUTO_NEG		  		0x1000      /* Enable/Disable Auto Negotiation   */
#define BMCR_SPEED_100M      		0x2000      /* Speed Select, 1 = 100M,0 = 10M    */
#define BMCR_RESTART_AUTO_NEG  		0x0200      /* Restart Auto Negotiation           */
#define BMCR_RESET					0x8000
#define BMCR_POWERDOWN				0x0800
#define BMCR_ISOLATE				0x0400

//PHY_REG_BMSR bit definition
#define BMSR_LINK_STATUS			0x0004
#define BMSR_AUTO_NEG_COMPLETE		0x0020
#define BMSR_100BT4					0x8000
#define BMSR_100BT_FULL				0x4000
#define BMSR_100BT_HALF				0x2000
#define BMSR_10BT_FULL				0x1000
#define BMSR_10BT_HALF				0x0800

//PHY_REG_ANAR bit definition
#define ANAR_100BT4					0x0200
#define ANAR_100BT_FULL				0x0100
#define ANAR_100BT_HALF				0x0080
#define ANAR_10BT_FULL				0x0040
#define ANAR_10BT_HALF				0x0020
#define	ANAR_IEEE802D3				0x0001

int32_t PHY_Init(EMAC_PHY_CFG_Type *pConfig);
int32_t PHY_Reset(uint16_t phyadd);
int32_t PHY_CheckStatus(uint16_t phyadd, uint32_t ulPHYState);
int32_t PHY_SetMode(uint16_t phyadd, uint32_t ulPHYMode);
int32_t PHY_UpdateStatus(uint16_t phyadd);

extern int32_t g_nModelPHY;

#endif
