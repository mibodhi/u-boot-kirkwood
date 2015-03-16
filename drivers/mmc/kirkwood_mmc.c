/*
 * (C) Copyright 2014 bodhi <mibodhi@gmail.com>
 *
 * Based on
 *
 * (C) Copyright 2014 <ebbes.ebbes@gmail.com>
 *
 * Based on
 *
 * Driver for Marvell SDIO/MMC controller
 *
 * (C) Copyright 2012
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Gérald Kerma <uboot at doukki.net>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <malloc.h>
#include <part.h>
#include <mmc.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/kirkwood.h>

#include <kirkwood_mmc.h>

#define DRIVER_NAME	"kwsdio"

static int kw_mmc_setup_data(struct mmc_data *data)
{
	u32 ctrl_reg;

#ifdef DEBUG
	printf("%s, data %s : blocks=%d blksz=%d\n", DRIVER_NAME,
		(data->flags & MMC_DATA_READ) ? "read" : "write",
		data->blocks, data->blocksize);
#endif

	/* default to maximum timeout */
	ctrl_reg = kwsd_read(SDIO_HOST_CTRL);
	ctrl_reg |= SDIO_HOST_CTRL_TMOUT(SDIO_HOST_CTRL_TMOUT_MAX);
	kwsd_write(SDIO_HOST_CTRL, ctrl_reg);

	if (data->flags & MMC_DATA_READ) {
		kwsd_write(SDIO_SYS_ADDR_LOW,(u32)data->dest & 0xffff);
		kwsd_write(SDIO_SYS_ADDR_HI,(u32)data->dest >> 16);
	} else {
		kwsd_write(SDIO_SYS_ADDR_LOW,(u32)data->src & 0xffff);
		kwsd_write(SDIO_SYS_ADDR_HI,(u32)data->src >> 16);
	}

	kwsd_write(SDIO_BLK_COUNT, data->blocks);
	kwsd_write(SDIO_BLK_SIZE, data->blocksize);

	return 0;
}

static int kw_mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	int	timeout = 10;
	int err;
	ushort waittype = 0;
	ushort resptype = 0;
	ushort xfertype = 0;
	ushort resp_indx = 0;

#ifdef CONFIG_MMC_DEBUG
	printf("cmdidx [0x%x] resp_type[0x%x] cmdarg[0x%x]\n", cmd->cmdidx, cmd->resp_type, cmd->cmdarg);
#endif

	udelay(10000);

#ifdef CONFIG_MMC_DEBUG
	printf("%s: cmd %d (hw state 0x%04x)\n", DRIVER_NAME, cmd->cmdidx, kwsd_read(SDIO_HW_STATE));
#endif

	/* Checking if card is busy */
	while ((kwsd_read(SDIO_HW_STATE) & CARD_BUSY)) {
		if (timeout == 0) {
			printf("%s: card busy!\n", DRIVER_NAME);
			return -1;
		}
		timeout--;
		udelay(1000);
	}

	/* Set up for a data transfer if we have one */
	if (data) {
		if ((err = kw_mmc_setup_data(data)))
			return err;
	}

	resptype = SDIO_CMD_INDEX(cmd->cmdidx);

	/* Analyzing resptype/xfertype/waittype for the command */
	if (cmd->resp_type & MMC_RSP_BUSY)
		resptype |= SDIO_CMD_RSP_48BUSY;
	else if (cmd->resp_type & MMC_RSP_136)
		resptype |= SDIO_CMD_RSP_136;
	else if (cmd->resp_type & MMC_RSP_PRESENT)
		resptype |= SDIO_CMD_RSP_48;
	else
		resptype |= SDIO_CMD_RSP_NONE;

	if (cmd->resp_type & MMC_RSP_CRC)
		resptype |= SDIO_CMD_CHECK_CMDCRC;

	if (cmd->resp_type & MMC_RSP_OPCODE)
		resptype |= SDIO_CMD_INDX_CHECK;

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		resptype |= SDIO_UNEXPECTED_RESP;
		waittype |= SDIO_NOR_UNEXP_RSP;
	}

	if (data) {
		resptype |= SDIO_CMD_DATA_PRESENT | SDIO_CMD_CHECK_DATACRC16;
		xfertype |= SDIO_XFER_MODE_HW_WR_DATA_EN;
		if (data->flags & MMC_DATA_READ) {
			xfertype |= SDIO_XFER_MODE_TO_HOST;
			waittype = SDIO_NOR_DMA_INI;
		} else
			waittype |= SDIO_NOR_XFER_DONE;
	} else
		waittype |= SDIO_NOR_CMD_DONE;

	/* Setting cmd arguments */
	kwsd_write(SDIO_ARG_LOW, cmd->cmdarg & 0xffff);
	kwsd_write(SDIO_ARG_HI, cmd->cmdarg >> 16);

	/* Setting Xfer mode */
	kwsd_write(SDIO_XFER_MODE, xfertype);

	kwsd_write(SDIO_NOR_INTR_STATUS, ~SDIO_NOR_CARD_INT);
	kwsd_write(SDIO_ERR_INTR_STATUS, SDIO_POLL_MASK);

	/* Sending command */
	kwsd_write(SDIO_CMD, resptype);
/*
	kwsd_write(SDIO_CMD, KW_MMC_MAKE_CMD(cmd->cmdidx, resptype));
*/

	kwsd_write(SDIO_NOR_INTR_EN, SDIO_POLL_MASK);
	kwsd_write(SDIO_ERR_INTR_EN, SDIO_POLL_MASK);

	/* Waiting for completion */
	timeout = 1000000;

	while (!((kwsd_read(SDIO_NOR_INTR_STATUS)) & waittype)) {
		if (kwsd_read(SDIO_NOR_INTR_STATUS) & SDIO_NOR_ERROR) {
#ifdef DEBUG
			printf("%s: kw_mmc_send_cmd: error! cmdidx : %d, err reg: %04x\n", DRIVER_NAME, cmd->cmdidx, kwsd_read(SDIO_ERR_INTR_STATUS));
#endif
			if (kwsd_read(SDIO_ERR_INTR_STATUS) & (SDIO_ERR_CMD_TIMEOUT | SDIO_ERR_DATA_TIMEOUT)) {
				return TIMEOUT;
			}
			return COMM_ERR;
		}

		timeout--;
		udelay(1);
		if (timeout <= 0) {
			printf("%s: command timed out\n", DRIVER_NAME);
			return TIMEOUT;
		}
	}

	/* Handling response */
	if (cmd->resp_type & MMC_RSP_136) {
		uint response[8];
		for (resp_indx = 0; resp_indx < 8; resp_indx++)
			response[resp_indx] = kwsd_read(SDIO_RSP(resp_indx));

		cmd->response[0] =		((response[0] & 0x03ff) << 22) |
								((response[1] & 0xffff) << 6) |
								((response[2] & 0xfc00) >> 10);
		cmd->response[1] =		((response[2] & 0x03ff) << 22) |
								((response[3] & 0xffff) << 6) |
								((response[4] & 0xfc00) >> 10);
		cmd->response[2] =		((response[4] & 0x03ff) << 22) |
								((response[5] & 0xffff) << 6) |
								((response[6] & 0xfc00) >> 10);
		cmd->response[3] =		((response[6] & 0x03ff) << 22) |
								((response[7] & 0x3fff) << 8);
	} else if (cmd->resp_type & MMC_RSP_PRESENT) {
		uint response[3];
		for (resp_indx = 0; resp_indx < 3; resp_indx++)
			response[resp_indx] = kwsd_read(SDIO_RSP(resp_indx));

		cmd->response[0] = 		((response[2] & 0x003f) << (8 - 8))		|
								((response[1] & 0xffff) << (14 - 8))	|
								((response[0] & 0x03ff) << (30 - 8));
		cmd->response[1] = 		((response[0] & 0xfc00) >> 10);
		cmd->response[2] =		0;
		cmd->response[3] =		0;
	}

#ifdef CONFIG_MMC_DEBUG
	printf("%s: resp[0x%x] ", DRIVER_NAME, cmd->resp_type);
	printf("[0x%x] ", cmd->response[0]);
	printf("[0x%x] ", cmd->response[1]);
	printf("[0x%x] ", cmd->response[2]);
	printf("[0x%x] ", cmd->response[3]);
	printf("\n");
#endif

	return 0;
}

#if 0
/* Disable these three functions as they are not used anyway */

static void kwsd_power_up(void)
{
#ifdef DEBUG
	printf("%s: power up\n", DRIVER_NAME);
#endif
	/* disable interrupts */
	kwsd_write(SDIO_NOR_INTR_EN, 0);
	kwsd_write(SDIO_ERR_INTR_EN, 0);

	/* SW reset */
	kwsd_write(SDIO_SW_RESET, SDIO_SW_RESET_NOW);

	kwsd_write(SDIO_XFER_MODE, 0);

	/* enable status */
	kwsd_write(SDIO_NOR_STATUS_EN, SDIO_POLL_MASK);
	kwsd_write(SDIO_ERR_STATUS_EN, SDIO_POLL_MASK);

	/* enable interrupts status */
	kwsd_write(SDIO_NOR_INTR_STATUS, SDIO_POLL_MASK);
	kwsd_write(SDIO_ERR_INTR_STATUS, SDIO_POLL_MASK);
}

static void kwsd_power_down(void)
{
#ifdef DEBUG
	printf("%s: power down\n", DRIVER_NAME);
#endif
	/* disable interrupts */
	kwsd_write(SDIO_NOR_INTR_EN, 0);
	kwsd_write(SDIO_ERR_INTR_EN, 0);

	/* SW reset */
	kwsd_write(SDIO_SW_RESET, SDIO_SW_RESET_NOW);

	kwsd_write(SDIO_XFER_MODE, SDIO_XFER_MODE_STOP_CLK);

	/* disable status */
	kwsd_write(SDIO_NOR_STATUS_EN, 0);
	kwsd_write(SDIO_ERR_STATUS_EN, 0);

	/* enable interrupts status */
	kwsd_write(SDIO_NOR_INTR_STATUS, SDIO_POLL_MASK);
	kwsd_write(SDIO_ERR_INTR_STATUS, SDIO_POLL_MASK);
}

static u32 kw_mmc_get_base_clock(void)
{
	/* Original version did a check for board device id and revision id
	 * and assigned one of these clocks:
	 *   KW_MMC_BASE_FAST_CLK_100 (revid == 0 && devid != MV88F6282_DEV_ID)
	 *   KW_MMC_BASE_FAST_CLK_200 (revid != 0 || devid == MV88F6282_DEV_ID)
	 * However, this check was disabled and
	 *   KW_MMC_BASE_FAST_CLOCK
	 * was returned in every case.
	 * Therefore, all of the dead logic was removed. */
	return KW_MMC_BASE_FAST_CLOCK;
}
#endif /* #if 0 */

static inline u32 kw_mmc_get_base_clock(void)
{
	/* get MMC base clock. If any logic other than just returning
	 * a fixed value is ever used, remove inline modifier. */

	/* Possible values:
	 *  - KW_MMC_BASE_FAST_CLOCK   (166 MHz)
	 *  - KW_MMC_BASE_FAST_CLK_100 (100 MHz)
	 *  - KW_MMC_BASE_FAST_CLK_200 (200 MHz)
	 *
	 * Tests have shown that 200 MHz is more reliable than
	 * 166 MHz, so this value is used. */
	return KW_MMC_BASE_FAST_CLK_200;
}

static void kw_mmc_set_clk(unsigned int clock)
{
	unsigned int m;

	if (clock == 0) {
#ifdef DEBUG
		printf("%s: clock off\n", DRIVER_NAME);
#endif
		kwsd_write(SDIO_XFER_MODE, SDIO_XFER_MODE_STOP_CLK);
		kwsd_write(SDIO_CLK_DIV, KW_MMC_BASE_DIV_MAX);
	} else {
		m = kw_mmc_get_base_clock() / (2 * clock) - 1;
		if (m > KW_MMC_BASE_DIV_MAX)
			m = KW_MMC_BASE_DIV_MAX;
#ifdef DEBUG
		printf("%s: kw_mmc_set_clk: base = %d dividor = 0x%x clock=%d\n", DRIVER_NAME, kw_mmc_get_base_clock(), m, clock);
#endif
		kwsd_write(SDIO_CLK_DIV, m & KW_MMC_BASE_DIV_MAX);
	}
	udelay(10000);
}

static void kw_mmc_set_bus(unsigned int bus)
{
	u32 ctrl_reg = 0;

	ctrl_reg = kwsd_read(SDIO_HOST_CTRL);
	ctrl_reg &= ~SDIO_HOST_CTRL_DATA_WIDTH_4_BITS;

	switch (bus) {
		case 4:
			ctrl_reg |= SDIO_HOST_CTRL_DATA_WIDTH_4_BITS;
			break;
		case 1:
		default:
			ctrl_reg |= SDIO_HOST_CTRL_DATA_WIDTH_1_BIT;
	}
	/* default transfer mode */
	ctrl_reg |= SDIO_HOST_CTRL_BIG_ENDIAN;
	ctrl_reg &= ~SDIO_HOST_CTRL_LSB_FIRST;

	/* default to maximum timeout */
	ctrl_reg |= SDIO_HOST_CTRL_TMOUT(SDIO_HOST_CTRL_TMOUT_MAX);

	ctrl_reg |= SDIO_HOST_CTRL_PUSH_PULL_EN;

	ctrl_reg |= SDIO_HOST_CTRL_CARD_TYPE_MEM_ONLY;

	/*
	 * The HI_SPEED_EN bit is causing trouble with many (but not all)
	 * high speed SD, SDHC and SDIO cards.  Not enabling that bit
	 * makes all cards work.  So let's just ignore that bit for now
	 * and revisit this issue if problems for not enabling this bit
	 * are ever reported.
	 */
#if 0
	if (ios->timing == MMC_TIMING_MMC_HS ||
	    ios->timing == MMC_TIMING_SD_HS)
		ctrl_reg |= SDIO_HOST_CTRL_HI_SPEED_EN;
#endif

#ifdef DEBUG
	printf("%s: ctrl 0x%04x: %s %s %s\n", DRIVER_NAME, ctrl_reg,
		(ctrl_reg & SDIO_HOST_CTRL_PUSH_PULL_EN) ?
			"push-pull" : "open-drain",
		(ctrl_reg & SDIO_HOST_CTRL_DATA_WIDTH_4_BITS) ?
			"4bit-width" : "1bit-width",
		(ctrl_reg & SDIO_HOST_CTRL_HI_SPEED_EN) ?
			"high-speed" : "");
#endif

	kwsd_write(SDIO_HOST_CTRL, ctrl_reg);
	udelay(10000);
}

static void kw_mmc_set_ios(struct mmc *mmc)
{
#ifdef DEBUG
	printf("%s: bus[%d] clock[%d]\n", DRIVER_NAME, mmc->bus_width, mmc->clock);
#endif
	kw_mmc_set_bus(mmc->bus_width);
	kw_mmc_set_clk(mmc->clock);
}

static int kw_mmc_init(struct mmc *mmc)
{
#ifdef DEBUG
	printf("%s: kw_mmc_init\n", DRIVER_NAME);
#endif

	/*
	* Setting host parameters
	* Initial Host Ctrl : Timeout : max , Normal Speed mode, 4-bit data mode
	* Big Endian, SD memory Card, Push_pull CMD Line
	*/
	kwsd_write(SDIO_HOST_CTRL,
		SDIO_HOST_CTRL_TMOUT(SDIO_HOST_CTRL_TMOUT_MAX) |
		SDIO_HOST_CTRL_DATA_WIDTH_4_BITS |
		SDIO_HOST_CTRL_BIG_ENDIAN |
		SDIO_HOST_CTRL_PUSH_PULL_EN |
		SDIO_HOST_CTRL_CARD_TYPE_MEM_ONLY);

	kwsd_write(SDIO_CLK_CTRL, 0);

	/* enable status */
	kwsd_write(SDIO_NOR_STATUS_EN, SDIO_POLL_MASK);
	kwsd_write(SDIO_ERR_STATUS_EN, SDIO_POLL_MASK);

	/* disable interrupts */
	kwsd_write(SDIO_NOR_INTR_EN, 0);
	kwsd_write(SDIO_ERR_INTR_EN, 0);

	/* SW reset */
	kwsd_write(SDIO_SW_RESET, SDIO_SW_RESET_NOW);

	udelay(10000);
	return 0;
}

int kw_mmc_initialize(bd_t *bis)
{
	struct mmc *mmc = NULL;
        struct mmc_config *cfg = NULL;
        struct mmc_ops *ops = NULL;
	char *name = NULL;

#ifdef DEBUG
	printf("%s: %s base_clock = %d\n", DRIVER_NAME, kirkwood_id(), kw_mmc_get_base_clock());
#endif
	mmc = malloc(sizeof(struct mmc));
	if (!mmc)
		return -1;
        memset(mmc, 0, sizeof(*mmc));

        cfg = malloc(sizeof(*cfg));
        if (cfg == NULL)
                return -1;
        memset(cfg, 0, sizeof(*cfg));
        mmc->cfg = cfg;   /* provided configuration */

        ops = malloc(sizeof(*ops));
        if (ops == NULL)
                return -1;
        memset(ops, 0, sizeof(*ops));
        cfg->ops = ops;

        name = malloc(sizeof(DRIVER_NAME)+1);
        if (name == NULL)
                return -1;
        cfg->name = name;

	sprintf(cfg->name, DRIVER_NAME);

	ops->send_cmd	= kw_mmc_send_cmd;
	ops->set_ios	= kw_mmc_set_ios;
	ops->init	= kw_mmc_init;

	cfg->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	cfg->host_caps = MMC_MODE_4BIT | MMC_MODE_HS;

	cfg->f_min = kw_mmc_get_base_clock()/KW_MMC_BASE_DIV_MAX;
	cfg->f_max = KW_MMC_CLOCKRATE_MAX;
	cfg->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	mmc = mmc_create (cfg, NULL);

        if (mmc == NULL) {
		free(name);
		free(ops);
                free(cfg);
	        printf("\nFailed to Initialize MMC\n");
                return -1;
        }

	return 0;
}
