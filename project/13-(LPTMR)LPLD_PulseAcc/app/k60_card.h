/**
 * @file k60_card.h
 * @version 3.1[By LPLD]
 * @date 2015-01-11
 * @brief ��������K60���İ������ļ�
 *
 * ���Ľ���:�ɸ��ݹ��������޸�
 *
 * **������ÿ�����̵�app�ļ����ڰ������ļ���**
 * ��ͷ�ļ�Ϊ��������K60�ײ��������������ļ���
 * ��������MCU�ͺš���Ƶ��������Ϣ�Ȳ�����
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
#ifndef __K60_CARD_H__
#define __K60_CARD_H__
 /*----------------------------------------------------------------------------
  �����Լ���Ƶĵ�Ƭ����Χ�����·��ѡ����ʵľ�������,Ĭ��Ϊ��Դ���� 50Mhz
  �������ѡ��Ϊ��Դ����CANNED OSC��,����OSC_CIRCUIT_TPYE == CANNED_OSC_CIRCUIT
 Vcc_   _
    |__| |___ XTAL Pin
       |_|
  �������ѡ��Ϊ��Դ����CRYSTAL OSC��,����OSC_CIRCUIT_TPYE == CRYSTAL_OSC_CIRCUIT
        ________XTAL Pin
       | |
       |_|______EXTAL Pin
*----------------------------------------------------------------------------*/
#define CRYSTAL_OSC_CIRCUIT (0)
#define CANNED_OSC_CIRCUIT  (1)
#define OSC_CIRCUIT_TPYE    CANNED_OSC_CIRCUIT
/*----------------------------------------------------------------------------
  �������ѡ��Ϊ��Դ����CANNED OSC��,����ʹ�þ���Ƶ�ʣ�֧����Դ����Ƶ��Ϊ50Mhz
 *----------------------------------------------------------------------------*/
#define CANNED_OSC_CLK_HZ                 50000000u       //�ⲿ��Դ����Ƶ�ʣ���λHz
/*----------------------------------------------------------------------------
  �������ѡ��Ϊ��Դ����CRYSTAL OSC��,����ʹ�þ���Ƶ�ʣ�֧����Դ����Ƶ��Ϊ8Mhz
 *----------------------------------------------------------------------------*/
#define CRYSTAL_OSC_CLK_HZ                8000000u        //�ⲿ��Դ����Ƶ�ʣ���λHz
   
/*
 * ϵͳ��Ƶ����(CORE_CLK_MHZ)
 * ��ѡ��Χ:
 * <MK60DZ10>
 * PLL_48        //48MHz ���ʹ��USBģ�飬����ѡ��48�ı�����Ƶ
 * PLL_50        //50MHz
 * PLL_96        //96MHz ���ʹ��USBģ�飬����ѡ��48�ı�����Ƶ
 * PLL_100       //100MHz ��MK60DZ10�Ķ�����Ƶ�������Ϸ�˼�����ɲ��ʱ�
 * PLL_120       //120MHz
 * PLL_150       //150MHz
 * PLL_180       //180MHz
 * PLL_200       //200MHz �������200�����Ⱥ����������˱�����>_>
 *
 * LPLD��ʾ��MK60DZ10���Ƽ�ʹ��100MHz����Ƶ�ʣ��������𲻿��޸��Ĺ���
 *           ���б�ΪLPLD�̼����г��ĳ���Ƶ�ʣ�����������ֹ��ܡ���������
 * <MK60F12 and MK60F15>
 * PLL_100       //100MHz 
 * PLL_120       //120MHz ��MK60F12�Ķ�����Ƶ�������Ϸ�˼�����ɲ��ʱ�
 * PLL_150       //150MHz ��MK60F15�Ķ�����Ƶ�������Ϸ�˼�����ɲ��ʱ�
 * PLL_180       //180MHz
 * PLL_200       //200MHz �������200�����Ⱥ����������˱�����>_>
 */
#if defined(USE_K60DZ10)
  #define CORE_CLK_MHZ        PLL_96
#elif defined(USE_K60D10)
  #define CORE_CLK_MHZ        PLL_96
#elif defined(USE_K60F12)
  #define CORE_CLK_MHZ        PLL_120
#elif defined(USE_K60F15)
  #define CORE_CLK_MHZ        PLL_150
#endif

   
/*
 * ����Ƶ�ʶ���(BUS_CLK_MHZ)
 * ��ѡ��Χ:
 *   ������ϵͳ��Ƶ�������鲻����100������100�������ܷɡ���������
 *
 * LPLD��ʾ��MK60DZ10��������Ƶ��Ϊ50MHz
 *           �˴����õ�Ϊ����ֵ�����������Ƶ�ʲ�����ڴ�Ƶ��
*/
#if defined(USE_K60DZ10)
  #define BUS_CLK_MHZ         50u   
#elif defined(USE_K60D10)
  #define BUS_CLK_MHZ         50u 
#elif (defined(USE_K60F12) || defined(USE_K60F15))
  #define BUS_CLK_MHZ         60u
#endif   
/*
 * Flex����Ƶ�ʶ���(FLEXBUS_CLK_MHZ)
 * ��ѡ��Χ:
 *   ������50
 *
 * LPLD��ʾ��MK60DZ10����Flex����Ƶ��Ϊ50MHz
 *           �˴����õ�Ϊ����ֵ�����������Ƶ�ʲ�����ڴ�Ƶ��
*/
#define FLEXBUS_CLK_MHZ     50u 
   
/*
 * FlashƵ�ʶ���(FLASH_CLK_MHZ)
 * ��ѡ��Χ:
 *   ������25
 *
 * LPLD��ʾ��MK60DZ10����FlashƵ��Ϊ25MHz
 *           �˴����õ�Ϊ����ֵ�����������Ƶ�ʲ�����ڴ�Ƶ��
*/
#define FLASH_CLK_MHZ       25u    


/* 
 * ѡ�������Ϣ������еĴ��ںš����źͲ�����
 */
#define TERM_PORT           UART5
#define TERM_RX_PIN         PTE9
#define TERM_TX_PIN         PTE8
#define TERMINAL_BAUD       115200

/*
 * �Ƿ����ô�����ʾ������Ϣ
 * ������ã����İ��������ʱͨ��UART5��ʾ���������Ϣ��ռ��PTE8��PTE9�����˿�
 * 1��ʾ  0����ʾ
 */
#define PRINT_ON_OFF    1

#if (PRINT_ON_OFF == 0)            
  #undef DEBUG_PRINT
#endif
   
/*
 * �Ƿ����ö��Ժ����жϲ����Ϸ���
 * ������ã������Ӳ��ֿ⺯������ʱ�����жϲ����ĺϷ���
 * 1ʹ��  0��ʹ��
 */
#define ASSERT_ON_OFF   1

#if (ASSERT_ON_OFF == 0)            
  #undef DEBUG_ASSERT
#endif

/*
 * �Ƿ�ʹ��uCOS II
 * 1ʹ��  0��ʹ��
 */
#define UCOS_II         0

/*
 * �Ƿ�ʹ��FatFs
 * 1ʹ��  0��ʹ��
 */
#define USE_FATFS       0

/*
 * ѡ��ǰUSB�豸������
 * USB_DEVICE_CLASS_CDC
 * USB_DEVICE_CLASS_HID
 */
#define USB_DEVICE_CLASS USB_DEVICE_CLASS_CDC

/*
 * ����MCU�ͺ�
 */
#if defined(USE_K60DZ10)
  #define CPU_MK60DZ10   
#elif defined(USE_K60D10)
  #define CPU_MK60D10 
#elif defined(USE_K60F12)
  #define CPU_MK60F12
#elif defined(USE_K60F15)
  #define CPU_MK60F15
#else
  #error "δ����CPU����"
#endif  



#endif /* __K60_CARD_H__ */
