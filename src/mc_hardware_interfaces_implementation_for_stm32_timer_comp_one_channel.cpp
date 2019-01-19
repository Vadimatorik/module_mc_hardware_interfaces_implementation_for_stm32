#include "mc_hardware_interfaces_implementation_for_stm32_timer_comp_one_channel.h"

#ifdef HAL_TIM_MODULE_ENABLED

namespace McHardwareInterfacesImplementation {

extern void clkTimInit (TIM_TypeDef *tim);

TimCompOneChannel::TimCompOneChannel (const TimCompOneChannelCfg *const cfg) : cfg(cfg) {
    this->tim.Instance = this->cfg->tim;
    
    this->tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    this->tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    
    this->timCh.OCMode = TIM_OCMODE_TOGGLE;
    this->timCh.OCPolarity = this->cfg->polarity;
    this->timCh.Pulse = 0;
}

McHardwareInterfaces::BaseResult TimCompOneChannel::reinit (uint32_t numberCfg) {
    if (numberCfg >= this->cfg->countCfg)
        return McHardwareInterfaces::BaseResult::errInputValue;
    
    this->tim.Init.Period = this->cfg->cfg[numberCfg].period;
    this->tim.Init.Prescaler = this->cfg->cfg[numberCfg].prescaler;
    this->timCh.Pulse = this->cfg->cfg[numberCfg].pulse;
    
    clkTimInit(this->tim.Instance);
    
    if (HAL_TIM_OC_DeInit(&this->tim) != HAL_OK)
        return McHardwareInterfaces::BaseResult::errInit;
    
    if (HAL_TIM_OC_Init(&this->tim) != HAL_OK)
        return McHardwareInterfaces::BaseResult::errInit;
    
    if (HAL_TIM_OC_ConfigChannel(&this->tim, &this->timCh, this->cfg->outChannel) != HAL_OK)
        return McHardwareInterfaces::BaseResult::errInit;
    
    return McHardwareInterfaces::BaseResult::ok;
}

McHardwareInterfaces::BaseResult TimCompOneChannel::on (void) {
    if (this->tim.State == HAL_TIM_STATE_RESET)
        return McHardwareInterfaces::BaseResult::errInit;
    
    HAL_TIM_OC_Start(&this->tim, this->cfg->outChannel);
    HAL_TIMEx_OCN_Start(&this->tim, this->cfg->outChannel);
    
    return McHardwareInterfaces::BaseResult::ok;
}

void TimCompOneChannel::off (void) {
    this->tim.Instance->CR1 &= ~(TIM_CR1_CEN);
}

}

#endif

