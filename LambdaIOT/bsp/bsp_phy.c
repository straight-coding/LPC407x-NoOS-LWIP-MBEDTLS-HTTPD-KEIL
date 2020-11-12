#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "cpu_s.h"
#include "bsp.h"
#include "mytypes.h"
#include "utils.h"

#include "log.h"

#include "lpc_emac.h"
#include "lpc_gpio.h"

#include "bsp_phy.h"

#define Write_PHY(x,y,z) 	EMAC_Write_PHY(y,z)
#define Read_PHY(x,y) 		EMAC_Read_PHY(y)

#define TIMEOUT 3000

unsigned short MyIP[2] =                    // "MYIP1.MYIP2.MYIP3.MYIP4"
{
  0xA8C0,//MYIP_1 + (MYIP_2 << 8),                        // use 'unsigned int' to
  0xAA00,//MYIP_3 + (MYIP_4 << 8)                         // achieve word alignment
};

unsigned short SubnetMask[2] =              // "SUBMASK1.SUBMASK2.SUBMASK3.SUBMASK4"
{
  0xFFFF,//SUBMASK_1 + (SUBMASK_2 << 8),                  // use 'unsigned int' to
  0x00FF,//SUBMASK_3 + (SUBMASK_4 << 8)                   // achieve word alignment
};

unsigned short GatewayIP[2] =               // "GWIP1.GWIP2.GWIP3.GWIP4"
{
  0xA8C0,//GWIP_1 + (GWIP_2 << 8),                        // use 'unsigned int' to
  0x0100,//GWIP_3 + (GWIP_4 << 8)                         // achieve word alignment
};

unsigned char MyMAC[6] =   // "M1-M2-M3-M4-M5-M6"
{
  0x1E, 0x30, 0x6C, 0xA2, 0x45, 0x5E
};

int g_nModelPHY = 0;
int	g_nAddrPHY = 0;

int32_t PHY_Detect(void);

extern void setEmacAddr(uint8_t abStationAddr[]);
extern unsigned long IsLinkUp(void);

extern EMAC_CFG_Type g_EmacConfig;

/*********************************************************************//**
 * @brief       Initialize PHY
 * @param[in]   pConfig PHY Configuration
 * @return      SUCCESS/ERROR/-1,
 *
 * Note:
 * This function does:
 *       - Reset PHY by writting to BMCR register
 *       - Setup PHY: PHY Mode, PHY Speed, PHY Full/Half Duplex
 **********************************************************************/
int PHY_Init(EMAC_PHY_CFG_Type *pConfig)
{
	int32_t temp;
	g_nModelPHY = PHY_Detect();
	if (g_nModelPHY < 0)
//		return ERROR;
		return -1;

	if (PHY_Reset(g_EmacConfig.bPhyAddr) == ERROR)
//		return ERROR;
		return -1;
	
	temp = 	PHY_SetMode(g_EmacConfig.bPhyAddr,pConfig->Mode);
	if(temp == SUCCESS)
	{
		setEmacAddr(MyMAC);
		return SUCCESS;
	}
	else 
		return temp;
}
/*********************************************************************//**
 * @brief       Reset PHY
 * @param[in]   None
 * @return      SUCCESS/ERROR
 **********************************************************************/
int PHY_Reset(u16_t phyadd)
{
    int32_t regv,tout;
    
    /* Put the DP83848C in reset mode */
    EMAC_Write_PHY(phyadd, EMAC_PHY_REG_BMCR, BMCR_RESET);

    /* Wait for hardware reset to end. */
    for (tout = EMAC_PHY_RESP_TOUT; tout >= 0; tout--)
    {
        regv = EMAC_Read_PHY (phyadd, EMAC_PHY_REG_BMCR);

        if (!(regv & (BMCR_RESET | BMCR_POWERDOWN)))
        {
            /* Reset complete, device not Power Down. */
            break;
        }

        if (tout == 0)
        {
            // Time out, return ERROR
            return (ERROR);
        }
    }
    
    return SUCCESS;
}


/*********************************************************************//**
 * @brief       Check specified PHY status in EMAC peripheral
 * @param[in]   ulPHYState  Specified PHY Status Type, should be:
 *                          - EMAC_PHY_STAT_LINK: Link Status
 *                          - EMAC_PHY_STAT_SPEED: Speed Status
 *                          - EMAC_PHY_STAT_DUP: Duplex Status
 * @return      Status of specified PHY status (0 or 1).
 *              (-1) if error.
 *
 * Note:
 * For EMAC_PHY_STAT_LINK, return value:
 * - 0: Link Down
 * - 1: Link Up
 * For EMAC_PHY_STAT_SPEED, return value:
 * - 0: 10Mbps
 * - 1: 100Mbps
 * For EMAC_PHY_STAT_DUP, return value:
 * - 0: Half-Duplex
 * - 1: Full-Duplex
 **********************************************************************/
int32_t PHY_CheckStatus(uint16_t phyadd, uint32_t ulPHYState)
{
    int32_t regv, tmp;

    regv = EMAC_Read_PHY (phyadd, EMAC_PHY_REG_BMSR);

    switch(ulPHYState)
    {
        case EMAC_PHY_STAT_LINK:
            tmp = (regv & BMSR_LINK_STATUS) ? 1 : 0;
            break;

        case EMAC_PHY_STAT_SPEED:
            tmp = ((regv & BMSR_100BT4)
                        || (regv & BMSR_100BT_FULL)
                        || (regv & BMSR_100BT_HALF)) ? 1 : 0;
            break;

        case EMAC_PHY_STAT_DUP:
            tmp = ((regv & BMSR_100BT_FULL)
                        || (regv & BMSR_10BT_FULL)) ? 1 : 0;
            break;

        default:
            tmp = -1;
            break;
    }

    return (tmp);
}


/*********************************************************************//**
 * @brief       Set specified PHY mode in EMAC peripheral
 * @param[in]   ulPHYState  Specified PHY mode, should be:
 *                          - EMAC_MODE_AUTO
 *                          - EMAC_MODE_10M_FULL
 *                          - EMAC_MODE_10M_HALF
 *                          - EMAC_MODE_100M_FULL
 *                          - EMAC_MODE_100M_HALF
 * @return      SUCCESS/ERROR
 **********************************************************************/
int32_t PHY_SetMode(uint16_t phyadd, uint32_t ulPHYMode)
{
	int i, timeout = 1;
	uint32_t regv;
	EMAC_Write_PHY(phyadd, PHY_REG_ANAR, ANAR_100BT_FULL | \
										ANAR_100BT_HALF | \
										ANAR_10BT_FULL  | \
										ANAR_10BT_HALF  | \
										ANAR_IEEE802D3);

	/* Configure the PHY device */
	switch(ulPHYMode)
	{
		case EMAC_MODE_AUTO:
			/* Use auto-negotiation about the link speed. */
			EMAC_Write_PHY (phyadd, EMAC_PHY_REG_BMCR, BMCR_AUTO_NEG);
			/* Wait to complete Auto_Negotiation */
			if (IsLinkUp() == 0) break;

			i = BSP_GetTickCount();
			while(TimeElapsed(i, BSP_GetTickCount()) < TIMEOUT)
			{
				if (EMAC_Read_PHY(phyadd, PHY_REG_BMSR) & BMSR_AUTO_NEG_COMPLETE)
				{
					timeout = 0;
					break;
				}
			}
			break;

		case EMAC_MODE_10M_FULL:
			/* Connect at 10MBit full-duplex */
			EMAC_Write_PHY (phyadd, EMAC_PHY_REG_BMCR, BMCR_FULLD_10M);
			break;

		case EMAC_MODE_10M_HALF:
			/* Connect at 10MBit half-duplex */
			EMAC_Write_PHY (phyadd, EMAC_PHY_REG_BMCR, BMCR_HALFD_10M);
			break;

		case EMAC_MODE_100M_FULL:
			/* Connect at 100MBit full-duplex */
			EMAC_Write_PHY (phyadd, EMAC_PHY_REG_BMCR, BMCR_FULLD_100M);
			break;

		case EMAC_MODE_100M_HALF:
			/* Connect at 100MBit half-duplex */
			EMAC_Write_PHY (phyadd, EMAC_PHY_REG_BMCR, BMCR_HALFD_100M);
			break;

		default:
			// un-supported
			return (ERROR);
	}
	
	if (g_nModelPHY == PHY_KSZ8041NL)
	{
//		regv = Read_PHY(g_nAddrPHY, PHY_REG_ANLPAR);
		i = BSP_GetTickCount();
		while(TimeElapsed(i, BSP_GetTickCount()) < TIMEOUT)
		{
			if (EMAC_Read_PHY(phyadd, KSZ8041_REG_PHYCTL2) & PHYCTL2_AUTO_NEG_COMPLETE)
			{
				timeout = 0;
				break;
			}
		}
	}
	else if (g_nModelPHY == PHY_DP83848C)	  //DP83848C
	{
		i = BSP_GetTickCount();
		while(TimeElapsed(i, BSP_GetTickCount()) < TIMEOUT)
		{
			if (EMAC_Read_PHY(phyadd, DP83848C_REG_STS) & 0x0001)
			{
				timeout = 0;
				break;
			}
		}
	}
	else if (g_nModelPHY == PHY_DM9161A)	  //DM9161A
	{	
		regv = EMAC_Read_PHY(phyadd, DM9161A_REG_SPEC_CFG);
		regv &= ~SPEC_CFG_REPEATER;
		EMAC_Write_PHY(phyadd, DM9161A_REG_SPEC_CFG, regv);

//		for(i=0; i<32; i++)
//		tempreg[i] = Read_PHY(g_nAddrPHY, i);
	
		i = BSP_GetTickCount();
		while(TimeElapsed(i, BSP_GetTickCount()) < TIMEOUT)
		{
			if (EMAC_Read_PHY(phyadd, DM9161A_REG_SPEC_CFG_STAT) & SPEC_CFG_STAT_AN_COMPLETE)
			{
				timeout = 0;
				break;
			}
		}

	}
    else
    {
        return (ERROR);
    }

    // Update EMAC configuration with current PHY status
    if(timeout == 0)
	{
		return (PHY_UpdateStatus(phyadd));
	}
	else
		return (ERROR);

}

int32_t PHY_Detect(void)
{
	uint32_t i, id1,id2;

	for(i = 1; i < 32; i ++)    		/* Wait for PHY reset to end. */
	{
		g_EmacConfig.bPhyAddr = i;
		id1 = EMAC_Read_PHY(i, PHY_REG_IDR1);
		id2 = EMAC_Read_PHY(i, PHY_REG_IDR2);
		if (((id1 << 16) | (id2 & 0xFFF0)) == (DP83848C_ID & 0xFFFFFFF0)) 
		{  	/* Check if this is a DP83848C PHY. */
			g_nAddrPHY = i;
			LogPrint(0, "PHY is DP83848C: addr=%d\r\n", i);
			return PHY_DP83848C;
		}
		if (((id1 << 16) | (id2 & 0XFC00)) == (KSZ8041NL_ID & 0xFFFFFC00))
		{  	/* Check if this is a KSZ8041NL PHY. */
			g_nAddrPHY = i;
			LogPrint(0, "PHY is KSZ8041NL: addr=%d\r\n", i);
			return PHY_KSZ8041NL;
		}
 		if (((id1 << 16) | (id2 & 0xFFF0)) == (DM9161A_ID & 0xFFFFFFF0)) 
		{
			g_nAddrPHY = i;
			LogPrint(0, "PHY is DM9161A: addr=%d\r\n", i);
			return PHY_DM9161A;
		}
	}
	g_EmacConfig.bPhyAddr = 0;
	g_nAddrPHY = 0;
	return -1;
}


/*********************************************************************//**
 * @brief       Auto-Configures value for the EMAC configuration register to
 *              match with current PHY mode
 * @param[in]   None
 *  @return     SUCCESS/ERROR
 *
 * Note: The EMAC configuration will be auto-configured:
 *      - Speed mode.
 *      - Half/Full duplex mode
 **********************************************************************/
int32_t PHY_UpdateStatus(uint16_t phyadd)
{
    int32_t regv, tout,dulxp,speed;

    /* Check the link status. */
    for (tout = EMAC_PHY_RESP_TOUT; tout >= 0; tout --)
    {
        regv = EMAC_Read_PHY (phyadd, EMAC_PHY_REG_BMSR);

        //Check Link Status
        if (regv & BMSR_LINK_STATUS)
        {
            /* Link is on. */
            break;
        }

        if (tout == 0)
        {
            // time out
            return (ERROR);
        }
    }

    /* Configure Full/Half Duplex mode. */
    if ((regv & BMSR_100BT_FULL) || (regv & BMSR_10BT_FULL))
    {
        /* Full duplex is enabled. */
        EMAC_SetFullDuplexMode(ENABLE);
		dulxp = 1;
    }
    else if ((regv & BMSR_100BT_HALF) || (regv & BMSR_10BT_HALF))
    {
        /* Half duplex mode. */
        EMAC_SetFullDuplexMode(DISABLE);
		dulxp = 0;
    }

    /* Configure 100MBit/10MBit mode. */
    if ((regv & BMSR_100BT4) || (regv & BMSR_100BT_FULL) || (regv & BMSR_100BT_HALF))
    {
        /* 100MBit mode. */
        EMAC_SetPHYSpeed(ENABLE);
		speed = 100;
    }
    else if ((regv & BMSR_10BT_FULL) || (regv & BMSR_10BT_HALF))
    {
        /* 10MBit mode. */
        EMAC_SetPHYSpeed(DISABLE);
		speed = 10;
    }
	
	LogPrint(0, "%s, %s\r\n", (dulxp==1)?"Full Duplex":"Half Duplex", (speed==100)?"100MHz":"10MHz");

    // Complete
    return (SUCCESS);
}
