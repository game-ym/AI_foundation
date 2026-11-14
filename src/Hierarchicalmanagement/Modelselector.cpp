#include "AI/Modelselector.h"
#include "AI/Loading_model.h"
#include "AI/VMmanager.h"
#include "AI/Readfilesize.h"
#include <vector>
#include <iostream>
#include "AI/AIConfig.h"

using namespace std;
void Modelselector::Startmodel(LModel& AI,vector<AIConfig>& my_ais,vector<LModel*>& sacrifice_list,const string& name) {
    if (AI.IsLoaded()) {
        cout << "Hint: model " << name << " Already resident in video memory, it can be reused directly, eliminating loading time." << endl;
        return;
    }
    bool AI_i = true;
    for (int i = 0; i < my_ais.size();i++) {
        if (my_ais[i].ai_name == name) {
            AI_i = false;
            size_t AIfile = GetFileSizeMB(my_ais[i].model_path);
            bool GPUMG = VMManager::HasEnoughVRAM(AIfile);
            if (GPUMG) {
                AI.Loading_model(my_ais[i].model_path, name);
            }
            if (!GPUMG) {
                cout << "Error: Insufficient video memory" << endl;
                bool AI_o = true;
                for (int o = 0; o < sacrifice_list.size();o++) {
                    if (sacrifice_list[o] == &AI) {
                        continue;
                    }
                    sacrifice_list[o]->Uninstall_model();
                    GPUMG = VMManager::HasEnoughVRAM(AIfile);
                    if (GPUMG) {
                        AI.Loading_model(my_ais[i].model_path, name);
                        AI_o = false;
                        cout << "VRAM has been reset, loading complete." << endl;
                        break;
                    }
                    if(!GPUMG) {
                        continue;
                    }
                }
                if (AI_o) {
                    cout << "Your video memory is too full." << endl;
                }
            }
            break;
        }
    }
    if (AI_i) {
        cout << "Error: NO Model Name" << endl;
    }
}