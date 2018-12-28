/*
 * Copyright (C) 2019 Damien Merenne <dmerenne@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#ifndef _CONFIG_MD86783_H
#define _CONFIG_MD86783_H

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_FEROCEON_88FR131	1	/* CPU Core subversion */
#define CONFIG_KIRKWOOD		1	/* SOC Family Name */
#define CONFIG_KW88F6192        1       /* Compatible SOC Name */
#define CONFIG_KW88F6702        1       /* SOC Name */
#define CONFIG_MACH_MD86783		/* Machine type */
#define CONFIG_MACH_TYPE	MACH_TYPE_MD86783
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */

/*
 * Misc Configuration Options
 */
#define CONFIG_SHOW_BOOT_PROGRESS 1	/* boot progess display (LED's) */

/*
 * Force number of DRAM banks otherwise detection fails
 */
#define CONFIG_NR_DRAM_BANKS 1

/*
 * Serial is on UART1
 */
#define CONFIG_CONS_INDEX 2
#define CONFIG_SYS_NS16550_COM2 KW_UART1_BASE
/*
 * Commands configuration
 */
#define CONFIG_SYS_NO_FLASH             /* Declare no flash (NOR/SPI) */
#define CONFIG_SYS_MVFS                 /* Picks up Filesystem from mv-common.h */
#define CONFIG_CMD_ENV
#define CONFIG_CMD_NAND
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
#define CONFIG_SYS_PROMPT	"MD86873> "	/* Command Prompt */

/*
 *  Environment variables configurations
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_ENV_IS_IN_NAND		1
#define CONFIG_ENV_SECT_SIZE		0x20000		/* 128K */
#else
#define CONFIG_ENV_IS_NOWHERE		1		/* if env in SDRAM */
#endif
/*
 * max 4k env size is enough, but in case of nand
 * it has to be rounded to sector size
 */
#define CONFIG_ENV_SIZE			0x20000		/* 128k */
#define CONFIG_ENV_ADDR			0xa0000
#define CONFIG_ENV_OFFSET		0xa0000	/* env starts here */

/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND \
	"run bootcmd_uenv; run bootcmd_usb; usb stop; run bootcmd_sata; reset"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"arcNumber=4931\0" \
	"bootcmd_exec=run load_uimage; if run load_initrd; then if run load_dtb; then bootm $load_uimage_addr $load_initrd_addr $load_dtb_addr; else bootm $load_uimage_addr $load_initrd_addr; fi; else if run load_dtb; then bootm $load_uimage_addr - $load_dtb_addr; else bootm$load_uimage_addr; fi; fi\0" \
	"bootcmd=run bootcmd_uenv; run scan_disk; run set_bootargs; run bootcmd_exec; reset\0" \
	"bootcmd_uenv=run uenv_load; if test $uenv_loaded -eq 1; then run uenv_import; fi\0" \
	"bootdelay=10\0" \
	"bootdev=usb\0" \
	"console=ttyS1,115200\0" \
	"device=0:1\0" \
	"devices=usb ide\0" \
	"disks=0 1 2 3\0" \
	"ethact=egiga0\0" \
	"ethaddr=52:3b:20:9c:11:51\0" \
	"if_netconsole=ping $serverip\0" \
	"ipaddr=192.168.0.231\0" \
	"led_error=orange blinking\0" \
	"led_exit=green off\0" \
	"led_init=green blinking\0" \
	"dtb_file=/boot/dts/kirkwood-md86783.dtb\0" \
	"load_dtb_addr=0x1c00000\0" \
	"load_initrd_addr=0x1100000\0" \
	"load_uimage_addr=0x800000\0" \
	"load_dtb=echo loading DTB $dtb_file ...; load $bootdev $device $load_dtb_addr $dtb_file\0" \
	"load_initrd=echo loading uInitrd ...; load $bootdev $device $load_initrd_addr /boot/uInitrd\0" \
	"load_uimage=echo loading uImage ...; load $bootdev $device $load_uimage_addr /boot/uImage\0" \
	"mainlineLinux=yes\0" \
	"mtdids=nand0=orion_nand\0" \
	"mtdparts=mtdparts=orion_nand:0x100000(u-boot),0x600000(uImage),0xf900000(rootfs)\0"\
	"partition=nand0,2\0" \
	"preboot_nc=run if_netconsole start_netconsole\0" \
	"scan_disk=echo running scan_disk ...; scan_done=0; setenv scan_usb \"usb start\";  setenv scan_ide \"ide reset\";  setenv scan_mmc \"mmc rescan\"; for dev in $devices; do if test $scan_done -eq 0; then echo Scan device $dev; run scan_$dev; for disknum in $disks; do if test $scan_done -eq 0; then echo device $dev $disknum:1; if load $dev $disknum:1 $load_uimage_addr /boot/uImage 1; then scan_done=1; echo Found bootable drive on $dev $disknum; setenv device $disknum:1; setenv bootdev $dev; fi; fi; done; fi; done\0" \
	"serverip=192.168.0.220\0" \
	"set_bootargs=setenv bootargs console=ttyS0,115200 root=LABEL=rootfs rootdelay=10 $mtdparts $custom_params\0" \
	"start_netconsole=setenv ncip $serverip; setenv bootdelay 10; setenv stdin nc; setenv stdout nc; setenv stderr nc; version;\0" \
	"stderr=serial\0" \
	"stdin=serial\0" \
	"stdout=serial\0" \
	"uenv_addr=0x810000\0" \
	"uenv_import=echo importing envs ...; env import -t $uenv_addr $filesize\0" \
	"uenv_init_devices=setenv init_usb \"usb start\";  setenv init_ide \"ide reset\";  setenv init_mmc \"mmc rescan\"; for devtype in $devices; do run init_$devtype; done;\0" \
	"uenv_load=run uenv_init_devices; setenv uenv_loaded 0; for devtype in $devices;  do for disknum in 0; do run uenv_read_disk; done; done;\0" \
	"uenv_read_disk=if test $devtype -eq mmc; then if $devtype part; then run uenv_read;  fi; else if $devtype part $disknum; then run uenv_read; fi;  fi\0" \
	"uenv_read=echo loading envs from $devtype $disknum ...; if load $devtype $disknum:1 $uenv_addr /boot/uEnv.txt; then setenv uenv_loaded 1; fi\0" \
	"usb_ready_retry=15\0" \
        "old_bootcmd=nand read.e 0x800000 0x100000 0x400000; setenv bootargs $console ubi.mtd=2,2048 root=ubi0:rootfs rootfstype=ubifs init=/linuxrc; bootm 0x800000\0"

/*
 * Ethernet Driver configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_MVGBE_PORTS		{1, 0}	/* enable port 0 only */
#define CONFIG_PHY_BASE_ADR		0x1
#define CONFIG_PHY_GIGE
#define CONFIG_NETCONSOLE
#endif /* CONFIG_CMD_NET */

/*
 * SATA Driver configuration
 */
#ifdef CONFIG_MVSATA_IDE
#define CONFIG_SYS_ATA_IDE0_OFFSET      MV_SATA_PORT0_OFFSET
#define CONFIG_SYS_ATA_IDE1_OFFSET      MV_SATA_PORT1_OFFSET
#endif /* CONFIG_MVSATA_IDE */

/*
 * File system
 */
#define CONFIG_JFFS2_NAND
#define CONFIG_JFFS2_LZO
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_MTD_DEVICE               /* needed for mtdparts commands */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_LZO

/*
 *  Date Time
 */
#ifdef CONFIG_CMD_DATE
#define CONFIG_RTC_MV
#define CONFIG_CMD_SNTP
#define CONFIG_CMD_DNS
#endif /* CONFIG_CMD_DATE */

#endif /* _CONFIG_MD86783_H */
