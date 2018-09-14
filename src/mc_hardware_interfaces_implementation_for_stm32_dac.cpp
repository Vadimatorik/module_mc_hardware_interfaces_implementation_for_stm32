#include "../inc/mc_hardware_interfaces_implementation_for_stm32_dac.h"

#ifdef HAL_DAC_MODULE_ENABLED

namespace McHardwareInterfacesImplementation {

Dac::Dac( const DacCfg* const cfg, uint32_t cfgCount ) :
	cfg( cfg ), cfgCount( cfgCount ) {
	this->dac.Instance							= DAC;
	this->dacChannel.DAC_Trigger				= DAC_TRIGGER_NONE;
}

McHardwareInterfaces::BaseResult Dac::reinit ( uint32_t numberCfg ) {
	if ( numberCfg >= this->cfgCount )	return McHardwareInterfaces::BaseResult::errInputValue;

	/// Заполнение HAL-структуры.
	this->dacChannel.DAC_OutputBuffer			= cfg[ numberCfg ].buffer;

	this->clkDisable();
	this->clkEnable();

	if ( HAL_DAC_DeInit( &this->dac ) != HAL_OK )
		return McHardwareInterfaces::BaseResult::errInit;

	if ( HAL_DAC_Init( &this->dac ) != HAL_OK )
		return McHardwareInterfaces::BaseResult::errInit;

	if ( HAL_DAC_ConfigChannel( &this->dac, &this->dacChannel, DAC_CHANNEL_1 ) != HAL_OK)
		return McHardwareInterfaces::BaseResult::errInit;

	if ( HAL_DAC_ConfigChannel( &this->dac, &this->dacChannel, DAC_CHANNEL_2 ) != HAL_OK)
		return McHardwareInterfaces::BaseResult::errInit;

	HAL_DAC_Start(  &this->dac, DAC_CHANNEL_1 );
	HAL_DAC_Start(  &this->dac, DAC_CHANNEL_2 );

	HAL_DAC_SetValue( &this->dac, DAC_CHANNEL_1,	DAC_ALIGN_12B_R, this->cfg[ numberCfg ].defaultValue );
	HAL_DAC_SetValue( &this->dac, DAC_CHANNEL_2,	DAC_ALIGN_12B_R, this->cfg[ numberCfg ].defaultValue );

	return McHardwareInterfaces::BaseResult::ok;
}

McHardwareInterfaces::BaseResult	Dac::setValue ( uint32_t channel, uint32_t value ) {
	if ( channel > 1 )			return McHardwareInterfaces::BaseResult::errInputValue;
	if ( value > 0xFFF )	return McHardwareInterfaces::BaseResult::errInputValue;

	if ( this->dac.State == HAL_DAC_STATE_RESET )	return McHardwareInterfaces::BaseResult::errInit;

	if ( channel == 0 ) {
		HAL_DAC_SetValue( &this->dac, DAC_CHANNEL_1,	DAC_ALIGN_12B_R, value );
	} else {
		HAL_DAC_SetValue( &this->dac, DAC_CHANNEL_2,	DAC_ALIGN_12B_R, value );
	}

	return McHardwareInterfaces::BaseResult::ok;
}

void Dac::clkEnable ( void ) {
	__HAL_RCC_DAC_CLK_ENABLE();
}

void Dac::clkDisable ( void ) {
	__HAL_RCC_DAC_CLK_DISABLE();
}

}

#endif
