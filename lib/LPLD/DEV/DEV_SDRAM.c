/*
* @file DEV_SDRAM.c
* @version 0.1[By LPLD]
* @date 2013-09-24
* @brief SDRAM设备驱动程序
*
* 更改建议:可根据实际硬件修改
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
#include "DEV_SDRAM.h"

//该结构体构成分配内存的最小单元
//同样构成用于分配和释放内存空间函数的链表结构
typedef struct LPLD_ALLOC_HDR
{
  struct
  {
    struct LPLD_ALLOC_HDR *ptr;
    unsigned int size;
  } s;
  unsigned int align;
  unsigned int pad;
} LPLD_ALLOC_HDR;

static LPLD_ALLOC_HDR EX_base;
static LPLD_ALLOC_HDR *EX_freep = NULL;

/*
 * LPLD_SDRAM_Init
 * 初始化SDRAM所需的FlexBus
 * 
 * 说明:
 *    本初始化函数用来配置FlexBus与SDRAM的连接
 *    配置为8位端口大小
 *    地址与数据线复用
 *    地址线为FB_AD[19:0]
 *    数据线为FB_AD[31:24]
 *    CS:FB_CS1
 *    RW:FB_FB_RW
 *    RD:FB_OE 
 */
void LPLD_SDRAM_Init()
{
  FB_InitTypeDef fb_init;
  fb_init.FB_Fbx = FB1;
  fb_init.FB_ChipSelAddress = SDRAM_ADDRESS;
  fb_init.FB_AutoAckEnable = TRUE;
  fb_init.FB_IsRightJustied = FALSE;
  fb_init.FB_WateStates = 0x02;
  fb_init.FB_PortSize = FB_SIZE_8BIT;
  fb_init.FB_AddressSpace = FB_SPACE_KB(SDRAM_SIZE); 
  fb_init.FB_ReadAddrHold = 0;
  fb_init.FB_WriteAddrHold = 0;
  LPLD_FlexBus_Init(fb_init); 
}

/*
 * LPLD_SDRAM_Free
 * 释放SDRAM中的空间
 * 
 * 参数：
 *    *ap--空间首地址指针
 */
void LPLD_SDRAM_Free(void *ap)
{
  LPLD_ALLOC_HDR *bp, *p;
  
  bp = (LPLD_ALLOC_HDR *)ap - 1;  
  for (p = EX_freep; !((bp > p) && (bp < p->s.ptr)) ; p = p->s.ptr)
  {
    if ((p >= p->s.ptr) && ((bp > p) || (bp < p->s.ptr)))
    {
      break; 
    }
  }
  
  if ((bp + bp->s.size) == p->s.ptr)
  {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  }
  else
  {
    bp->s.ptr = p->s.ptr;
  }
  
  if ((p + p->s.size) == bp)
  {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  }
  else
  {
    p->s.ptr = bp;
  }
  
  EX_freep = p;
}

/*
 * LPLD_SDRAM_Malloc
 * 动态分配SDRAM中的空间
 * 
 * 参数：
 *    nbytes--数据长度
 *
 * 返回值：
 *    空间地址指针
 */
void *LPLD_SDRAM_Malloc(unsigned nbytes)
{
  LPLD_ALLOC_HDR *p, *prevp;
  unsigned nunits;   
  char* __HEAP_START;
  char* __HEAP_END;
  
  __HEAP_START = (char*)(SDRAM_ADDRESS);
  __HEAP_END = (char*)(SDRAM_ADDRESS+SDRAM_SIZE*1024);
  
  nunits = ((nbytes+sizeof(LPLD_ALLOC_HDR)-1) / sizeof(LPLD_ALLOC_HDR)) + 1;
  
  if ((prevp = EX_freep) == NULL)
  {
    p = (LPLD_ALLOC_HDR *)__HEAP_START;
    p->s.size = ( ((uint32)__HEAP_END - (uint32)__HEAP_START)
                 / sizeof(LPLD_ALLOC_HDR) );
    p->s.ptr = &EX_base;
    EX_base.s.ptr = p;
    EX_base.s.size = 0;
    prevp = EX_freep = &EX_base;
  }
  
  for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr)
  {
    if (p->s.size >= nunits)
    {
      if (p->s.size == nunits)
      {
        prevp->s.ptr = p->s.ptr;
      }
      else
      {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      EX_freep = prevp;
      return (void *)(p + 1);
    }
    
    if (p == EX_freep)
      return NULL;
  }
}


