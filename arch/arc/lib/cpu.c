/*
 * Copyright (C) 2013-2014 Synopsys, Inc. All rights reserved.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <asm/arcregs.h>
#include <asm/cache.h>

DECLARE_GLOBAL_DATA_PTR;

//int clk_init(ulong rate)
//{
//	struct udevice *dev;
//	struct clk *cpuclk;
//	int ret = 0;
//
//	//Get clock driver device
//	ret = uclass_get_device(UCLASS_CLK, 0, &dev);
//	if(ret < 0)
//	{
//		return -1;
//	}
//
//	ret = clk_request(dev, cpuclk);
//	if(ret < 0)
//	{
//		return -1;
//	}
//
//	if((rate = clk_set_rate(cpuclk)) < 0)
//	{
//		return -1;
//	}
//
//	clk_free(cpuclk);
//
//	return rate;
//}

int arch_cpu_init(void)
{
	timer_init();

	//gd->cpu_clk = clk_init(CONFIG_SYS_CLK_FREQ);
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;

	cache_init();

	return 0;
}

int arch_early_init_r(void)
{
	gd->bd->bi_memstart = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_memsize = CONFIG_SYS_SDRAM_SIZE;
	return 0;
}
