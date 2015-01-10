/**
 * --------------基于"拉普兰德K60底层库V3"的工程（LPLD_TestRUSH）-----------------
 * @file LPLD_TestRUSH.c
 * @version 0.1
 * @date 2013-10-22
 * @brief 用于整体测试RUSH Kinetis开发板
 *
 * 版权所有:北京拉普兰德电子技术有限公司
 * http://www.lpld.cn
 * mail:support@lpld.cn
 * 硬件平台:  LPLD K60 Card / LPLD K60 Nano
 *
 * 本工程基于"拉普兰德K60底层库V3"开发，
 * 所有开源代码均在"lib"文件夹下，用户不必更改该目录下代码，
 * 所有用户工程需保存在"project"文件夹下，以工程名定义文件夹名，
 * 底层库使用方法见相关文档。 
 *
 */
#include "common.h"
//包含LCD设备驱动
#include "DEV_LCD.h"
//包含SDRAM设备驱动
#include "DEV_SDRAM.h"
//触摸屏驱动
#include "DEV_Touchscreen.h"

//产品SN号长度
#define SN_LEN  6

//为简化代码，定义统一颜色背景的打印函数
#define LCD_PRINT_LINE(str) LPLD_LCD_PrintStringLine(str,COLOR_White,COLOR_Blue2)
#define LCD_PRINT(str) LPLD_LCD_PrintString(str,COLOR_White,COLOR_Blue2)

//触摸有效计数值，用于识别有效触摸
#define PRESS_CNT       (50)
//触摸按键编号
#define PAD_X   0
#define PAD_O   1
//TSI函数声明
void tsi_init(void);
void tsi_isr(void);
//TSI变量定义
TSI_InitTypeDef tsi_init_struct;
uint8 press_counter[2] = {PRESS_CNT, PRESS_CNT};

//GPIO函数声明
void init_gpio(void);
void portb_isr(void);
//GPIO变量声明
GPIO_InitTypeDef gpiob_init_struct;
GPIO_InitTypeDef gpioe_init_struct;

//ADC函数声明
void adc_init(void);
void delay(uint16);
//ADC变量定义
ADC_InitTypeDef adc_init_struct;

//ENET变量定义
ENET_InitTypeDef enet_init_struct;
//本地MAC物理地址定义
uint8  gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};

//RTC变量定义
RTC_InitTypeDef rtc_init_struct;

//用于显示字符串
uint8 strBuff[40];
//外围设备初始化检查
boolean check_sdcard(void);
boolean check_sdram(void);
boolean check_netport(void);
//错误计数
uint8 err=0;

// 用户自定义的为FatFs系统提供实时时间的函数
DWORD get_fattime (void)
{
  return ((DWORD)(2013 - 1980) << 25)	//2013年
       | ((DWORD)3 << 21)               //3月
       | ((DWORD)15 << 16)              //15日
       | ((DWORD)0 << 11)
       | ((DWORD)0 << 5)
       | ((DWORD)0 >> 1);
}

void main (void)
{
  boolean check_result;
  uint16 color;
  int16 result;
  uint16 X_Positon = 0; //声明X轴变量
  uint16 Y_Positon = 0; //声明Y轴变量

  //初始化RTC
  if(LPLD_RTC_IsRunning() == 0) 
  {  
    rtc_init_struct.RTC_Seconds = 0;
    LPLD_RTC_Init(rtc_init_struct);
  }
  
  //初始化LCD
  LPLD_LCD_Init();
  //初始化LCD后延时一段时间
  LPLD_LPTMR_DelayMs(100);
  //填充背景色
  LPLD_LCD_FillBackground(COLOR_Blue2);
  //打印开发板信息
  LCD_PRINT_LINE("******** RUSH Kinetis ********");
  //为字符串赋值
  sprintf((int8 *)strBuff, "Core CLK:%dMHZ  Bus CLK:%dMHZ", g_core_clock/1000000u, g_bus_clock/1000000u);
  LCD_PRINT_LINE(strBuff);
  
  check_result = check_sdcard();
  LCD_PRINT_LINE("Check SD card:    ");
  if(check_result)
  {
    LCD_PRINT("[OK!]");
  }
  else
  {
    err++;
    LCD_PRINT("[Error!]");
  }
       
  check_result = check_sdram();
  LCD_PRINT_LINE("Check Ext SDRAM:  ");
  if(check_result)
  {
    LCD_PRINT("[OK!]");
  }
  else
  {
    err++;
    LCD_PRINT("[Error!]");
  }
  
  check_result = check_netport();
  LCD_PRINT_LINE("Check Ether Port: ");
  if(check_result)
  {
    LCD_PRINT("[OK!]");
  }
  else
  {
    err++;
    LCD_PRINT("[Error!]");
  }
  
  //初始化输入设备
  tsi_init();
  init_gpio();
  adc_init();
  LPLD_Touchscreen_Init(); 
  
  LCD_PRINT_LINE("Check TouchPad\\Button\\ADC\\TouchScreen:");
  LCD_PRINT_LINE("  TouchPad:");
  LCD_PRINT_LINE("  Button:");
  LCD_PRINT_LINE("  ADC:");
  LCD_PRINT_LINE("  TouchScreen: X-      Y-");

  if(err==0)
  {
    LPLD_LCD_ShowString(220,198,"Test Passed!",COLOR_Green,COLOR_Blue2);
  }
  else
  {
    LPLD_LCD_ShowString(220,198,"Test Failed!",COLOR_Red,COLOR_Blue2);
  }
  
  while(1)
  {
    //显示差分ADC采集值
    result = (int16)LPLD_ADC_Get(ADC0, DAD1); 
    color = result>0?COLOR_Cyan:COLOR_Magenta;
    sprintf((int8 *)strBuff, "%5d", result);
    LPLD_LCD_ShowString(88,162,strBuff,color,COLOR_Blue2);
      
    //显示触摸屏XY值
    //如果LCD IRQ引脚为低代表有触摸
    if(PTE12_I == 0)
    {
      X_Positon=LPLD_Touchscreen_GetResult(LPLD_SER_SAMPLE_X);
      Y_Positon=LPLD_Touchscreen_GetResult(LPLD_SER_SAMPLE_Y);
      sprintf((int8 *)strBuff, "%4d", X_Positon);
      LPLD_LCD_ShowString(144,180,strBuff,COLOR_Magenta,COLOR_Blue2);
      sprintf((int8 *)strBuff, "%4d", Y_Positon);
      LPLD_LCD_ShowString(200,180,strBuff,COLOR_Cyan,COLOR_Blue2);
    }
    
    //显示系统运行时间 
    sprintf((int8 *)strBuff, "Run Time:%ds", LPLD_RTC_GetRealTime());
    LPLD_LCD_ShowString(0,218,strBuff,COLOR_Black,COLOR_Blue2);
    
    delay(500);
  } 
}


boolean check_sdcard(void)
{
  // 以下变量定义均采用FatFs文件系统变量类型
  FRESULT rc;			//结果代码 
  FATFS fatfs;			// 文件系统对象 
  FIL fil;			// 文件对象 
  UINT br;
  BYTE buff[SN_LEN+4] = "SN:";
  
  // 注册一个磁盘工作区 
  f_mount(0, &fatfs);
  
  //打开SN文件
  rc = f_open(&fil, "0:/SN", FA_READ);
  if (rc) 
    return FALSE;
  
  //打印出文件内的内容
  for (;;) 
  {
    rc = f_read(&fil, buff+3, sizeof(buff)-3, &br);	// 读取文件的一块 
    if (rc || !br) break;			// 错误或读取完毕 
    buff[SN_LEN+3] = '\0';
  }
  LCD_PRINT_LINE(buff);
  if (rc)
    return FALSE;
  
  //关闭文件
  rc = f_close(&fil);
  
  if (rc) 
    return FALSE;
  
  return TRUE;
}


boolean check_sdram(void)
{ 
  uint8 *buff;
  //初始化SDRAM
  //包含：
  //  初始化FlexBus：FB1模块、256K寻址空间、8位宽读写等
  LPLD_SDRAM_Init();
  
  //动态分配SDRAM中的一段空间
  buff = LPLD_SDRAM_Malloc(10);
  
  //将数据写入动态分配的SDRAM空间
  for(uint8 i=0; i<10; i++)
  {
    buff[i] = i;
  }  
  
  //读取写入到SDRAM中的数据
  for(uint8 i=0; i<10; i++)
  {
    if(buff[i] != i)
      return FALSE;
  }
  
  return TRUE;
}


boolean check_netport(void)
{
  uint32 IDR=0;
  uint16 data;
  
  //这里仅检测以太网物理芯片是否正常
  //所以只初始化MII接口
 
  //使能ENET时钟
  SIM->SCGC2 |= SIM_SCGC2_ENET_MASK;
  //复位ENET
  ENET->ECR = ENET_ECR_RESET_MASK;
  //初始化MII接口
  LPLD_ENET_MiiInit(g_bus_clock/1000000/*MHz*/);   
  //使能GPIO引脚复用功能
  PORTB->PCR[0]  = PORT_PCR_MUX(4);  //GPIO;//RMII0_MDIO/MII0_MDIO
  PORTB->PCR[1]  = PORT_PCR_MUX(4);  //GPIO;//RMII0_MDC/MII0_MDC    
  PORTA->PCR[14] = PORT_PCR_MUX(4);  //RMII0_CRS_DV/MII0_RXDV
  PORTA->PCR[12] = PORT_PCR_MUX(4);  //RMII0_RXD1/MII0_RXD1
  PORTA->PCR[13] = PORT_PCR_MUX(4);  //RMII0_RXD0/MII0_RXD0
  PORTA->PCR[15] = PORT_PCR_MUX(4);  //RMII0_TXEN/MII0_TXEN
  PORTA->PCR[16] = PORT_PCR_MUX(4);  //RMII0_TXD0/MII0_TXD0
  PORTA->PCR[17] = PORT_PCR_MUX(4);  //RMII0_TXD1/MII0_TXD1  
  
  //读取物理芯片识别号
  LPLD_ENET_MiiRead(CFG_PHY_ADDRESS, PHY_PHYIDR1, &data );     
  IDR = (uint32)data<<16;
  LPLD_ENET_MiiRead(CFG_PHY_ADDRESS, PHY_PHYIDR2, &data );  
  IDR |= data;
  
  if(IDR == 0x20005C90)
    return TRUE;
  
  return FALSE;
}

/*
 * 初始化TSI
 *
 */
void tsi_init(void)
{
  //配置TSI初始化参数
  tsi_init_struct.TSI_Chs = TSI_Ch7 | TSI_Ch8;  //X、O触摸按键分别为7、8通道
  tsi_init_struct.TSI_ScanTriggerMode = TSI_SCAN_PERIOD;        //周期扫描模式
  tsi_init_struct.TSI_IsInitSelfCal = TRUE;     //启用初始化时自校验功能
  tsi_init_struct.TSI_EndScanOrOutRangeInt = TSI_ENDOFDCAN_INT; //使能扫描结束中断
  tsi_init_struct.TSI_EndScanIsr = tsi_isr;     //设置中断函数
  //初始化TSI
  LPLD_TSI_Init(tsi_init_struct);
  //使能TSI中断
  LPLD_TSI_EnableIrq();
}

/*
 * TSI周期扫描中断
 *
 */
void tsi_isr()
{
  uint8 valid_touch = 0;
  
  //判断7通道是否超过触摸阈值
  if(LPLD_TSI_IsChxTouched(7))
  {
    //触摸有效计数器自减
    press_counter[PAD_X]--;
    //当自减到0时，即识别为有效触摸
    if(press_counter[PAD_X]==0)
    {
      //有效触摸标志置位
      valid_touch |= (1<<PAD_X);
      //恢复有效计数器初值
      press_counter[PAD_X] = PRESS_CNT;
    }
  }
  else
  {
    //否则恢复有效计数器初值
    press_counter[PAD_X] = PRESS_CNT;
  }
  
  //方法同上
  if(LPLD_TSI_IsChxTouched(8))
  {
    press_counter[PAD_O]--;
    if(press_counter[PAD_O]==0)
    {
      valid_touch |= (1<<PAD_O);
      press_counter[PAD_O] = PRESS_CNT;
    }
  }
  else
  {
    press_counter[PAD_O] = PRESS_CNT;
  }
  
  //如果X按键为有效触摸，则打印状态
  if(valid_touch & (1<<PAD_X))
  {
    LPLD_LCD_ShowString(88,126,"X",COLOR_Magenta,COLOR_Blue2);
  }
  
  //如果O按键为有效触摸，则打印状态
  if(valid_touch & (1<<PAD_O))
  {
    LPLD_LCD_ShowString(88,126,"O",COLOR_Cyan,COLOR_Blue2);
  }
}


/*
 * 初始化连接按键的GPIO引脚、LCD触摸屏的中断引脚
 *
 */
void init_gpio()
{
  //按键初始化
  // 配置 PTB7、PTB6 为GPIO功能,输入,内部上拉，上升沿产生中断
  gpiob_init_struct.GPIO_PTx = PTB;      //PORTB
  gpiob_init_struct.GPIO_Pins = GPIO_Pin6|GPIO_Pin7;     //引脚6、7
  gpiob_init_struct.GPIO_Dir = DIR_INPUT;        //输入
  gpiob_init_struct.GPIO_PinControl = INPUT_PULL_UP|IRQC_FA;     //内部上拉|上升沿中断
  gpiob_init_struct.GPIO_Isr = portb_isr;        //中断函数
  LPLD_GPIO_Init(gpiob_init_struct);
  //使能中断
  LPLD_GPIO_EnableIrq(gpiob_init_struct);
  
  //LCD触摸中断引脚
  gpioe_init_struct.GPIO_PTx = PTE;
  gpioe_init_struct.GPIO_Pins = GPIO_Pin12;
  gpioe_init_struct.GPIO_Dir = DIR_INPUT;  
  LPLD_GPIO_Init(gpioe_init_struct);
}

/*
 * PortB中断回调函数
 *
 */
void portb_isr()
{
  //如果PTB6产生中断
  if(LPLD_GPIO_IsPinxExt(PORTB, GPIO_Pin6))
  {
    if(PTB6_I==0)
    {
      LPLD_LCD_ShowString(88,144,"S1-PTB6",COLOR_Magenta,COLOR_Blue2);
    }
  }
  
  //如果PTB7产生中断
  if(LPLD_GPIO_IsPinxExt(PORTB, GPIO_Pin7))
  {
    if(PTB7_I==0)
    {
      LPLD_LCD_ShowString(88,144,"S2-PTB7",COLOR_Cyan,COLOR_Blue2);
    }
  }
}

/*
 * 初始化ADC及其通道
 *
 */
void adc_init(void)
{
  //配置ADC采样参数
  adc_init_struct.ADC_Adcx = ADC0;
  adc_init_struct.ADC_DiffMode = ADC_DIFF;      //差分采集
  adc_init_struct.ADC_BitMode = DIFF_13BIT;     //差分13位精度
  adc_init_struct.ADC_SampleTimeCfg = SAMTIME_LONG;    //短采样时间
  adc_init_struct.ADC_HwAvgSel = HW_32AVG;       //4次硬件平均
  adc_init_struct.ADC_CalEnable = TRUE; //使能初始化校验
  //初始化ADC
  LPLD_ADC_Init(adc_init_struct);
  //为了保证复用功能不被占用，最好使能相应ADC通道
  //RUSH开发板的电位器使用DAD1通道进行采集
  //差分DAD1通道的引脚为(ADC0_DP1和ADC0_DM1)
  LPLD_ADC_Chn_Enable(ADC0, DAD1); 
}

/*
 * 延时函数
 */
void delay(uint16 n)
{
  uint16 i;
  while(n--)
  {
    for(i=0; i<1000; i++)
    {
      asm("nop");
    }
  }
}