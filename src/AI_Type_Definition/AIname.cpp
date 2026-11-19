#include <iostream>
#include <fstream>
#include <vector>
#include "nlohmann/json.hpp" 
#include "AI/AIConfig.h"
// 重新命名json
using json = nlohmann::json;
using namespace std;
// 从json中获取AI信息
void AIname(vector<AIConfig>&my_ais)
{
    string AIcfgfile = "E:/AI_foundation/config.json";// json文件路径
    ifstream file(AIcfgfile);
    if(!file.is_open()){
        cout << "Error: No file found!" << endl;
        return;
    }
    json AIdatd;
    file >> AIdatd;
    file.close();
    // for传入AI信息
    for (const auto& ai_json : AIdatd["ais"]) {
        AIConfig cfg;
        cfg.ai_name = ai_json["name"].get<string>();
        cfg.model_path = ai_json["path"].get<string>();
        cfg.ai_type = ai_json["type"].get<string>();

        my_ais.push_back(cfg);
    }

}