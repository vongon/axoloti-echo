#pragma once

#include <cstdint>
#include <optional>
#include <initializer_list>

namespace bl
{

class I2CModule
{
public:
    void Init(void);
    bool Write(uint8_t device_addr, uint16_t register_addr, const uint8_t* data,
        uint32_t length);
    bool Read(uint8_t device_addr, uint16_t register_addr, uint8_t* data,
        uint32_t length);

    bool Write(uint8_t device_addr, uint16_t register_addr,
        std::initializer_list<uint8_t> data)
    {
        return Write(device_addr, register_addr, data.begin(), data.size());
    }

    std::optional<uint8_t> Read(uint8_t device_addr, uint16_t register_addr)
    {
        uint8_t data;

        if (Read(device_addr, register_addr, &data, 1))
        {
            return data;
        }
        else
        {
            return std::nullopt;
        }
    }

    bool Write(uint8_t device_addr, uint16_t register_addr, uint8_t data)
    {
        Write(device_addr, register_addr, &data, 1);
        return data;
    }
};

}
