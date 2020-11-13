# LPC407x-NoOS-LWIP-MBEDTLS-HTTPD-KEIL

This project originated from [`straight-httpd`](https://github.com/straight-coding/straight-httpd), but it works on an real embedded platform. The hardware is an evaluation board and the following is the configuration. (该项目是 straight-httpd 的后续之作，将 Windows 模拟环境的代码移植到真实的嵌入式系统，基于从简到繁的原则，本项目未采用RTOS，后续再增加对 RTOS 的支持).
* `MCU`: NXP LPC4078-FET208, Cortex-M4
* `SDRAM`: 16MB @ 0xA0000000
* `Ethernet PHY`: KSZ8041NL. This project also supports other PHYs and can automatically identify DP83848C and DM9161A as well. (该项目可以自动识别三种PHY芯片)
* `GPIO P4.23` for LED：Clear this PIN to light the LED and set this PIN to turn off the LED.(在 DHCP 获得 IP 之前闪烁间隔0.5s，拿到 IP 地址后间隔为1s)

# Configured Interrupts
* System Tick Timer: tick/1ms, 64-bit tick counter is used for timeout check.
* RTC: clock, used for log timing.
* ENET: configured, but not used.

# MAC Generator
* Every LPC4078 chip has a unique identification number which can be used to generate MAC address.

