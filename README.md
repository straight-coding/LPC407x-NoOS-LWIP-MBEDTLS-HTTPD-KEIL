# LPC407x-NoOS-LWIP-MBEDTLS-HTTPD-KEIL

The project originated from [straight-httpd](https://github.com/straight-coding/straight-httpd), but it works on an real embedded platform. This project is implemented on an evaluation board and the following is the hardware configuration.
* MCU: NXP LPC4078-FET208, Cortex-M4
* SDRAM: 16MB @ 0xA0000000
* Ethernet PHY: KSZ8041NL. This project also supports other PHYs and can automatically identify DP83848C and DM9161A as well.
* GPIO P4.23 for LED：Clear this PIN to light the LED and set this PIN to turn off the LED.
