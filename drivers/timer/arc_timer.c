/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2004, Psyent Corporation <www.psyent.com>
 * Scott McNutt <smcnutt@psyent.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <timer.h>
#include <asm/io.h>
#include <asm/arcregs.h>

DECLARE_GLOBAL_DATA_PTR;

#define NH_MODE (1 << 1)

static int arc_timer_get_count(struct udevice *dev, u64 *count)
{
	*count = read_aux_reg(ARC_AUX_TIMER0_CNT);
	return 0;
}

static int arc_timer_probe(struct udevice *dev)
{
	write_aux_reg(ARC_AUX_TIMER0_CTRL, NH_MODE);
	/* Set max value for counter/timer */
	write_aux_reg(ARC_AUX_TIMER0_LIMIT, 0xffffffff);
	/* Set initial count value and restart counter/timer */
	write_aux_reg(ARC_AUX_TIMER0_CNT, 0);
	return 0;
}

static const struct timer_ops arc_timer_ops = {
	.get_count = arc_timer_get_count,
};

static const struct udevice_id arc_timer_ids[] = {
	{ .compatible = "snps,arc-timer0" },
	{}
};

U_BOOT_DRIVER(arc_timer) = {
	.name	= "arc_timer",
	.id	= UCLASS_TIMER,
	.of_match = arc_timer_ids,
	.probe = arc_timer_probe,
	.ops	= &arc_timer_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
