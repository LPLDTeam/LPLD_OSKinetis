/**
 * @file system_MK60.c
 * @version 1.2.1[By LPLD]
 * @date 2013-06-18
 * @brief MK60ϵ�е�Ƭ��ϵͳ�����ļ�
 *
 * ���Ľ���:��ֹ�޸�
 *
 * �ô����ṩϵͳ���ú����Լ�һ������ϵͳ��Ƶ��ȫ�ֱ�����
 * ���ú�����Ҫ������ϵͳ��ģ���ʱ�ӡ�
 * ���뻹ʵ�ֳ�����ϵͳ�жϺ�����
 *
 * ��Ȩ����:�����������µ��Ӽ������޹�˾
 * http://www.lpld.cn
 * mail:support@lpld.cn
 *
 * @par
 * ����������������[LPLD]������ά������������ʹ���߿���Դ���롣
 * �����߿���������ʹ�û��Դ���롣�����μ�����ע��Ӧ���Ա�����
 * ���ø��Ļ�ɾ��ԭ��Ȩ���������������ο����߿��Լ�ע���ΰ�Ȩ�����ߡ�
 * ��Ӧ�����ش�Э��Ļ����ϣ�����Դ���롢���ó��۴��뱾��
 * �������²���������ʹ�ñ��������������κ��¹ʡ��������λ���ز���Ӱ�졣
 * ����������������͡�˵��������ľ���ԭ�����ܡ�ʵ�ַ�����
 * ������������[LPLD]��Ȩ�������߲��ý�������������ҵ��Ʒ��
 */

#include <stdint.h>
#include "common.h"

#define DISABLE_WDOG    1

/*----------------------------------------------------------------------------
  ����ʱ�����ֵ
 *----------------------------------------------------------------------------*/
#define CPU_XTAL32k_CLK_HZ              32768u          //�ⲿ32kʱ�Ӿ���Ƶ�ʣ���λHz    
#define CPU_INT_SLOW_CLK_HZ             32768u          //�����ڲ�������ֵ����λHz
#define CPU_INT_FAST_CLK_HZ             4000000u        //�����ڲ�������ֵ����λHz
#define DEFAULT_SYSTEM_CLOCK            100000000u      //Ĭ��ϵͳ��Ƶ����λHz

static void flash_identify (void);
static void cpu_identify (void);
static void Diagnostic_Reset_Source(void);
/**
 * @brief ϵͳ��Ƶ����λHz��
 */
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/**
 * ��ʼ��ϵͳ
 *
 * @param  none
 * @return none
 *
 * @brief  ��ʼ��MCUӲ��ϵͳ����������IOʱ�ӡ���ʼ����Ƶ����ʼ�����Դ��ڣ�
 *         ��ӡ������Ϣ��SystemInit��MCU�����ļ����á�
 *         
 */
void SystemInit (void) {
  UART_InitTypeDef term_port_structure;
  //ʹ��ȫ��IOʱ��
  SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK
              | SIM_SCGC5_PORTB_MASK
              | SIM_SCGC5_PORTC_MASK
              | SIM_SCGC5_PORTD_MASK
              | SIM_SCGC5_PORTE_MASK );
#if (DISABLE_WDOG)
  //���ÿ��Ź�ģ��
  /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG->UNLOCK = (uint16_t)0xC520u;     /* Key 1 */
  /* WDOG_UNLOCK : WDOGUNLOCK=0xD928 */
  WDOG->UNLOCK  = (uint16_t)0xD928u;    /* Key 2 */
  /* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,STNDBYEN=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  WDOG->STCTRLH = (uint16_t)0x01D2u;
#endif /* (DISABLE_WDOG) */
  
  //���ж�����������RAM�����еĺ��������ݿ�����RAM��
  common_relocate();
  
  //��ʼ��������ʱ�ӣ�ϵͳ�ں���Ƶ������ʱ�ӡ�FlexBusʱ�ӡ�Flashʱ��
  LPLD_PLL_Setup(CORE_CLK_MHZ);
  
  //�����ں���Ƶ
  SystemCoreClockUpdate();
  
  //��ȡ������ʱ��
  g_core_clock = SystemCoreClock;
  g_bus_clock = g_core_clock / ((uint32_t)((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> SIM_CLKDIV1_OUTDIV2_SHIFT)+ 1u);
  g_flexbus_clock =  g_core_clock / ((uint32_t)((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV3_MASK) >> SIM_CLKDIV1_OUTDIV3_SHIFT)+ 1u);
  g_flash_clock =  g_core_clock / ((uint32_t)((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >> SIM_CLKDIV1_OUTDIV4_SHIFT)+ 1u);
  
  //��ʼ�����ڴ�ӡ������Ϣ�Ĵ���ģ��
  //TERM_PORTΪUARTͨ������k60_card.h�ж���
  //TERM_RX_PINΪUART_RX���ţ���k60_card.h�ж���
  //TERM_TX_PINΪUART_TX���ţ���k60_card.h�ж���
  //TERMINAL_BAUDΪUART�����ʣ���k60_card.h�ж���
  term_port_structure.UART_Uartx = TERM_PORT;
  term_port_structure.UART_RxPin = TERM_RX_PIN;
  term_port_structure.UART_TxPin = TERM_TX_PIN;
  term_port_structure.UART_BaudRate = TERMINAL_BAUD;
  LPLD_UART_Init(term_port_structure);
  
  //��ӡϵͳ������Ϣ
#ifdef DEBUG_PRINT     
  printf("\r\n");   
  printf("*********** ������������K60�ײ�� http://www.lpld.cn ***********\r\n");
  printf("OSKinetis�̼���汾:%s\tmail:support@lpld.cn\r\n", OSKinetis_Version);
  printf("ϵͳ�ں�ʱ��:%dMHz\t����ʱ��:%dMHz\r\n", g_core_clock/1000000u, g_bus_clock/1000000u);
  printf("FlexBusʱ��:%dMHz\tFlashʱ��:%dMHz\r\n", g_flexbus_clock/1000000u, g_flash_clock/1000000u);
  printf("ϵͳ������ϣ���Ҫ���õ�������붨��PRINT_ON_OFFΪ1(k60_card.h)\r\n");
  cpu_identify();
  Diagnostic_Reset_Source();
  printf("********************************************************************\r\n");
#endif
  
}

/**
 * ����SystemCoreClock
 *
 * @param  none
 * @return none
 *
 * @brief  ����ȫ�ֱ���SystemCoreClock��ֵ���Ա��ȡ���µ�ϵͳ�ں�Ƶ�ʡ�
 *         
 */
void SystemCoreClockUpdate (void) {
  uint32_t temp;
#if (defined(CPU_MK60DZ10))
  #if OSC_CIRCUIT_TPYE == CANNED_OSC_CIRCUIT
  temp =  CANNED_OSC_CLK_HZ *((uint32_t)(MCG->C6 & MCG_C6_VDIV_MASK) + 24u );
  temp = (uint32_t)(temp/((uint32_t)(MCG->C5 & MCG_C5_PRDIV_MASK) +1u ));
  #elif OSC_CIRCUIT_TPYE == CRYSTAL_OSC_CIRCUIT
  temp =  CRYSTAL_OSC_CLK_HZ *((uint32_t)(MCG->C6 & MCG_C6_VDIV_MASK) + 24u );
  temp = (uint32_t)(temp/((uint32_t)(MCG->C5 & MCG_C5_PRDIV_MASK) +1u ));
  #endif
#elif (defined(CPU_MK60D10))
  #if OSC_CIRCUIT_TPYE == CANNED_OSC_CIRCUIT
  temp =  CANNED_OSC_CLK_HZ *((uint32_t)(MCG->C6 & MCG_C6_VDIV0_MASK) + 24u );
  temp = (uint32_t)(temp/((uint32_t)(MCG->C5 & MCG_C5_PRDIV0_MASK) +1u ));
  temp = temp;
  #elif OSC_CIRCUIT_TPYE == CRYSTAL_OSC_CIRCUIT
  temp =  CRYSTAL_OSC_CLK_HZ *((uint32_t)(MCG->C6 & MCG_C6_VDIV0_MASK) + 24u );
  temp = (uint32_t)(temp/((uint32_t)(MCG->C5 & MCG_C5_PRDIV0_MASK) +1u ));
  temp = temp;
  #endif
#elif (defined(CPU_MK60F12) || defined(CPU_MK60F15))
  #if OSC_CIRCUIT_TPYE == CANNED_OSC_CIRCUIT
  temp =  CANNED_OSC_CLK_HZ /((uint32_t)(MCG->C5 & MCG_C5_PRDIV0_MASK) + 1u );
  temp = (uint32_t)(temp*((uint32_t)(MCG->C6 & MCG_C6_VDIV0_MASK) +16u ));
  temp = temp/2;
  #elif OSC_CIRCUIT_TPYE == CRYSTAL_OSC_CIRCUIT
  temp =  CRYSTAL_OSC_CLK_HZ /((uint32_t)(MCG->C5 & MCG_C5_PRDIV0_MASK) + 1u );
  temp = (uint32_t)(temp*((uint32_t)(MCG->C6 & MCG_C6_VDIV0_MASK) +16u ));
  temp = temp/2;
  #endif
#endif
  SystemCoreClock = temp;
}

/**
 * Ӳ�������жϺ���
 *
 * @param  none
 * @return none
 *
 * @brief  �������жϵ�ԭ��һ��Ϊ��ģ��ʹ��δ��ʼ���������Ѱַ�ռ�ȡ�
 *         
 */
void HardFault_Handler(void)
{
#ifdef DEBUG_PRINT 
   printf("\r\n****�ں˷���Ӳ������*****\r\n");
#endif
   return;
}

/**
 * Ĭ���жϺ���
 *
 * @param  none
 * @return none
 *
 * @brief  δ�����жϴ��������ж�Դ�����˺���������ӡ�ж������š�
 *         
 */
void DefaultISR(void)
{
   #define VECTORNUM                     (*(volatile uint8_t*)(0xE000ED04)) 
#ifdef DEBUG_PRINT 
   printf("\r\n****����δ�����ж�,Interrupt Number %d*****\r\n",VECTORNUM-16);
#endif
   return;
}

/**
 * ��ʼ������ϵͳ�ĵδ�ʱ��(OS Tick)
 *
 * @param  none
 * @return none
 *
 * @brief  �ú�������uC/OS��ϵͳ�е��ã��б�����OSStart()�ʹ�������ʼ�������
 *         
 */
#if UCOS_II > 0u
void SystemTickInit (void)
{
  uint32  cpu_clk_freq;
  uint32  cnts;

  cpu_clk_freq = g_core_clock;  //��ȡSysTickʱ��

#if (OS_VERSION >= 30000u)
  cnts  = cpu_clk_freq / (uint32)OSCfg_TickRate_Hz;    
#else
  cnts  = cpu_clk_freq / (uint32)OS_TICKS_PER_SEC;        
#endif

  OS_CPU_SysTickInit(cnts);     //��ʼ��uCOS�δ�ʱ��SysTick
}
#endif

/**
 * Kinetis CPU Identify
 *
 * @param  none
 * @return none
 *
 * @brief ���Kinetis������Ϣ
 * - Kinetis family
 * - package
 * - die revision
 * - P-flash size
 * - Ram size
 */
static void cpu_identify (void)
{
    /* �ж�Kinetis ��Ƭ�����ͺ� */
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
	default:printf("\n����ʶ��Ƭ���ͺ�-");break; 
#else 	
        default:break; 
#endif
    }

     /* �ж�Kinetis ��Ƭ���ķ�װ */
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
	default:printf("����ʶ��Ƭ����װ-.");break;
#else 	
        default:break; 
#endif  	
    }                

#ifdef DEBUG_PRINT
    printf("Silicon rev 1.%d\n",(SIM->SDID & SIM_SDID_REVID(0xF))>>SIM_SDID_REVID_SHIFT);
#endif  
    /* �ж�Kinetis ��Ƭ����P-flash size */
    switch((SIM->FCFG1 & SIM_FCFG1_PFSIZE(0xF))>>SIM_FCFG1_PFSIZE_SHIFT)
    {
#ifdef DEBUG_PRINT
  #if (defined(CPU_MK60DZ10) || defined(CPU_MK60D10)) 
    	case 0x7:printf("128 kBytes of P-flash	");break;
    	case 0x9:printf("256 kBytes of P-flash	");break;
        case 0xB:printf("512 kBytes of P-flash	");break;
    	case 0xF:printf("512 kBytes of P-flash	");break;
	default:printf("����ʶ��Ƭ�� P-flash size\n");break;
  #elif (defined(CPU_MK60F12) || defined(CPU_MK60F15)) 
        case 0xB:printf("512 kBytes of P-flash	");break;
    	case 0xD:printf("1024 kBytes of P-flash	");break;
    	case 0xF:printf("1024 kBytes of P-flash	");break;
	default:printf("����ʶ��Ƭ�� P-flash size\n");break;
  #endif
#else 	
        default:break; 
#endif
    }

#if (defined(CPU_MK60DZ10) || defined(CPU_MK60D10))
    /* �ж��Ƿ�ֻ�� P-flash  ���� P-flash �� FlexNVM */
    if (SIM->FCFG2 & SIM_FCFG2_PFLSH_MASK) 
  #ifdef DEBUG_PRINT
      printf("P-flash only\n");
  #else 
      asm("nop");
  #endif
    else
      /* �����Ƭ����FlexNVM���ж�Kinetis ��Ƭ����FlexNVM size */
      switch((SIM->FCFG1 & SIM_FCFG1_NVMSIZE(0xF))>>SIM_FCFG1_NVMSIZE_SHIFT)
      {
  #ifdef DEBUG_PRINT

      	case 0x0:printf("0 kBytes of FlexNVM\n");break;
    	case 0x7:printf("128 kBytes of FlexNVM\n");break;
        case 0x9:printf("256 kBytes of FlexNVM\n");break;
    	case 0xF:printf("256 kBytes of FlexNVM\n");break;
	default:printf("����ʶ��Ƭ�� FlexNVM size\n");break;
  #else 	
        default:break; 
  #endif
      }
#endif
    
    /* �ж�Kinetis ��Ƭ����RAM size */
    switch((SIM->SOPT1 & SIM_SOPT1_RAMSIZE(0xF))>>SIM_SOPT1_RAMSIZE_SHIFT)
    {
#ifdef DEBUG_PRINT
  #if (defined(CPU_MK60DZ10) || defined(CPU_MK60D10))
    	case 0x5:printf("32 kBytes of RAM\n");break;
    	case 0x7:printf("64 kBytes of RAM\n");break;
    	case 0x8:printf("96 kBytes of RAM\n");break;
    	case 0x9:printf("128 kBytes of RAM\n");break;
	default:printf("����ʶ��Ƭ�� RAM size\n");break; 
  #elif (defined(CPU_MK60F12) || defined(CPU_MK60F15))
        case 0x9:printf("128 kBytes of RAM\n");break;
	default:printf("����ʶ��Ƭ�� RAM size\n");break;
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
 * @brief ���Kinetis������Ϣ
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
 * @brief ���Kinetis��λ��Ϣ
 */
void Diagnostic_Reset_Source(void)
{
#ifdef DEBUG_PRINT 
#if (defined(CPU_MK60DZ10)) 
  /* �ж���һ�θ�λ��ԭ��*/
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

