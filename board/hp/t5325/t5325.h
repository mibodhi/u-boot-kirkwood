/*
 * (C) Copyright 2016 bodhi <mibodhi@gmail.com>
 *
 * Based on
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __T5325_H
#define __T5325_H

/* low GPIO's */
#define SYS_GREEN_LED		(1 << 21)

#define T5325_OE_LOW		(~(0))
#define T5325_OE_VAL_LOW	(SYS_GREEN_LED)

/* high GPIO's */

#define USB_HDD_POWER          (1 << 15)

#define T5325_OE_HIGH		(~(0))
#define T5325_OE_VAL_HIGH	(USB_HDD_POWER)

/* PHY related */
#define MV88E3016_LED_FCTRL_REG		24 //???
#define MV88E3016_MAC_CTRL_REG		28
#define MV88E3016_PGADR_REG		22
#define MV88E3016_RGMII_TXTM_CTRL	(1 << 4)   //???
#define MV88E3016_RGMII_RXTM_CTRL	(1 << 5)   //???

#define BTN_POWER	45

#endif /* __T5325_H */
