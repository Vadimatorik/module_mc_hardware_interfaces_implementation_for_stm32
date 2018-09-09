/*

@startuml

namespace McHardwareInterfacesImplementation {

McHardwareInterfacesImplementation.Uart ..|> McHardwareInterfaces.Uart

class Uart {
	{field}-	const UartCfg*						const cfg
	{field}-	const uint32_t						cfgCount
	{field}-	DMA_HandleTypeDef					dmaTx
	{field}-	UART_HandleTypeDef					uart
	{field}-	USER_OS_STATIC_BIN_SEMAPHORE		s
	{field}-	USER_OS_STATIC_MUTEX				m
	{field}-	USER_OS_STATIC_BIN_SEMAPHORE_BUFFER	sb
	{field}-	USER_OS_STATIC_MUTEX_BUFFER			mb
	__Constructor__
	{method}+	Uart	( const UartCfg*		const cfg,\n\t  uint32_t			cfgCount	= 1 )
	__Public methods__
	{method}+	void	irqHandler			( void )
	{method}+	void	giveSemaphore		( void )
	__Private methods__
	{method}-	bool 	clkInit			( void )
	{method}-	bool	clkDeinit			( void )
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_UART_MODULE_ENABLED

#include "dma.h"
#include "pin.h"
#include "mc_hardware_interfaces_uart.h"
#include "user_os.h"

namespace McHardwareInterfacesImplementation {

struct UartCfg {
	USART_TypeDef*				uart;
	Pin*						de;
	uint32_t					baudrate;
	uint32_t					mode;						// UART_MODE_RX/UART_MODE_TX/UART_MODE_TX_RX.
	DMA_Stream_TypeDef*			dmaTx;						// Из мерии DMAx_Streamx.
	uint32_t					dmaTxCh;					// Из серии DMA_CHANNEL_x.
	uint32_t					dmaTxPrio;					// Приоритет прерывания.
};

class Uart : public McHardwareInterfaces::Uart {
public:
	Uart	(	const UartCfg*		const cfg,
				uint32_t			cfgCount	= 1 );

	McHardwareInterfaces::BaseResult		reinit						(	uint32_t			cfgNumber		= 0 );

	McHardwareInterfaces::BaseResult		on							(	void	);
	void									off							(	void	);

	McHardwareInterfaces::BaseResult		tx							(	const uint8_t*		const txArray,
																			uint16_t			length			=	1,
																			uint32_t			timeoutMs		=	100	);

	McHardwareInterfaces::BaseResult		getByte						(	uint8_t*			retrunData,
																			uint32_t			timeoutMs		=	100	);

	McHardwareInterfaces::BaseResult		getByteWitchout				(	uint8_t* 			retrunData	);

	void									irqHandler					(	void	);

	/// Для внутреннего пользования HAL-а.
	void			giveSemaphore				(	void	);

private:
	bool 			clkInit						(	void	);
	bool			clkDeinit					(	void	);

private:
	const UartCfg*								const cfg;
	const uint32_t								cfgCount;

	DMA_HandleTypeDef							dmaTx;
	UART_HandleTypeDef							uart;
	USER_OS_STATIC_BIN_SEMAPHORE				s = nullptr;
	USER_OS_STATIC_MUTEX						m = nullptr;
	USER_OS_STATIC_BIN_SEMAPHORE_BUFFER			sb;
	USER_OS_STATIC_MUTEX_BUFFER					mb;
};

}

#endif

#endif
