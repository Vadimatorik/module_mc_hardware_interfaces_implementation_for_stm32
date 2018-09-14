/*

@startuml

namespace McHardwareInterfacesImplementation {

McHardwareInterfacesImplementation.Pin ..|> McHardwareInterfaces.Pin

class Pin {
	{field}-	const PinCfg*		const cfg;
	__Constructor__
	{method}+	Pin	( const PinCfg*	const cfg )
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "mc_hardware_interfaces_implementation_for_stm32_platform.h"

#ifdef HAL_GPIO_MODULE_ENABLED

#include "mc_hardware_interfaces_pin.h"

#define PIN_COUNT(x) sizeof(x)/sizeof(x[0])

#define	PINS_ADC_MODE								\
		.Mode		=	GPIO_MODE_ANALOG,			\
		.Pull		=	GPIO_NOPULL,				\
		.Speed		=	GPIO_SPEED_FREQ_LOW,		\
		.Alternate	=	0

#define	PINS_INPUT_MODE								\
		.Mode		=	GPIO_MODE_INPUT,			\
		.Pull		=	GPIO_PULLUP,				\
		.Speed		=	GPIO_SPEED_FREQ_VERY_HIGH,	\
		.Alternate	=	0

#define	PINS_EXTI_MODE								\
		.Mode		=	GPIO_MODE_IT_FALLING,		\
		.Pull		=	GPIO_PULLUP,				\
		.Speed		=	GPIO_SPEED_FREQ_LOW,		\
		.Alternate	=	0

#define PINS_OUTPUT_PP_MODE							\
		.Mode		=	GPIO_MODE_OUTPUT_PP,		\
		.Pull		=	GPIO_NOPULL,				\
		.Speed		=	GPIO_SPEED_FREQ_VERY_HIGH,	\
		.Alternate	=	0

namespace McHardwareInterfacesImplementation {

struct PinCfg {
	const GPIO_TypeDef*		 const GPIOx;
	const GPIO_InitTypeDef	init;
};

class Pin : public McHardwareInterfaces::Pin {
public:
	Pin( const PinCfg* const cfg ) : cfg( cfg ) {}

	void	init			( void );	// Перед инициализацией включается тактирование портов.

	void	set				( void );
	void	reset			( void );
	void	toggle			( void );

	void	set				( bool state );
	void	set				( int state );
	void	set				( uint8_t state );

	bool	read			( void );

protected:
	const PinCfg*			const cfg;

};

}

#endif

#endif