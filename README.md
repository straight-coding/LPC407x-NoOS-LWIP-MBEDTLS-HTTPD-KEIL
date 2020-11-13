# LPC407x-NoOS-LWIP-MBEDTLS-HTTPD-KEIL

This project originated from [`straight-httpd`](https://github.com/straight-coding/straight-httpd), but it works on an real embedded platform. The hardware is an evaluation board and the following is the configuration. (该项目是 straight-httpd 的顺势之作，将 Windows 模拟环境的代码移植到真实的嵌入式系统，基于从简到繁的原则，本项目未采用RTOS，后续再增加对 RTOS 的支持).
* `MCU`: NXP LPC4078-FET208, Cortex-M4
* `SDRAM`: 16MB @ 0xA0000000
* `Ethernet PHY`: KSZ8041NL. This project also supports other PHYs and can automatically identify DP83848C and DM9161A as well. (该项目可以自动识别三种PHY芯片)
* `GPIO P4.23` for LED：Clear this PIN to light the LED and set this PIN to turn off the LED.(在 DHCP 获得 IP 之前闪烁间隔0.5s，拿到 IP 地址后间隔为1s)

# Configured Interrupts
* System Tick Timer: tick/1ms, 64-bit tick counter is used for timeout check. (毫秒定时用于通信超时处理)
* RTC: clock, used for log timing.(实时钟用于日志的时戳)
* ENET: configured, but not used.(网络中断虽已配置，但并无功能，接收入口放在主循环中询查)

# MAC Address Generator
* Every LPC4078 chip has a unique identification number which can be read by IAP and used to generate the MAC address.(每颗CPU都有全球唯一的ID，可用于产生其他ID、或MAC地址)

# Debug Log Redirector
* When the protocol stack is ready with a valid IP, an UDP based socket is created to broadcast log information to port 8899. (协议栈跑起来后，所有日志可以通过UDP广播到端口8899，采用网络日志的原因是：网络DMA速度非常快，足于支持大量日志的发送，而对网络通信影响并不大)

# Configuration for lwip
* lwip-port/lpc407x/lwipopts.h
```
#define NO_SYS                  1

/* MEMP_MEM_MALLOC==1: Use mem_malloc/mem_free instead of the lwip pool allocator.
 * Especially useful with MEM_LIBC_MALLOC but handle with care regarding execution
 * speed (heap alloc can be much slower than pool alloc) and usage from interrupts
 * (especially if your netif driver allocates PBUF_POOL pbufs for received frames
 * from interrupt)!
 * ATTENTION: Currently, this uses the heap for ALL pools (also for private pools,
 * not only for internal pools defined in memp_std.h)! */
#undef  MEMP_MEM_MALLOC
#define MEMP_MEM_MALLOC 		1 //Only one can be selected with MEM_USE_POOLS

/* MEM_USE_POOLS==1: Use an alternative to malloc() by allocating from a set
 * of memory pools of various sizes. When mem_malloc is called, an element of
 * the smallest pool that can provide the length needed is returned.
 * To use this, MEMP_USE_CUSTOM_POOLS also has to be enabled. */
#undef  MEM_USE_POOLS
#define MEM_USE_POOLS           0 //Only one can be selected with MEMP_MEM_MALLOC

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
//100KB to support large file downloading and uploading, and keep-alive works good
//57KB basic memory to create TLS connections, and support large file downloading and uploading, but keep-alive may fail
#undef  MEM_SIZE
#if (ENABLE_HTTPS > 0)
#define MEM_SIZE                (100*1024) 
#else
#define MEM_SIZE                (24*1024) 
#endif

/* TCP Maximum segment size. http://lwip.wikia.com/wiki/Tuning_TCP */
#undef TCP_MSS
#define TCP_MSS                 800//(1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/* TCP receive window. */
#undef TCP_WND
#if (ENABLE_HTTPS > 0)
#define TCP_WND                 (22 * TCP_MSS) //TCP_WND >= MBEDTLS_SSL_MAX_CONTENT_LEN
#else
#define TCP_WND                 (4 * TCP_MSS) //TCP_WND >= MBEDTLS_SSL_MAX_CONTENT_LEN
#endif

```

# Configuration for mbedtls
* mbedtls/include/config_user.h
```
///////////////////////////////////////////////////////////////////////////////////////
// disable MICRO
///////////////////////////////////////////////////////////////////////////////////////

#undef  MBEDTLS_HAVE_TIME
#undef  MBEDTLS_HAVE_TIME_DATE

#undef  MBEDTLS_SSL_PROTO_DTLS
#undef  MBEDTLS_SSL_DTLS_ANTI_REPLAY
#undef  MBEDTLS_SSL_DTLS_HELLO_VERIFY
#undef  MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#undef  MBEDTLS_SSL_DTLS_BADMAC_LIMIT

#undef  MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH

///////////////////////////////////////////////////////////////////////////////////////
// enable MICRO
///////////////////////////////////////////////////////////////////////////////////////

#define MBEDTLS_SSL_PROTO_TLS1
#define MBEDTLS_SSL_PROTO_TLS1_1
#define MBEDTLS_SSL_PROTO_TLS1_2

#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_NV_SEED
#define MBEDTLS_ENTROPY_C
#define ALTCP_MBEDTLS_RNG_FN					mbedtls_entropy_func 
#define MBEDTLS_PLATFORM_NV_SEED_READ_MACRO		mbedtls_platform_std_nv_seed_read
#define MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO	mbedtls_platform_std_nv_seed_write

#define MBEDTLS_HAVE_ASM

#define MBEDTLS_NO_64BIT_MULTIPLICATION

#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_C

#define MBEDTLS_SSL_MAX_CONTENT_LEN             16384
#define MBEDTLS_SSL_IN_CONTENT_LEN              16384

#ifdef _WIN32
#define MBEDTLS_SSL_OUT_CONTENT_LEN             16384
#undef  MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#else
#define MBEDTLS_SSL_OUT_CONTENT_LEN             1024    //16384
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

#define MBEDTLS_PLATFORM_SNPRINTF_MACRO			snprintf
#define MBEDTLS_PLATFORM_VSNPRINTF_MACRO		vsnprintf
#endif

//0 No debug,1 Error,2 State change,3 Informational,4 Verbose
#define MBEDTLS_DEBUG_LEVEL 					1

extern int mbedtls_platform_std_nv_seed_read(unsigned char *buf, unsigned int buf_len);
extern int mbedtls_platform_std_nv_seed_write(unsigned char *buf, unsigned int buf_len);

```
# Preprocessor
* lwip-port/lpc407x/arch/sys_arch.h
 ```
 #define LOCAL_FILE_SYSTEM	0
 ```
* lwip-port/lpc407x/lwip_api.c
 ```
 #define KEYSIZE		512
 ```
* httpd/http_core.h
```
#define ALWAYS_REDIRECT_HTTPS	1
#define CONNECTION_HEADER	"keep-alive" //"close"
#define LWIP_GetTickCount 	BSP_GetTickCount
#define MAX_CONNECTIONS 				4
```

* httpd/http_fs.c
```
#define FAKE_FS		1
```

# Web Demo

* httpd/cgi/cgi_upload.c
* httpd/cgi/cgi_files.c
