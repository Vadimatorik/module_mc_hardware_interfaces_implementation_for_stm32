#include "mc_hardware_interfaces_implementation_for_stm32_pwr.h"

#ifdef HAL_PWR_MODULE_ENABLED

namespace McHardwareInterfacesImplementation {

mc_interfaces::res Pwr::reinit (uint32_t numberCfg) {
    if (numberCfg >= this->cfgCount)
        return mc_interfaces::res::errInputValue;
    
    __HAL_RCC_PWR_CLK_ENABLE();
    this->cfgNow = numberCfg;
    
    HAL_PWR_ConfigPVD((PWR_PVDTypeDef *)&this->cfg[this->cfgNow].cfg);
    
    return mc_interfaces::res::ok;
}

void Pwr::pvdEnable (void) {
    HAL_PWR_EnablePVD();
}

void Pwr::pvdDisable (void) {
    HAL_PWR_DisablePVD();
}

void Pwr::irqHandler (void) {
    HAL_PWR_PVD_IRQHandler();
}

}

#endif
