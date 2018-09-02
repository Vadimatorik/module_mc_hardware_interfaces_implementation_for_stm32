/*

@startuml

class Pwr {
	{field}-	const PwrCfg*		const cfg
	{field}-	const uint32_t		cfgCount
	{field}-	uint32_t			cfgNow
	__Constructor__
	{method}+	Pwr ( const PwrCfg*	const cfg,\n\tuint32_t		cfgCount = 1 )
	__Public methods__
	{method}+	void	irqHandler		( void )
}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_PWR_MODULE_ENABLED

#include "mc_hardware_interfaces_pwr.h"
#include "user_os.h"

struct PwrCfg {
	const PWR_PVDTypeDef			cfg;
};

class Pwr : public PwrBase {
public:
	Pwr (	const PwrCfg*	const cfg,
			uint32_t		cfgCount = 1 )
		: cfg( cfg ), cfgCount( cfgCount ) {}

	BaseResult	reinit			( uint32_t cfgNumber = 0 );

	void		pvdEnable		( void );
	void		pvdDisable		( void );

	void		irqHandler		( void );

private:
	const PwrCfg*							const cfg;
	const uint32_t							cfgCount;

	uint32_t								cfgNow			=	0;
};

#endif

#endif
