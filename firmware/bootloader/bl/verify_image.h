#pragma once

#include <cstdint>

#include "qpsk/inc/crc32.h"

namespace bl
{

class Crc32 : public qpsk::Crc32
{
public:
    void Init(void)
    {
        // This gets called before the data and bss segments are initialized,
        // so `initialized_` is in an unknown state. Zero it to force the
        // superclass to initialize its CRC table.
        initialized_ = false;
        qpsk::Crc32::Init();
    }
};

inline bool VerifyImage(uint32_t address, uint32_t max_length)
{
    uint32_t offset;
    uint32_t image_length = 0;
    uint32_t image_crc = 0;
    uint32_t image_addr = 0;

    // Find the image signature.
    constexpr uint32_t kMaxOffsetWords = 257;
    auto image = reinterpret_cast<uint32_t*>(address);

    for (offset = 0; offset <= kMaxOffsetWords; offset++)
    {
        uint32_t word0 = image[offset];
        uint32_t word1 = image[offset + 1];

        if (word0 == 0xFF01FF02 && word1 == 0xFF03FF04)
        {
            image_length  = image[offset + 2];
            image_crc     = image[offset + 3];
            image_addr    = image[offset + 4];
            break;
        }
    }

    if ((offset > kMaxOffsetWords) || (image_addr != address) ||
        (image_length == 0) || (image_length > max_length))
    {
        return false;
    }

    Crc32 crc;
    crc.Init();
    crc.Seed(0);
    auto bytes = reinterpret_cast<const uint8_t*>(image);
    // Skip the checksum word when calculating the checksum
    uint32_t length1 = 4 * (offset + 3);
    uint32_t length2 = image_length - length1 - 4;
    crc.Process(bytes,               length1);
    crc.Process(bytes + length1 + 4, length2);

    uint32_t computed_crc = crc.crc();
    return computed_crc == image_crc;
}

}
