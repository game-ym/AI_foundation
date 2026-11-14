#pragma once
#include <string>
#include <vector>

struct AIConfig {
    std::string ai_name;
    std::string model_path;
    std::string ai_type;
};
void AIname(std::vector<AIConfig>& my_ais);