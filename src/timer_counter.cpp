#include "timer_counter.h"

#ifdef HAL_TIM_MODULE_ENABLED

namespace McHardwareInterfacesImplementation {

extern void clkTimInit ( TIM_TypeDef* tim );

TimCounter::TimCounter ( const TimCounterCfg* const cfg ) : cfg( cfg ) {
	this->tim.Instance						= this->cfg->tim;

	this->tim.Init.ClockDivision			= TIM_CLOCKDIVISION_DIV1;
	this->tim.Init.CounterMode				= TIM_COUNTERMODE_UP;
}

BaseResult TimCounter::reinit ( uint32_t numberCfg ) {
	if ( numberCfg >= this->cfg->countCfg )
		return BaseResult::errInputValue;

	this->tim.Init.Period					= this->cfg->cfg[ numberCfg ].period;
	this->tim.Init.Prescaler				= this->cfg->cfg[ numberCfg ].prescaler;

	clkTimInit( this->tim.Instance );

	if ( HAL_TIM_Base_DeInit( &this->tim ) != HAL_OK )
		return BaseResult::errInit;

	if ( HAL_TIM_Base_Init( &this->tim ) != HAL_OK )
		return BaseResult::errInit;

	return BaseResult::ok;
}

BaseResult TimCounter::on ( void ) {
	if ( this->tim.State == HAL_TIM_STATE_RESET )
		return BaseResult::errInit;

	HAL_TIM_Base_Start( &this->tim );

	return BaseResult::ok;
}

void TimCounter::off ( void ) {
	HAL_TIM_Base_Stop( &this->tim  );
}

uint32_t TimCounter::getCounter ( void ) {
	return this->tim.Instance->CNT;
}

}

#endif
