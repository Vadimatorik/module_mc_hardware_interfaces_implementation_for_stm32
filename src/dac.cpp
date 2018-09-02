#include "dac.h"

#ifdef HAL_DAC_MODULE_ENABLED

Dac::Dac( const DacCfg* const cfg, const uint32_t countCfg ) :
	cfg( cfg ), countCfg( countCfg ) {
	this->dac.Instance							= DAC;
	this->dacCh.DAC_Trigger						= DAC_TRIGGER_NONE;
}

BaseResult Dac::reinit ( uint32_t numberCfg ) {
	if ( numberCfg >= this->countCfg )	return BaseResult::errInputValue;

	/// Заполнение HAL-структуры.
	this->dacCh.DAC_OutputBuffer				= cfg[ numberCfg ].buffer;

	this->clkDisable();
	this->clkEnable();

	if ( HAL_DAC_DeInit( &this->dac ) != HAL_OK )
		return BaseResult::errInit;

	if ( HAL_DAC_Init( &this->dac ) != HAL_OK )
		return BaseResult::errInit;

	if ( HAL_DAC_ConfigChannel( &this->dac, &this->dacCh, DAC_CHANNEL_1 ) != HAL_OK)
		return BaseResult::errInit;

	if ( HAL_DAC_ConfigChannel( &this->dac, &this->dacCh, DAC_CHANNEL_2 ) != HAL_OK)
		return BaseResult::errInit;

	HAL_DAC_Start(  &this->dac, DAC_CHANNEL_1 );
	HAL_DAC_Start(  &this->dac, DAC_CHANNEL_2 );

	HAL_DAC_SetValue( &this->dac, DAC_CHANNEL_1,	DAC_ALIGN_12B_R, this->cfg[ numberCfg ].defaultValue );
	HAL_DAC_SetValue( &this->dac, DAC_CHANNEL_2,	DAC_ALIGN_12B_R, this->cfg[ numberCfg ].defaultValue );

	return BaseResult::ok;
}

BaseResult	Dac::setValue ( const uint32_t ch, const uint32_t value ) {
	if ( ch > 1 )			return BaseResult::errInputValue;
	if ( value > 0xFFF )	return BaseResult::errInputValue;

	if ( this->dac.State == HAL_DAC_STATE_RESET )	return BaseResult::errInit;

	if ( ch == 0 ) {
		HAL_DAC_SetValue( &this->dac, DAC_CHANNEL_1,	DAC_ALIGN_12B_R, value );
	} else {
		HAL_DAC_SetValue( &this->dac, DAC_CHANNEL_2,	DAC_ALIGN_12B_R, value );
	}

	return BaseResult::ok;
}

void Dac::clkEnable ( void ) {
	__HAL_RCC_DAC_CLK_ENABLE();
}

void Dac::clkDisable ( void ) {
	__HAL_RCC_DAC_CLK_DISABLE();
}

#endif
