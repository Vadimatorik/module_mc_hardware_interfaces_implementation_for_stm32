#include "mc_hardware_interfaces_implementation_for_stm32_rcc.h"

#ifdef HAL_RCC_MODULE_ENABLED

namespace McHardwareInterfacesImplementation {

McHardwareInterfaces::RccResult Rcc::setCfg ( const uint32_t numberCfg ) {
	if ( numberCfg >= this->cfgCount )
		return McHardwareInterfaces::RccResult::errCfgNumber;

	HAL_RCC_DeInit();

	if ( HAL_RCC_OscConfig( ( RCC_OscInitTypeDef* )&this->cfg[ numberCfg ].osc ) != HAL_OK ) {
		this->cfgNumberSet = -1;
		return McHardwareInterfaces::RccResult::errOscInit;
	}

	if ( HAL_RCC_ClockConfig( ( RCC_ClkInitTypeDef* )&this->cfg[ numberCfg ].clk,
								this->cfg[ numberCfg ].fLatency ) != HAL_OK ) {
		this->cfgNumberSet = -1;
		return  McHardwareInterfaces::RccResult::errClkInit;
	}

	this->cfgNumberSet = static_cast< int >( numberCfg );
	return McHardwareInterfaces::RccResult::ok;
}

McHardwareInterfaces::RccResult Rcc::getCfgNumber ( uint32_t& cfgNumber ) {
	if ( this->cfgNumberSet == -1 )
		return McHardwareInterfaces::RccResult::errNoInit;

	cfgNumber = static_cast< int >( this->cfgNumberSet );

	return McHardwareInterfaces::RccResult::ok;
}

}

#endif
