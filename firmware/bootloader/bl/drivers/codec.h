#pragma once

#include <cstdint>

namespace bl
{

class Codec
{
public:
    using Callback = void (*)(int32_t sample);
    void Init(Callback callback);
    void Start(void);
    void Stop(void);
};
    
}
