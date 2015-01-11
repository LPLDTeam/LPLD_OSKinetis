# LPLD_OSKinetis
拉普兰德开源Kinetis固件库   
官方网站：[www.lpld.cn](http://www.lpld.cn)   
在线文档：[wiki.lpld.cn](http://wiki.lpld.cn)   

***
## 欢迎使用OSKinetis
我们将OSKinetis代码托管到GitHub，并随时保持更新。你可以在这里获取最新版本的固件库，**但是这是开发版本，如果你需要获取我们正式发布的稳定版本，请直接到我们的官网获取**。   
[GitHub地址](https://github.com/LPLDTeam/LPLD_OSKinetis/)  
OSKinetis是拉普兰德开发并维护的基于C语言编写的Kinetis K系列单片机固件库（驱动）。它是免费的、开源的代码，你可以自由使用本代码，但是请勿作为闭源软件发布，请勿再未得到LPLD许可的情况下用于商业软件。

***
## 日志 ##
1/11/2015 4:30:31 PM  
- 为了适应更多的Kinetis芯片，修改工程文件，将不同系列芯片的project添加到同一个IAR的workspace下。例如workpspace的文件名为`LPLD_HelloWorld.eww`，那么它就包含了以下不同芯片的project,分别为`LPLD_HelloWorld_K60DZ10.ewp`，`LPLD_HelloWorld_K60F12.ewp`等。  
- 修改不同CPU的宏定义名称，使之更加具体针对某一系列的CPU。例如将`USE_K60D`改为`USE_K60DZ10`，代表本工程只针对MK60DZ10系列的单片机。  
- 添加几个CPU的宏定义:`USE_K60D10`,`USE_K60F12`。  
- 修改.s启动文件名，使之更加具体针对某一系列的CPU。  
- 重命名.icf文件名，统一格式为`LPLD_{CpuName}_{RAM|FLASH|BOOT}`


