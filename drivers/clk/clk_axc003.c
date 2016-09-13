/*
 * Copyright (C) 2015 Purna Chandra Mandal <purna.mandal@microchip.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#include <common.h>
#include <clk-uclass.h>
#include <dm.h>
#include <div64.h>
#include <wait_bit.h>
#include <dm/lists.h>
#include <asm/io.h>
DECLARE_GLOBAL_DATA_PTR;

#define AXC003_CGU                        0xF0000000
#define AXC003_CPU_PLL AXC003_CGU       + 0x80
#define AXC003_IDIV    AXC003_CPU_PLL   + 0x00
#define AXC003_FBDIV   AXC003           + 0x04 
#define AXC003_IDIV    AXC003           + 0x08 

union pll_reg {                                                     
    struct {                                                        
        unsigned int low:6, high:6, edge:1, bypass:1, noupd:1, pad:17
    };                                                              
    unsigned int val;                                               
};                                                                  


struct axc003_clk_priv {
	void __iomem *iobase;
	void __iomem *syscfg_base;
};


static void axc003_clk_init(struct udevice *dev)
{
    return 0;
}

static ulong axc003_get_rate(struct clk *clk)
{
	//unsigned int rate = 33333333;
	//pll_reg idiv, fbdiv, odiv;
	//idiv.val = readl((void __iomem *)AXC003_IDIV); 
	//fbdiv.val = readl((void __iomem *)AXC003_FBDIV);
	//odiv.val = readl((void __iomem *)AXC003_ODIV); 
	//if (idiv.bypass != 1)                     
	//	rate = rate / (idiv.low + idiv.high);       

	//if (fbdiv.bypass != 1)                    
	//	rate = rate * (fbdiv.low + fbdiv.high);     

	//if (odiv.bypass != 1)                     
	//	rate = rate / (odiv.low + odiv.high);       

	//rate = (rate + 500000) / 1000000; /* Rounding */

	//return rate;
	return 10;
}

static ulong axc003_set_rate(struct clk *clk, ulong rate)
{
	return 0;
}

static struct clk_ops axc003_axc003_clk_ops = {
	.set_rate = axc003_set_rate,
	.get_rate = axc003_get_rate,
};

static int axc003_clk_probe(struct udevice *dev)
{
    //static int counter = 1;
    //printf("Probing CLK_DRIVER was callen %d times with name %s and address %p \n", counter++, dev->name, dev);
	return 0;
}

static int axc003_clk_remove(struct udevice *dev)
{
    return 0;
}

static const struct udevice_id axc003_clk_ids[] = {
	{ .compatible = "synopsys,axc003-clk"},
	{}
};

U_BOOT_DRIVER(axc003_clk) = {
	.name = "axc003_clk",
	.id = UCLASS_CLK,
	.of_match = axc003_clk_ids,
	.ops =  &axc003_axc003_clk_ops,
	.probe = axc003_clk_probe,
	.priv_auto_alloc_size = sizeof(struct axc003_clk_priv),
	//.flags = DM_FLAG_PRE_RELOC,
};
