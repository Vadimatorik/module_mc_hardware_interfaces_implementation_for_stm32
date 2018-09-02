/*

@startuml

class TimCompOneChannel {
	{field}-	const TimCompOneChannelCfg*	const cfg
	{field}-	TIM_HandleTypeDef			tim
	{field}-	TIM_OC_InitTypeDef			timCh
	__Constructor__
	{method}+	TimCompOneChannel	( const TimCompOneChannelCfg*	const cfg )
}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include "timer_counter.h"
#include "mc_hardware_interfaces_timer_comp_one_channel.h"

struct TimCompOneChannelCfg {
	// Используемый таймер.
	TIM_TypeDef*					tim;

	const ClkTimBaseCfg*			const cfg;
	uint32_t						countCfg;

	// Параметры выходного канала.
	uint8_t							outChannel;		// TIM_CHANNEL_x.
	uint32_t						polarity;		// TIM_OCPOLARITY_LOW  / TIM_OCPOLARITY_HIGH.

};

class TimCompOneChannel : public TimCompOneChannelBase {
public:
	TimCompOneChannel	(	const TimCompOneChannelCfg*	const cfg	);

	BaseResult		reinit					( uint32_t cfgNumber = 0 );

	BaseResult		on						( void );
	void			off						( void );

private:
	const TimCompOneChannelCfg*		const cfg;

	TIM_HandleTypeDef				tim;
	TIM_OC_InitTypeDef				timCh;
};

#endif

#endif
