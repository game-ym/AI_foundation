#pragma once
#include <cstddef>
#include "ggml/ggml-backend.h"

// 判断显存
class VMManager {
public:
    // 获取可用显存大小
    static size_t GetFreeVRAM_MB();
    // 判断显存是否够用
    static bool HasEnoughVRAM(size_t required_mb);
};