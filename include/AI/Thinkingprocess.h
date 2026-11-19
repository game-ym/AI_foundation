#pragma once
#include "AI/Modelselector.h"
#include "AI/Loading_model.h"
#include "AI/AIConfig.h"
#include <vector>

struct Status
{
    bool is_passed;          
    int total_rounds;       
    std::string final_Think;
    std::string final_Reflection; 
    std::string error_msg;
};
// 思考和反思的逻辑实现
class Thinkingprocess {
public:
    static Status Run(LModel& thinker,LModel& critic,std::vector<AIConfig>& my_ais, std::vector<LModel*>& sacrifice_list, const std::string& task_prompt,int max_retries);
};