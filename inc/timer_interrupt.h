/*

@startuml

class TimInterrupt {
	{field}-	const TimInterruptCfg*			const cfg
	{field}-	TIM_HandleTypeDef				tim
	__Constructor__
	{method}+	TimInterrupt	( const TimInterruptCfg*	const cfg )
}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include "timer_counter.h"
#include "mc_hardware_interfaces_timer_interrupt.h"

struct TimInterruptCfg {
	TIM_TypeDef*					tim;
	const ClkTimBaseCfg*			const cfg;
	uint32_t						countCfg;
};

class TimInterrupt : public TimInterruptBase {
public:
	TimInterrupt	(	const TimInterruptCfg*	const cfg	);

	BaseResult		reinit							( uint32_t cfgNumber = 0 );

	BaseResult		setState						(	bool	state	);

	BaseResult		on								( void );
	void			off								( void );

	void			clearInterruptFlag				( void );

private:
	const TimInterruptCfg*			const cfg;

	TIM_HandleTypeDef				tim;
};

#endif

#endif
