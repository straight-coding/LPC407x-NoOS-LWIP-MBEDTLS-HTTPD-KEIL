#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>
#include <stdarg.h>

#include <lwip/sys.h>
#include <lwip/err.h>
#include <lwip/opt.h>

#include "lpc_rtc.h"

#include "bsp.h"
#include "log.h"

const char wday_name[][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const char mon_name[][4]  = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

unsigned long sys_jiffies(void)
{
	return (unsigned long)BSP_GetTickCount();
}

unsigned long sys_now(void)
{
	return (unsigned long)BSP_GetTickCount();
}

unsigned long msDiff(unsigned long now, unsigned long last)
{
	if (now >= last)
		return (now - last);
	return ((unsigned long)0xFFFFFFFF - last + now);
}

#if (NO_SYS == 0)

////////////////////////////////////////////////////////////////////////
// The following semaphore functions must be implemented by the sys_arch
////////////////////////////////////////////////////////////////////////

void sys_init(void)
{
}

err_t sys_sem_new(sys_sem_t *sem, unsigned char count)
{
	sys_sem_t* new_sem = (sys_sem_t*)1;
	if (new_sem != NULL) 
	{
		sem->sem = new_sem;
		return ERR_OK;
	}
	sem->sem = NULL;
	return ERR_MEM;
}

void sys_sem_free(sys_sem_t *sem)
{
	if (sem->sem != NULL)
		;//CloseHandle(sem->sem);
	sem->sem = NULL;
}

void sys_sem_signal(sys_sem_t *sem)
{
	if (sem->sem != NULL)
		;//ReleaseSemaphore(sem->sem, 1, NULL);
}

unsigned long sys_arch_sem_wait(sys_sem_t *sem, unsigned long timeout)
{
	unsigned long ret;
	unsigned long long starttime, endtime;

	if (timeout == 0) 
	{	/* wait infinite */
		starttime = (unsigned long)BSP_GetTickCount();
		//ret = WaitForSingleObject(sem->sem, INFINITE);
		endtime = (unsigned long)BSP_GetTickCount();

		return msDiff(endtime, starttime);
	}

	starttime = (unsigned long)BSP_GetTickCount();
	//ret = WaitForSingleObject(sem->sem, timeout);
	if (1)//(ret == WAIT_OBJECT_0) 
	{
		endtime = (unsigned long)BSP_GetTickCount();

		return msDiff(endtime, starttime);
	}
	return SYS_ARCH_TIMEOUT;
}

////////////////////////////////////////////////////////////////////////
// The following mailbox functions must be implemented by the sys_arch
////////////////////////////////////////////////////////////////////////

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
	mbox->sem = (sys_mbox_t*)1;//CreateSemaphore(0, 0, MAX_QUEUE_ENTRIES, 0);
	if (mbox->sem == NULL) 
		return ERR_MEM;

	memset(&mbox->q_mem, 0, sizeof(unsigned long)*MAX_QUEUE_ENTRIES);
	mbox->head = 0;
	mbox->tail = 0;

	return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
	if (mbox->sem != NULL)
		;//CloseHandle(mbox->sem);
	mbox->sem = NULL;
}

void sys_mbox_post(sys_mbox_t *q, void *msg)
{
	//BOOL ret;
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);

	q->q_mem[q->head] = msg;
	q->head++;
	if (q->head >= MAX_QUEUE_ENTRIES)
		q->head = 0;

	//ret = ReleaseSemaphore(q->sem, 1, 0);

	SYS_ARCH_UNPROTECT(lev);
}

err_t sys_mbox_trypost(sys_mbox_t *q, void *msg)
{
	unsigned long new_head;
	//BOOL ret;
	SYS_ARCH_DECL_PROTECT(lev);

	SYS_ARCH_PROTECT(lev);

	new_head = q->head + 1;
	if (new_head >= MAX_QUEUE_ENTRIES) 
		new_head = 0;

	if (new_head == q->tail) 
	{
		SYS_ARCH_UNPROTECT(lev);
		return ERR_MEM;
	}

	q->q_mem[q->head] = msg;
	q->head = new_head;

	//ret = ReleaseSemaphore(q->sem, 1, 0);

	SYS_ARCH_UNPROTECT(lev);
	return ERR_OK;
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *q, void *msg)
{
	return sys_mbox_trypost(q, msg);
}

unsigned long sys_arch_mbox_fetch(sys_mbox_t *q, void **msg, unsigned long timeout)
{
	unsigned long ret;
	unsigned long long starttime, endtime;
	SYS_ARCH_DECL_PROTECT(lev);

	if (timeout == 0) 
		timeout = 0xFFFFFFFF;//INFINITE;

	starttime = (unsigned long)BSP_GetTickCount();
	//ret = WaitForSingleObject(q->sem, timeout);
	//if (ret == WAIT_OBJECT_0) 
	{
		SYS_ARCH_PROTECT(lev);
		if (msg != NULL) 
			*msg = q->q_mem[q->tail];

		q->tail++;
		if (q->tail >= MAX_QUEUE_ENTRIES) 
			q->tail = 0;

		SYS_ARCH_UNPROTECT(lev);

		endtime = (unsigned long)BSP_GetTickCount();
		return msDiff(endtime, starttime);
	}

	if (msg != NULL) 
		*msg = NULL;

	return SYS_ARCH_TIMEOUT;
}

unsigned long sys_arch_mbox_tryfetch(sys_mbox_t *q, void **msg)
{
	unsigned long ret;
	SYS_ARCH_DECL_PROTECT(lev);

	//ret = WaitForSingleObject(q->sem, 0);
	//if (ret == WAIT_OBJECT_0) 
	{
		SYS_ARCH_PROTECT(lev);
		if (msg != NULL) 
			*msg = q->q_mem[q->tail];

		q->tail++;
		if (q->tail >= MAX_QUEUE_ENTRIES) 
			q->tail = 0;

		SYS_ARCH_UNPROTECT(lev);
		return 0;
	}

	if (msg != NULL) 
		*msg = NULL;

	return SYS_MBOX_EMPTY;
}

////////////////////////////////////////////////////////////////////////
// The following mutex functions must be implemented by the sys_arch
////////////////////////////////////////////////////////////////////////

err_t sys_mutex_new(sys_mutex_t *mutex)
{
	sys_mutex_t* new_mut = (sys_mutex_t*)1;//CreateMutex(NULL, FALSE, NULL);
	if (new_mut != NULL) 
	{
		mutex->mut = new_mut;
		return ERR_OK;
	}
	mutex->mut = NULL;
	return ERR_MEM;
}

void sys_mutex_free(sys_mutex_t *mutex)
{
	if (mutex->mut != NULL)
		;//CloseHandle(mutex->mut);
	mutex->mut = NULL;
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
	;//WaitForSingleObject(mutex->mut, INFINITE);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
	;//ReleaseMutex(mutex->mut);
}

////////////////////////////////////////////////////////////////////////
// The following timeouts functions must be implemented by the sys_arch
////////////////////////////////////////////////////////////////////////

struct sys_timeouts* sys_arch_timeouts(void)
{
	return (struct sys_timeouts*)0;
}

////////////////////////////////////////////////////////////////////////
// The following threads functions are supported by the underlying operating system
////////////////////////////////////////////////////////////////////////
	
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
	return 0;
}

/*
//////////////////////////////////////////////////////////////////////
 This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions.
////////////////////////////////////////////////////////////////////////
*/
sys_prot_t sys_arch_protect(void)
{
}

void sys_arch_unprotect(sys_prot_t pval)
{
}

#endif //NO_OS

char* gmt4http(time_t* t, char* out, int maxSize)
{ //Last-Modified: Fri, 02 Oct 2015 07:28:00 GMT
	struct tm* timeptr = gmtime(t);
	snprintf(out, maxSize-1, "%.3s, %02d %.3s %d %02d:%02d:%02d GMT",
		wday_name[timeptr->tm_wday], timeptr->tm_mday,
		mon_name[timeptr->tm_mon], 1900 + timeptr->tm_year,
		timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec);
	return out;
}

extern int Strnicmp(char *str1, char *str2, int n);
time_t parseHttpDate(char* s)
{ //Last-Modified: Fri, 02 Oct 2015 07:28:00 GMT
	int i;
	struct tm timeptr;
	memset(&timeptr, 0, sizeof(struct tm));

	timeptr.tm_sec = atol(s + 23);  // seconds after the minute - [0, 60] including leap second
	timeptr.tm_min = atol(s + 20);  // minutes after the hour - [0, 59]
	timeptr.tm_hour = atol(s + 17); // hours since midnight - [0, 23]
	timeptr.tm_mday = atol(s + 5);  // day of the month - [1, 31]
	timeptr.tm_mon = -1;			// months since January - [0, 11]
	for (i = 0; i < 12; i ++)
	{
		if (Strnicmp(s+8, (char*)mon_name[i], 3) == 0)
		{
			timeptr.tm_mon = i;
			break;
		}
	}
	timeptr.tm_year = atol(s + 12)-1900;  // years since 1900

	return mktime(&timeptr);
}

#include <time.h>

char* getClock(char* buf, int maxSize)
{
	//time_t now;
	
	RTC_TIME_Type FullTime;
	RTC_GetFullTime(LPC_RTC, &FullTime);

	//now = time(&now);
	//gmt4http(&now, buf, maxSize);
	
	snprintf(buf, maxSize-1, "%.3s, %02d %.3s %d %02d:%02d:%02d GMT",
		wday_name[FullTime.DOW], 
		FullTime.DOM,
		mon_name[FullTime.MONTH-1], 
		FullTime.YEAR,
		FullTime.HOUR, FullTime.MIN, FullTime.SEC);
	
	return buf;
}

extern void SendLog(int toPort, char* data, int len);
void LwipLogPrint(char* format, ...)
{
	unsigned long len;
	char szMsg[512];
	va_list ap;

	RTC_TIME_Type FullTime;
	RTC_GetFullTime(LPC_RTC, &FullTime);
	
	va_start(ap, format);
	{
		snprintf(szMsg, 14, "%02d:%02d:%02d ", FullTime.HOUR, FullTime.MIN, FullTime.SEC);
		vsnprintf(szMsg + strlen(szMsg), sizeof(szMsg) - 16, format, ap);
	}
	va_end(ap);

	SendLog(8899, szMsg, strlen(szMsg));
	//printf(szMsg);

	len = strlen((const char*)szMsg);
	if ((szMsg[len - 1] != '\r') && (szMsg[len - 1] != '\n'))
		;//printf("\r\n");
}

void LWIP_sprintf(char* buf, char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
}
