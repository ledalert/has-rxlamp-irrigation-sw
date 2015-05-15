/*! \file dma_macro.h
    \brief Helper macros for DMA HAL
 
*/
#ifndef _DMA_MACRO_H_
#include "dma.h"


/*! Create DMA instance */
#define DMA_INSTANCE(Name, Dma, Rcc) \
	struct dma_config Name = {\
		.dma = Dma,\
		.rcc = Rcc,\
	};


/*! Create DMA channel instance */
#define DMA_CHANNEL_INSTANCE(Name, Dma, Channel, Perhipheral, Enable_Reg, Enable_Flag) \
	struct dma_channel_config Name = {\
		.dma = Dma,\
		.channel = Channel,\
		.perhipheral = Perhipheral,\
		.enable_reg = Enable_Reg,\
		.enable_flag = Enable_Flag,\
	};


/*! Create unconfigured DMA channel */
#define DMA_CHANNEL_NOCONF(Name, Dma, Channel) DMA_CHANNEL_INSTANCE(Name, Dma, Channel, 0, 0, 0)

#define _DMA_MACRO_H_
#endif