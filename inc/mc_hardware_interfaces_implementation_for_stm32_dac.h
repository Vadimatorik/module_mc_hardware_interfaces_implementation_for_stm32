/*

@startuml

McHardwareInterfacesImplementation.Dac ..|> McHardwareInterfaces.Dac

namespace McHardwareInterfacesImplementation {

class Dac {
	{field}-	const DacCfg*				const cfg
	{field}-	const uint32_t				cfgCount
	{field}-	DAC_HandleTypeDef			dac
	{field}-	DAC_ChannelConfTypeDef		dacChannel
	__Constructor__
	{method}+	Dac	( const DacCfg*		const cfg,\n\t  uint32_t			cfgCount )
	__Private methods__
	{method}-	void	clkEnable		( void );
	{method}-	void	clkDisable		( void );
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "mc_hardware_interfaces_implementation_for_stm32_platform.h"

#ifdef HAL_DAC_MODULE_ENABLED

#include "mc_hardware_interfaces_dac.h"

namespace McHardwareInterfacesImplementation {

struct DacCfg {
	uint32_t			buffer;				// DAC_OUTPUTBUFFER_ENABLE/DAC_OUTPUTBUFFER_DISABLE.
	uint32_t			defaultValue;		// Значение, выставляемое на выводы каналов Dac после инициализации.
};

class Dac : public McHardwareInterfaces::Dac {
public:
	Dac	(	const DacCfg*		const cfg,
			uint32_t			cfgCount	);

	McHardwareInterfaces::BaseResult	reinit			( uint32_t cfgNumber = 0 );
	McHardwareInterfaces::BaseResult	setValue		( uint32_t channel, uint32_t value );

private:
	void								clkEnable		( void );
	void								clkDisable		( void );

	const DacCfg*					const cfg;
	const uint32_t					cfgCount;

	DAC_HandleTypeDef				dac;
	DAC_ChannelConfTypeDef			dacChannel;
};

}

#endif

#endif