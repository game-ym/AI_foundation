#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include "nlohmann/json.hpp" 
#include "AI/AIConfig.h"

using json = nlohmann::json;
using namespace std;
void AIname(vector<AIConfig>&my_ais)
{
    string AIcfgfile = "E:/AI_foundation/config.json";
    ifstream file(AIcfgfile);
    if(!file.is_open()){
        cout << "Error: No file found!" << endl;
        return;
    }
    json AIdatd;
    file >> AIdatd;
    file.close();
    
    for (const auto& ai_json : AIdatd["ais"]) {
        AIConfig cfg;
        cfg.ai_name = ai_json["name"].get<string>();
        cfg.model_path = ai_json["path"].get<string>();
        cfg.ai_type = ai_json["type"].get<string>();

        my_ais.push_back(cfg);
    }

}