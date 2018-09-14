
#ifdef __cplusplus

#include "mc_hardware_interfaces_implementation_for_stm32_platform.h"

#ifdef HAL_DMA_MODULE_ENABLED

namespace McHardwareInterfacesImplementation {

#if defined( STM32F2 ) && defined( STM32F4 )
void dmaClkOn ( DMA_Stream_TypeDef* dma);
void dmaIrqOn ( DMA_Stream_TypeDef* dma, uint32_t prio );
#endif

#if defined( STM32F1 )
void dmaClkOn ( DMA_Channel_TypeDef* dma);
void dmaIrqOn ( DMA_Channel_TypeDef* dma, uint32_t prio );
#endif

}

#endif

#endif
