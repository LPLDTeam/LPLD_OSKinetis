# LPLD_OSKinetis
拉普兰德开源Kinetis固件库   
官方网站：[www.lpld.cn](http://www.lpld.cn)   
在线文档：[wiki.lpld.cn](http://wiki.lpld.cn)   

***
## 欢迎使用OSKinetis
我们将OSKinetis代码托管到GitHub，并随时保持更新。你可以在这里获取最新版本的固件库，**但是这是开发版本，如果你需要获取我们正式发布的稳定版本，请直接到我们的官网获取**。   
[GitHub地址](https://github.com/LPLDTeam/LPLD_OSKinetis/)  
OSKinetis固件库在V3.10以上版本需要使用***IAR for ARM V7.20***以上版本，下载地址见`project/例程使用说明.txt`  
OSKinetis是拉普兰德开发并维护的基于C语言编写的Kinetis K系列单片机固件库（驱动）。它是免费的、开源的代码，你可以自由使用本代码，但是请勿作为闭源软件发布，请勿再未得到LPLD许可的情况下用于商业软件。

***
## 日志 ##

2/13/2015 8:58:02 PM  
- 升级快速创建工程工具，添加创建K60F,K60D,K60DZ,升级clean功能。

2/13/2015 8:02:31 AM  
- 修改HW_UART.c和HW_UART.h，添加**UART FIFO DMA**功能，并在
`05-(UARTint)LPLD_SerialInterrupt`中添加FIFO和DMA测试程序。
- 修改system_MK60.c，增加cpu_identify()和Diagnostic_Reset_Source()函数
- 修复LPLD_SerialComm_K60F12.ewp XML引入.c文件bug。
- 修改HW_MCG.c**增加8Mhz 无源晶振启动程序**，根据无源晶振情况计算system_MK60.c中的SystemCoreClockUpdate。
- 修复system_MK60D10.h文件的endline warning。
- 在个project中的k60_card.h中添加晶振选择配置。
- **添加设备MPU6050驱动文件**。

1/18/2015 12:42:04 AM   
- 为每个例程添加K60D10芯片工程，待移植K60D10驱动。  
- 修改例程名LPLD_SDRAM为LPLD_SRAM。  

1/11/2015 4:30:31 PM  
- 为了适应更多的Kinetis芯片，修改工程文件，将不同系列芯片的project添加到同一个IAR的workspace下。例如workpspace的文件名为`LPLD_HelloWorld.eww`，那么它就包含了以下不同芯片的project,分别为`LPLD_HelloWorld_K60DZ10.ewp`，`LPLD_HelloWorld_K60F12.ewp`等。  
- 修改不同CPU的宏定义名称，使之更加具体针对某一系列的CPU。例如将`USE_K60D`改为`USE_K60DZ10`，代表本工程只针对MK60DZ10系列的单片机。  
- 添加几个CPU的宏定义:`USE_K60D10`,`USE_K60F12`。  
- 修改.s启动文件名，使之更加具体针对某一系列的CPU。  
- 重命名.icf文件名，统一格式为`LPLD_{CpuName}_{RAM|FLASH|BOOT}`
