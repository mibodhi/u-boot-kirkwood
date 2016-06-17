/*
 * Copyright (C) 2016 bodhi <mibodhi@gmail.com>
 *
 * Based on sheevaplug.h originally written by
 * Prafulla Wadaskar <prafulla@marvell.com>
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _CONFIG_T5325_H
#define _CONFIG_T5325_H

/*
 * Version number information
 */
#define CONFIG_IDENT_STRING	"\nHP Thin Client T5325"

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_FEROCEON_88FR131	1	/* CPU Core subversion */
#define CONFIG_KIRKWOOD		1	/* SOC Family Name */
#define CONFIG_KW88F6281	1	/* SOC Name */
#define CONFIG_MACH_T5325		/* Machine type */
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */


/*
 * Commands configuration
 */
#define CONFIG_SYS_NO_FLASH		/* Declare no flash (NOR/SPI) */
#undef CONFIG_ENV_SPI_MAX_HZ
#define CONFIG_ENV_SPI_MAX_HZ		86000000
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_SPI_FLASH_MTD
#define CONFIG_CMD_SF

#define CONFIG_SYS_MVFS			/* Picks up Filesystem from mv-common.h */
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
#define CONFIG_CMD_MII
#define CONFIG_CMD_PING
#define CONFIG_CMD_USB
#define CONFIG_CMD_IDE
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

/*
 *  Environment variables configurations
 */
#define CONFIG_ENV_SECT_SIZE		0x10000	/* 64KB */ 
#define CONFIG_ENV_SIZE			0x10000	/* 64KB */
#define CONFIG_ENV_ADDR                 0xC0000
#define CONFIG_ENV_OFFSET               0xC0000 /* env starts here */

/*
 *
 * Default environment variables
 */

#define CONFIG_BOOTCOMMAND \
	"run bootcmd_uenv; run bootcmd_usb; reset"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"arcNumber=2846\0" \
	"ethaddr=b6:d0:5e:0f:a1:17\0" \
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
	"mtdparts=mtdparts=spi0.0:512K(uboot),256K(sdd_firmware),64K(uboot_env),64K(permanent_uboot_env),64K(hp_env)\0"\
	"rootdelay=10\0"\
	"set_bootargs_usb=setenv bootargs console=$console root=$usb_root rootdelay=$rootdelay $mtdparts\0"\
	"stderr=serial\0"\
	"stdin=serial\0"\
	"stdout=serial\0"\
	"usb_boot=mw 0x800000 0 1; run usb_load_uimage; if run usb_load_uinitrd; then bootm 0x800000 0x1100000; else bootm 0x800000; fi\0"\
	"usb_init=usb start\0"\
	"usb_load_uimage=ext2load usb $device 0x800000 /boot/uImage\0"\
	"usb_load_uinitrd=ext2load usb $device 0x1100000 /boot/uInitrd\0"\
	"usb_root='LABEL=rootfs'\0" \
	"bootcmd_uenv=run uenv_load; if test $uenv_loaded -eq 1; then run uenv_import; fi\0" \
	"uenv_import=echo importing envs ...; env import -t 0x810000\0" \
	"uenv_load=usb start; ide reset; setenv uenv_loaded 0; for devtype in usb ide; do for disknum in 0; do run uenv_read_disk; done; done\0" \
	"uenv_read=echo loading envs from $devtype $disknum ...; if load $devtype $disknum:1 0x810000 /boot/uEnv.txt; then setenv uenv_loaded 1; fi\0" \
	"uenv_read_disk=if $devtype part $disknum; then run uenv_read; fi"

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
#define CONFIG_RBTREE
#define CONFIG_MTD_DEVICE               /* needed for mtdparts commands */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS


#ifdef CONFIG_MVSATA_IDE
#define CONFIG_SYS_ATA_IDE0_OFFSET	MV_SATA_PORT0_OFFSET
#define CONFIG_SYS_ATA_IDE1_OFFSET	MV_SATA_PORT1_OFFSET
#endif

/*
 * EFI partition
 */
#define CONFIG_EFI_PARTITION

/*
 *  Date Time
 *
 */
#ifdef CONFIG_CMD_DATE
#define CONFIG_RTC_MV
#define CONFIG_CMD_SNTP
#define CONFIG_CMD_DNS
#endif /* CONFIG_CMD_DATE */

#endif /* _CONFIG_T5325_H */
