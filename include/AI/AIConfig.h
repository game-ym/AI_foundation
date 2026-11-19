#pragma once
#include <string>
#include <vector>

struct AIConfig {
    std::string ai_name;// 模型名字
    std::string model_path;// 模型路径
    std::string ai_type;// 模型类型
};
void AIname(std::vector<AIConfig>& my_ais);// 获取模型参数