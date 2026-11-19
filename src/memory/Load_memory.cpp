#include "AI/Loading_model.h"
#include "AI/Readfilesize.h"
#include <iostream>
#include <vector>
#include "llama/llama.h"

using namespace std;
bool LModel::Load_L2memory(const vector<uint8_t>& kv_data, int seq_id) {
    if (!ctx || kv_data.empty()) return false;

    // 把 KV 给回显存上下文
    size_t loaded = llama_state_seq_set_data(
        ctx.get(),
        kv_data.data(),
        kv_data.size(),
        seq_id
    );

    if (loaded > 0) {
        cout << "Successfully injected" << (loaded / 1024 / 1024) << "MB of memory into VRAM!" << endl;
        return true;
    }
    else {
        cout << "Error: Memory injection failed!" << endl;
        return false;
    }
}
vector<llama_token> LModel::Load_L3memory(const string& File_path, int dest_seq_id,int Quantity_limit) {
    vector<llama_token> loaded_tokens;
    if (Quantity_limit<0) {
        cout << "Error: Invalid Quantity_limit." << endl;
        return loaded_tokens;
    }
    // 检测是否拥有上下文
    if (!ctx) {
        cout << "Error: Context does not exist." << endl;
        return loaded_tokens;
    }

    size_t max_capacity = Quantity_limit;
    loaded_tokens.resize(max_capacity);
    size_t n_token_count_out = 0;

    size_t Memory = llama_state_seq_load_file(
        ctx.get(),               // 上下文指针
        File_path.c_str(),       // 读取路径
        dest_seq_id,             // 目标序列（比如主脑是 0）
        loaded_tokens.data(),    // token 接收数组
        max_capacity,            // 接收最大容量
        &n_token_count_out       // 实际读到了多少个 token
    );
    //检测记忆是否为0，并返回token
    if (Memory > 0) {
        loaded_tokens.resize(n_token_count_out);

        cout << "Tip: Memory loaded successfully." << endl;
        cout << "Size: " << (Memory / 1024 / 1024) << " MB" << endl;
        cout << "Restored tokens: " << n_token_count_out << endl;

        return loaded_tokens;
    }
    else {
        cout << "Error: Load failed. The file may not exist or is corrupted." << endl;
        cout << "Error: Load failed." << endl;
        cout << "Memory returned: " << Memory << endl;
        cout << "Path: " << File_path << endl;
        cout << "Seq ID: " << dest_seq_id << endl;
        loaded_tokens.clear();
        return loaded_tokens;
    }
}