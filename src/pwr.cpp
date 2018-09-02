#include "pwr.h"

#ifdef HAL_PWR_MODULE_ENABLED

BaseResult Pwr::reinit ( uint32_t numberCfg ) {
	if ( numberCfg >= this->cfgCount )
		return BaseResult::errInputValue;

	__HAL_RCC_PWR_CLK_ENABLE();
	this->cfgNow	=	numberCfg;

	HAL_PWR_ConfigPVD( ( PWR_PVDTypeDef* )&this->cfg[ this->cfgNow ].cfg );

	return BaseResult::ok;
}

void Pwr::pvdEnable ( void ) {
	HAL_PWR_EnablePVD();
}

void Pwr::pvdDisable ( void ) {
	HAL_PWR_DisablePVD();
}

void Pwr::irqHandler ( void ) {
	HAL_PWR_PVD_IRQHandler();
}

#endif
