#pragma once
#include <string>
#include "AI/AIConfig.h"
#include <vector>
#include "AI/Loading_model.h"
// 动态模型加载
class Modelselector {
    public:
        static void Startmodel(LModel& AI,std::vector<AIConfig>& my_ais,std::vector<LModel*>& sacrifice_list,const std::string& name);
};