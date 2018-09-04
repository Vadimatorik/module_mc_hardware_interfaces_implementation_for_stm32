/*

@startuml

McHardwareInterfacesImplementation.Wdt ..|> McHardwareInterfaces.Wdt

namespace McHardwareInterfacesImplementation {

class Wdt {
	{field}-	const WdtCfg*					const cfg
	{field}-	const uint32_t					cfgCount
	{field}-	uint32_t						cfgNow
	{field}-	USER_OS_STATIC_STACK_TYPE		taskStack[ 64 ]
	{field}-	USER_OS_STATIC_TASK_STRUCT_TYPE	taskStruct
	{field}-	uint8_t						reboot
	__Constructor__
	{method}+	Wdt ( const WdtCfg*		const cfg,\n\tuint32_t			cfgCount = 1 )
	__Private methods__
	{method}{static}-	void	task	( void*	obj )
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_WWDG_MODULE_ENABLED

#include "mc_hardware_interfaces_wdt.h"
#include "user_os.h"

namespace McHardwareInterfacesImplementation {

struct WdtCfg {
	const uint8_t	taskPrio;				// Приоритет задачи, сбрасывающий wdt.
	const uint32_t	runTimeMs;				// Время перезагрузки по сторожевому таймеру.
											// при номинальном режиме работы системы.
	const uint32_t	startupTimeMs;			// Время перезагрузки по сторожевому таймеру при запуске системы.
	const uint32_t	serviceTimeMs;			// Время перезагрузки по сторожевому таймеру
											// во время сервисных операций (например, стирание и перезапись flash).
};

class Wdt : public McHardwareInterfaces::Wdt {
public:
	Wdt (	const WdtCfg*		const cfg,
			uint32_t			cfgCount = 1 )
		: cfg( cfg ), cfgCount( cfgCount ),
		  cfgNow( 0 ) {}

	McHardwareInterfaces::BaseResult		reinit			( uint32_t numberCfg = 0 );
	void									reset			( void );
	void									resetService	( void );

private:
	const WdtCfg*							const cfg;
	const uint32_t							cfgCount;

	uint32_t								cfgNow;

	static void task ( void* obj );

	USER_OS_STATIC_STACK_TYPE				taskStack[ 64 ] = { 0 };
	USER_OS_STATIC_TASK_STRUCT_TYPE			taskStruct;

	uint8_t									reboot;
};

}

#endif

#endif
