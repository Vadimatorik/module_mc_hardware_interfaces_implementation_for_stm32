#include "pin_multifunc.h"

#ifdef HAL_GPIO_MODULE_ENABLED

bool PinMultifunc::reinit ( uint32_t numberCfg ) {
	if ( numberCfg >= this->countCfg ) return false;
	HAL_GPIO_DeInit( ( GPIO_TypeDef* )this->cfg->GPIOx, this->cfg->init.Pin );
	this->init();
	return true;
}

#endif
