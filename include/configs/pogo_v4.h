/*
 * Copyright (C) 2014 bodhi
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
#define MACH_TYPE_POGO_V4		3960
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
	"setenv bootargs $(bootargs_console); " \
	"run bootcmd_mmc; " \
	"bootm 0x00800000 0x01100000"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"mtdparts=mtdparts=orion_nand:2M(u-boot),3M(uImage)," \
	"3M(uImage2),8M(failsafe),112M(root)\0"\
	"mtdids=nand0=orion_nand\0"\
	"bootargs_console=console=ttyS0,115200\0" \
	"bootcmd_mmc=mmc init; ext2load mmc 0:1 0x00800000 /uImage; " \
	"ext2load mmc 0:1 0x01100000 /uInitrd\0" \
	"bootcmd_usb=usb start; ext2load usb 0:1 0x00800000 /uImage; " \
	"ext2load usb 0:1 0x01100000 /uInitrd\0" 

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
