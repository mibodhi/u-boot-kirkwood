/*
 * (C) Copyright 2016-2017 bodhi <mibodhi@gmail.com>
 *
 * Based on
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <miiphy.h>
#include <asm/arch/cpu.h>
#include <asm/arch/soc.h>
#include <asm/arch/mpp.h>
#include <asm/arch/gpio.h>
#include <asm/io.h>
#include "t5325.h"
#include <asm/mach-types.h>

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	/*
	 * default gpio configuration
	 * There are maximum 64 gpios controlled through 2 sets of registers
	 * the  below configuration configures mainly initial LED status
	 */
	mvebu_config_gpio(T5325_OE_VAL_LOW, 
			T5325_OE_VAL_HIGH, 
			T5325_OE_LOW, 
			T5325_OE_HIGH);

	/* Multi-Purpose Pins Functionality configuration */
	static const u32 kwmpp_config[] = {
		MPP0_GPIO,
		MPP1_SPI_MOSI,
		MPP2_SPI_SCK,
		MPP3_SPI_MISO,
		MPP4_NF_IO6,
		MPP5_NF_IO7,
		MPP6_SYSRST_OUTn,
                MPP7_SPI_SCn,
		MPP8_UART0_RTS,
		MPP9_UART0_CTS,
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
		MPP22_GPIO,
		MPP23_GPIO,
		MPP24_GPIO,
		MPP25_GPIO,
		MPP26_GPIO,
		MPP27_GPIO,
		MPP28_GPIO,
		MPP29_TSMP9,
		MPP30_GPIO,
		MPP31_GPIO,
		MPP32_GPIO,
		MPP33_GPIO,
                MPP34_GPIO,
                MPP35_GPIO,
		MPP34_GPIO,
		MPP35_GPIO,
		MPP36_GPIO,
		MPP37_GPIO,
		MPP38_GPIO,
		MPP39_GPIO,
		MPP40_GPIO,
		MPP41_GPIO,
		MPP42_GPIO,
		MPP43_GPIO,
		MPP44_GPIO,
		MPP45_GPIO,
		MPP46_GPIO,
		MPP47_GPIO,	/* USB HDD Power */
		MPP48_GPIO,
		MPP49_GPIO,
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
	gd->bd->bi_arch_number = MACH_TYPE_T5325;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = mvebu_sdram_bar(0) + 0x100;

        kw_gpio_set_valid(47, 1);
        kw_gpio_direction_output(47, 0);
        kw_gpio_set_value(47, 1);

	return 0;
}

#ifdef CONFIG_RESET_PHY_R
/* Configure and enable MV88E3016 PHY */
void reset_phy(void)
{
	u16 reg;
	u16 devadr;
	char *name = "egiga0";

	if (miiphy_set_current_dev(name))
		return;

	
	if (miiphy_read(name, 0xEE, 0xEE, (u16 *) &devadr)) {
		printf("Err..%s could not read PHY dev address\n",
			__FUNCTION__);
		return;
	}

	/* Set RGMII (RX) delay and copper mode */
	miiphy_read(name, devadr, 28, &reg);
	reg &= ~( (1<<3) | (1<<10) | (1<<11) );
	reg |= (1<<10);
	miiphy_write(name, devadr, 28, reg);

	/* reset the phy */
	miiphy_reset(name, devadr);

	printf("88E3016 Initialized on %s\n", name);
}
#endif /* CONFIG_RESET_PHY_R */

#if defined(CONFIG_KIRKWOOD_GPIO)
/* Return GPIO button status */
/*
un-pressed:
 gpio-45 (Power Button ) in hi (act lo) - IRQ edge (clear )
pressed
 gpio-45 (Power Button ) in lo (act hi) - IRQ edge (clear )
*/




static int
do_read_button(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (strcmp(argv[1], "power") == 0) {
			kw_gpio_set_valid(BTN_POWER, GPIO_INPUT_OK);
			kw_gpio_direction_input(BTN_POWER);
			return kw_gpio_get_value(BTN_POWER);
	}
	else
		return -1;
}


U_BOOT_CMD(button, 2, 0, do_read_button,
	   "Return GPIO button status 0=off 1=on",
	   "- button power: test buttons states\n"
);

#endif

