#include <string.h>
#include "spi_master_8bit.h"

#ifdef HAL_SPI_MODULE_ENABLED

namespace McHardwareInterfacesImplementation {

SpiMaster8Bit::SpiMaster8Bit( const SpiMaster8BitCfg* const cfg, const uint32_t countCfg ) :
	cfg( cfg ), cfgCount( countCfg ) {
	this->spi.obj									=	this;
	this->m											=	USER_OS_STATIC_MUTEX_CREATE( &mb );
	this->s											=	USER_OS_STATIC_BIN_SEMAPHORE_CREATE( &this->sb );
}

McHardwareInterfaces::BaseResult SpiMaster8Bit::reinit ( uint32_t numberCfg  ) {
	if ( numberCfg >= this->cfgCount ) return McHardwareInterfaces::BaseResult::errInputValue;

	this->spi.Instance								=	cfg->SPIx;
	this->spi.Init.Mode								=	SPI_MODE_MASTER;
	this->spi.Init.Direction						=	SPI_DIRECTION_2LINES;
	this->spi.Init.DataSize							=	SPI_DATASIZE_8BIT;
	this->spi.Init.CLKPolarity						=	cfg->clkPolarity;
	this->spi.Init.CLKPhase							=	cfg->clkPhase;
	this->spi.Init.BaudRatePrescaler				=	cfg->baudratePrescalerArray[ 0 ];

	this->spi.Init.NSS								=	SPI_NSS_SOFT;
	this->spi.Init.FirstBit							=	SPI_FIRSTBIT_MSB;
	this->spi.Init.TIMode							=	SPI_TIMODE_DISABLED;
	this->spi.Init.CRCCalculation					=	SPI_CRCCALCULATION_DISABLED;
	this->spi.Init.CRCPolynomial					=	0xFF;

	if ( cfg->dmaTx != nullptr ) {
		this->spi.hdmatx							=	&this->dmaTx;
		this->spi.hdmatx->Instance					=	this->cfg[ numberCfg ].dmaTx;
		this->spi.hdmatx->Init.Channel				=	this->cfg[ numberCfg ].dmaTxCh;
		this->spi.hdmatx->Init.Direction			=	DMA_MEMORY_TO_PERIPH;
		this->spi.hdmatx->Init.PeriphInc			=	DMA_PINC_DISABLE;
		this->spi.hdmatx->Init.MemInc				=	DMA_MINC_ENABLE;
		this->spi.hdmatx->Init.PeriphDataAlignment	=	DMA_PDATAALIGN_BYTE;
		this->spi.hdmatx->Init.MemDataAlignment		=	DMA_MDATAALIGN_BYTE;
		this->spi.hdmatx->Init.Mode					=	DMA_NORMAL;
		this->spi.hdmatx->Init.Priority				=	DMA_PRIORITY_HIGH;
		this->spi.hdmatx->Init.FIFOMode				=	DMA_FIFOMODE_DISABLE;

		this->spi.hdmatx							=	&this->dmaTx;
		this->spi.hdmatx->Parent					=	&this->spi;
	}

	if ( cfg->dmaRx != nullptr ) {
		this->spi.hdmarx							=	&this->dmaRx;
		this->spi.hdmarx->Instance					=	this->cfg[ numberCfg ].dmaRx;
		this->spi.hdmarx->Init.Channel				=	this->cfg[ numberCfg ].dmaRxCh;
		this->spi.hdmarx->Init.Direction			=	DMA_PERIPH_TO_MEMORY;
		this->spi.hdmarx->Init.PeriphInc			=	DMA_PINC_DISABLE;
		this->spi.hdmarx->Init.MemInc				=	DMA_MINC_ENABLE;
		this->spi.hdmarx->Init.PeriphDataAlignment	=	DMA_PDATAALIGN_BYTE;
		this->spi.hdmarx->Init.MemDataAlignment		=	DMA_MDATAALIGN_BYTE;
		this->spi.hdmarx->Init.Mode					=	DMA_NORMAL;
		this->spi.hdmarx->Init.Priority				=	DMA_PRIORITY_HIGH;
		this->spi.hdmarx->Init.FIFOMode				=	DMA_FIFOMODE_DISABLE;

		this->spi.hdmarx							=	&this->dmaRx;
		this->spi.hdmarx->Parent					=	&this->spi;
	}

	this->baudratePrescalerArray					=	this->cfg[ numberCfg ].baudratePrescalerArray;
	this->numberBaudratePrescalerCfg				=	this->cfg[ numberCfg ].numberBaudratePrescalerCfg;

	if ( this->initClkSpi() == false )		return McHardwareInterfaces::BaseResult::errInit;				// Включаем тактирование SPI.

	if ( this->initSpi() == false )
		return McHardwareInterfaces::BaseResult::errInit;

	this->cs										=	this->cfg[ numberCfg ].pinCs;

	return McHardwareInterfaces::BaseResult::ok;
}

McHardwareInterfaces::BaseResult SpiMaster8Bit::on ( void ) {
	if ( this->spi.State ==  HAL_SPI_STATE_RESET )	return McHardwareInterfaces::BaseResult::errInit;
	__HAL_SPI_ENABLE( &this->spi );
	return McHardwareInterfaces::BaseResult::ok;
}

void SpiMaster8Bit::off ( void ) {
	__HAL_SPI_DISABLE( &this->spi );
}

McHardwareInterfaces::BaseResult SpiMaster8Bit::tx (	const uint8_t*		const txArray,
								uint16_t			length,
								uint32_t			timeoutMs ) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

	McHardwareInterfaces::BaseResult rv = McHardwareInterfaces::BaseResult::errTimeOut ;
	xSemaphoreTake ( this->s, 0 );

	if ( this->cs != nullptr )
		this->cs->set( 0 );

	if ( this->spi.hdmatx != nullptr ) {
		HAL_SPI_Transmit_DMA( &this->spi, ( uint8_t* )txArray, length );
	}

	if ( xSemaphoreTake ( this->s, timeoutMs ) == pdTRUE ) {
		rv = McHardwareInterfaces::BaseResult::ok;
	}

	if ( this->cs != nullptr )
		this->cs->set( 1 );

	USER_OS_GIVE_MUTEX( this->m );

	return rv;
}

McHardwareInterfaces::BaseResult SpiMaster8Bit::tx (	const uint8_t*		const txArray,
								uint8_t*			rxArray,
								uint16_t			length,
								uint32_t			timeoutMs	) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );
	xSemaphoreTake ( this->s, 0 );

	McHardwareInterfaces::BaseResult rv = McHardwareInterfaces::BaseResult::errTimeOut;

	if ( this->cs != nullptr )
		this->cs->set( 0 );

	if ( ( this->spi.hdmatx != nullptr ) && ( this->spi.hdmarx != nullptr ) ) {
		HAL_SPI_TransmitReceive_DMA( &this->spi, ( uint8_t* )txArray, rxArray, length );
	} else {
		HAL_SPI_TransmitReceive_IT( &this->spi, ( uint8_t* )txArray, rxArray, length );
	}

	if ( xSemaphoreTake ( this->s, timeoutMs ) == pdTRUE ) {
		rv = McHardwareInterfaces::BaseResult::ok;
	}

	if ( this->cs != nullptr )
		this->cs->set( 1 );

	USER_OS_GIVE_MUTEX( this->m );

	return rv;
}

McHardwareInterfaces::BaseResult SpiMaster8Bit::txOneItem (	uint8_t		txByte,
										uint16_t	count,
										uint32_t	timeoutMs	) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

	McHardwareInterfaces::BaseResult rv = McHardwareInterfaces::BaseResult::errTimeOut ;
	xSemaphoreTake ( this->s, 0 );

	if ( this->cs != nullptr )
		this->cs->set( 0 );

	uint8_t txArray[count];
	memset( txArray, txByte, count );

	if ( this->spi.hdmatx != nullptr ) {
		HAL_SPI_Transmit_DMA( &this->spi, txArray, count );
	} else {
		HAL_SPI_Transmit_IT( &this->spi, txArray, count );
	}

	if ( xSemaphoreTake ( this->s, timeoutMs ) == pdTRUE ) {
		rv = McHardwareInterfaces::BaseResult::ok;
	}

	if ( this->cs != nullptr )
		this->cs->set( 1 );

	USER_OS_GIVE_MUTEX( this->m );

	return rv;
}

McHardwareInterfaces::BaseResult SpiMaster8Bit::rx (	uint8_t*		rxArray,
								uint16_t		length,
								uint32_t		timeoutMs,
								uint8_t			outValue	) {
	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );
	xSemaphoreTake ( this->s, 0 );

	McHardwareInterfaces::BaseResult rv = McHardwareInterfaces::BaseResult::errTimeOut ;

	if ( this->cs != nullptr )		 // Опускаем CS (для того, чтобы "выбрать" устроство).
		this->cs->set( 0 );

	uint8_t txDummy[ length ];
	memset( txDummy, outValue, length );

	if ( ( this->spi.hdmatx != nullptr ) && ( this->spi.hdmarx != nullptr ) ) {
		HAL_SPI_TransmitReceive_DMA( &this->spi, ( uint8_t* )txDummy, rxArray, length );
	} else {
		HAL_SPI_TransmitReceive_IT( &this->spi, txDummy, rxArray, length );
	}

	if ( xSemaphoreTake ( this->s, timeoutMs ) == pdTRUE ) {
		rv = McHardwareInterfaces::BaseResult::ok;
	}

	if ( this->cs != nullptr )
		this->cs->set( 1 );

	USER_OS_GIVE_MUTEX( this->m );

	return rv;
}

void SpiMaster8Bit::irqHandler ( void ) {
	if ( this->spi.hdmatx != nullptr )
		HAL_DMA_IRQHandler( &this->dmaTx );

	if ( this->spi.hdmarx != nullptr )
		HAL_DMA_IRQHandler( &this->dmaRx );
}

McHardwareInterfaces::BaseResult SpiMaster8Bit::setPrescaler (	uint32_t prescalerNumber	) {
	if ( prescalerNumber >= this->numberBaudratePrescalerCfg ) return McHardwareInterfaces::BaseResult::errInputValue;

	USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

	this->spi.Instance->CR1 &= ~( ( uint32_t )SPI_CR1_BR_Msk );
	this->spi.Instance->CR1 |= this->baudratePrescalerArray[ prescalerNumber ];

	USER_OS_GIVE_MUTEX( this->m );

	return McHardwareInterfaces::BaseResult::ok;
}

/*!
 * Костыль для HAL.
 */
void SpiMaster8Bit::giveSemaphore ( void ) {
	if ( this->s ) {
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR ( this->s, &xHigherPriorityTaskWoken);
	}
}

/*******************************************************************************************************
 * Обработчики.
 *******************************************************************************************************/
extern "C" {

void HAL_SPI_TxCpltCallback ( SPI_HandleTypeDef *hspi ) {
		 SpiMaster8Bit* o = ( SpiMaster8Bit* )hspi->obj;
		 o->giveSemaphore();
}

void HAL_SPI_RxCpltCallback ( SPI_HandleTypeDef *hspi ) {
		 SpiMaster8Bit* o = ( SpiMaster8Bit* )hspi->obj;
		 o->giveSemaphore();
}

void HAL_SPI_TxRxCpltCallback ( SPI_HandleTypeDef *hspi ) {
		 SpiMaster8Bit* o = ( SpiMaster8Bit* )hspi->obj;
		 o->giveSemaphore();
}

}

// Включаем тактирование SPI.
bool SpiMaster8Bit::initClkSpi ( void ) {
	switch ( ( uint32_t )this->spi.Instance ) {
#ifdef SPI
	case		SPI_BASE:		 __SPI_CLK_ENABLE();		return true;
#endif
#ifdef SPI1
	case		SPI1_BASE:		__SPI1_CLK_ENABLE();		return true;
#endif
#ifdef SPI2
	case		SPI2_BASE:		__SPI2_CLK_ENABLE();		return true;
#endif
#ifdef SPI3
	case		SPI3_BASE:		__SPI3_CLK_ENABLE();		return true;
#endif
#ifdef SPI4
	case		SPI4_BASE:		__SPI4_CLK_ENABLE();		return true;
#endif
#ifdef SPI5
	case		SPI5_BASE:		__SPI5_CLK_ENABLE();		return true;
#endif
#ifdef SPI6
	case		SPI6_BASE:		__SPI6_CLK_ENABLE();		return true;
#endif
	};
	return false;
}

bool SpiMaster8Bit::initSpi ( void ) {
	HAL_SPI_DeInit( &this->spi );

	HAL_StatusTypeDef r;
	r = HAL_SPI_Init ( &this->spi );
	if ( r != HAL_OK ) return false;

	if ( this->spi.hdmatx != nullptr ) {
		dmaClkOn( this->spi.hdmatx->Instance );
		r = HAL_DMA_Init( &this->dmaTx );
		if ( r != HAL_OK ) return false;
	}

	if ( this->spi.hdmarx != nullptr ) {
		dmaClkOn( this->spi.hdmarx->Instance );
		r = HAL_DMA_Init( &this->dmaRx );
		if ( r != HAL_OK ) return false;
	}

	if ( this->cs != nullptr )
		this->cs->set( 1 );

	return true;
}

}

#endif
