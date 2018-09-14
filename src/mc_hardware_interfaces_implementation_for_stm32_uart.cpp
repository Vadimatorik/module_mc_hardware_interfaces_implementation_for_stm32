#include "../inc/mc_hardware_interfaces_implementation_for_stm32_uart.h"

#ifdef HAL_UART_MODULE_ENABLED

namespace McHardwareInterfacesImplementation {

Uart::Uart( const UartCfg* const cfg, uint32_t countCfg  ) :
	cfg( cfg ), cfgCount( countCfg ) {
	this->uart.Init.HwFlowCtl							= UART_HWCONTROL_NONE;
#ifdef UART_OVERSAMPLING_8
	this->uart.Init.OverSampling						= UART_OVERSAMPLING_8;
#elif UART_OVERSAMPLING_16
	this->uart.Init.OverSampling						= UART_OVERSAMPLING_16;
#endif
	this->uart.Init.Parity								= UART_PARITY_NONE;
	this->uart.Init.StopBits							= UART_STOPBITS_1;
	this->uart.Init.WordLength							= UART_WORDLENGTH_8B;

	if ( cfg->dmaTx != nullptr ) {
		this->uart.hdmatx								= &this->dmaTx;
		this->uart.hdmatx->Parent						= &this->uart;

		this->uart.hdmatx->Init.Direction				= DMA_MEMORY_TO_PERIPH;
		this->uart.hdmatx->Init.PeriphInc				= DMA_PINC_DISABLE;
		this->uart.hdmatx->Init.MemInc					= DMA_MINC_ENABLE;
		this->uart.hdmatx->Init.PeriphDataAlignment		= DMA_PDATAALIGN_BYTE;
		this->uart.hdmatx->Init.MemDataAlignment		= DMA_MDATAALIGN_BYTE;
		this->uart.hdmatx->Init.Mode					= DMA_NORMAL;
		this->uart.hdmatx->Init.Priority				= DMA_PRIORITY_HIGH;
#if defined(STM32F2) || defined(STM32F4)
		this->uart.hdmatx->Init.FIFOMode				= DMA_FIFOMODE_DISABLE;
#endif
	}

	this->uart.obj										= this;

	this->m = USER_OS_STATIC_MUTEX_CREATE( &this->mb );
	this->s = USER_OS_STATIC_BIN_SEMAPHORE_CREATE( &this->sb );
}

McHardwareInterfaces::BaseResult Uart::reinit ( uint32_t numberCfg ) {
	if ( numberCfg >= this->cfgCount )
		return McHardwareInterfaces::BaseResult::errInputValue;

	this->clkDeinit();

	this->uart.Instance									= cfg[ numberCfg ].uart;
	this->uart.Init.Mode								= cfg[ numberCfg ].mode;
	this->uart.Init.BaudRate							= cfg[ numberCfg ].baudrate;
	this->uart.hdmatx->Instance							= this->cfg[ numberCfg ].dmaTx;
#if defined(STM32F2) || defined(STM32F4)
	this->uart.hdmatx->Init.Channel						= this->cfg[ numberCfg ].dmaTxCh;
#endif

	if ( cfg[ numberCfg ].dmaTx != nullptr ) {
		this->uart.hdmatx								= &this->dmaTx;
		this->uart.hdmatx->Parent						= &this->uart;
		dmaClkOn( this->cfg->dmaTx );

		if ( HAL_DMA_Init( &this->dmaTx ) != HAL_OK )
			return McHardwareInterfaces::BaseResult::errInit;

		dmaIrqOn( this->cfg->dmaTx, cfg[ numberCfg ].dmaTxPrio );
	}

	this->clkInit();

	HAL_StatusTypeDef r;

	r = HAL_UART_DeInit( &this->uart );
	if ( r != HAL_OK )
		return McHardwareInterfaces::BaseResult::errInit;

	r = HAL_UART_Init( &this->uart );
	if ( r != HAL_OK )
		return McHardwareInterfaces::BaseResult::errInit;

	if ( this->cfg->de != nullptr )    	this->cfg->de->reset();

	return McHardwareInterfaces::BaseResult::ok;
}

McHardwareInterfaces::BaseResult Uart::on ( void ) {
	if (   this->uart.gState == HAL_UART_STATE_RESET )
		return McHardwareInterfaces::BaseResult::errInit;

	__HAL_UART_ENABLE( &this->uart );
	return McHardwareInterfaces::BaseResult::ok;
}

void Uart::off ( void ) {
	__HAL_UART_DISABLE( &this->uart );
}

McHardwareInterfaces::BaseResult Uart::tx (	const uint8_t*		const txArray,
											uint16_t			length,
											uint32_t			timeoutMs	) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );
	USER_OS_TAKE_BIN_SEMAPHORE ( this->s, 0 );

	if ( this->cfg->de != nullptr )    	this->cfg->de->set();

	if ( this->uart.hdmatx != nullptr ) {												// Если передача идет по DMA.
		HAL_UART_Transmit_DMA( &this->uart, ( uint8_t* )txArray, length );
	} else {																	// Если по прерываниям.
		HAL_UART_Transmit_IT( &this->uart, ( uint8_t* )txArray, length );
	}

	McHardwareInterfaces::BaseResult rv = McHardwareInterfaces::BaseResult::errTimeOut;
	if ( USER_OS_TAKE_BIN_SEMAPHORE ( this->s, timeoutMs ) == pdTRUE ) {
		rv = McHardwareInterfaces::BaseResult::ok;
	}

	if ( this->cfg->de != nullptr )    		this->cfg->de->reset();

	USER_OS_GIVE_MUTEX( this->m );
	return rv;
}

McHardwareInterfaces::BaseResult Uart::getByteWitchout (	uint8_t* retrunData	) {
	if (   this->uart.gState == HAL_UART_STATE_RESET )
		return McHardwareInterfaces::BaseResult::errInit;

	/// Если есть данные.
	if ( __HAL_UART_GET_FLAG( &this->uart, UART_FLAG_RXNE ) ) {
		*retrunData = this->uart.Instance->DR;
		return McHardwareInterfaces::BaseResult::ok;
	}

	return McHardwareInterfaces::BaseResult::errNotData;
}

McHardwareInterfaces::BaseResult Uart::getByte (	uint8_t*		retrunData,
							uint32_t		timeoutMs	) {
	if (   this->uart.gState == HAL_UART_STATE_RESET )
		return McHardwareInterfaces::BaseResult::errInit;

	/// Если есть данные.
	if ( __HAL_UART_GET_FLAG( &this->uart, UART_FLAG_RXNE ) ) {
		*retrunData = this->uart.Instance->DR;
		return McHardwareInterfaces::BaseResult::ok;
	}

	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );
	USER_OS_TAKE_BIN_SEMAPHORE ( this->s, 0 );

	volatile McHardwareInterfaces::BaseResult rv = McHardwareInterfaces::BaseResult::errTimeOut;
	if ( USER_OS_TAKE_BIN_SEMAPHORE ( this->s, timeoutMs ) == pdTRUE ) {
		*retrunData = this->uart.Instance->DR;
		rv = McHardwareInterfaces::BaseResult::ok;
	}

	USER_OS_GIVE_MUTEX( this->m );

	return rv;
}

void Uart::irqHandler ( void ) {
	HAL_UART_IRQHandler( &this->uart );
	if ( this->uart.hdmatx != nullptr ) {
		HAL_DMA_IRQHandler( this->uart.hdmatx );
	}
}

extern "C" {

void HAL_UART_TxCpltCallback( UART_HandleTypeDef *huart ) {
	((Uart*)huart->obj)->giveSemaphore();
}

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart ) {
	((Uart*)huart->obj)->giveSemaphore();
}

}


// Private.
void Uart::giveSemaphore ( void ) {
	if ( this->s ) {
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		USER_OS_GIVE_BIN_SEMAPHORE_FROM_ISR ( this->s, &xHigherPriorityTaskWoken);
	}
}

bool Uart::clkInit ( void ) {
	switch ( ( uint32_t )this->cfg->uart ) {
#ifdef USART1
	case	USART1_BASE:	 __HAL_RCC_USART1_CLK_ENABLE();		return true;
#endif
#ifdef USART2
	case	USART2_BASE:	 __HAL_RCC_USART2_CLK_ENABLE();		 return true;
#endif
#ifdef USART3
	case	USART3_BASE:	 __HAL_RCC_USART3_CLK_ENABLE();		return true;
#endif
#ifdef UART4
	case	UART4_BASE:		__HAL_RCC_UART4_CLK_ENABLE();		return true;
#endif
#ifdef UART5
	case	UART5_BASE:		__HAL_RCC_UART5_CLK_ENABLE();		return true;
#endif
#ifdef UART6
	case	UART6_BASE:		__HAL_RCC_UART6_CLK_ENABLE();		return true;
#endif
#ifdef UART7
	case	UART7_BASE:		__HAL_RCC_UART7_CLK_ENABLE();		return true;
#endif
#ifdef UART8
	case	UART8_BASE:		__HAL_RCC_UART8_CLK_ENABLE();		return true;
#endif
	};
	return false;
}

bool Uart::clkDeinit ( void ) {
	switch ( ( uint32_t )this->cfg->uart ) {
#ifdef USART1
	case	USART1_BASE:	 __HAL_RCC_USART1_CLK_DISABLE();	 return true;
#endif
#ifdef USART2
	case	USART2_BASE:	 __HAL_RCC_USART2_CLK_DISABLE();	 return true;
#endif
#ifdef USART3
	case	USART3_BASE:	 __HAL_RCC_USART3_CLK_DISABLE();	 return true;
#endif
#ifdef UART4
	case	UART4_BASE:		__HAL_RCC_UART4_CLK_DISABLE();		return true;
#endif
#ifdef UART5
	case	UART5_BASE:		__HAL_RCC_UART5_CLK_DISABLE();		return true;
#endif
#ifdef UART6
	case	UART6_BASE:		__HAL_RCC_UART6_CLK_DISABLE();		return true;
#endif
#ifdef UART7
	case	UART7_BASE:		__HAL_RCC_UART7_CLK_DISABLE();		return true;
#endif
#ifdef UART8
	case	UART8_BASE:		__HAL_RCC_UART8_CLK_DISABLE();		return true;
#endif
	};
	return false;
}

}

#endif
