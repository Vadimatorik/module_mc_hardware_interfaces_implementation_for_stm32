#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_DMA_MODULE_ENABLED

namespace McHardwareInterfacesImplementation {

void dmaClkOn ( DMA_Stream_TypeDef* dma);
void dmaIrqOn ( DMA_Stream_TypeDef* dma, uint32_t prio );

}

#endif

#endif
