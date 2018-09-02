/*!
 *	@startuml
 *
 *	class GlobalPort {
 *		{field}-	const PinCfg*			const cfg
 *		{field}-	const uint32_t			cfgCount
 *		__Constructor__
 *		{method}+	GlobalPort	( const PinCfg*	const cfg,\n\t\t  uint32_t		cfgCount )
 *	}
 *
 *	@enduml
 */

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_GPIO_MODULE_ENABLED

#include <mc_hardware_interfaces_port_global.h>

#include "pin.h"

class GlobalPort : public GlobalPortBase {
public:
	GlobalPort	(	const PinCfg*		const cfg,
					uint32_t			cfgCount	)
		: cfg( cfg ), cfgCount( cfgCount ) {}

	BaseResult	reinitAllPorts		( void );

private:
	const PinCfg*				const cfg;
	const uint32_t				cfgCount;

};

#endif

#endif
