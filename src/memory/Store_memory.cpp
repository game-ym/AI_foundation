#include "AI/Loading_model.h"
#include "llama/llama.h"
#include "llama/llama-cpp.h"
#include <vector>
#include <iostream>

using namespace std;
vector<uint8_t> LModel::Store_L2memory(int seq_id) {
    vector<uint8_t> L2memory;
    if (!ctx) return L2memory;
    // 计算记忆的大小
    size_t size = llama_state_seq_get_size(ctx.get(), seq_id);
    if (size == 0) {
        cout << "Error: The current context is empty, cannot extract!" << endl;
        return L2memory;
    }
    // 给L2memory申请足够大的内存
    L2memory.resize(size);
    size_t written = llama_state_seq_get_data(
        ctx.get(),
        L2memory.data(),
        size,
        seq_id
    );
    if (written == 0) {
        cout << "Error: Failed to extract context tensor!" << endl;
        L2memory.clear();
    }
    else {
        cout << "Successfully extracted the context, size:" << (written / 1024 / 1024) << " MB" << endl;
    }
    return L2memory;
}
bool LModel::Store_L3memory(const string& File_path, int seq_id, const vector<llama_token>& tokens) {
    if (!ctx) {
        cout << "Error: Context does not exist." << endl;
        return false;
    }
    size_t Memory = llama_state_seq_save_file(
        ctx.get(),            // 上下文指针
        File_path.c_str(),     // 记忆存储路径
        seq_id,               // 要保存哪个序列（主脑就是 seq 0）
        tokens.data(),        // token 数组
        tokens.size()         // token 数量
    );
    if (Memory > 0) {
        cout << "Tip: Memory saved successfully." << endl << "Size:" << (Memory / 1024 / 1024) << endl;
        return true;
    }
    else {
        cout << "Error: Save failed; the likely cause is insufficient access permissions for the file path." << endl;
        return false;
    }
}