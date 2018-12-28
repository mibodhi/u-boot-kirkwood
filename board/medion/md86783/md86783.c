/*
 * Copyright (C) 2019  Damien Merenne <dmerenne@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#include <common.h>
#include <miiphy.h>
#include <asm/arch/soc.h>
#include <asm/arch/mpp.h>
#include <asm/arch/cpu.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include "md86783.h"
#include <asm/arch/gpio.h>
#include <asm/mach-types.h>

DECLARE_GLOBAL_DATA_PTR;

static void board_reset_led(void)
{
        kw_gpio_set_valid(GPIO_LED_HDD0_RED, 1);
        kw_gpio_set_valid(GPIO_LED_HDD1_RED, 1);
        kw_gpio_set_valid(GPIO_LED_SYS_ORANGE, 1);
        kw_gpio_set_valid(GPIO_LED_SYS_BLUE, 1);
        kw_gpio_set_valid(GPIO_SATA0_POWER, 1);
        kw_gpio_set_valid(GPIO_SATA1_POWER, 1);
        kw_gpio_set_valid(GPIO_BTN_COPY, GPIO_INPUT_OK);
        kw_gpio_set_valid(GPIO_BTN_RESET, GPIO_INPUT_OK);
        kw_gpio_set_valid(GPIO_BTN_POWER, GPIO_INPUT_OK);


        kw_gpio_direction_output(GPIO_LED_HDD0_RED, 1);
        kw_gpio_direction_output(GPIO_LED_HDD1_RED, 1);
        kw_gpio_direction_output(GPIO_LED_SYS_ORANGE, 1);
        kw_gpio_direction_output(GPIO_LED_SYS_BLUE, 0);
        kw_gpio_set_blink(GPIO_LED_SYS_BLUE, 1);
}

int board_early_init_f(void)
{
	/*
	 * default gpio configuration
	 * There are maximum 64 gpios controlled through 2 sets of registers
	 * the below configuration configures mainly initial LED status
	 */
        mvebu_config_gpio(GPIO_MD86783_INIT_LOW_VALUES,
                          GPIO_MD86783_INIT_HIGH_VALUES,
                          GPIO_MD86783_INIT_LOW_OUTPUT_ENABLE,
                          GPIO_MD86783_INIT_LOW_OUTPUT_ENABLE);


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
		MPP8_TW_SDA,
		MPP9_TW_SCK,
		MPP10_GPO,
		MPP11_GPIO,
		MPP12_GPO,
		MPP13_GPIO,
		MPP14_GPIO,
		MPP15_UART1_TXD,
                MPP16_UART1_RXD,
		MPP17_GPIO,
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
		MPP29_GPIO,
		MPP30_GPIO,
		MPP31_GPIO,
		MPP32_GPIO,
 		MPP33_GPIO,
		MPP34_GPIO,
		MPP35_GPIO,
		0
	};
	kirkwood_mpp_conf(kwmpp_config, NULL);

        kw_gpio_set_valid(GPIO_LED_HDD0_RED, 1);
        kw_gpio_set_valid(GPIO_LED_HDD1_RED, 1);
        kw_gpio_set_valid(GPIO_LED_SYS_ORANGE, 1);
        kw_gpio_set_valid(GPIO_LED_SYS_BLUE, 1);
        kw_gpio_set_valid(GPIO_SATA0_POWER, 1);
        kw_gpio_set_valid(GPIO_SATA1_POWER, 1);
        kw_gpio_set_valid(GPIO_BTN_COPY, GPIO_INPUT_OK);
        kw_gpio_set_valid(GPIO_BTN_RESET, GPIO_INPUT_OK);
        kw_gpio_set_valid(GPIO_BTN_POWER, GPIO_INPUT_OK);

        kw_gpio_direction_output(GPIO_LED_HDD0_RED, 1);
        kw_gpio_direction_output(GPIO_LED_HDD1_RED, 1);
        kw_gpio_direction_output(GPIO_LED_SYS_ORANGE, 1);
        kw_gpio_direction_output(GPIO_LED_SYS_BLUE, 0);
	return 0;
}

int board_init(void)
{
	/*
	 * arch number of board
	 */
	gd->bd->bi_arch_number = MACH_TYPE_MD86783;

	/* address of boot parameters */
	gd->bd->bi_boot_params = mvebu_sdram_bar(0) + 0x100;
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

	/* The Medion MD86783 uses the 88E1310S Alaska (interface identical to 88E1318) */
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

        board_reset_led();

	switch (val) {
                case BOOTSTAGE_ID_DECOMP_IMAGE:
                        kw_gpio_set_value(GPIO_LED_SYS_ORANGE, 1);
                        kw_gpio_set_value(GPIO_LED_SYS_BLUE, 1);
                        break;
                case BOOTSTAGE_ID_RUN_OS:
                        kw_gpio_set_blink(GPIO_LED_SYS_BLUE, 1);
                        break;
                case BOOTSTAGE_ID_NET_START:
                        kw_gpio_set_blink(GPIO_LED_HDD0_RED, 1);
                        break;
                case BOOTSTAGE_ID_NET_LOADED:
                        kw_gpio_set_value(GPIO_LED_HDD0_RED, 1);
                        break;
                case -BOOTSTAGE_ID_NET_NETLOOP_OK:
                case -BOOTSTAGE_ID_NET_LOADED:
                        kw_gpio_set_blink(GPIO_LED_HDD1_RED, 1);
		break;
	default:
		if (val < 0) {
			/* error */
			printf("Error occured, error code = %d\n", -val);
                        kw_gpio_set_blink(GPIO_LED_HDD0_RED, 1);
                        kw_gpio_set_blink(GPIO_LED_HDD1_RED, 1);
                        kw_gpio_set_blink(GPIO_LED_SYS_ORANGE, 1);
		}
		break;
	}
}
#endif

#if defined(CONFIG_KIRKWOOD_GPIO)
/* Return GPIO button status */
/*
un-pressed:
 gpio-14 (Reset Button ) in hi (act lo) - IRQ edge (clear )
 gpio-35 (Copy Button  ) in hi (act lo) - IRQ edge (clear )
 gpio-26 (Power Button ) in lo (act hi) - IRQ edge (clear )
pressed
 gpio-14 (Reset Button ) in lo (act hi) - IRQ edge (clear )
 gpio-35 (Copy Button  ) in lo (act hi) - IRQ edge (clear )
 gpio-26 (Power Button ) in hi (act lo) - IRQ edge (clear )
*/

static int
do_read_button(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        int value = -1;
	if (strcmp(argv[1], "power") == 0) {
                kw_gpio_direction_input(GPIO_BTN_POWER);
                value = kw_gpio_get_value(GPIO_BTN_POWER);
	} else if (strcmp(argv[1], "reset") == 0) {
		value = !kw_gpio_get_value(GPIO_BTN_RESET);
        } else if (strcmp(argv[1], "copy") == 0) {
		value = !kw_gpio_get_value(GPIO_BTN_COPY);
        } else {
                printf("invalid button %s", argv[1]);
                return -1;
        }

        if (value < 0) {
                printf("unable to read button %s", argv[1]);
                return -1;
        }

        printf("button %s state: %s\n", argv[1], (value ? "pressed" : "normal"));
        return 0;
}


U_BOOT_CMD(button, 2, 0, do_read_button,
	   "Return GPIO button status 0=off 1=on",
	   "- button power|reset|copy: test buttons states\n"
);

#endif
