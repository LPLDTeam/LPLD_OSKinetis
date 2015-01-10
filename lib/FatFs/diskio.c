/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
//#include "sdcard.h"		/* Example: MMC/SDC contorl */
#include "common.h"     //包含LPLD底层驱K60动文件[By LPLD]


//SD卡信息全局变量[By LPLD]
extern SDCARD_STRUCT_PTR sdcard_ptr;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
  if(pdrv)
    return RES_PARERR;
 
  return (DSTATUS)LPLD_SDHC_InitCard();
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
  return (DSTATUS)sdcard_ptr->STATUS;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
  if(pdrv || (count == 0))
    return RES_PARERR;

  return (DRESULT)LPLD_SDHC_ReadBlocks(buff, sector, count);
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
  if(pdrv || (count == 0))
    return RES_PARERR;
  
  return (DRESULT)LPLD_SDHC_WriteBlocks((uint8*)buff, sector, count);
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT result = RES_OK;
  
  if(pdrv)
    return RES_PARERR;
  
  switch(cmd)
  {
    case CTRL_SYNC:
      /*
      确定磁盘驱动已经完成写操作挂起的处理. 
      当磁盘IO模块有一个会写缓存，会立即擦除扇区. 该命令不能再只读模式使用.
      */
      // 在POLLING模式中，所有写操作已完成。
      break;
    case GET_SECTOR_SIZE:
      /*
      以WORD型指针变量的形式返回扇区大小.
      此命令不能用在可变扇区大小的配置, 
      _MAX_SS 为 512.
      */
      if(buff == NULL)
        result = RES_PARERR;
      else
        *(uint32*)buff = IO_SDCARD_BLOCK_SIZE;
      
      break;
    case GET_SECTOR_COUNT:
      /*
      以UINT32型指针变量的形式返回磁盘的可用扇区数. 
      该命令仅被f_mkfs函数调用以决定可创建多大的卷. 
      */
      if(buff == NULL)
        result = RES_PARERR;
      else
        *(uint32*)buff = sdcard_ptr->NUM_BLOCKS;
      break;
    case GET_BLOCK_SIZE:
      /*
      以UINT32类型的指针变量返回返回flash内存中擦除的扇区数.
      合法的数值为2的1至32768次方.
      返回1代表擦除大小或磁盘设备未知.
      该命令仅被f_mkfs函数调用并试图将擦除的扇区边界进行数据对齐.
      */
      result = RES_PARERR;
      break;
    case CTRL_ERASE_SECTOR:
      /*
      擦除由UINT32类型指针数组指定的flash内存,{<start sector>, <end sector>}.
      如果介质为非flash内存,则该命令无效.
      FatFs系统不会检查结果,如果擦除失败也不会影响文件函数.
      当_USE_ERASE为1时移动一个簇链会调用此命令.
      */
      result = RES_PARERR;
      break;
    default:
      return RES_PARERR;
    
  }
  return result;
}
#endif
