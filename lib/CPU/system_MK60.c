/**
 * @file system_MK60.c
 * @version 1.2.1[By LPLD]
 * @date 2013-06-18
 * @brief MK60系列单片机系统配置文件
 *
 * 更改建议:禁止修改
 *
 * 该代码提供系统配置函数以及一个储存系统主频的全局变量。
 * 配置函数主要负责建立系统各模块的时钟。
 * 代码还实现常见的系统中断函数。
 *
 * 版权所有:北京拉普兰德电子技术有限公司
 * http://www.lpld.cn
 * mail:support@lpld.cn
 *
 * @par
 * 本代码由拉普兰德[LPLD]开发并维护，并向所有使用者开放源代码。
 * 开发者可以随意修使用或改源代码。但本段及以上注释应予以保留。
 * 不得更改或删除原版权所有者姓名，二次开发者可以加注二次版权所有者。
 * 但应在遵守此协议的基础上，开放源代码、不得出售代码本身。
 * 拉普兰德不负责由于使用本代码所带来的任何事故、法律责任或相关不良影响。
 * 拉普兰德无义务解释、说明本代码的具体原理、功能、实现方法。
 * 除非拉普兰德[LPLD]授权，开发者不得将本代码用于商业产品。
 */

#include <stdint.h>
#include "common.h"

#define DISABLE_WDOG    1

/*----------------------------------------------------------------------------
  定义时钟相关值
 *----------------------------------------------------------------------------*/
#define CPU_XTAL_CLK_HZ                 50000000u       //外部有源晶振频率，单位Hz
#define CPU_XTAL32k_CLK_HZ              32768u          //外部32k时钟晶振频率，单位Hz    
#define CPU_INT_SLOW_CLK_HZ             32768u          //慢速内部振荡器的值，单位Hz
#define CPU_INT_FAST_CLK_HZ             4000000u        //快速内部振荡器的值，单位Hz
#define DEFAULT_SYSTEM_CLOCK            100000000u      //默认系统主频，单位Hz

static void flash_identify (void);
static void cpu_identify (void);
static void Diagnostic_Reset_Source(void);
/**
 * @brief 系统主频（单位Hz）
 */
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/**
 * 初始化系统
 *
 * @param  none
 * @return none
 *
 * @brief  初始化MCU硬件系统，包括开放IO时钟、初始化主频、初始化调试串口，
 *         打印调试信息。SystemInit由MCU启动文件调用。
 *         
 */
void SystemInit (void) {
  UART_InitTypeDef term_port_structure;
  //使能全部IO时钟
  SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK
              | SIM_SCGC5_PORTB_MASK
              | SIM_SCGC5_PORTC_MASK
              | SIM_SCGC5_PORTD_MASK
              | SIM_SCGC5_PORTE_MASK );
#if (DISABLE_WDOG)
  //禁用看门狗模块
  /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG->UNLOCK = (uint16_t)0xC520u;     /* Key 1 */
  /* WDOG_UNLOCK : WDOGUNLOCK=0xD928 */
  WDOG->UNLOCK  = (uint16_t)0xD928u;    /* Key 2 */
  /* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,STNDBYEN=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  WDOG->STCTRLH = (uint16_t)0x01D2u;
#endif /* (DISABLE_WDOG) */
  
  //将中断向量表、需在RAM中运行的函数等数据拷贝到RAM中
  common_relocate();
  
  //初始化各部分时钟：系统内核主频、总线时钟、FlexBus时钟、Flash时钟
  LPLD_PLL_Setup(CORE_CLK_MHZ);
  
  //更新内核主频
  SystemCoreClockUpdate();
  
  //获取各部分时钟
  g_core_clock = SystemCoreClock;
  g_bus_clock = g_core_clock / ((uint32_t)((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> SIM_CLKDIV1_OUTDIV2_SHIFT)+ 1u);
  g_flexbus_clock =  g_core_clock / ((uint32_t)((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV3_MASK) >> SIM_CLKDIV1_OUTDIV3_SHIFT)+ 1u);
  g_flash_clock =  g_core_clock / ((uint32_t)((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >> SIM_CLKDIV1_OUTDIV4_SHIFT)+ 1u);
  
  //初始化用于打印调试信息的串口模块
  //TERM_PORT为UART通道，在k60_card.h中定义
  //TERMINAL_BAUD为UART波特率，在k60_card.h中定义
  term_port_structure.UART_Uartx = TERM_PORT;
  term_port_structure.UART_BaudRate = TERMINAL_BAUD;
  LPLD_UART_Init(term_port_structure);
  
  //打印系统调试信息
#ifdef DEBUG_PRINT     
  printf("\r\n");   
  printf("*********** 基于拉普兰德K60底层库 http://www.lpld.cn ***********\r\n");
  printf("OSKinetis固件库版本:%s\tmail:support@lpld.cn\r\n", OSKinetis_Version);
  printf("系统内核时钟:%dMHz\t总线时钟:%dMHz\r\n", g_core_clock/1000000u, g_bus_clock/1000000u);
  printf("FlexBus时钟:%dMHz\tFlash时钟:%dMHz\r\n", g_flexbus_clock/1000000u, g_flash_clock/1000000u);
  printf("系统启动完毕，若要禁用调试输出请定义PRINT_ON_OFF为1(k60_card.h)\r\n");
  cpu_identify();
  Diagnostic_Reset_Source();
  printf("********************************************************************\r\n");
#endif
  
}

/**
 * 更新SystemCoreClock
 *
 * @param  none
 * @return none
 *
 * @brief  更新全局变量SystemCoreClock的值，以便获取最新的系统内核频率。
 *         
 */
void SystemCoreClockUpdate (void) {
  uint32_t temp;
#if (defined(CPU_MK60DZ10))
  #if 0
  temp =  CPU_XTAL_CLK_HZ *((uint32_t)(MCG->C6 & MCG_C6_VDIV_MASK) + 24u );
  temp = (uint32_t)(temp/((uint32_t)(MCG->C5 & MCG_C5_PRDIV_MASK) +1u ));
  #endif
  temp =  8000000u *((uint32_t)(MCG->C6 & MCG_C6_VDIV_MASK) + 24u );
  temp = (uint32_t)(temp/((uint32_t)(MCG->C5 & MCG_C5_PRDIV_MASK) +1u ));
#elif (defined(CPU_MK60D10))
  temp =  CPU_XTAL_CLK_HZ *((uint32_t)(MCG->C6 & MCG_C6_VDIV0_MASK) + 24u );
  temp = (uint32_t)(temp/((uint32_t)(MCG->C5 & MCG_C5_PRDIV0_MASK) +1u ));
  temp = temp;
#elif (defined(CPU_MK60F12) || defined(CPU_MK60F15))
  temp =  CPU_XTAL_CLK_HZ /((uint32_t)(MCG->C5 & MCG_C5_PRDIV0_MASK) + 1u );
  temp = (uint32_t)(temp*((uint32_t)(MCG->C6 & MCG_C6_VDIV0_MASK) +16u ));
  temp = temp/2;
#endif
  SystemCoreClock = temp;
}

/**
 * 硬件错误中断函数
 *
 * @param  none
 * @return none
 *
 * @brief  触发此中断的原因一般为：模块使用未初始化、错误的寻址空间等。
 *         
 */
void HardFault_Handler(void)
{
#ifdef DEBUG_PRINT 
   printf("\r\n****内核发生硬件错误*****\r\n");
#endif
   return;
}

/**
 * 默认中断函数
 *
 * @param  none
 * @return none
 *
 * @brief  未定义中断处理函数的中断源会进入此函数，并打印中断向量号。
 *         
 */
void DefaultISR(void)
{
   #define VECTORNUM                     (*(volatile uint8_t*)(0xE000ED04)) 
#ifdef DEBUG_PRINT 
   printf("\r\n****进入未定义中断,Interrupt Number %d*****\r\n",VECTORNUM-16);
#endif
   return;
}

/**
 * 初始化操作系统的滴答定时器(OS Tick)
 *
 * @param  none
 * @return none
 *
 * @brief  该函数仅在uC/OS等系统中调用，切必须在OSStart()和处理器初始化后调用
 *         
 */
#if UCOS_II > 0u
void SystemTickInit (void)
{
  uint32  cpu_clk_freq;
  uint32  cnts;

  cpu_clk_freq = g_core_clock;  //获取SysTick时钟

#if (OS_VERSION >= 30000u)
  cnts  = cpu_clk_freq / (uint32)OSCfg_TickRate_Hz;    
#else
  cnts  = cpu_clk_freq / (uint32)OS_TICKS_PER_SEC;        
#endif

  OS_CPU_SysTickInit(cnts);     //初始化uCOS滴答定时器SysTick
}
#endif

/**
 * Kinetis CPU Identify
 *
 * @param  none
 * @return none
 *
 * @brief 输出Kinetis以下信息
 * - Kinetis family
 * - package
 * - die revision
 * - P-flash size
 * - Ram size
 */
static void cpu_identify (void)
{
    /* 判断Kinetis 单片机的型号 */
    switch((SIM->SDID & SIM_SDID_FAMID(0x7))>>SIM_SDID_FAMID_SHIFT) 
    {
#ifdef DEBUG_PRINT 
    	case 0x0:printf("\nK10-");break;
    	case 0x1:printf("\nK20-");break;
    	case 0x2:printf("\nK30-");break;
    	case 0x3:printf("\nK40-");break;
    	case 0x4:printf("\nK60-");break;
    	case 0x5:printf("\nK70-");break;
    	case 0x6:printf("\nK50-");break;
    	case 0x7:printf("\nK53-");break;
	default:printf("\n不能识别单片机型号-");break; 
#else 	
        default:break; 
#endif
    }

     /* 判断Kinetis 单片机的封装 */
    switch((SIM->SDID & SIM_SDID_PINID(0xF))>>SIM_SDID_PINID_SHIFT) 
    {
#ifdef DEBUG_PRINT
    	case 0x2:printf("32pin-");break;
    	case 0x4:printf("48pin-");break;
    	case 0x5:printf("64pin-");break;
    	case 0x6:printf("80pin-");break;
    	case 0x7:printf("81pin-");break;
    	case 0x8:printf("100pin-");break;
    	case 0x9:printf("104pin-");break;
    	case 0xA:printf("144pin-");break;
    	case 0xC:printf("196pin-");break;
    	case 0xE:printf("256pin-");break;
	default:printf("不能识别单片机封装-.");break;
#else 	
        default:break; 
#endif  	
    }                

#ifdef DEBUG_PRINT
    printf("Silicon rev 1.%d\n",(SIM->SDID & SIM_SDID_REVID(0xF))>>SIM_SDID_REVID_SHIFT);
#endif  
    /* 判断Kinetis 单片机的P-flash size */
    switch((SIM->FCFG1 & SIM_FCFG1_PFSIZE(0xF))>>SIM_FCFG1_PFSIZE_SHIFT)
    {
#ifdef DEBUG_PRINT
  #if (defined(CPU_MK60DZ10) || defined(CPU_MK60D10)) 
    	case 0x7:printf("128 kBytes of P-flash	");break;
    	case 0x9:printf("256 kBytes of P-flash	");break;
        case 0xB:printf("512 kBytes of P-flash	");break;
    	case 0xF:printf("512 kBytes of P-flash	");break;
	default:printf("不能识别单片机 P-flash size\n");break;
  #elif (defined(CPU_MK60F12) || defined(CPU_MK60F15)) 
        case 0xB:printf("512 kBytes of P-flash	");break;
    	case 0xD:printf("1024 kBytes of P-flash	");break;
    	case 0xF:printf("1024 kBytes of P-flash	");break;
	default:printf("不能识别单片机 P-flash size\n");break;
  #endif
#else 	
        default:break; 
#endif
    }

#if (defined(CPU_MK60DZ10) || defined(CPU_MK60D10))
    /* 判断是否只有 P-flash  或者 P-flash 和 FlexNVM */
    if (SIM->FCFG2 & SIM_FCFG2_PFLSH_MASK) 
  #ifdef DEBUG_PRINT
      printf("P-flash only\n");
  #else 
      asm("nop");
  #endif
    else
      /* 如果单片机有FlexNVM、判断Kinetis 单片机的FlexNVM size */
      switch((SIM->FCFG1 & SIM_FCFG1_NVMSIZE(0xF))>>SIM_FCFG1_NVMSIZE_SHIFT)
      {
  #ifdef DEBUG_PRINT

      	case 0x0:printf("0 kBytes of FlexNVM\n");break;
    	case 0x7:printf("128 kBytes of FlexNVM\n");break;
        case 0x9:printf("256 kBytes of FlexNVM\n");break;
    	case 0xF:printf("256 kBytes of FlexNVM\n");break;
	default:printf("不能识别单片机 FlexNVM size\n");break;
  #else 	
        default:break; 
  #endif
      }
#endif
    
    /* 判断Kinetis 单片机的RAM size */
    switch((SIM->SOPT1 & SIM_SOPT1_RAMSIZE(0xF))>>SIM_SOPT1_RAMSIZE_SHIFT)
    {
#ifdef DEBUG_PRINT
  #if (defined(CPU_MK60DZ10) || defined(CPU_MK60D10))
    	case 0x5:printf("32 kBytes of RAM\n");break;
    	case 0x7:printf("64 kBytes of RAM\n");break;
    	case 0x8:printf("96 kBytes of RAM\n");break;
    	case 0x9:printf("128 kBytes of RAM\n");break;
	default:printf("不能识别单片机 RAM size\n");break; 
  #elif (defined(CPU_MK60F12) || defined(CPU_MK60F15))
        case 0x9:printf("128 kBytes of RAM\n");break;
	default:printf("不能识别单片机 RAM size\n");break;
  #endif
#else 	
        default:break; 
#endif
    }
    flash_identify(); 
}

/**
 * Kinetis flash Identify
 *
 * @param  none
 * @return none
 *
 * @brief 输出Kinetis以下信息
 * - flash parameter revision
 * - flash version ID
 */
static void flash_identify (void)
{
  uint8 info[8];
#if (defined(CPU_MK60DZ10) || defined(CPU_MK60D10)) 
    FTFL->FCCOB0 = 0x03;
    FTFL->FCCOB1 = 0x00;
    FTFL->FCCOB2 = 0x00;
    FTFL->FCCOB3 = 0x00;
    FTFL->FCCOB8 = 0x01;
    FTFL->FSTAT = FTFL_FSTAT_CCIF_MASK;
    while(!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK));
    info[0] = FTFL->FCCOB4; info[4] = FTFL->FCCOB8;
    info[1] = FTFL->FCCOB5; info[5] = FTFL->FCCOB9;
    info[2] = FTFL->FCCOB6; info[6] = FTFL->FCCOBA;
    info[3] = FTFL->FCCOB7; info[7] = FTFL->FCCOBB;
#ifdef DEBUG_PRINT  
    printf("Flash parameter version %d.%d.%d.%d\n",info[0],info[1],info[2],info[3]);
    printf("Flash version ID %d.%d.%d.%d\n",info[4],info[5],info[6],info[7]); 
#endif  
    FTFL->FSTAT = 0xFF;
#elif (defined(CPU_MK60F12) || defined(CPU_MK60F15))
    FTFE->FCCOB0 = 0x03;
    FTFE->FCCOB1 = 0x00;
    FTFE->FCCOB2 = 0x00;
    FTFE->FCCOB3 = 0x08;
    FTFE->FCCOB4 = 0x01;   
    FTFE->FSTAT = FTFE_FSTAT_CCIF_MASK;
    while(!(FTFE->FSTAT & FTFE_FSTAT_CCIF_MASK));
    info[0] = FTFE->FCCOB4; info[4] = FTFE->FCCOB8;
    info[1] = FTFE->FCCOB5; info[5] = FTFE->FCCOB9;
    info[2] = FTFE->FCCOB6; info[6] = FTFE->FCCOBA;
    info[3] = FTFE->FCCOB7; info[7] = FTFE->FCCOBB;
#ifdef DEBUG_PRINT    
    printf("Flash parameter version %d.%d.%d.%d\n",info[0],info[1],info[2],info[3]);
    printf("Flash version ID %d.%d.%d.%d\n",info[4],info[5],info[6],info[7]);  
#endif   
    FTFE->FSTAT = 0x7F;
#endif
}

/**
 * Diagnostic_Reset_Source
 *
 * @param  none
 * @return none
 *
 * @brief 输出Kinetis复位信息
 */
void Diagnostic_Reset_Source(void)
{
#ifdef DEBUG_PRINT 
#if (defined(CPU_MK60DZ10)) 
  /* 判断上一次复位的原因*/
  if (MC->SRSH & MC_SRSH_SW_MASK)
          printf("Software Reset\n");
  if (MC->SRSH & MC_SRSH_LOCKUP_MASK)
          printf("Core Lockup Event Reset\n");
  if (MC->SRSH & MC_SRSH_JTAG_MASK)
          printf("JTAG Reset\n");
  if (MC->SRSL & MC_SRSL_POR_MASK)
          printf("Power-on Reset\n");
  if (MC->SRSL & MC_SRSL_PIN_MASK)
          printf("External Pin Reset\n");
  if (MC->SRSL & MC_SRSL_COP_MASK)
          printf("Watchdog(COP) Reset\n");
  if (MC->SRSL & MC_SRSL_LOC_MASK)
          printf("Loss of Clock Reset\n");
  if (MC->SRSL & MC_SRSL_LVD_MASK)
          printf("Low-voltage Detect Reset\n");
  if (MC->SRSL & MC_SRSL_WAKEUP_MASK)
          printf("LLWU Reset\n");
#elif (defined(CPU_MK60F12) || defined(CPU_MK60F15) || defined(CPU_MK60D10))
  
  if (RCM->SRS1 & RCM_SRS1_SACKERR_MASK)
          printf("Stop Mode Acknowledge Error Reset\n");
  if (RCM->SRS1 & RCM_SRS1_EZPT_MASK)
          printf("EzPort Reset\n");
  if (RCM->SRS1 & RCM_SRS1_MDM_AP_MASK)
          printf("MDM-AP Reset\n");
  if (RCM->SRS1 & RCM_SRS1_SW_MASK)
          printf("Software Reset\n");
  if (RCM->SRS1 & RCM_SRS1_LOCKUP_MASK)
          printf("Core Lockup Event Reset\n");
  if (RCM->SRS1 & RCM_SRS1_JTAG_MASK)
          printf("JTAG Reset\n");
  if (RCM->SRS0 & RCM_SRS0_POR_MASK)
          printf("Power-on Reset\n");
  if (RCM->SRS0 & RCM_SRS0_PIN_MASK)
          printf("External Pin Reset\n");
  if (RCM->SRS0 & RCM_SRS0_WDOG_MASK)
          printf("Watchdog(COP) Reset\n");
  if (RCM->SRS0 & RCM_SRS0_LOC_MASK)
          printf("Loss of Clock Reset\n");
  if (RCM->SRS0 & RCM_SRS0_LVD_MASK)
          printf("Low-voltage Detect Reset\n");
  if (RCM->SRS0 & RCM_SRS0_WAKEUP_MASK)
  {
    printf("[outSRS]Wakeup bit set from low power mode exit\n");
    printf("[outSRS]SMC_PMPROT = %#02X ", (SMC->PMPROT))  ;
    printf("[outSRS]SMC_PMCTRL = %#02X ", (SMC->PMCTRL))  ;
    printf("[outSRS]SMC_VLLSCTRL = %#02X ", (SMC->VLLSCTRL))  ;
    printf("[outSRS]SMC_PMSTAT = %#02X \r\n", (SMC->PMSTAT))  ;

    if ((SMC->PMCTRL & SMC_PMCTRL_STOPM_MASK)== 3)
      printf("[outSRS] LLS exit \n") ;
    if (((SMC->PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC->VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 1))
      printf("[outSRS] VLLS1 exit \n") ;
    if (((SMC->PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC->VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 2))
      printf("[outSRS] VLLS2 exit \n") ;
    if (((SMC->PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC->VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 3))
      printf("[outSRS] VLLS3 exit \n") ; 
  }
#endif
#endif
}

