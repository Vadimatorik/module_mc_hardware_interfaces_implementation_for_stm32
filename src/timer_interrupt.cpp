#include "timer_interrupt.h"

#ifdef HAL_TIM_MODULE_ENABLED

extern void clkTimInit ( TIM_TypeDef* tim );

TimInterrupt::TimInterrupt( const TimInterruptCfg* const cfg ) : cfg( cfg ) {
	this->tim.Instance						= this->cfg->tim;

	this->tim.Init.ClockDivision			= TIM_CLOCKDIVISION_DIV1;
	this->tim.Init.CounterMode				= TIM_COUNTERMODE_UP;
}

BaseResult TimInterrupt::reinit ( uint32_t numberCfg ) {
	if ( numberCfg >= this->cfg->countCfg )
		return BaseResult::errInputValue;

	this->tim.Init.Period					= this->cfg->cfg[ numberCfg ].period;
	this->tim.Init.Prescaler				= this->cfg->cfg[ numberCfg ].prescaler;

	clkTimInit( this->cfg->tim );

	if ( HAL_TIM_Base_DeInit( &this->tim ) != HAL_OK )
		return BaseResult::errInit;

	if ( HAL_TIM_Base_Init( &this->tim ) != HAL_OK )
		return BaseResult::errInit;

	return BaseResult::ok;
}

BaseResult TimInterrupt::setState ( bool state ) {
	if ( this->tim.State == HAL_TIM_STATE_RESET )
		return BaseResult::errInit;

	if ( state ) {
		HAL_TIM_Base_Start_IT( &this->tim );
	} else {
		HAL_TIM_Base_Stop_IT( &this->tim );
	}

	return BaseResult::ok;
}

BaseResult TimInterrupt::on ( void ) {
	if ( this->tim.State == HAL_TIM_STATE_RESET )
		return BaseResult::errInit;

	HAL_TIM_Base_Start_IT( &this->tim );

	return BaseResult::ok;
}

void TimInterrupt::off ( void ) {
	HAL_TIM_Base_Stop_IT( &this->tim );
}

void TimInterrupt::clearInterruptFlag ( void ) {
	HAL_TIM_IRQHandler( &this->tim );
}

#endif
