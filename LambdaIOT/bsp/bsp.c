/*
  bsp.c
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: Sept 01, 2020
*/

#include "LPC407x_8x_177x_8x.h"

#include "lpc_types.h"
#include "lpc_clkpwr.h"
#include "lpc_rtc.h"
#include "lpc_iap.h"
#include "lpc_gpio.h"
#include "lpc_emac.h"

#include "cpu_s.h"
#include "log.h"
#include "bsp.h"

#include "bsp_phy.h"

EMAC_CFG_Type g_EmacConfig;

/*
void BSP_UART_Init_For_Booting(void)
{
    UART_CFG_Type UARTConfigStruct; // UART Configuration structure variable
    UART_FIFO_CFG_Type UARTFIFOConfigStruct; // UART FIFO configuration Struct variable
	
	//Initialize UART Configuration parameter structure to default state: Baudrate = 115200 bps 8 data bit 1 Stop bit None parity
	
	UARTConfigStruct.Baud_rate = 115200;
	UARTConfigStruct.Databits = UART_DATABIT_8;
	UARTConfigStruct.Parity = UART_PARITY_NONE;
	UARTConfigStruct.Stopbits = UART_STOPBIT_1;
	
	UART_ConfigStructInit(&UARTConfigStruct);
	UART_Init(UART_LOG, &UARTConfigStruct);

	// Initialize FIFOConfigStruct to default state:
	 //              - FIFO_DMAMode = DISABLE
	 //              - FIFO_Level = UART_FIFO_TRGLEV0
	 //              - FIFO_ResetRxBuf = ENABLE
	 //              - FIFO_ResetTxBuf = ENABLE
	 //              - FIFO_State = ENABLE

	UARTFIFOConfigStruct.FIFO_DMAMode = DISABLE;
	UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV0; //UART FIFO trigger level 0: 1 character
	UARTFIFOConfigStruct.FIFO_ResetRxBuf = ENABLE;
	UARTFIFOConfigStruct.FIFO_ResetTxBuf = ENABLE;
	
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_FIFOConfig(UART_LOG, &UARTFIFOConfigStruct);	

	// Enable UART Transmit
	UART_TxCmd(UART_LOG, ENABLE);
}

void BSP_UART_SyncSend(unsigned char* buf, int len)
{
	UART_Send(UART_LOG, buf, len, BLOCKING);
}
*/

void BSP_GPIO_Init(void)
{
	GPIO_Init();
}

#define ENET_TXD0		((1<<12) | (1 << 8) | (0 << 1)  | 1)		
#define ENET_TXD1		((1<<12) | (1 << 8) | (1 << 1)  | 1)		
#define ENET_TX_EN		((1<<12) | (1 << 8) | (4 << 1)  | 1)		
#define ENET_CRS		((1<<12) | (1 << 8) | (8 << 1)  | 1)		
#define ENET_RXD0		((1<<12) | (1 << 8) | (9 << 1)  | 1)		
#define ENET_RXD1		((1<<12) | (1 << 8) | (10 << 1)  | 1)		
#define ENET_RX_ER		((1<<12) | (1 << 8) | (14 << 1)  | 1)		
#define ENET_REF_CLK	((1<<12) | (1 << 8) | (15 << 1)  | 1)		
#define ENET_MDC		((1<<12) | (1 << 8) | (16 << 1)  | 1)		
#define ENET_MDIO		((1<<12) | (1 << 8) | (17 << 1)  | 1)	

unsigned long BSP_EMAC_Init(unsigned char* mac)
{
    /* EMAC configuration type */
    PIN_CFG(LPC_GPIO1,ENET_TXD0);
    PIN_CFG(LPC_GPIO1,ENET_TXD1);
    PIN_CFG(LPC_GPIO1,ENET_TX_EN);
    PIN_CFG(LPC_GPIO1,ENET_CRS);
    PIN_CFG(LPC_GPIO1,ENET_RXD0);
    PIN_CFG(LPC_GPIO1,ENET_RXD1);
    PIN_CFG(LPC_GPIO1,ENET_RX_ER);
    PIN_CFG(LPC_GPIO1,ENET_REF_CLK);
    PIN_CFG(LPC_GPIO1,ENET_MDC);
    PIN_CFG(LPC_GPIO1,ENET_MDIO);

	g_EmacConfig.PhyCfg.Mode = EMAC_MODE_AUTO;
    g_EmacConfig.pbEMAC_Addr = mac;
//    g_EmacConfig.bPhyAddr = 0;
    g_EmacConfig.nMaxFrameSize = EMAC_ETH_MAX_FLEN;
    g_EmacConfig.pfnPHYInit = PHY_Init;
//    g_EmacConfig.pfnPHYReset = PHY_Reset;
    g_EmacConfig.pfnFrameReceive = NULL;//emac_frame_receive_cb;
    g_EmacConfig.pfnErrorReceive = NULL;//emac_error_receive_cb;
    g_EmacConfig.pfnTransmitFinish = NULL;
    g_EmacConfig.pfnSoftInt = NULL;
    g_EmacConfig.pfnWakeup = NULL;
		
    // Initialize EMAC module with given parameter
    return EMAC_Init(&g_EmacConfig, 1);
}

unsigned long IsLinkUp(void)
{
	unsigned long regv = 0;
	regv = EMAC_Read_PHY(g_EmacConfig.bPhyAddr, PHY_REG_BMSR);
	return (regv & BMSR_LINK_STATUS);
}

unsigned long g_nLastLinkStatus = 0xFFFFFFFF;
unsigned long GetLinkStatus(void)
{
	unsigned long regv = IsLinkUp();
	if (regv != g_nLastLinkStatus)
	{
		g_nLastLinkStatus = regv;
		return (g_nLastLinkStatus | 0x8000);			   //link up
	}
	return g_nLastLinkStatus;	   //
}

void BSP_RTC_Init(void)
{
    // Init RTC module
	
    RTC_Init(LPC_RTC);

    RTC_Cmd(LPC_RTC, ENABLE);

    /* Set the CIIR for second counter interrupt*/
    RTC_CntIncrIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);

#if 0	
	{ //set initial time to RTC
		RTC_TIME_Type FullTime;
		FullTime.YEAR = 2020;
		FullTime.MONTH = 10;
		FullTime.DOM = 7;
		FullTime.HOUR = 23;
		FullTime.MIN = 0;
		FullTime.SEC = 0;
		
		RTC_SetFullTime(LPC_RTC, &FullTime);
	}
#endif

    /* Enable RTC interrupt */
    NVIC_EnableIRQ(RTC_IRQn);
}

void RTC_IRQHandler(void)
{
    /* This is increment counter interrupt*/
    if (RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE))
    {
        unsigned long secval = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_SECOND);

        // Clear pending interrupt
        RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
    }
}

//#######################################################################################################
// for system id via IAP API
//#######################################################################################################
unsigned long g_luPartID;
unsigned long g_luRevision;
unsigned long g_luSN[4];

unsigned long BSP_IAP_Init(void)
{
	unsigned char major,minor;
	
	g_luPartID = 0;	
	g_luRevision = 0;
	g_luSN[0] = 0; g_luSN[1] = 0; g_luSN[2] = 0; g_luSN[3] = 0;

	if (CMD_SUCCESS != ReadPartID((unsigned int*)&g_luPartID))
		return 0;
	
	if (CMD_SUCCESS != ReadBootCodeVer(&major, &minor))
		return 0;
	g_luRevision = ((unsigned long)major<<8)|minor;
	
	if (CMD_SUCCESS != ReadDeviceSerialNum((unsigned int*)&g_luSN[0]))	
		return 0;
	
	return 1;
}

//#######################################################################################################
// for system tick configuration
//#######################################################################################################
#define CPU_CLOCK 		((unsigned long)120000000UL)	/*clock frequency in Hertz*/
#define TICK_RATE_HZ 	1000u 				  /*Tick interval: 1ms, Tick rate in Hertz (10 to 1000 Hz) */
#define MS_PER_TICK 	(1000u/TICK_RATE_HZ)

volatile long long g_llTickCount = 0;

/***********************************************************************************************************/
#define  OS_CPU_CFG_SYSTICK_PRIO           		0u

/* ---------- SYSTICK CTRL & STATUS REG BITS ---------- */
#define  CPU_REG_NVIC_ST_CTRL_COUNTFLAG         0x00010000
#define  CPU_REG_NVIC_ST_CTRL_CLKSOURCE         0x00000004
#define  CPU_REG_NVIC_ST_CTRL_TICKINT           0x00000002
#define  CPU_REG_NVIC_ST_CTRL_ENABLE            0x00000001

#define  CPU_REG_NVIC_ST_CTRL        (*((unsigned long *)(0xE000E010)))   /* SysTick Ctrl & Status Reg.           */
#define  CPU_REG_NVIC_ST_RELOAD      (*((unsigned long *)(0xE000E014)))   /* SysTick Reload      Value Reg.       */
#define  CPU_REG_NVIC_ST_CURRENT     (*((unsigned long *)(0xE000E018)))   /* SysTick Current     Value Reg.       */
#define  CPU_REG_NVIC_ST_CAL         (*((unsigned long *)(0xE000E01C)))   /* SysTick Calibration Value Reg.       */

#define  CPU_REG_NVIC_SHPRI1         (*((unsigned long *)(0xE000ED18)))   /* System Handlers  4 to  7 Prio.       */
#define  CPU_REG_NVIC_SHPRI2         (*((unsigned long *)(0xE000ED1C)))   /* System Handlers  8 to 11 Prio.       */
#define  CPU_REG_NVIC_SHPRI3         (*((unsigned long *)(0xE000ED20)))   /* System Handlers 12 to 15 Prio.       */

static void SysTickInit (unsigned long cnts);

void BSP_SysTickInit(void)
{
    unsigned long cnts;
	
	g_llTickCount = 0;
	
    cnts = CPU_CLOCK / TICK_RATE_HZ;
    SysTickInit(cnts);
}

void SysTickInit (unsigned long cnts)
{
    unsigned long  prio;

    CPU_REG_NVIC_ST_RELOAD = cnts - 1u;

	/* Set SysTick handler prio. */
    prio  = CPU_REG_NVIC_SHPRI3;
    prio &= ((24-1) << 0);
    prio |= (OS_CPU_CFG_SYSTICK_PRIO << 24);

    CPU_REG_NVIC_SHPRI3 = prio;

	/* Enable timer. */
    CPU_REG_NVIC_ST_CTRL |= (CPU_REG_NVIC_ST_CTRL_CLKSOURCE | CPU_REG_NVIC_ST_CTRL_ENABLE);
	
	/* Enable timer interrupt. */
    CPU_REG_NVIC_ST_CTRL |= CPU_REG_NVIC_ST_CTRL_TICKINT;
}

long long BSP_GetTickCount(void)
{
	long long tick = 0;
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
		tick = g_llTickCount*MS_PER_TICK;
	CPU_CRITICAL_EXIT();
	return tick;
}

void SysTick_Handler(void)
{
	g_llTickCount ++;
}
