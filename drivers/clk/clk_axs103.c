/*
 * Copyright (C) 2016
 *
 *
 */

#include <common.h>
#include <linux/compiler.h>
#include <clk-uclass.h>
#include <dm.h>
#include <div64.h>
#include <wait_bit.h>
#include <dm/lists.h>
#include <asm/io.h>
DECLARE_GLOBAL_DATA_PTR;


#define AXC003_XTAL_RATE				  33333333 /* 33 MHz quartz rate*/
// Offset of CGU PLL registers from CGU base address
#define AXC003_CPU_PLL 					  0x80
#define AXC003_IDIV    	AXC003_CPU_PLL	+ 0x00
#define AXC003_FBDIV   	AXC003_CPU_PLL	+ 0x04 
#define AXC003_ODIV    	AXC003_CPU_PLL	+ 0x08 
#define AXC003_CGU_LOCK 				  0x110

// Bit fields of the PLL IDIV/FBDIV/ODIV registers:
//  ________________________________________________________________________
// |31                15|    14    |   13   |  12  |11         6|5         0|
// |-------RESRVED------|-NOUPDATE-|-BYPASS-|-EDGE-|--HIGHTIME--|--LOWTIME--|
// |____________________|__________|________|______|____________|___________|
//
// Following macros detirmine the way of access to these registers
// They should be set up only using the macros
//
#define PLL_REG_GET_LOW(reg)			(((reg) & (0x3F << 0)) >> 0)
#define PLL_REG_GET_HIGH(reg)			(((reg) & (0x3F << 6)) >> 6)
#define PLL_REG_GET_EDGE(reg)			(reg & (0x01 << 12) ? 1 : 0)
#define PLL_REG_GET_BYPASS(reg)			(reg & (0x01 << 13) ? 1 : 0)
#define PLL_REG_GET_NOUPD(reg)			(reg & (0x01 << 14) ? 1 : 0)
#define PLL_REG_GET_PAD(reg)			(((reg) & (0x1FFFF << 15)) >> 15)

#define PLL_REG_SET_LOW(reg, value) 	reg |= (((value) & 0x3F) << 0)
#define PLL_REG_SET_HIGH(reg, value)	reg |= (((value) & 0x3F) << 6)
#define PLL_REG_SET_EDGE(reg, value)	reg |= (((value) & 0x01) << 12)
#define PLL_REG_SET_BYPASS(reg, value)	reg |= (((value) & 0x01) << 13)
#define PLL_REG_SET_NOUPD(reg, value)	reg |= (((value) & 0x01) << 14)
#define PLL_REG_SET_PAD(reg, value)		reg |= (((value) & 0x1FFFF) << 15)

// Driver private data. Contains CGU base address pointer.
struct axc003_clk_priv {
	void __iomem *cgu_base;
};

// This function accepts div value and return corresponding PLL register value
static uint32_t encode_div(uint32_t id, int32_t upd)
{
	uint32_t div = 0;

	// Use special macros to set up bit fields of PLL register
	PLL_REG_SET_LOW(div, (id%2 == 0) ? id >> 1 : (id >> 1) + 1); 
	PLL_REG_SET_HIGH(div, id >> 1); 
	PLL_REG_SET_EDGE(div, id%2); 
	PLL_REG_SET_BYPASS(div, id==1 ? 1 : 0);
	PLL_REG_SET_NOUPD(div, !upd);
	
	return div;
}

// Write CGU (Clock generation unit) register 
static void write_cgu_reg(uint32_t value, void __iomem *reg, void __iomem *lock_reg)
{
	uint32_t loops = 128 * 1024;
  	uint32_t volatile ctr;

	// Write value to CGU register
	writel(value, reg);

	// Wait value to be set up	
	ctr = loops;
	while (((readl(lock_reg) & 1) == 1) && ctr--); /* wait for unlock */

	ctr = loops;
	while (((readl(lock_reg) & 1) == 0) && ctr--); /* wait for re-lock */
}


static ulong axc003_get_rate(struct clk *clk)
{
	uint32_t rate = AXC003_XTAL_RATE;
	uint32_t id, fb, od;
	struct axc003_clk_priv *priv = dev_get_priv(clk->dev);

	// Read PLL registers
	id = readl(priv->cgu_base + AXC003_IDIV); 
	fb = readl(priv->cgu_base + AXC003_FBDIV);
	od = readl(priv->cgu_base + AXC003_ODIV); 

	// Calculate frequency according to PLL registers values
	if (PLL_REG_GET_BYPASS(id) != 1)                     
		rate = rate / (PLL_REG_GET_LOW(id) + PLL_REG_GET_HIGH(id));       

	if (PLL_REG_GET_BYPASS(fb) != 1)                    
		rate = rate * (PLL_REG_GET_LOW(fb) + PLL_REG_GET_HIGH(fb));     

	if (PLL_REG_GET_BYPASS(od) != 1)                     
		rate = rate / (PLL_REG_GET_LOW(od) + PLL_REG_GET_HIGH(od));

	return rate;
}

static ulong axc003_set_rate(struct clk *clk, ulong rate)
{
	uint32_t id, fd, od;
	// Get device private data that stores CGU base address
	struct axc003_clk_priv *priv = dev_get_priv(clk->dev);
	
	// ARC AXC003 supports only fixed list of frequencies
	switch(rate)
	{
		case 33333333:  /* 33 MHz */
			id = 1; fd = 1; od = 1;
			break;
		case 50000000:  /* 50 MHz */
			id = 1; fd = 30; od = 20;
			break;
		case 75000000:  /* 75 MHz */
			id = 2; fd = 45; od = 10;
			break;
		case 90000000:  /* 90 MHz */
			id = 2; fd = 54; od = 10;
			break;
		case 100000000: /* 100 MHz */
			id = 1; fd = 30; od = 10;
			break;
		case 125000000: /* 125 MHz */
			id = 2; fd = 45; od = 6;
			break;
		default:  /* Use 100 MHz as default */
			rate = 100000000; /* Cast off "rate" variable to default value */
			id = 1; fd = 30; od = 10;
	}
	// Set up corresponding PLL registers
	write_cgu_reg(encode_div(id, 0),
			priv->cgu_base + AXC003_IDIV,
			priv->cgu_base + AXC003_CGU_LOCK);

	write_cgu_reg(encode_div(fd, 0),
			priv->cgu_base + AXC003_FBDIV,
			priv->cgu_base + AXC003_CGU_LOCK);

	write_cgu_reg(encode_div(od, 1),
			priv->cgu_base + AXC003_ODIV,
			priv->cgu_base + AXC003_CGU_LOCK);

	// Return the real rat that was set up 
	return rate;
}

static int axc003_clk_probe(struct udevice *dev)
{
	fdt_addr_t addr;
	fdt_size_t size;

	// Get device priver data pointer
	struct axc003_clk_priv *priv = dev_get_priv(dev);

	// Get registers offset and size 
	addr = fdtdec_get_addr_size(gd->fdt_blob, dev->of_offset, "reg", &size);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	// Set up driver private data
	priv->cgu_base = (void __iomem*)addr;
	
	return 0;

}

static struct clk_ops axc003_axc003_clk_ops = {
	.set_rate = axc003_set_rate,
	.get_rate = axc003_get_rate,
};

static const struct udevice_id axc003_clk_ids[] = {
	{ .compatible = "snps,axc003-clk"},
	{}
};

U_BOOT_DRIVER(axc003_clk) = {
	.name = "axc003_clk",
	.id = UCLASS_CLK,
	.of_match = axc003_clk_ids,
	.probe = axc003_clk_probe,
	.priv_auto_alloc_size = sizeof(struct axc003_clk_priv),
	.ops =  &axc003_axc003_clk_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
