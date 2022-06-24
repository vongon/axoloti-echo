// MIT License
//
// Copyright 2021 Tyler Coy
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_system.h"
#include "drivers/gpio.h"
#include "drivers/button.h"
#include "drivers/codec.h"

#include "qpsk/decoder.h"

namespace bl
{

constexpr uint32_t kAppStartAddress = FLASH_BASE + BOOTLOADER_SIZE;

OutputPin red_led_;
OutputPin green_led_;
OutputPin led1_;
OutputPin led2_;
OutputPin relay_control_;

Button button1_;
Button button2_;

Codec codec_;

static_assert(SAMPLE_RATE % SYMBOL_RATE == 0);
constexpr uint32_t kSampleRate = SAMPLE_RATE;
constexpr uint32_t kSymbolRate = SYMBOL_RATE;
constexpr uint32_t kPacketSize = PACKET_SIZE;
constexpr uint32_t kBlockSize = BLOCK_SIZE;
constexpr uint32_t kCRCSeed = CRC_SEED;

qpsk::Decoder<kSampleRate, kSymbolRate, kPacketSize, kBlockSize> decoder_;

#ifdef USE_FULL_ASSERT
extern "C"
void assert_failed(uint8_t* file, uint32_t line)
{
    (void)file;
    (void)line;

    for (;;)
    {
        red_led_.Toggle();
        HAL_Delay(100);
    }
}
#endif

extern "C"
void SysTick_Handler(void)
{
    HAL_IncTick();
}

void CodecCallback(int32_t sample)
{
    decoder_.Push(sample * 1.0 / INT32_MAX);
}

void InitPowerAndClock(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef osc_init =
    {
        .OscillatorType      = RCC_OSCILLATORTYPE_HSE,
        .HSEState            = RCC_HSE_ON,
        .LSEState            = RCC_LSE_OFF,
        .HSIState            = RCC_HSI_OFF,
        .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
        .LSIState            = RCC_LSI_OFF,

        // Assuming HSE is 8MHz,
        // VCO clock is HSE * N/M = 8 MHz * 84/2 = 336 MHz
        // System clock is VCO / P = 336 MHz / 2 = 168 MHz
        // USB clock is VCO / Q = 336 MHz / 7 = 48 MHz
        .PLL =
        {
            .PLLState  = RCC_PLL_ON,
            .PLLSource = RCC_PLLSOURCE_HSE,
            .PLLM      = 2,
            .PLLN      = 84,
            .PLLP      = RCC_PLLP_DIV2,
            .PLLQ      = 7,
        },
    };

    assert_param(HAL_OK == HAL_RCC_OscConfig(&osc_init));

    RCC_ClkInitTypeDef clk_init =
    {
        .ClockType      = RCC_CLOCKTYPE_SYSCLK |
                          RCC_CLOCKTYPE_HCLK |
                          RCC_CLOCKTYPE_PCLK1 |
                          RCC_CLOCKTYPE_PCLK2,
        .SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK,
        .AHBCLKDivider  = RCC_SYSCLK_DIV1,
        .APB1CLKDivider = RCC_HCLK_DIV4,
        .APB2CLKDivider = RCC_HCLK_DIV2,
    };

    assert_param(HAL_OK == HAL_RCC_ClockConfig(&clk_init, FLASH_LATENCY_5));
}

struct SectorInfo
{
    uint32_t sector_num;
    uint32_t address;
    uint32_t erase_time_ms;
};

const SectorInfo kSectors[] =
{
    { FLASH_SECTOR_0,  0x08000000,  500 },
    { FLASH_SECTOR_1,  0x08004000,  500 },
    { FLASH_SECTOR_2,  0x08008000,  500 },
    { FLASH_SECTOR_3,  0x0800C000,  500 },
    { FLASH_SECTOR_4,  0x08010000, 1100 },
    { FLASH_SECTOR_5,  0x08020000, 2000 },
    { FLASH_SECTOR_6,  0x08040000, 2000 },
    { FLASH_SECTOR_7,  0x08060000, 2000 },
    { FLASH_SECTOR_8,  0x08080000, 2000 },
    { FLASH_SECTOR_9,  0x080A0000, 2000 },
    { FLASH_SECTOR_10, 0x080C0000, 2000 },
    { FLASH_SECTOR_11, 0x080E0000, 2000 },
    { FLASH_SECTOR_12, 0x08100000,  500 },
    { FLASH_SECTOR_13, 0x08104000,  500 },
    { FLASH_SECTOR_14, 0x08108000,  500 },
    { FLASH_SECTOR_15, 0x0810C000,  500 },
    { FLASH_SECTOR_16, 0x08110000, 1100 },
    { FLASH_SECTOR_17, 0x08120000, 2000 },
    { FLASH_SECTOR_18, 0x08140000, 2000 },
    { FLASH_SECTOR_19, 0x08160000, 2000 },
    { FLASH_SECTOR_20, 0x08180000, 2000 },
    { FLASH_SECTOR_21, 0x081A0000, 2000 },
    { FLASH_SECTOR_22, 0x081C0000, 2000 },
    { FLASH_SECTOR_23, 0x081E0000, 2000 },
};

bool WriteBlock(uint32_t address, const uint32_t* data)
{
    SectorInfo sector_info;
    bool do_erase = false;

    for (auto& sector : kSectors)
    {
        if (address == sector.address)
        {
            sector_info = sector;
            do_erase = true;
        }
    }

    if (HAL_OK != HAL_FLASH_Unlock())
    {
        return false;
    }

    if (do_erase)
    {
        FLASH_Erase_Sector(sector_info.sector_num,
            FLASH_VOLTAGE_RANGE_3);
        FLASH_WaitForLastOperation(HAL_MAX_DELAY);
    }

    for (uint32_t i = 0; i < kBlockSize; i += 4)
    {
        if (HAL_OK != HAL_FLASH_Program(
            FLASH_TYPEPROGRAM_WORD, address + i, *data++))
        {
            return false;
        }
    }

    if (HAL_OK != HAL_FLASH_Lock())
    {
        return false;
    }

    return true;
}

extern "C"
int main(void)
{
    button1_.Init(GPIOC, GPIO_PIN_4);

    if (!button1_.Pressed())
    {
        return 0;
    }

    assert_param(HAL_OK == HAL_Init());
    InitPowerAndClock();

    red_led_.Init(GPIOC, GPIO_PIN_6);
    green_led_.Init(GPIOG, GPIO_PIN_6);
    led1_.Init(GPIOC, GPIO_PIN_13);
    led2_.Init(GPIOC, GPIO_PIN_14);
    button2_.Init(GPIOC, GPIO_PIN_5);
    codec_.Init(CodecCallback);
    decoder_.Init(kCRCSeed);
    __enable_irq();

    codec_.Start();
    uint32_t block_address = kAppStartAddress;

    auto& packet_led = led1_;
    auto& write_led = led2_;
    auto& error_led = red_led_;
    auto& success_led = green_led_;

    for (;;)
    {
        auto result = decoder_.Process();

        if (result == qpsk::RESULT_PACKET_COMPLETE)
        {
            packet_led.Toggle();
        }
        else if (result == qpsk::RESULT_BLOCK_COMPLETE)
        {
            packet_led.Clear();
            write_led.Set();

            if (!WriteBlock(block_address, decoder_.block_data()))
            {
                decoder_.Abort();
            }

            block_address += kBlockSize;
            write_led.Clear();
        }
        else if (result == qpsk::RESULT_END)
        {
            for (uint32_t i = 0; i < 20; i++)
            {
                success_led.Toggle();
                HAL_Delay(100);
            }

            codec_.Stop();
            NVIC_SystemReset();
        }
        else if (result == qpsk::RESULT_ERROR)
        {
            packet_led.Clear();

            switch (decoder_.error())
            {
                case qpsk::ERROR_SYNC:
                    write_led.Set();
                    break;
                case qpsk::ERROR_CRC:
                    packet_led.Set();
                    break;
                case qpsk::ERROR_OVERFLOW:
                    write_led.Set();
                    packet_led.Set();
                    break;
                default:
                    break;
            }

            while (!button1_.Pressed())
            {
                error_led.Toggle();
                HAL_Delay(100);
            }

            error_led.Set();
            while (button1_.Pressed());
            error_led.Clear();
            write_led.Clear();
            packet_led.Clear();

            block_address = kAppStartAddress;
            decoder_.Reset();
        }
    }
}

}
