/*

@startuml

namespace McHardwareInterfacesImplementation {

McHardwareInterfacesImplementation.PinMultifunc		--|>	McHardwareInterfacesImplementation.Pin
McHardwareInterfacesImplementation.PinMultifunc 	..|> 	McHardwareInterfaces.PinMultifunc

class PinMultifunc {
	{field}-	const uint32_t		cfgCount
	__Constructor__
	{method}+	PinMultifunc\t( const PinCfg*\tconst cfg,\n\t\t\t  uint32_t\t\tcfgCount )
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "mc_hardware_interfaces_implementation_for_stm32_platform.h"

#ifdef HAL_GPIO_MODULE_ENABLED

#include "mc_hardware_interfaces_implementation_for_stm32_pin.h"
#include "mc_hardware_interfaces_pin_multifunc.h"

namespace McHardwareInterfacesImplementation {

class PinMultifunc : public Pin, public mc_interfaces::PinMultifunc {
public:
    PinMultifunc (const PinCfg *const cfg,
                  uint32_t cfgCount)
        : Pin(cfg), cfgCount(cfgCount) {}
    
    bool reinit (uint32_t cfgNumber);

protected:
    const uint32_t cfgCount;
};

}

#endif

#endif
