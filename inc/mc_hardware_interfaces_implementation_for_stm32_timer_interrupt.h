/*

@startuml

namespace McHardwareInterfacesImplementation {

McHardwareInterfacesImplementation.TimInterrupt ..|> McHardwareInterfaces.TimInterrupt

class TimInterrupt {
	{field}-	const TimInterruptCfg*			const cfg
	{field}-	TIM_HandleTypeDef				tim
	__Constructor__
	{method}+	TimInterrupt	( const TimInterruptCfg*	const cfg )
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "mc_hardware_interfaces_implementation_for_stm32_platform.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include "mc_hardware_interfaces_implementation_for_stm32_timer_counter.h"
#include "mc_hardware_interfaces_timer_interrupt.h"

namespace McHardwareInterfacesImplementation {

struct TimInterruptCfg {
	TIM_TypeDef*					tim;
	const ClkTimBaseCfg*			const cfg;
	uint32_t						countCfg;
};

class TimInterrupt : public McHardwareInterfaces::TimInterrupt {
public:
	TimInterrupt	(	const TimInterruptCfg*	const cfg	);

	McHardwareInterfaces::BaseResult		reinit							( uint32_t cfgNumber = 0 );

	McHardwareInterfaces::BaseResult		setState						(	bool	state	);

	McHardwareInterfaces::BaseResult		on								( void );
	void									off								( void );

	void									clearInterruptFlag				( void );

private:
	const TimInterruptCfg*			const cfg;

	TIM_HandleTypeDef				tim;
};

}

#endif

#endif
