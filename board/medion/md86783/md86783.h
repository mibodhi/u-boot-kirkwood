/*
 * Copyright (C) 2019	Damien Merenne <dmerenne@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#ifndef __MD86783_H
#define __MD86783_H

#define GPIO_MD86783_INIT_LOW_VALUES          0x05c22000
#define GPIO_MD86783_INIT_LOW_OUTPUT_ENABLE   0xfa3ddfff
#define GPIO_MD86783_INIT_HIGH_VALUES         0x00000000
#define GPIO_MD86783_INIT_HIGH_OUTPUT_ENABLE  0fffffffb


#define GPIO_LED_HDD0_RED		23 // active low
#define GPIO_LED_HDD1_RED		22 // active low
#define GPIO_LED_SYS_ORANGE		25 // active low
#define GPIO_LED_SYS_BLUE          	24 // active high
#define GPIO_SATA0_POWER                17 // active high
#define GPIO_SATA1_POWER                13 // active high
#define GPIO_BTN_COPY                   35 // active low
#define GPIO_BTN_RESET                  14 // active low
#define GPIO_BTN_POWER                  26 // active to low

/* PHY related */
#define MV88E1318_PGADR_REG		22
#define MV88E1318_MAC_CTRL_PG	        2
#define MV88E1318_MAC_CTRL_REG	        21
#define MV88E1318_RGMII_TXTM_CTRL	(1 << 4)
#define MV88E1318_RGMII_RXTM_CTRL	(1 << 5)
#define MV88E1318_LED_PG                3
#define MV88E1318_LED_POL_REG	        17
#define MV88E1318_LED2_4	        (1 << 4)
#define MV88E1318_LED2_5	        (1 << 5)


#endif /* __MD86783_H */

/*
6 is active on linux not in uboot
10 et 11 active sous linux et pas sous uboot
*/
