/*
 * Copyright (C) 2014 bodhi <mibodhi@gmail.com>
 * Based on 
 *
 * Copyright (C) 2012
 * David Purdy <david.c.purdy@gmail.com>
 *
 * Based on Kirkwood support:
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CONFIG_POGO_V4_H
#define _CONFIG_POGO_V4_H

/*
 * Machine type definition and ID
 */
#define MACH_TYPE_POGO_V4		2884	/* use openrd ult */
#define CONFIG_MACH_TYPE		MACH_TYPE_POGO_V4
#define CONFIG_IDENT_STRING		"\nPogoplug V4"

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_FEROCEON_88FR131		/* #define CPU Core subversion */
#define CONFIG_KIRKWOOD			/* SOC Family Name */
#define CONFIG_KW88F6192		/* SOC Name */
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */

/*
 * Commands configuration
 */
#define CONFIG_SYS_NO_FLASH		/* Declare no flash (NOR/SPI) */
#define CONFIG_SYS_MVFS
#include <config_cmd_default.h>
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
#define CONFIG_CMD_MII
#define CONFIG_CMD_MMC
#define CONFIG_CMD_NAND
#define CONFIG_CMD_PING
#define CONFIG_CMD_USB
#define CONFIG_CMD_DATE
#define CONFIG_CMD_IDE
#define CONFIG_SYS_LONGHELP
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING
#define CONFIG_PREBOOT
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2 "> "	

/*
 * mv-common.h should be defined after CMD configs since it used them
 * to enable certain macros
 */
#include "mv-common.h"

/* Remove or override few declarations from mv-common.h */
#undef CONFIG_SYS_PROMPT	/* previously defined in mv-common.h */
#define CONFIG_SYS_PROMPT	"Pogov4> "

/*
 *  Environment variables configurations
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SECT_SIZE		0x20000	/* 128K */
#else
#define CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_ENV_SIZE			0x20000	/* 128k */
#define CONFIG_ENV_OFFSET		0xc0000	/* env starts here */

/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND \
	"run bootcmd_uenv; run bootcmd_usb; reset"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"arcNumber=3960\0" \
	"machid=f78\0" \
	"ethaddr=b6:d0:5e:0f:a1:17\0" \
	"mtdparts=mtdparts=orion_nand:2M(u-boot),3M(uImage)," \
	"3M(uImage2),8M(failsafe),112M(root)\0"\
	"baudrate=115200\0"\
	"bootcmd_usb=run usb_init; run set_bootargs_usb; run usb_boot;\0"\
	"bootdelay=10\0"\
	"console=ttyS0,115200\0"\
	"device=0:1\0"\
	"ethact=egiga0\0"\
	"led_error=orange blinking\0"\
	"led_exit=green off\0"\
	"led_init=green blinking\0"\
	"mainlineLinux=yes\0"\
	"mtdids=nand0=orion_nand\0"\
	"partition=nand0,2\0"\
	"rootdelay=10\0"\
	"rootfstype=ext2\0"\
	"set_bootargs_usb=setenv bootargs console=$console root=$usb_root rootdelay=$rootdelay rootfstype=$rootfstype $mtdparts\0"\
	"stderr=serial\0"\
	"stdin=serial\0"\
	"stdout=serial\0"\
	"usb_boot=mw 0x800000 0 1; run usb_load_uimage; if run usb_load_uinitrd; then bootm 0x800000 0x1100000; else bootm 0x800000; fi\0"\
	"usb_init=usb start\0"\
	"usb_load_uimage=ext2load usb $device 0x800000 /boot/uImage\0"\
	"usb_load_uinitrd=ext2load usb $device 0x1100000 /boot/uInitrd\0"\
	"usb_root=/dev/sda1\0" \
	"bootcmd_uenv=run uenv_load; if test $uenv_loaded -eq 1; then run uenv_import; fi\0" \
	"uenv_import=echo importing envs ...; env import -t 0x810000\0" \
	"uenv_load=usb start; mmc rescan; ide reset; setenv uenv_loaded 0; for devtype in usb mmc ide; do for disknum in 0; do run uenv_read_disk; done; done\0" \
	"uenv_read=echo loading envs from $devtype $disknum ...; if load $devtype $disknum:1 0x810000 /boot/uEnv.txt; then setenv uenv_loaded 1; fi\0" \
	"uenv_read_disk=if test $devtype -eq mmc; then if $devtype part; then run uenv_read;  fi; else if $devtype part $disknum; then run uenv_read; fi;  fi"

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
 * SATA 
 */
#ifdef CONFIG_MVSATA_IDE
#define CONFIG_SYS_ATA_IDE0_OFFSET	MV_SATA_PORT0_OFFSET
#endif

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

/*
 *
 */

#define CONFIG_KIRKWOOD_GPIO

#endif /* _CONFIG_POGO_V4_H */
