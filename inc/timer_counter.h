/*

@startuml

TimCounter ..|> TimCounterBase

class TimCounter {
	{field}-	const ClkTimBaseCfg*		const cfg
	{field}-	TIM_HandleTypeDef		tim

	__Constructor__
	{method}+	TimCounter	( const ClkTimBaseCfg*	const cfg )
}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include "mc_hardware_interfaces_timer_counter.h"

struct ClkTimBaseCfg {
	const uint32_t					period;					// 0..0xFFFF или 0..0xFFFFFFFF
	const uint32_t					prescaler;				// 0..0xFFFF или 0..0xFFFFFFFF.

	/// Используется только в:
	/// 1. timCompOneChannelCfg
	const uint16_t					pulse;					// 0..0xFFFF или 0..0xFFFFFFFF.
};

struct timCounterCfg {
	// Используемый таймер.
	TIM_TypeDef*					tim;

	const ClkTimBaseCfg*			const cfg;
	uint32_t						countCfg;
};

class TimCounter : public TimCounterBase {
public:
	TimCounter	(	const timCounterCfg*	const cfg	);

	BaseResult		reinit					( uint32_t cfgNumber = 0 );

	BaseResult		on						( void );
	void			off						( void );

	uint32_t		getCounter				( void );

private:
	const timCounterCfg*		const cfg;

	TIM_HandleTypeDef				tim;
};

#endif

#endif
