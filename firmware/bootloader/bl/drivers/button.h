#pragma once

#include <cstdint>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"

namespace bl
{
    
class Button
{
public:
    void Init(GPIO_TypeDef* base, uint32_t pin)
    {
        base_ = base;
        pin_ = pin;

        switch (reinterpret_cast<uint32_t>(base_))
        {
            case GPIOA_BASE: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
            case GPIOB_BASE: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
            case GPIOC_BASE: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
            case GPIOD_BASE: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
            case GPIOE_BASE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
            case GPIOF_BASE: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
            case GPIOG_BASE: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
            case GPIOH_BASE: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
            case GPIOI_BASE: __HAL_RCC_GPIOI_CLK_ENABLE(); break;
            case GPIOJ_BASE: __HAL_RCC_GPIOJ_CLK_ENABLE(); break;
            case GPIOK_BASE: __HAL_RCC_GPIOK_CLK_ENABLE(); break;
            default: return;
        }

        GPIO_InitTypeDef gpio_init =
        {
            .Pin       = pin_,
            .Mode      = GPIO_MODE_INPUT,
            .Pull      = GPIO_PULLUP,
            .Speed     = GPIO_SPEED_FREQ_LOW,
            .Alternate = 0,
        };

        HAL_GPIO_Init(base_, &gpio_init);

    }

    bool Pressed(void)
    {
        return !HAL_GPIO_ReadPin(base_, pin_);
    }

protected:
    GPIO_TypeDef* base_;
    uint32_t pin_;
};

}
