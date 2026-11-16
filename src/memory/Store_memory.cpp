#include "AI/Loading_model.h"
#include "llama/llama.h"
#include "llama/llama-cpp.h"
#include <vector>
#include <iostream>

using namespace std;
bool LModel::Store_memory(const string& File_path, int seq_id, const vector<llama_token>& tokens) {
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