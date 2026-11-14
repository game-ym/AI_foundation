#pragma once
#include <cstddef>
#include "ggml/ggml-backend.h"

class VMManager {
public:
    static size_t GetFreeVRAM_MB();
    static bool HasEnoughVRAM(size_t required_mb);
};