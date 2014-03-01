/*
 * Copyright (C) 2014 bodhi
 * Based on 
 *
 * Copyright (C) 2010  Eric C. Cooper <ecc@cmu.edu>
 *
 * Based on sheevaplug.h originally written by
 * Prafulla Wadaskar <prafulla@marvell.com>
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _CONFIG_DOCKSTAR_H
#define _CONFIG_DOCKSTAR_H

/*
 * Version number information
 */
#define CONFIG_IDENT_STRING	"\nSeagate FreeAgent DockStar"

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_FEROCEON_88FR131	1	/* CPU Core subversion */
#define CONFIG_KIRKWOOD		1	/* SOC Family Name */
#define CONFIG_KW88F6281	1	/* SOC Name */
#define CONFIG_MACH_DOCKSTAR	/* Machine type */
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */

/*
 * Commands configuration
 */
#define CONFIG_SYS_NO_FLASH		/* Declare no flash (NOR/SPI) */
#include <config_cmd_default.h>
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
#define CONFIG_CMD_MII
#define CONFIG_CMD_NAND
#define CONFIG_CMD_PING
#define CONFIG_CMD_USB
#define CONFIG_CMD_DATE
#define CONFIG_SYS_LONGHELP
#define CONFIG_PREBOOT
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2 "> "
/*
 * mv-common.h should be defined after CMD configs since it used them
 * to enable certain macros
 */
#include "mv-common.h"

#undef CONFIG_SYS_PROMPT	/* previously defined in mv-common.h */
#define CONFIG_SYS_PROMPT	"DockStar> "	/* Command Prompt */

/*
 *  Environment variables configurations
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_ENV_IS_IN_NAND		1
#define CONFIG_ENV_SECT_SIZE		0x20000	/* 128K */
#else
#define CONFIG_ENV_IS_NOWHERE		1	/* if env in SDRAM */
#endif
/*
 * max 4k env size is enough, but in case of nand
 * it has to be rounded to sector size
 */
#define CONFIG_ENV_SIZE			0x20000	/* 128k */
#define CONFIG_ENV_ADDR			0xc0000
#define CONFIG_ENV_OFFSET		0xc0000	/* env starts here */


/*
 * Default environment variables
 */
#define CONFIG_EXTRA_ENV_SETTINGS \
  "arcNumber=2097\0" \
  "mainlineLinux=yes\0" \
  "console=ttyS0,115200\0" \
  "usb_init=usb start\0" \
  "usb_device=0:1\0" \
  "usb_root=/dev/sda1\0" \
  "usb_rootfstype=ext2\0" \
  "usb_rootdelay=10\0"\
  \
  "mtdparts=mtdparts=orion_nand:1M(u-boot),4M(uImage),32M(rootfs),-(data)\0"\
  "mtdids=nand0=orion_nand\0"\
  "partition=nand0,2\0"\
  \
  "bootcmd_pogo=fsload uboot-original-mtd0.kwb; go 0x800200\0" \
  \
  /* Zero the first bit at 0x800000 to clear any old image still in RAM after a warm reboot */\
  "usb_load_uimage=mw 0x800000 0 1; ext2load usb $usb_device 0x800000 /boot/uImage\0"\
  "usb_boot="\
    "if ext2load usb $usb_device 0x1100000 /boot/uInitrd; then"\
    " bootm 0x800000 0x1100000;"\
    "else"\
    " bootm 0x800000;"\
    "fi;\0"\
  \
  "set_bootargs_usb=setenv bootargs console=$console root=$usb_root rootdelay=$usb_rootdelay rootfstype=$usb_rootfstype $mtdparts\0" \
  "bootcmd_usb=run usb_init; run usb_load_uimage; run set_bootargs_usb; run usb_boot;\0" \
  \
  "led_init=green blinking\0" \
  "led_exit=green off\0" \
  "led_error=orange blinking\0"


#define CONFIG_BOOTCOMMAND "run bootcmd_usb; usb stop; run bootcmd_pogo; reset"

/*
 * Ethernet Driver configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_MVGBE_PORTS	{1, 0}	/* enable port 0 only */
#define CONFIG_PHY_BASE_ADR	0
#endif /* CONFIG_CMD_NET */

/*
 * File system
 */
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FAT
#define CONFIG_CMD_JFFS2
#define CONFIG_JFFS2_NAND
#define CONFIG_JFFS2_LZO
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_MTD_DEVICE               /* needed for mtdparts commands */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_LZO

/*
 * Device Tree
 */
#define CONFIG_OF_LIBFDT

/*
 * EFI partition
 */
#define CONFIG_EFI_PARTITION

/*
 *  Date Time
 *   */
#ifdef CONFIG_CMD_DATE
#define CONFIG_RTC_MV
#define CONFIG_CMD_SNTP
#define CONFIG_CMD_DNS
#endif /* CONFIG_CMD_DATE */

#endif /* _CONFIG_DOCKSTAR_H */
