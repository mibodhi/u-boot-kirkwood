/*
 * Copyright (C) 2015 bodhi <mibodhi@gmail.com>
 *
 * Based on
 * Copyright (C) 2014  Jason Plum <jplum@archlinuxarm.org>
 *
 * Based on nsa320.c originall written by
 * Copyright (C) 2012  Peter Schildmann <linux@schildmann.info>
 *
 * Based on guruplug.c originally written by
 * Siddarth Gore <gores@marvell.com>
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

#include <common.h>
#include <miiphy.h>
#include <asm/arch/soc.h>
#include <asm/arch/mpp.h>
#include <asm/arch/cpu.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include "ts219p.h"
#include <asm/arch/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	/* Multi-Purpose Pins Functionality configuration */
	u32 kwmpp_config[] = {
                MPP0_SPI_SCn,
                MPP1_SPI_MOSI,
                MPP2_SPI_SCK,
                MPP3_SPI_MISO,
                MPP4_SATA1_ACTn,
                MPP5_SATA0_ACTn,
                MPP8_TW_SDA,
                MPP9_TW_SCK,
                MPP10_UART0_TXD,
                MPP11_UART0_RXD,
                MPP13_UART1_TXD,        /* PIC controller */
                MPP14_UART1_RXD,        /* PIC controller */
                MPP15_GPIO,             /* USB Copy button (on devices with 88F6281) */
                MPP16_GPIO,             /* Reset button (on devices with 88F6281) */
                MPP36_GPIO,             /* RAM: 0: 256 MB, 1: 512 MB */
                MPP37_GPIO,             /* Reset button (on devices with 88F6282) */
                MPP43_GPIO,             /* USB Copy button (on devices with 88F6282) */
                MPP44_GPIO,             /* Board ID: 0: TS-11x, 1: TS-21x */
                0
	};
	kirkwood_mpp_conf(kwmpp_config, NULL);
	return 0;
}

int board_init(void)
{
	/*
	 * arch number of board
	 */
	gd->bd->bi_arch_number = MACH_TYPE_TS219P;

	/* address of boot parameters */
	gd->bd->bi_boot_params = mvebu_sdram_bar(0) + 0x100;

        kw_gpio_set_valid(14, 1);
	kw_gpio_direction_output(14, 0);
	kw_gpio_set_value(14, 1);

	return 0;
}

#ifdef CONFIG_RESET_PHY_R
/* Configure and enable MV88E1318 PHY */
void reset_phy(void)
{
	u16 reg;
	u16 devadr;
	char *name = "egiga0";

	if (miiphy_set_current_dev(name))
		return;

	/* command to read PHY dev address */
	if (miiphy_read(name, 0xEE, 0xEE, (u16 *) &devadr)) {
		printf("Err..%s could not read PHY dev address\n",
			__FUNCTION__);
		return;
	}

	/* Set RGMII delay */
	miiphy_write(name, devadr, MV88E1318_PGADR_REG, MV88E1318_MAC_CTRL_PG);
	miiphy_read(name, devadr, MV88E1318_MAC_CTRL_REG, &reg);
	reg |= (MV88E1318_RGMII_RXTM_CTRL | MV88E1318_RGMII_TXTM_CTRL);
	miiphy_write(name, devadr, MV88E1318_MAC_CTRL_REG, reg);
	miiphy_write(name, devadr, MV88E1318_PGADR_REG, 0);

	/* reset the phy */
	miiphy_reset(name, devadr);

	/* The QNAP TS219+ uses the 88E1318 */

	/* Set the phy back to auto-negotiation mode */
	miiphy_write(name, devadr, 0x4, 0x1e1);
	miiphy_write(name, devadr, 0x9, 0x300);
	/* Downshift */
	miiphy_write(name, devadr, 0x10, 0x3860);
	miiphy_write(name, devadr, 0x0, 0x9140);

	printf("MV88E1318 PHY initialized on %s\n", name);

}
#endif /* CONFIG_RESET_PHY_R */

#ifdef CONFIG_SHOW_BOOT_PROGRESS
void show_boot_progress(int val)
{
}
#endif

#if defined(CONFIG_KIRKWOOD_GPIO)
static int
do_read_button(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (strcmp(argv[1], "reset") == 0)
		return kw_gpio_get_value(BTN_RESET);
	else if (strcmp(argv[1], "copy") == 0)
		return kw_gpio_get_value(BTN_COPY);
	else
		return -1;
}


U_BOOT_CMD(button, 2, 0, do_read_button,
	   "Return GPIO button status 0=off 1=on",
	   "- button reset|copy: test buttons states\n"
);

#endif

