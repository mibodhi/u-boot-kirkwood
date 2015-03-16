/*
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
#include <asm/arch/kirkwood.h>
#include <asm/arch/mpp.h>
#include <asm/arch/cpu.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include "nsa320.h"

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	/*
	 * default gpio configuration
	 * There are maximum 64 gpios controlled through 2 sets of registers
	 * the below configuration configures mainly initial LED status
	 */
	kw_config_gpio(NSA320_VAL_LOW, NSA320_VAL_HIGH,
		       NSA320_OE_LOW, NSA320_OE_HIGH);

	/* Multi-Purpose Pins Functionality configuration */
	/* (all LEDs & power off active high) */
	u32 kwmpp_config[] = {
		MPP0_NF_IO2,
		MPP1_NF_IO3,
		MPP2_NF_IO4,
		MPP3_NF_IO5,
		MPP4_NF_IO6,
		MPP5_NF_IO7,
		MPP6_SYSRST_OUTn,
		MPP7_GPO,
		MPP8_TW_SDA,		/* PCF8563 RTC chip   */
		MPP9_TW_SCK,		/* connected to TWSI  */
		MPP10_UART0_TXD,
		MPP11_UART0_RXD,
		MPP12_GPO,		/* HDD2 LED (green)   */
		MPP13_GPIO,		/* HDD2 LED (red)     */
		MPP14_GPIO,		/* MCU DATA pin (in)  */
		MPP15_GPIO,		/* USB LED (green)    */
		MPP16_GPIO,		/* MCU CLK pin (out)  */
		MPP17_GPIO,		/* MCU ACT pin (out)  */
		MPP18_NF_IO0,
		MPP19_NF_IO1,
		MPP20_GPIO,
		MPP21_GPIO,		/* USB power          */
		MPP22_GPIO,
		MPP23_GPIO,
		MPP24_GPIO,
		MPP25_GPIO,
		MPP26_GPIO,
		MPP27_GPIO,
		MPP28_GPIO,		/* SYS LED (green)    */
		MPP29_GPIO,		/* SYS LED (orange)   */
		MPP30_GPIO,
		MPP31_GPIO,
		MPP32_GPIO,
		MPP33_GPIO,
		MPP34_GPIO,
		MPP35_GPIO,
		MPP36_GPIO,		/* reset button       */
		MPP37_GPIO,		/* copy button        */
		MPP38_GPIO,		/* VID B0             */
		MPP39_GPIO,		/* COPY LED (green)   */
		MPP40_GPIO,		/* COPY LED (red)     */
		MPP41_GPIO,		/* HDD1 LED (green)   */
		MPP42_GPIO,		/* HDD1 LED (red)     */
		MPP43_GPIO,		/* HTP pin            */
		MPP44_GPIO,		/* buzzer             */
		MPP45_GPIO,		/* VID B1             */
		MPP46_GPIO,		/* power button       */
		MPP47_GPIO,		/* power resume data  */
		MPP48_GPIO,		/* power off          */
		MPP49_GPIO,		/* power resume clock */
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
	gd->bd->bi_arch_number = MACH_TYPE_NSA320;

	/* address of boot parameters */
	gd->bd->bi_boot_params = kw_sdram_bar(0) + 0x100;

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
	miiphy_write(name, devadr, MV88E1318_PGADR_REG, 2);
	miiphy_read(name, devadr, MV88E1318_MAC_CTRL_REG, &reg);
	reg |= (MV88E1318_RGMII_RXTM_CTRL | MV88E1318_RGMII_TXTM_CTRL);
	miiphy_write(name, devadr, MV88E1318_MAC_CTRL_REG, reg);
	miiphy_write(name, devadr, MV88E1318_PGADR_REG, 0);

	/* reset the phy */
	miiphy_reset(name, devadr);

	printf("MV88E1318 PHY initialized on %s\n", name);
}
#endif /* CONFIG_RESET_PHY_R */

#ifdef CONFIG_SHOW_BOOT_PROGRESS
void show_boot_progress(int val)
{
	struct kwgpio_registers *gpio0 = (struct kwgpio_registers *)KW_GPIO0_BASE;
	u32 dout0 = readl(&gpio0->dout);
	u32 blen0 = readl(&gpio0->blink_en);

	struct kwgpio_registers *gpio1 = (struct kwgpio_registers *)KW_GPIO1_BASE;
	u32 dout1 = readl(&gpio1->dout);
	u32 blen1 = readl(&gpio1->blink_en);

	switch (val) {
	case BOOTSTAGE_ID_DECOMP_IMAGE:
		writel(blen0 & ~(SYS_GREEN_LED | SYS_ORANGE_LED), &gpio0->blink_en);
		writel((dout0 & ~SYS_GREEN_LED) | SYS_ORANGE_LED, &gpio0->dout);
		break;
	case BOOTSTAGE_ID_RUN_OS:
		writel(dout0 & ~SYS_ORANGE_LED, &gpio0->dout);
		writel(blen0 | SYS_GREEN_LED, &gpio0->blink_en);
		break;
	case BOOTSTAGE_ID_NET_START:
		writel(dout1 & ~COPY_RED_LED, &gpio1->dout);
		writel((blen1 & ~COPY_RED_LED) | COPY_GREEN_LED, &gpio1->blink_en);
		break;
	case BOOTSTAGE_ID_NET_LOADED:
		writel(blen1 & ~(COPY_RED_LED | COPY_GREEN_LED), &gpio1->blink_en);
		writel((dout1 & ~COPY_RED_LED) | COPY_GREEN_LED, &gpio1->dout);
		break;
	case -BOOTSTAGE_ID_NET_NETLOOP_OK:
	case -BOOTSTAGE_ID_NET_LOADED:
		writel(dout1 & ~COPY_GREEN_LED, &gpio1->dout);
		writel((blen1 & ~COPY_GREEN_LED) | COPY_RED_LED, &gpio1->blink_en);
		break;
	default:
		if (val < 0) {
			/* error */
			printf("Error occured, error code = %d\n", -val);
			writel(dout0 & ~SYS_GREEN_LED, &gpio0->dout);
			writel(blen0 | SYS_ORANGE_LED, &gpio0->blink_en);
		}
		break;
	}
}
#endif
