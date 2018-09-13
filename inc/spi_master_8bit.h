/*

@startuml

namespace McHardwareInterfacesImplementation {

McHardwareInterfacesImplementation.SpiMaster8Bit ..|> McHardwareInterfaces.SpiMaster8Bit

class SpiMaster8Bit {
	{field}-	const SpiMaster8BitCfg*				const cfg
	{field}-	const uint32_t						cfgCount
	{field}-	uint32_t*							baudratePrescalerArray
	{field}-	uint32_t							numberBaudratePrescalerCfg
	{field}-	SPI_HandleTypeDef					spi
	{field}-	USER_OS_STATIC_BIN_SEMAPHORE_BUFFER	sb
	{field}-	USER_OS_STATIC_BIN_SEMAPHORE		s
	{field}-	DMA_HandleTypeDef					dmaTx
	{field}-	DMA_HandleTypeDef					dmaRx
	{field}-	PinBase*							cs
	{field}-	USER_OS_STATIC_MUTEX				m = nullptr
	{field}-	USER_OS_STATIC_MUTEX_BUFFER			mb
	__Constructor__
	{method}+	SpiMaster8Bit	( const SpiMaster8BitCfg*		const cfg,\n\t\t\t  uint32_t\t\t\t\t\tcfgCount	= 1 )
	__Public methods__
	{method}+	void	giveSemaphore	( void )
	{method}+	void	irqHandler		( void )
	__Private methods__
	{method}-	bool	initClkSpi		( void )
	{method}-	bool	initSpi		( void )
}

}

@enduml

*/

#pragma once

#ifdef __cplusplus

#include "platform.h"

#ifdef HAL_SPI_MODULE_ENABLED

#include "dma.h"
#include "user_os.h"
#include "../../module_mc_hardware_interfaces/mc_hardware_interfaces_spi_master_8bit.h"
#include "pin.h"

namespace McHardwareInterfacesImplementation {

struct SpiMaster8BitCfg {
	SPI_TypeDef*							SPIx;

	McHardwareInterfaces::Pin*				pinCs;

	uint32_t								clkPolarity;					/// SPI_Clock_Polarity.
	uint32_t								clkPhase;						/// SPI_Clock_Phase.

	uint32_t*								baudratePrescalerArray;			/// SPI_BaudRate_Prescaler
	uint32_t								numberBaudratePrescalerCfg;		/// Колличество режимов.

	/// В случае, если DMA не используется (передача и прием на прерываниях),
	/// то следует указать nullptr.
#if defined( STM32F2 ) && defined( STM32F4 )
	DMA_Stream_TypeDef*						dmaTx;							/// Из мерии DMAx_Streamx.
	DMA_Stream_TypeDef*						dmaRx;							/// Из мерии DMAx_Streamx.
	uint32_t								dmaTxCh;						/// Из серии DMA_CHANNEL_x.
	uint32_t								dmaRxCh;						/// Из серии DMA_CHANNEL_x.
#elif defined( STM32F1 )
	DMA_Channel_TypeDef*					dmaTx;
	DMA_Channel_TypeDef*					dmaRx;
#endif


};

class SpiMaster8Bit : public McHardwareInterfaces::SpiMaster8Bit {
public:
	SpiMaster8Bit	(	const SpiMaster8BitCfg*		const cfg,
						uint32_t					cfgCount	= 1	);

	McHardwareInterfaces::BaseResult		reinit			(	uint32_t		numberCfg = 0	);

	McHardwareInterfaces::BaseResult		on				(	void	);
	void									off				(	void	);

	McHardwareInterfaces::BaseResult		tx				(	const uint8_t*		const txArray,
																uint16_t			length				=	1,
																uint32_t			timeoutMs			=	100	);

	McHardwareInterfaces::BaseResult 		tx				(	const uint8_t*		const txArray,
																uint8_t*			rxArray,
																uint16_t			length				=	1,
																uint32_t			timeoutMs			=	100	);

	McHardwareInterfaces::BaseResult		txOneItem		(	uint8_t				txByte,
																uint16_t			count				=	1,
																uint32_t			timeoutMs			=	100	);

	McHardwareInterfaces::BaseResult		rx				(	uint8_t*			rxArray,
																uint16_t			length				=	1,
																uint32_t			timeoutMs			=	100,
																uint8_t				outValue			=	0xFF );

	McHardwareInterfaces::BaseResult		setPrescaler	(	uint32_t			prescalerNumber		=	0	);


	void									giveSemaphore	(	void	);		// Отдать симафор из прерывания (внутренняя функция.
	void									irqHandler		(	void	);

private:
	bool									initClkSpi		(	void	);		// Включаем тактирование SPI и DMA (если используется).
	bool									initSpi			(	void	);		// Инициализируем только SPI (считается, что он уже затактирован).

private:
	const SpiMaster8BitCfg*									const cfg;
	const uint32_t											cfgCount;

	uint32_t*												baudratePrescalerArray;
	uint32_t												numberBaudratePrescalerCfg;

	SPI_HandleTypeDef										spi;

	USER_OS_STATIC_BIN_SEMAPHORE_BUFFER						sb;
	USER_OS_STATIC_BIN_SEMAPHORE							s = nullptr;

	DMA_HandleTypeDef										dmaTx;
	DMA_HandleTypeDef										dmaRx;

	McHardwareInterfaces::Pin*								cs;

	USER_OS_STATIC_MUTEX									m = nullptr;
	USER_OS_STATIC_MUTEX_BUFFER								mb;

};

}

#endif

#endif
