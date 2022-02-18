#include "drivers/codec.h"

#include "stm32f4xx_hal.h"
#include "drivers/i2c.h"

namespace bl
{

enum CodecRegister
{
    CODEC_REG_R0_CLKC      = 0x4000,
    CODEC_REG_R1_PLLC      = 0x4002,
    CODEC_REG_R2_DMICJ     = 0x4008,
    CODEC_REG_R3_RES       = 0x4009,
    CODEC_REG_R4_RMIXL0    = 0x400A,
    CODEC_REG_R5_RMIXL1    = 0x400B,
    CODEC_REG_R6_RMIXR0    = 0x400C,
    CODEC_REG_R7_RMIXR1    = 0x400D,
    CODEC_REG_R8_LDIVOL    = 0x400E,
    CODEC_REG_R9_RDIVOL    = 0x400F,
    CODEC_REG_R10_MICBIAS  = 0x4010,
    CODEC_REG_R11_ALC0     = 0x4011,
    CODEC_REG_R12_ALC1     = 0x4012,
    CODEC_REG_R13_ALC2     = 0x4013,
    CODEC_REG_R14_ALC3     = 0x4014,
    CODEC_REG_R15_SERP0    = 0x4015,
    CODEC_REG_R16_SERP1    = 0x4016,
    CODEC_REG_R17_CON0     = 0x4017,
    CODEC_REG_R18_CON1     = 0x4018,
    CODEC_REG_R19_ADCC     = 0x4019,
    CODEC_REG_R20_LDVOL    = 0x401A,
    CODEC_REG_R21_RDVOL    = 0x401B,
    CODEC_REG_R22_PMIXL0   = 0x401C,
    CODEC_REG_R23_PMIXL1   = 0x401D,
    CODEC_REG_R24_PMIXR0   = 0x401E,
    CODEC_REG_R25_PMIXR1   = 0x401F,
    CODEC_REG_R26_PLRML    = 0x4020,
    CODEC_REG_R27_PLRMR    = 0x4021,
    CODEC_REG_R28_PLRMM    = 0x4022,
    CODEC_REG_R29_PHPLVOL  = 0x4023,
    CODEC_REG_R30_PHPRVOL  = 0x4024,
    CODEC_REG_R31_PLLVOL   = 0x4025,
    CODEC_REG_R32_PLRVOL   = 0x4026,
    CODEC_REG_R33_PMONO    = 0x4027,
    CODEC_REG_R34_POPCLICK = 0x4028,
    CODEC_REG_R35_PWRMGMT  = 0x4029,
    CODEC_REG_R36_DACC0    = 0x402A,
    CODEC_REG_R37_DACC1    = 0x402B,
    CODEC_REG_R38_DACC2    = 0x402C,
    CODEC_REG_R39_SERPP    = 0x402D,
    CODEC_REG_R40_CPORTP0  = 0x402F,
    CODEC_REG_R41_CPORTP1  = 0x4030,
    CODEC_REG_R42_JACKDETP = 0x4031,
    CODEC_REG_R67_DJITTER  = 0x4036,
    CODEC_REG_R65_CLKEN0   = 0x40F9,
    CODEC_REG_R66_CLKEN1   = 0x40FA,
    CODEC_REG_R58_SIRC     = 0x40F2,
    CODEC_REG_R59_SORC     = 0x40F3,
};

struct CodecRegisterInit
{
    CodecRegister address;
    uint8_t value;
};

static constexpr CodecRegisterInit kCodecInitValues[] =
{
    {CODEC_REG_R0_CLKC, 0x09},
    {CODEC_REG_R2_DMICJ, 0x00},
    {CODEC_REG_R3_RES, 0x00},
    {CODEC_REG_R4_RMIXL0, 0x00},
    {CODEC_REG_R5_RMIXL1, 0x00},
    {CODEC_REG_R6_RMIXR0, 0x00},
    {CODEC_REG_R7_RMIXR1, 0x00},
    {CODEC_REG_R8_LDIVOL, 0x00},
    {CODEC_REG_R9_RDIVOL, 0x00},
    {CODEC_REG_R10_MICBIAS, 0x00},
    {CODEC_REG_R11_ALC0, 0x00},
    {CODEC_REG_R12_ALC1, 0x00},
    {CODEC_REG_R13_ALC2, 0x00},
    {CODEC_REG_R14_ALC3, 0x00},
    {CODEC_REG_R15_SERP0, 0x01},
    {CODEC_REG_R16_SERP1, 0x00},
    {CODEC_REG_R17_CON0, 0x00},
    {CODEC_REG_R18_CON1, 0x00},
    {CODEC_REG_R19_ADCC, 0x10},
    {CODEC_REG_R20_LDVOL, 0x00},
    {CODEC_REG_R21_RDVOL, 0x00},
    {CODEC_REG_R22_PMIXL0, 0x00},
    {CODEC_REG_R23_PMIXL1, 0x00},
    {CODEC_REG_R24_PMIXR0, 0x00},
    {CODEC_REG_R25_PMIXR1, 0x00},
    {CODEC_REG_R26_PLRML, 0x00},
    {CODEC_REG_R27_PLRMR, 0x00},
    {CODEC_REG_R28_PLRMM, 0x00},
    {CODEC_REG_R29_PHPLVOL, 0x02},
    {CODEC_REG_R30_PHPRVOL, 0x02},
    {CODEC_REG_R31_PLLVOL, 0x02},
    {CODEC_REG_R32_PLRVOL, 0x02},
    {CODEC_REG_R33_PMONO, 0x02},
    {CODEC_REG_R34_POPCLICK, 0x00},
    {CODEC_REG_R35_PWRMGMT, 0x00},
    {CODEC_REG_R36_DACC0, 0x00},
    {CODEC_REG_R37_DACC1, 0x00},
    {CODEC_REG_R38_DACC2, 0x00},
    {CODEC_REG_R39_SERPP, 0x00},
    {CODEC_REG_R40_CPORTP0, 0xAA},
    {CODEC_REG_R41_CPORTP1, 0xAA},
    {CODEC_REG_R42_JACKDETP, 0x00},
    {CODEC_REG_R65_CLKEN0, 0x7F},
    {CODEC_REG_R66_CLKEN1, 0x03},
    {CODEC_REG_R58_SIRC, 0x01},
    {CODEC_REG_R59_SORC, 0x01},
    {CODEC_REG_R19_ADCC, 0x03},
    {CODEC_REG_R36_DACC0, 0x03},
    {CODEC_REG_R31_PLLVOL,(58<<2)+2},
    {CODEC_REG_R32_PLRVOL,(58<<2)+2},
    {CODEC_REG_R26_PLRML, (1<<1)+1},
    {CODEC_REG_R27_PLRMR, (1<<3)+1},
    {CODEC_REG_R22_PMIXL0, 0x21},
    {CODEC_REG_R24_PMIXR0, 0x41},
    {CODEC_REG_R35_PWRMGMT, 0x03},
    {CODEC_REG_R4_RMIXL0, 0x01},
    {CODEC_REG_R5_RMIXL1, 0x08},
    {CODEC_REG_R6_RMIXR0, 0x01},
    {CODEC_REG_R7_RMIXR1, 0x08},
    {CODEC_REG_R8_LDIVOL, (16<<2)+3},
    {CODEC_REG_R9_RDIVOL, (16<<2)+3},
    {CODEC_REG_R33_PMONO, 0x03},
    {CODEC_REG_R28_PLRMM, 0x01},
    {CODEC_REG_R29_PHPLVOL, 0xC3},
    {CODEC_REG_R30_PHPRVOL, 0xC3},
};

static I2CModule i2c_;
static constexpr uint8_t kCodecAddress = 0x70;
static Codec::Callback callback_;

static void InitGPIO(void)
{
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init =
    {
        .Pin       = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6,
        .Mode      = GPIO_MODE_AF_PP,
        .Pull      = GPIO_NOPULL,
        .Speed     = GPIO_SPEED_FREQ_MEDIUM,
        .Alternate = GPIO_AF6_SAI1,
    };

    HAL_GPIO_Init(GPIOE, &gpio_init);
}

static void InitSAI(void)
{
    __HAL_RCC_SAI1_CLK_ENABLE();

    uint32_t frame_length = 64;
    uint32_t sync_length = 32;
    uint32_t slot_en = SAI_SLOTACTIVE_0;
    uint32_t num_slots = 1;

    uint32_t frcr = ((frame_length - 1) << SAI_xFRCR_FRL_Pos)
        | ((sync_length - 1) << SAI_xFRCR_FSALL_Pos)
        | SAI_xFRCR_FSOFF;

    uint32_t slotr = (slot_en << SAI_xSLOTR_SLOTEN_Pos)
        | ((num_slots - 1) << SAI_xSLOTR_NBSLOT_Pos);

    SAI1_Block_A->CR2 = SAI_FIFOTHRESHOLD_HF;
    SAI1_Block_B->CR2 = SAI_FIFOTHRESHOLD_HF;
    SAI1_Block_A->FRCR = frcr;
    SAI1_Block_B->FRCR = frcr;
    SAI1_Block_A->SLOTR = slotr;
    SAI1_Block_B->SLOTR = slotr;

    uint32_t cr1 = SAI_DATASIZE_32 | SAI_xCR1_CKSTR;
    SAI1_Block_A->CR1 = cr1 | SAI_MODESLAVE_TX;
    SAI1_Block_B->CR1 = cr1 | SAI_MODESLAVE_RX | SAI_xCR1_SYNCEN_0;

    SAI1_Block_A->IMR = SAI_xIMR_FREQIE;
    NVIC_EnableIRQ(SAI1_IRQn);
}

void Codec::Start(void)
{
    SAI1_Block_A->CR1 |= SAI_xCR1_SAIEN;
    SAI1_Block_B->CR1 |= SAI_xCR1_SAIEN;
}

void Codec::Stop(void)
{
    SAI1_Block_A->CR1 &= ~SAI_xCR1_SAIEN;
    SAI1_Block_B->CR1 &= ~SAI_xCR1_SAIEN;
}

void Codec::Init(Callback callback)
{
    callback_ = callback;

    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
    InitGPIO();
    InitSAI();
    i2c_.Init();

    i2c_.Write(kCodecAddress, CODEC_REG_R1_PLLC,
        {0x00, 0x7D, 0x00, 0x12, 0x31, 0x01});

    for (;;)
    {
        // Wait for PLL
        HAL_Delay(1);

        uint8_t buf[6];
        bool success = i2c_.Read(kCodecAddress, CODEC_REG_R1_PLLC, buf, 6);

        if (success && (buf[5] & 0x02))
        {
            break;
        }
    }

    for (auto& v : kCodecInitValues)
    {
        i2c_.Write(kCodecAddress, v.address, v.value);
    }
}

extern "C"
void SAI1_IRQHandler(void)
{
    while (SAI1_Block_A->SR & SAI_xSR_FREQ)
    {
        int32_t in = SAI1_Block_B->DR;
        SAI1_Block_A->DR = in;
        callback_(SAI1_Block_B->DR);
    }
}

}
