# LPC407x-NoOS-LWIP-MBEDTLS-HTTPD-KEIL

This project originates from [`straight-httpd`](https://github.com/straight-coding/straight-httpd), but it works on an real embedded platform. The hardware is an evaluation board and the following is the configuration. (该项目是 straight-httpd 的顺势之作，将 Windows 模拟环境的代码移植到真实的嵌入式系统，基于从简到繁的原则，本项目未采用RTOS，后续会再增加对 RTOS 的支持).
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
//If there is no external SDRAM, https can not work because there is only 64kB on-chip RAM available. 
//(支持https需要更多的内存RAM，如果内存不足128kB建议ENABLE_HTTPS设置为0，即仅支持HTTP)
#define ENABLE_HTTPS      1

//No RTOS
#define NO_SYS            1

//always use the lwip allocator for both lwip and mbedtls.
#undef  MEMP_MEM_MALLOC
#define MEMP_MEM_MALLOC   1 //Only one can be selected with MEM_USE_POOLS

//do not use memory pool
#undef  MEM_USE_POOLS
#define MEM_USE_POOLS     0 //Only one can be selected with MEMP_MEM_MALLOC

//total memory size for the lwip allocator, and HTTPS may need much more memory
#undef  MEM_SIZE
#if (ENABLE_HTTPS > 0)
#define MEM_SIZE          (100*1024) 
#else
#define MEM_SIZE          (24*1024) 
#endif

//use smaller tcp segment to reduce the memory requirement.
//as the segment size decreases, the throughput will decrease too.
//(如果仅用HTTP，MSS可以再减小，以减少内存开销)
#undef TCP_MSS
#define TCP_MSS           800//(1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

//TCP receive window.
//for HTTP, default TCP_WND is OK
//for HTTPS, TCP_WND should be greater than MBEDTLS_SSL_MAX_CONTENT_LEN for uploading huge data
//    please refer to the comments about MBEDTLS_SSL_MAX_CONTENT_LEN in file mbedtls/include/config.h
//(如果仅用HTTP，WND可以采用默认值，如果使用https，则不得少于MBEDTLS_SSL_MAX_CONTENT_LEN，否则大文件（固件升级）上载会失败)
#undef TCP_WND
#if (ENABLE_HTTPS > 0)
#define TCP_WND          (22 * TCP_MSS) //TCP_WND >= MBEDTLS_SSL_MAX_CONTENT_LEN
#else
#define TCP_WND          (4 * TCP_MSS) //TCP_WND >= MBEDTLS_SSL_MAX_CONTENT_LEN
#endif
```

# Configuration for mbedtls
* mbedtls/include/config_user.h, options defined in this file will overwrite the existed
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

//如果仅仅是网页应用，不需要大文件上载（如固件升级），则可以采用下面这项
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
#define ALTCP_MBEDTLS_RNG_FN                   mbedtls_entropy_func 
#define MBEDTLS_PLATFORM_NV_SEED_READ_MACRO    mbedtls_platform_std_nv_seed_read
#define MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO   mbedtls_platform_std_nv_seed_write

#define MBEDTLS_HAVE_ASM

#define MBEDTLS_NO_64BIT_MULTIPLICATION

#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_C

#define MBEDTLS_SSL_MAX_CONTENT_LEN            16384
#define MBEDTLS_SSL_IN_CONTENT_LEN             16384

#ifdef _WIN32
#define MBEDTLS_SSL_OUT_CONTENT_LEN            16384
#undef  MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#else
#define MBEDTLS_SSL_OUT_CONTENT_LEN            1024    //16384
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

#define MBEDTLS_PLATFORM_SNPRINTF_MACRO        snprintf
#define MBEDTLS_PLATFORM_VSNPRINTF_MACRO       vsnprintf
#endif

//0 No debug,1 Error,2 State change,3 Informational,4 Verbose
#define MBEDTLS_DEBUG_LEVEL                    1

//The following functions use fixed data as the entropy seed. 
//This seed is used only for experimental purposes 
//   and it is strongly recommended that it MUST not be used for the official products.
//mbedtls_platform_std_nv_seed_read读取的随机数种子数据，可以用随机事件积累产生
extern int mbedtls_platform_std_nv_seed_read(unsigned char *buf, unsigned int buf_len);
extern int mbedtls_platform_std_nv_seed_write(unsigned char *buf, unsigned int buf_len);

```
# Preprocessor
* lwip-port/lpc407x/arch/sys_arch.h
 ```
 //There is no local file system in this project
 #define LOCAL_FILE_SYSTEM     0
 ```
* lwip-port/lpc407x/lwip_api.c
 ```
 //The key size of the server ceritificate.
 //use 512 bits because there is no hardware cryptographic engines.
 #define KEYSIZE               512
 ```
* httpd/http_core.h
```
//redirect all http request to https
#define ALWAYS_REDIRECT_HTTPS	 1

//"keep-alive" will reuse existing connections and occupy more connection resources and memory, recommended for HTTPS
//set "close" to close connection after used to free memory, recommended for http. 
//若无加密硬件支持，则https建立连接的过程非常慢，因此建议重复使用连接，但会长时间占用更多并发连接数和内存
//如果仅用http，则建议用“close”以快速释放内存
#define CONNECTION_HEADER      "keep-alive" //"close"

//interface to get system tick counter
#define LWIP_GetTickCount     BSP_GetTickCount

//max. number of http connections. increase MAX_CONNECTIONS when CONNECTION_HEADER is "keep-alive"
//如果CONNECTION_HEADER 设为 "keep-alive"，则需要适当增加 MAX_CONNECTIONS，因为有些浏览器会同时使用多达4个以上的连接
#define MAX_CONNECTIONS        4
```

* httpd/http_fs.c
```
//used fake files to test file transfer performance including uploading and downloading
//虚假的文件用于测试上载和下载传输性能
#define FAKE_FS                1
```

# Web Access Demo

* httpd/cgi/cgi_upload.c, for uploading test, data will be discarded when received.
* httpd/cgi/cgi_files.c, for downloading test, fixed data will be trandferred

# Scatter File

* LambdaIOT.sct, if there is no external SDRAM, just comment the block "RW_SDRAM"
```
; *************************************************************
; *** Scatter-Loading Description File generated by uVision ***
; *************************************************************
LR_IROM1 0x00000000 0x00080000  {    ; load region size_region, for work code
  ER_IROM1 0x00000000 0x00080000  {  ; load address = execution address
   *.o (RESET, +First)
   *(InRoot$$Sections)
   .ANY (+RO)
  }
  RW_IRAM1 0x10000060 0x00010000  {  ; RW data
   .ANY (+RW +ZI)
  }
  RW_SDRAM 0xA0000000 0x01000000  {  ; RW data
   .ANY (+RW +ZI)	;//for dynamic assign
  }
}
```

# Update Web Pages
* Web pages and scripts are located `httpd/webroot`.
* `buildfs.bat` can be used to convert web pages and scripts into `httpd/fs_data.c`, then rebuild the firmware to include all changes.

# More Details
* Please see the document of project [`straight-httpd`](https://github.com/straight-coding/straight-httpd)
