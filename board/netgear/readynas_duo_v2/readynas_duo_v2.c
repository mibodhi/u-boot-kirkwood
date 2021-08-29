/*
 * Copyright (C) 2021 ingmar-k <ingmar_klein@web.de>
 *
 * Based on
 * Copyright (C) 2015-2017 bodhi <mibodhi@gmail.com>
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
#include "readynas_duo_v2.h"
#include <asm/arch/gpio.h>
#include <asm/mach-types.h>
#include <i2c.h>

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	/*
	 * default gpio configuration
	 * There are maximum 64 gpios controlled through 2 sets of registers
	 * the below configuration configures mainly initial LED status
	 */

	/* Multi-Purpose Pins Functionality configuration */
	/* (all LEDs & power off active high) */
	u32 kwmpp_config[] = {
		MPP0_NF_IO2,		/* nand */
		MPP1_NF_IO3,		/* nand */
		MPP2_NF_IO4,		/* nand */
		MPP3_NF_IO5,		/* nand */
		MPP4_NF_IO6,		/* nand ; pmx-sata1 */
		MPP5_NF_IO7,		/* nand ; pmx-sata0 */
		MPP6_SYSRST_OUTn,	/* reset out*/
		MPP7_PEX_RST_OUTn,	/* PEX reset */
		MPP8_TW_SDA,		/* SDA */
		MPP9_TW_SCK,		/* SCK */
		MPP10_UART0_TXD,	/* UART0_TX */
		MPP11_UART0_RXD,	/* UART0_RX */
		MPP12_GPO,
		MPP13_GPIO,			/* pmx-button-reset */
		MPP14_GPIO,
		MPP15_GPIO,
		MPP16_GPIO,
		MPP17_GPIO,
		MPP18_NF_IO0,		/* nand */
		MPP19_NF_IO1,		/* nand */
		MPP20_GPIO,			/* pmx-sata1 */
		MPP21_GPIO,			/* pmx-sata0 */
		MPP22_GPIO,			/* pmx-sata1 ; pmx-led-blue-disk2 */
		MPP23_GPIO,			/* pmx-sata0 ; pmx-led-blue-disk1 */
		MPP24_GPIO,
		MPP25_GPIO,
		MPP26_GPIO,			/* HDD-0 Power */
		MPP27_GPIO,
		MPP28_GPIO,			/* HDD-1 Power */
		MPP29_GPIO,			/* pmx-led-blue-backup */
		MPP30_GPIO,			/* pmx-poweroff */
		MPP31_GPIO,			/* pmx-led-blue-power */
		MPP32_GPIO,
		MPP33_GPIO,
		MPP34_GPIO,
		MPP35_GPIO,			/* USB 2.0 power */
		MPP36_GPIO,
		MPP37_GPIO,
		MPP38_GPIO,			/* pmx-led-blue-activity */
		MPP39_GPIO,
		MPP40_GPIO,
		MPP41_GPIO,
		MPP42_GPIO,
		MPP43_GPIO,
		MPP44_GPIO,
		MPP45_GPIO,			/* pmx-button-backup  */
		MPP46_GPIO,			/* USB 3.0 Power */
		MPP47_GPIO,			/* pmx-button-power   */
		MPP48_GPIO,
		MPP49_GPIO,
		0
	};
	kirkwood_mpp_conf(kwmpp_config, NULL);
	return 0;
}

int board_init(void)
{
	char **val1, **val2; // variables for getting the 'yes' or 'no' switch from some environment variables
	val1="";
	val2="";
	/*
	 * arch number of board
	 */
	gd->bd->bi_arch_number = MACH_TYPE_DB88F6281_BP;

	/* address of boot parameters */
	gd->bd->bi_boot_params = mvebu_sdram_bar(0) + 0x100;

    kw_gpio_set_valid(22, 1); /* pmx-led-blue-disk2 */
    kw_gpio_set_valid(23, 1); /* pmx-led-blue-disk1 */
    kw_gpio_set_valid(26, 1); /* HDD0 Power */
    kw_gpio_set_valid(28, 1); /* HDD1 power */
    kw_gpio_set_valid(29, 1); /* pmx-led-blue-backup */
    kw_gpio_set_valid(31, 1); /* pmx-led-blue-power */
    kw_gpio_set_valid(35, 1); /* USB 2.0 Power */
    kw_gpio_set_valid(38, 1); /* pmx-led-blue-activity */
    kw_gpio_set_valid(46, 1); /* USB 3.0 Power */

    // Enable power LED
    kw_gpio_direction_output(31, 0);
	kw_gpio_set_value(31, 0);

	// initialize the cooling fan
	unsigned char byte=0x40; // the higher the hex value, the faster the fan
	int fi;
	// try fan init 10 times, due to unreliable i2c access
	for (fi = 0; fi < 10; fi++)
	{
		if(i2c_write(CONFIG_SYS_I2C_G762_ADDR, 0x0, 1, &byte, 1) != 0)
		{
			if(fi == 9)
			{
				puts ("Error writing the i2c chip : G76x(Fan controller).\n");
			}
			else
			{
				continue;
			}
			udelay(50000);
		}
		else
		{
			puts ("G762 Fan controller: Successfully initialized.\n");
			break;
		}
	}

	// Enabling USB 2.0 Power
	puts ("Enabled USB 2.0 Power.\n");
	kw_gpio_direction_output(35, 0);
	kw_gpio_set_value(35, 1);
	val1="";

	// Enabling USB 3.0 Power
	puts ("Enabled USB 3.0 Power.\n");
	kw_gpio_direction_output(46, 0);
	kw_gpio_set_value(46, 1);

	puts ("Running HDD staggered spin-up...\n"); // be a little easier on the power supply
	puts ("Enabling HDD-0 Power in 1s.\n");
	udelay(1000000);
	kw_gpio_direction_output(26, 0);
	kw_gpio_set_value(26, 1);
	puts ("Enabling HDD-1 Power in 3s.\n");
	udelay(3000000);
	kw_gpio_direction_output(28, 0);
	kw_gpio_set_value(28, 1);

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

	// commented out PHY reset, as it always failed, anyway (worked around via mii tool call in 'preboot' variable)
	/* reset the phy */
	//miiphy_reset(name, devadr);

	/* The READYNAS DUO V2 uses the 88E1310S Alaska (interface identical to 88E1318) */
	/* and has an MCU attached to the LED[2] via tristate interrupt */
	reg = 0;

	/* switch to LED register page */
	miiphy_write(name, devadr, MV88E1318_PGADR_REG, MV88E1318_LED_PG);
	/* read out LED polarity register */
	miiphy_read(name, devadr, MV88E1318_LED_POL_REG, &reg);
	/* clear 4, set 5 - LED2 low, tri-state */
	reg &= ~(MV88E1318_LED2_4);
	reg |= (MV88E1318_LED2_5);
	/* write back LED polarity register */
	miiphy_write(name, devadr, MV88E1318_LED_POL_REG, reg);
	/* jump back to page 0, per the PHY chip documenation. */
	miiphy_write(name, devadr, MV88E1318_PGADR_REG, 0);

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
	struct kwgpio_registers *gpio0 = (struct kwgpio_registers *)MVEBU_GPIO0_BASE;
	u32 dout0 = readl(&gpio0->dout);
	u32 blen0 = readl(&gpio0->blink_en);

	struct kwgpio_registers *gpio1 = (struct kwgpio_registers *)MVEBU_GPIO1_BASE;
	u32 dout1 = readl(&gpio1->dout);
	u32 blen1 = readl(&gpio1->blink_en);
}
#endif

#if defined(CONFIG_KIRKWOOD_GPIO)
/* Return GPIO button status */
/*
un-pressed:
 gpio-36 (Reset Button ) in hi (act lo) - IRQ edge (clear )
 gpio-37 (Copy Button  ) in hi (act lo) - IRQ edge (clear )
 gpio-46 (Power Button ) in lo (act hi) - IRQ edge (clear )
pressed
 gpio-36 (Reset Button ) in lo (act hi) - IRQ edge (clear )
 gpio-37 (Copy Button  ) in lo (act hi) - IRQ edge (clear )
 gpio-46 (Power Button ) in hi (act lo) - IRQ edge (clear )
*/

static int
do_read_button(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (strcmp(argv[1], "power") == 0) {
			kw_gpio_set_valid(BTN_POWER, GPIO_INPUT_OK);
			kw_gpio_direction_input(BTN_POWER);
			return !kw_gpio_get_value(BTN_POWER);
	}
	else if (strcmp(argv[1], "reset") == 0)
		return kw_gpio_get_value(BTN_RESET);
	else if (strcmp(argv[1], "copy") == 0)
		return kw_gpio_get_value(BTN_COPY);
	else
		return -1;
}


U_BOOT_CMD(button, 2, 0, do_read_button,
	   "Return GPIO button status 0=off 1=on",
	   "- button power|reset|copy: test buttons states\n"
);

#endif

