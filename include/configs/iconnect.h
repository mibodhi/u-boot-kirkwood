/*
 * Copyright (C) 2014 bodhi
 * Based on 
 *
 * (C) Copyright 2009-2012
 * Wojciech Dubowik <wojciech.dubowik@neratec.com>
 * Luka Perkov <luka@openwrt.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _CONFIG_ICONNECT_H
#define _CONFIG_ICONNECT_H

/*
 * Version number information
 */
#define CONFIG_IDENT_STRING	"\nIomega iConnect"

/*
 * High level configuration options
 */
#define CONFIG_FEROCEON_88FR131		/* CPU Core subversion */
#define CONFIG_KIRKWOOD			/* SOC Family Name */
#define CONFIG_KW88F6281		/* SOC Name */
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */

/*
 * Machine type
 */
#define CONFIG_MACH_TYPE	MACH_TYPE_ICONNECT

/*
 * Compression configuration
 */
#define CONFIG_BZIP2
#define CONFIG_LZMA
#define CONFIG_LZO

/*
 * Commands configuration
 */
#define CONFIG_SYS_NO_FLASH		/* declare no flash (NOR/SPI) */
#define CONFIG_SYS_MVFS
#include <config_cmd_default.h>
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

#undef CONFIG_SYS_PROMPT
#define CONFIG_SYS_PROMPT	"iConnect> "

/*
 * Environment variables configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SECT_SIZE	0x20000
#else
#define CONFIG_ENV_IS_NOWHERE
#endif
#define CONFIG_ENV_SIZE			0x20000	/* 128k */
#define CONFIG_ENV_ADDR			0xc0000
#define CONFIG_ENV_OFFSET		0xc0000	/* env starts here */

/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND \
	"setenv bootargs ${console} ${mtdparts} ${bootargs_root}; "	\
	"ubi part rootfs; "						\
	"ubifsmount ubi:rootfs; "					\
	"ubifsload 0x800000 ${kernel}; "				\
	"bootm 0x800000"

#define CONFIG_MTDPARTS \
	"mtdparts=orion_nand:"		\
	"0x80000@0x0(uboot),"		\
	"0x20000@0x80000(uboot_env),"	\
	"-@0xa0000(rootfs)\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"console=console=ttyS0,115200\0"	\
	"mtdids=nand0=orion_nand\0"		\
	"mtdparts="CONFIG_MTDPARTS		\
	"kernel=/boot/uImage\0"			\
	"bootargs_root=noinitrd ubi.mtd=2 root=ubi0:rootfs rootfstype=ubifs\0"

/*
 * Ethernet driver configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_MVGBE_PORTS	{1, 0}	/* enable port 0 only */
#define CONFIG_PHY_BASE_ADR	11
#undef CONFIG_RESET_PHY_R
#endif /* CONFIG_CMD_NET */

/*
 * File system
 */
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FAT
#define CONFIG_CMD_JFFS2
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS

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

#endif /* _CONFIG_ICONNECT_H */
