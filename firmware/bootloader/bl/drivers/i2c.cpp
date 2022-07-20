#include "drivers/i2c.h"

#include <cstring>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"

namespace bl
{

#define GPIO_MODULE GPIOB
#define GPIO_PERIPH_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE
static constexpr uint32_t kI2CPins = LL_GPIO_PIN_10 | LL_GPIO_PIN_11;
static constexpr uint32_t kGPIOAlternateFunction = GPIO_AF4_I2C2;
#define I2C_MODULE I2C2
#define I2C_PERIPH_ENABLE __HAL_RCC_I2C2_CLK_ENABLE
#define I2C_PERIPH_FORCE_RESET __HAL_RCC_I2C2_FORCE_RESET
#define I2C_PERIPH_RELEASE_RESET __HAL_RCC_I2C2_RELEASE_RESET

static void InitGPIO(void)
{
    LL_GPIO_InitTypeDef gpio_init;
    LL_GPIO_StructInit(&gpio_init);
    gpio_init.Pin        = kI2CPins;
    gpio_init.Mode       = LL_GPIO_MODE_ALTERNATE;
    gpio_init.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    gpio_init.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    gpio_init.Pull       = LL_GPIO_PULL_NO;
    gpio_init.Alternate  = kGPIOAlternateFunction;

    GPIO_PERIPH_ENABLE();
    assert_param(SUCCESS == LL_GPIO_Init(GPIO_MODULE, &gpio_init));
}

static I2C_HandleTypeDef i2c_handle;
static uint8_t tx_buffer[256];

void I2CModule::Init(void)
{
    InitGPIO();

    I2C_PERIPH_ENABLE();
    I2C_PERIPH_FORCE_RESET();
    I2C_PERIPH_RELEASE_RESET();

    i2c_handle.Instance = I2C_MODULE;
    i2c_handle.State = HAL_I2C_STATE_RESET;
    i2c_handle.Init =
    {
        .ClockSpeed      = 100000,
        .DutyCycle       = I2C_DUTYCYCLE_2,
        .OwnAddress1     = 0x10,
        .AddressingMode  = I2C_ADDRESSINGMODE_7BIT,
        .DualAddressMode = I2C_DUALADDRESS_DISABLE,
        .OwnAddress2     = 0,
        .GeneralCallMode = I2C_GENERALCALL_DISABLE,
        .NoStretchMode   = I2C_NOSTRETCH_DISABLE,
    };

    assert_param(HAL_OK == HAL_I2C_Init(&i2c_handle));
}

bool I2CModule::Read(uint8_t device_addr, uint16_t register_addr,
    uint8_t* data, uint32_t length)
{
    auto status = HAL_I2C_Mem_Read(
        &i2c_handle,
        device_addr,
        register_addr,
        I2C_MEMADD_SIZE_16BIT,
        data,
        length,
        HAL_MAX_DELAY);

    return HAL_OK == status;
}

bool I2CModule::Write(uint8_t device_addr, uint16_t register_addr,
    const uint8_t* data, uint32_t length)
{
    if (length > 256)
    {
        return false;
    }

    std::memcpy(tx_buffer, data, length);

    auto status = HAL_I2C_Mem_Write(
        &i2c_handle,
        device_addr,
        register_addr,
        I2C_MEMADD_SIZE_16BIT,
        tx_buffer,
        length,
        HAL_MAX_DELAY);

    return HAL_OK == status;
}

}
