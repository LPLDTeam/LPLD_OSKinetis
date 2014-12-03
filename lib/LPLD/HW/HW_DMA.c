/**
 * @file HW_DMA.c
 * @version 3.0[By LPLD]
 * @date 2013-06-18
 * @brief DMA底层模块相关函数
 *
 * 更改建议:不建议修改
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
#include "common.h"
#include "HW_DMA.h"

//用户自定义中断服务函数数组
DMA_ISR_CALLBACK DMA_ISR[16];


/*
 * LPLD_DMA_Init
 * 初始化eDMA模块
 * 
 * 参数:
 *    dma_init_struct--eDMA初始化结构体，
 *                        具体定义见DMA_InitTypeDef
 *
 * 输出:
 *    0--配置错误
 *    1--配置成功
 */
uint8 LPLD_DMA_Init(DMA_InitTypeDef dma_init_struct)
{
  uint8 chx = dma_init_struct.DMA_CHx;
  uint8 req = dma_init_struct.DMA_Req;
  boolean periodic_trigg = dma_init_struct.DMA_PeriodicTriggerEnable;
  uint16 major_cnt = dma_init_struct.DMA_MajorLoopCnt;
  uint32 minor_cnt = dma_init_struct.DMA_MinorByteCnt;
  uint32 src_addr = dma_init_struct.DMA_SourceAddr;
  uint8 src_dsize = dma_init_struct.DMA_SourceDataSize;
  int16 src_addroffset = dma_init_struct.DMA_SourceAddrOffset;
  int32 src_lastadj = dma_init_struct.DMA_LastSourceAddrAdj;
  uint32 dst_addr = dma_init_struct.DMA_DestAddr;
  uint8 dst_dsize = dma_init_struct.DMA_DestDataSize;
  int16 dst_addroffset = dma_init_struct.DMA_DestAddrOffset;
  int32 dst_lastadj = dma_init_struct.DMA_LastDestAddrAdj;
  boolean auto_disable = dma_init_struct.DMA_AutoDisableReq;
  
  //参数检查
  ASSERT( chx <= DMA_CH15 );       //eDMA通道选择
  ASSERT( req <= DMA_MUX_63 );     //请求源选择
  ASSERT( major_cnt <= 0x7FFF );   //主计数判断
  ASSERT( src_addr != NULL );      //源地址判断
  ASSERT( (src_dsize <= DMA_SRC_32BIT)||(src_dsize == DMA_SRC_16BYTE) );     //源数据传输大小判断
  ASSERT( dst_addr != NULL );      //目的地址判断
  ASSERT( (dst_dsize <= DMA_DST_32BIT)||(dst_dsize == DMA_DST_16BYTE) );     //目的数据传输大小判断
 
  SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;  //打开DMA通道多路复用器时钟   
  SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;     //打开DMA模块时钟
  
  //关闭通道x硬件DMA请求 
  DMA0->ERQ &= ~(1<<chx);
  
  //选择 通道x 配置外设的DMA源请求编号
  DMAMUX->CHCFG[chx] = DMAMUX_CHCFG_SOURCE(req);
  //是否使能周期触发功能
  if(periodic_trigg == TRUE)
  {
    DMAMUX->CHCFG[chx] |= DMAMUX_CHCFG_TRIG_MASK;
  }
  else
  {
    DMAMUX->CHCFG[chx] &= ~(DMAMUX_CHCFG_TRIG_MASK);
  }
  
  
  //设置源地址   
  DMA0->TCD[chx].SADDR = DMA_SADDR_SADDR(src_addr);
  //在执行完针对源地址的操作之后，在源地址的基础上增加/减少偏移地址
  DMA0->TCD[chx].SOFF = DMA_SOFF_SOFF(src_addroffset);
  //设置源地址的传输大小
  DMA0->TCD[chx].ATTR = 0 | DMA_ATTR_SSIZE(src_dsize);
  //主的计数次数（major iteration count）达到后，重新调整源地址
  DMA0->TCD[chx].SLAST = DMA_SLAST_SLAST(src_lastadj);
  
  //设置目的地址 
  DMA0->TCD[chx].DADDR = DMA_DADDR_DADDR(dst_addr);
  //在执行完针对目的地址的操作之后，在目的地址的基础上增加/减少偏移地址
  DMA0->TCD[chx].DOFF = DMA_DOFF_DOFF(dst_addroffset);
  //设置目的地址的传输宽度
  DMA0->TCD[chx].ATTR |= DMA_ATTR_DSIZE(dst_dsize);
  //主的计数次数（major iteration count）达到后，重新调整目的地址
  DMA0->TCD[chx].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(dst_lastadj);
  
  //默认为禁用通道链接功能，后续更新添加此功能
  if( 1 == 1)
  {
    //===============设置主计数器长度，循环次数====================================
    //设置主循环计数器 current major loop count
    DMA0->TCD[chx].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(major_cnt);
    //起始循环计数器，当主循环计数器为零的时候，将装载起始循环计数器的值
    DMA0->TCD[chx].BITER_ELINKNO = DMA_CITER_ELINKNO_CITER(major_cnt);
  }
  
  //默认为禁用次循环地址偏移功能，后续更新添加此功能
  if( 1 == 1)
  {
    //次循环一次传输字节的个数
    DMA0->TCD[chx].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(minor_cnt);
  }
  
  //清空TCD控制寄存器     
  DMA0->TCD[chx].CSR = 0;
  
  //配置eDMA中断
  if((dma_init_struct.DMA_Isr != NULL) && 
     (dma_init_struct.DMA_MajorCompleteIntEnable == TRUE))
  {

       DMA0->TCD[chx].CSR |= DMA_CSR_INTMAJOR_MASK; //使能DMA 主循环计数器减到零 中断
       DMA_ISR[chx] = dma_init_struct.DMA_Isr;
  }
  if((dma_init_struct.DMA_Isr != NULL) && 
     (dma_init_struct.DMA_MajorHalfCompleteIntEnable == TRUE))
  {

       DMA0->TCD[chx].CSR |= DMA_CSR_INTHALF_MASK; //使能DMA 主循环计数器减到一半 中断
       DMA_ISR[chx] = dma_init_struct.DMA_Isr;
  }
    
  if(auto_disable == TRUE)
  {
     DMA0->TCD[chx].CSR |= DMA_CSR_DREQ_MASK; //主循环计数器等于零后，自动关闭DMA 
  }
  else
  {
     DMA0->TCD[chx].CSR &= ~(DMA_CSR_DREQ_MASK); //主循环计数器等于零后，不关闭DMA
  } 
 
  //DMA通道使能
  DMAMUX->CHCFG[chx] |= DMAMUX_CHCFG_ENBL_MASK;
    
  return 1;
}

/*
 * LPLD_DMA_EnableIrq
 * 使能eDMA中断
 * 
 * 参数:
 *    dma_init_struct--eDMA初始化结构体，
 *                        具体定义见DMA_InitTypeDef
 *
 * 输出:
 *    0--失败
 *    1--成功
 */
uint8 LPLD_DMA_EnableIrq(DMA_InitTypeDef dma_init_struct)
{
  enable_irq((IRQn_Type)(dma_init_struct.DMA_CHx + DMA0_IRQn));  
  return 1;
}

/*
 * LPLD_DMA_DisableIrq
 * 禁用eDMA中断
 * 
 * 参数:
 *    dma_init_struct--eDMA初始化结构体，
 *                        具体定义见DMA_InitTypeDef
 *
 * 输出:
 *    0--失败
 *    1--成功
 */
uint8 LPLD_DMA_DisableIrq(DMA_InitTypeDef dma_init_struct)
{
  disable_irq((IRQn_Type)(dma_init_struct.DMA_CHx + DMA0_IRQn));
  return 1;
}

/*
 * LPLD_DMA_SoftwareStartService
 * DMA服务请求软件开始
 * 
 * 参数:
 *    dma_init_struct--eDMA初始化结构体，
 *                        具体定义见DMA_InitTypeDef
 *
 * 输出:
 *    无
 */
__INLINE void LPLD_DMA_SoftwareStartService(DMA_InitTypeDef dma_init_struct)
{
  DMA0->TCD[dma_init_struct.DMA_CHx].CSR |= DMA_CSR_START_MASK; 
}

/*
 * eDMA中断处理函数
 * 与启动文件startup_K60.s中的中断向量表关联
 * 用户无需修改，程序自动进入对应通道中断函数
 */
void DMA0_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[0]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<0;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA1_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[1]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<1;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA2_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[2]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<2;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA3_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[3]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<3;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA4_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[4]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<4;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA5_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[5]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<5;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA6_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[6]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<6;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA7_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[7]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<7;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA8_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[8]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<8;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA9_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[9]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<9;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA10_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[10]();
  //清除中断标志位
  DMA0->INT |= 0x1u<10;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA11_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[11]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<11;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA12_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[12]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<12;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA13_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[13]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<13;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA14_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[14]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<14;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}

void DMA15_IRQHandler(void)
{
#if (UCOS_II > 0u)
  OS_CPU_SR  cpu_sr = 0u;
  OS_ENTER_CRITICAL(); //告知系统此时已经进入了中断服务子函数
  OSIntEnter();
  OS_EXIT_CRITICAL();
#endif  
  
  //调用用户自定义中断服务
  DMA_ISR[15]();
  //清除中断标志位
  DMA0->INT |= 0x1u<<15;
  
#if (UCOS_II > 0u)
  OSIntExit();          //告知系统此时即将离开中断服务子函数
#endif
}
















