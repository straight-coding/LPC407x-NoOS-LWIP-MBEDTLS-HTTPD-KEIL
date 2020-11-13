# LPC407x-NoOS-LWIP-MBEDTLS-HTTPD-KEIL

This project originated from [`straight-httpd`](https://github.com/straight-coding/straight-httpd), but it works on an real embedded platform. The hardware is an evaluation board and the following is the configuration. (该项目是 straight-httpd 的后续之作，将 Windows 模拟环境的代码移植到真实的嵌入式系统，基于从简到繁的原则，本项目未采用RTOS，后续再增加对 RTOS 的支持).
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

