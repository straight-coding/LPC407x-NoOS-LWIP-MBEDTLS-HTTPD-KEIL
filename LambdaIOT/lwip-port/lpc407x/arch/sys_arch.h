#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include <time.h>

#ifdef WIN32
#pragma warning(disable:4996) //_CRT_SECURE_NO_WARNINGS
#endif

///////////////////////////////////////////////////////////////////////////////////////////
// OS
///////////////////////////////////////////////////////////////////////////////////////////

unsigned long sys_jiffies(void);
unsigned long sys_now(void);
unsigned long msDiff(unsigned long now, unsigned long last);

void LogPrint(int level, char* format, ...);
void LWIP_sprintf(char* buf, char* format, ...);

char* getClock(char* buf, int maxSize);
time_t parseHttpDate(char* s);

extern const char wday_name[][4];
extern const char mon_name[][4];

///////////////////////////////////////////////////////////////////////////////////////////
// File
///////////////////////////////////////////////////////////////////////////////////////////
#define LOCAL_FILE_SYSTEM	0

typedef struct FILE LWIP_FIL;

void* LWIP_fopen(const char* szTemp, const char* mode);
time_t LWIP_ftime(char* fname, char* buf, int maxSize);
int  LWIP_fseek(void* f, long offset);
void LWIP_fclose(void* f);
long LWIP_fsize(void* f);
int  LWIP_fread(void* f, char* buf, int count, unsigned int* bytes); //0=success
int  LWIP_fwrite(void* f, char* buf, int count); //>0:success

void  MakeDeepPath(char* szPath);
void* LWIP_firstdir(void* filter, int* isFolder, char* name, int maxLen, int* size, time_t* date);
int   LWIP_readdir(void* hFind, int* isFolder, char* name, int maxLen, int* size, time_t* date);
void  LWIP_closedir(void* hFind);

#endif
