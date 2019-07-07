/*

@startuml

namespace McHardwareInterfacesImplementation {

McHardwareInterfacesImplementation.TimCompOneChannel ..|> McHardwareInterfaces.TimCompOneChannel

class TimCompOneChannel {
	{field}-	const TimCompOneChannelCfg*	const cfg
	{field}-	TIM_HandleTypeDef			tim
	{field}-	TIM_OC_InitTypeDef			timCh
	__Constructor__
	{method}+	TimCompOneChannel	( const TimCompOneChannelCfg*	const cfg )
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "mc_hardware_interfaces_implementation_for_stm32_platform.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include "mc_hardware_interfaces_implementation_for_stm32_timer_counter.h"
#include "mc_hardware_interfaces_timer_comp_one_channel.h"

namespace McHardwareInterfacesImplementation {

struct TimCompOneChannelCfg {
    // Используемый таймер.
    TIM_TypeDef *tim;
    
    const ClkTimBaseCfg *const cfg;
    uint32_t countCfg;
    
    // Параметры выходного канала.
    uint8_t outChannel;        // TIM_CHANNEL_x.
    uint32_t polarity;        // TIM_OCPOLARITY_LOW  / TIM_OCPOLARITY_HIGH.
    
};

class TimCompOneChannel : public mc_interfaces::TimCompOneChannel {
public:
    TimCompOneChannel (const TimCompOneChannelCfg *const cfg);
    
    mc_interfaces::res reinit (uint32_t cfgNumber = 0);
    
    mc_interfaces::res on (void);
    
    void off (void);

private:
    const TimCompOneChannelCfg *const cfg;
    
    TIM_HandleTypeDef tim;
    TIM_OC_InitTypeDef timCh;
};

}

#endif

#endif
