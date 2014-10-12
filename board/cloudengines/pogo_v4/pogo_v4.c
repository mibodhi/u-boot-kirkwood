/*
 * Copyright (C) 2014 bodhi
 *
 * Based on
 *
 * Copyright (C) 2014 <ebbes.ebbes@gmail.com>
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

#include <common.h>
#include <miiphy.h>
#include <asm/arch/cpu.h>
#include <asm/arch/kirkwood.h>
#include <asm/arch/mpp.h>
#include <asm/io.h>
#include "pogo_v4.h"

#ifdef CONFIG_KIRKWOOD_MMC
#include <kirkwood_mmc.h>
#endif /* CONFIG_KIRKWOOD_MMC */


DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	/*
	 * default gpio configuration
	 * There are maximum 64 gpios controlled through 2 sets of registers
	 * the  below configuration configures mainly initial LED status
	 */
	kw_config_gpio(POGO_V4_OE_VAL_LOW,
			POGO_V4_OE_VAL_HIGH,
			POGO_V4_OE_LOW, POGO_V4_OE_HIGH);

	/* Multi-Purpose Pins Functionality configuration */
	u32 kwmpp_config[] = {
		MPP0_NF_IO2,
		MPP1_NF_IO3,
		MPP2_NF_IO4,
		MPP3_NF_IO5,
		MPP4_NF_IO6,
		MPP5_NF_IO7,
		MPP6_SYSRST_OUTn,
		MPP7_GPO,
		MPP8_TW_SDA,
		MPP9_TW_SCK,
		MPP10_UART0_TXD,
		MPP11_UART0_RXD,
		MPP12_SD_CLK,
		MPP13_SD_CMD,
		MPP14_SD_D0,
		MPP15_SD_D1,
		MPP16_SD_D2,
		MPP17_SD_D3,
		MPP18_NF_IO0,
		MPP19_NF_IO1,
                MPP20_SATA1_ACTn,
                MPP21_SATA0_ACTn,
		MPP22_GPIO,	/* Green LED */
		MPP23_GPIO,
		MPP24_GPIO,	/* Red LED */
		MPP25_GPIO,
		MPP26_GPIO,
		MPP27_GPIO,
		MPP28_GPIO,
		MPP29_GPIO,	/* Eject button */
		MPP30_GPIO,
		MPP31_GPIO,
		MPP32_GPIO,
		MPP33_GPIO,
		MPP34_GPIO,
		MPP35_GPIO,	/* FR6192 has only 36 GPIOs */
		0
	};
	kirkwood_mpp_conf(kwmpp_config, NULL);

	return 0;
}

int board_init(void)
{
	/* Boot parameters address */
	gd->bd->bi_boot_params = kw_sdram_bar(0) + 0x100;

	return 0;
}

#ifdef CONFIG_RESET_PHY_R
/* Configure and initialize PHY */
void reset_phy(void)
{
	u16 reg;
	u16 devadr;
	char *name = "egiga0";

	if (miiphy_set_current_dev(name))
		return;

	/* command to read PHY dev address */
	if (miiphy_read(name, 0xEE, 0xEE, (u16 *) &devadr)) {
		printf("Err..(%s) could not read PHY dev address\n", __func__);
		return;
	}

	/*
	 * Enable RGMII delay on Tx and Rx for CPU port
	 * Ref: sec 4.7.2 of chip datasheet
	 */
	miiphy_write(name, devadr, MV88E1116_PGADR_REG, 2);
	miiphy_read(name, devadr, MV88E1116_MAC_CTRL_REG, &reg);
	reg |= (MV88E1116_RGMII_RXTM_CTRL | MV88E1116_RGMII_TXTM_CTRL);
	miiphy_write(name, devadr, MV88E1116_MAC_CTRL_REG, reg);
	miiphy_write(name, devadr, MV88E1116_PGADR_REG, 0);

	/* reset the phy */
	miiphy_reset(name, devadr);

	debug("88E1116 Initialized on %s\n", name);
}
#endif /* CONFIG_RESET_PHY_R */

#ifdef CONFIG_KIRKWOOD_MMC
int board_mmc_init(bd_t *bis)
{
       kw_mmc_initialize(bis);
       return 0;
}
#endif /* CONFIG_KIRKWOOD_MMC */


#define GREEN_LED	(1 << 22)
#define RED_LED		(1 << 24)
#define BOTH_LEDS	(GREEN_LED | RED_LED)
#define NEITHER_LED	0

static void set_leds(u32 leds, u32 blinking)
{
	struct kwgpio_registers *r;
	u32 oe;
	u32 bl;

	r = (struct kwgpio_registers *)KW_GPIO0_BASE;
	oe = readl(&r->oe) | BOTH_LEDS;
	writel(oe & ~leds, &r->oe);	/* active low */
	bl = readl(&r->blink_en) & ~BOTH_LEDS;
	writel(bl | blinking, &r->blink_en);
}

void show_boot_progress(int val)
{
	switch (val) {
	case BOOTSTAGE_ID_RUN_OS:		/* booting Linux */
		set_leds(BOTH_LEDS, NEITHER_LED);
		break;
	case BOOTSTAGE_ID_NET_ETH_START:	/* Ethernet initialization */
		set_leds(GREEN_LED, GREEN_LED);
		break;
	default:
		if (val < 0)	/* error */
			set_leds(RED_LED, RED_LED);
		break;
	}
}

