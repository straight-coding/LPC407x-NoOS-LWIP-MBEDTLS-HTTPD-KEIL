/*
  port.h
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: April 12, 2020
*/

#ifndef _PORT_H_
#define _PORT_H_

///////////////////////////////////////////////////////////////////////////////////////////
// Network
///////////////////////////////////////////////////////////////////////////////////////////

#define MAX_DMA_QUEUE	1024
struct packet_wrapper
{
	long len;
	unsigned char* packet;	/* allocated */
	void* next;				/* next packet */
};

typedef struct _DeviceConfig
{
	unsigned long nDhcpEnabled;// = 1;// 0 or 1;
	unsigned long dwIP;// = 0;		// 0xC0A80563;
	unsigned long dwSubnet;// = 0;	// 0xFFFFFF00;
	unsigned long dwGateway;// = 0;	// 0xC0A80501;

	long nLog;// = 0;
	long nSessionTimeout;// = 3 * 60 * 1000;

	char szColor[16];// = { 0 };
	char szDate[16];// = { 0 };
	char szFont[16];// = { 0 };
	char szLocation[16];// = { 0 };
}DeviceConfig;

void LwipInit(void);
void LwipStop(void);

extern int tcpip_inloop(void);
void InitDevInfo(void);

void NotifyFromEthISR(void);

///////////////////////////////////////////////////////////////////////////////////////////
// Network
///////////////////////////////////////////////////////////////////////////////////////////

void LoadConfig4Edit(void);
void AppyConfig(void);

unsigned char* GetMyMAC(void);
int FillMAC(void* context, char* buffer, int maxSize);

unsigned long GetMyIPLong(void);
int FillIP(void* context, char* buffer, int maxSize);
void SetMyIPLong(unsigned long addr);
void SetMyIPString(char* addr);

unsigned long GetGatewayLong(void);
int FillGateway(void* context, char* buffer, int maxSize);
void SetGatewayLong(unsigned long addr);
void SetGatewayString(char* addr);

unsigned long GetSubnetLong(void);
int FillSubnet(void* context, char* buffer, int maxSize);
void SetSubnetLong(unsigned long addr);
void SetSubnetString(char* addr);

unsigned long IsDhcpEnabled(void);
int FillDhcp(void* context, char* buffer, int maxSize);
void SetDhcpEnabled(char* value);

void OnDhcpFinished(void);

long GetSessionTimeout(void);
int FillSessionTimeout(void* context, char* buffer, int maxSize);
void SetSessionTimeout(char* value);

///////////////////////////////////////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////////////////////////////////////

char* GetVendor(void);
char* GetVendorURL(void);

char* GetModel(void);
char* GetModelURL(void);

char* GetDeviceName(void);
char* GetDeviceSN(void);
char* GetDeviceUUID(void);
char* GetDeviceVersion(void);

char* GetLocation(void);
void SetLocation(char* value);

char* GetColor(void);
void SetColor(char* value);

char* GetDate(void);
void SetTestDate(char* value);

char* GetFont(void);
void SetFont(char* value);

int FillLog(void* context, char* buffer, int maxSize);
void SetTestLog(char* value);

#endif

