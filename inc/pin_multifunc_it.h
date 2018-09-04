/*

@startuml

McHardwareInterfacesImplementation.PinMultifuncIt --|> McHardwareInterfacesImplementation.PinMultifunc
McHardwareInterfacesImplementation.PinMultifuncIt ..|> McHardwareInterfaces.PinMultifuncItBase

namespace McHardwareInterfacesImplementation {

class PinMultifuncIt {
	{field}-	const uint32_t			exitPin
	__Constructor__
	{method}+	PinMultifuncIt\t( const PinCfg*\t\tconst cfg,\n\t\t\t  uint32_t\t\t\tcfgCount,\n\t\t\t  uint32_t\t\t\texitPin )
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_GPIO_MODULE_ENABLED

#include "pin_multifunc.h"
#include "mc_hardware_interfaces_pin_multifunc_it.h"

namespace McHardwareInterfacesImplementation {

class PinMultifuncIt : public PinMultifunc, public McHardwareInterfaces::PinMultifuncIt {
public:
	/// exitPin - GPIO_PIN_x.
	PinMultifuncIt	(	const PinCfg*		const cfg,
						uint32_t			cfgCount,
						uint32_t			exitPin	)
		: PinMultifunc( cfg, cfgCount ), exitPin( exitPin ) {}

	bool	checkIt			( void );
	void	clearIt			( void );

private:
	const uint32_t			exitPin;

};

}

#endif

#endif
