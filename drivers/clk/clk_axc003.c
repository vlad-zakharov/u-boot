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
//#include <mach/axc003.h>
#include <dt-bindings/clock/microchip,clock.h>

DECLARE_GLOBAL_DATA_PTR;

///* Primary oscillator */
//#define SYS_POSC_CLK_HZ	24000000
//
///* FRC clk rate */
//#define SYS_FRC_CLK_HZ	8000000
//
///* Clock Registers */
//#define OSCCON		0x0000
//#define OSCTUNE		0x0010
//#define SPLLCON		0x0020
//#define REFO1CON	0x0080
//#define REFO1TRIM	0x0090
//#define PB1DIV		0x0140
//
///* SPLL */
//#define ICLK_MASK	0x00000080
//#define PLLIDIV_MASK	0x00000007
//#define PLLODIV_MASK	0x00000007
//#define CUROSC_MASK	0x00000007
//#define PLLMUL_MASK	0x0000007F
//#define FRCDIV_MASK	0x00000007
//
///* PBCLK */
//#define PBDIV_MASK	0x00000007
//
///* SYSCLK MUX */
//#define SCLK_SRC_FRC1	0
//#define SCLK_SRC_SPLL	1
//#define SCLK_SRC_POSC	2
//#define SCLK_SRC_FRC2	7
//
///* Reference Oscillator Control Reg fields */
//#define REFO_SEL_MASK	0x0f
//#define REFO_SEL_SHIFT	0
//#define REFO_ACTIVE	BIT(8)
//#define REFO_DIVSW_EN	BIT(9)
//#define REFO_OE		BIT(12)
//#define REFO_ON		BIT(15)
//#define REFO_DIV_SHIFT	16
//#define REFO_DIV_MASK	0x7fff
//
///* Reference Oscillator Trim Register Fields */
//#define REFO_TRIM_REG	0x10
//#define REFO_TRIM_MASK	0x1ff
//#define REFO_TRIM_SHIFT	23
//#define REFO_TRIM_MAX	511
//
//#define ROCLK_SRC_SCLK		0x0
//#define ROCLK_SRC_SPLL		0x7
//#define ROCLK_SRC_ROCLKI	0x8
//
///* Memory PLL */
//#define MPLL_IDIV		0x3f
//#define MPLL_MULT		0xff
//#define MPLL_ODIV1		0x7
//#define MPLL_ODIV2		0x7
//#define MPLL_VREG_RDY		BIT(23)
//#define MPLL_RDY		BIT(31)
//#define MPLL_IDIV_SHIFT		0
//#define MPLL_MULT_SHIFT		8
//#define MPLL_ODIV1_SHIFT	24
//#define MPLL_ODIV2_SHIFT	27
//#define MPLL_IDIV_INIT		0x03
//#define MPLL_MULT_INIT		0x32
//#define MPLL_ODIV1_INIT		0x02
//#define MPLL_ODIV2_INIT		0x01

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
//    unsigned int rate = 33333333;
//    pll_reg idiv, fbdiv, odiv;
//    idiv.val = readl((void __iomem *)AXC003_IDIV); 
//    fbdiv.val = readl((void __iomem *)AXC003_FBDIV);
//    odiv.val = readl((void __iomem *)AXC003_ODIV); 
//    if (idiv.bypass != 1)                     
//            rate = rate / (idiv.low + idiv.high);       
//                                                  
//    if (fbdiv.bypass != 1)                    
//            rate = rate * (fbdiv.low + fbdiv.high);     
//                                                      
//    if (odiv.bypass != 1)                     
//            rate = rate / (odiv.low + odiv.high);       
//                                                          
//    rate = (rate + 500000) / 1000000; /* Rounding */
//
//	return rate;
    return 5;
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
    printf("Hello CLK DRIVER YEAH YEAH YEAH NIGGA WHAT'S UP\n")l
	return 0;
}

static const struct udevice_id axc003_clk_ids[] = {
	{ .compatible = "synopsys,axc003-clk"},
	{}
};

U_BOOT_DRIVER(axc003_clk) = {
	.name		= "axc003_clk",
	.id		= UCLASS_CLK,
	.of_match	= axc003_clk_ids,
	.flags		= DM_FLAG_PRE_RELOC,
	.ops		= &axc003_axc003_clk_ops,
	.probe		= axc003_clk_probe,
	.priv_auto_alloc_size = sizeof(struct axc003_clk_priv),
};
