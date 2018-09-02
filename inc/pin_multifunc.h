/*!
 *	@startuml
 *
 *	class PinMultifunc {
 *		{field}-	const uint32_t		cfgCount
 *		__Constructor__
 *		{method}+	PinMultifunc\t( const pinCfg*\tconst cfg,\n\t\t\t  uint32_t\t\tcfgCount )
 *	}
 *
 *	@enduml
 */

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_GPIO_MODULE_ENABLED

#include "pin.h"
#include "mc_hardware_interfaces_pin_multifunc.h"

class PinMultifunc : public Pin, public PinMultifuncBase {
public:
	PinMultifunc	(	const PinCfg*		const cfg,
						uint32_t			cfgCount	)
		: Pin( cfg ), cfgCount( cfgCount ) {}

	bool	reinit			( uint32_t cfgNumber );

protected:
	const uint32_t		cfgCount;
};

#endif

#endif
