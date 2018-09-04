/*

@startuml

McHardwareInterfacesImplementation.AdcOneChannel ..|> McHardwareInterfaces.AdcOneChannel

namespace McHardwareInterfacesImplementation {

class AdcOneChannel {
	{field}-	const AdcOneChannelCfg*		const cfg
	{field}-	const uint32_t				cfgCount
	{field}-	ADC_HandleTypeDef			adc
	{field}-	ADC_ChannelConfTypeDef		channelCfg
	__Constructor__
	{method}+	AdcOneChannel	( const AdcOneChannelCfg*		const cfg,\n\t\t\t  uint32_t\t\t\t\t\tcountCfg )
	__Public methods__
	{method}+	void	irqHandler		( void )
	__Private methods__
	{method}-	void	clkEnable		( void )
	{method}-	void	clkDisable		( void )
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_ADC_MODULE_ENABLED

#include "mc_hardware_interfaces_adc_one_channel.h"

namespace McHardwareInterfacesImplementation {

struct AdcOneChannelCfg {
	ADC_TypeDef*		ADCx;
	uint32_t			clockPrescaler;								// ADC_ClockPrescaler
	uint32_t			resolution;									// ADC_Resolution
	uint32_t			dataAlign;									// ADC_Data_align
	uint32_t			channel;									// ADC_channels
	uint32_t			samplingTime;								// ADC_sampling_times
};

class AdcOneChannel : public McHardwareInterfaces::AdcOneChannel {
public:
	AdcOneChannel	(	const AdcOneChannelCfg*		const cfg,
						uint32_t					countCfg	);

	BaseResult		reinit								( uint32_t numberCfg = 0 );

	BaseResult		startContinuousConversion			( void );
	void			stopContinuousConversion			( void );
	uint32_t		getMeasurement						( void );

	void			irqHandler							( void );

private:
	void			clkEnable							( void );
	void			clkDisable							( void );

	const AdcOneChannelCfg*			const cfg;
	const uint32_t					countCfg;

	ADC_HandleTypeDef				adc;
	ADC_ChannelConfTypeDef			channelCfg;
};

}

#endif

#endif

