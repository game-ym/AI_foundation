#include "AI/Loading_model.h"
#include "AI/AIConfig.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "AI/VMmanager.h"
#include "llama/llama.h"
#include "llama/llama-cpp.h"
using namespace std;
void LModel::Loading_model(string& model_path, string name) {
    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 99;
    if (model != nullptr) {
        throw runtime_error("Error: Model is already loaded! Uninstall it first.");
    }
    model = llama_model_ptr(llama_model_load_from_file(model_path.c_str(), model_params));
    if (!model) {
        throw runtime_error("Error: Model loading failed");
    }
    int model_max_ctx = llama_model_n_ctx_train(model.get());
    if (model_max_ctx <= 0) model_max_ctx = 4096;
    int n_layers = llama_model_n_layer(model.get());
    int n_heads = llama_model_n_head(model.get());   
    int n_kv_heads = llama_model_n_head_kv(model.get());
    int n_embd = llama_model_n_embd(model.get());
    int head_dim = n_embd / n_heads;                   
    if (head_dim <= 0) head_dim = 128;                   
    size_t kv_cache_per_token = 2 * n_layers * n_kv_heads * head_dim * 2;
    
    size_t free_vram_bytes = VMManager::GetFreeVRAM_MB() * 1024ULL * 1024ULL;
    size_t safety_buffer = 1024 * 1024ULL * 1024ULL; // 预留 1G 安全区
    size_t available_for_cache = (free_vram_bytes > safety_buffer) ? (free_vram_bytes - safety_buffer) : 0;

    //计算显存允许的 Token 上限
    int max_ctx_by_vram = available_for_cache / kv_cache_per_token;

    //取交集，并设置硬底线
    int dynamic_ctx = std::min(max_ctx_by_vram, model_max_ctx);
    //判断最终结果低于512,输出错误。
    if (dynamic_ctx < 512) {
        model.reset();
        throw std::runtime_error(
            "Insufficient VRAM for minimal context (512). free="
            + std::to_string(free_vram_bytes / 1024 / 1024) + "MB");
    }
    cout << "[引擎] 模型上限: " << model_max_ctx
        << " | 显存允许: " << max_ctx_by_vram
        << " | 最终分配 n_ctx: " << dynamic_ctx << endl;
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = dynamic_ctx;

    const int MIN_BATCH = 64;
    int try_batch = std::min(dynamic_ctx, 512);

    llama_context_ptr new_ctx;
    while (true) {
        ctx_params.n_batch = try_batch;
        ctx_params.n_ubatch = try_batch;

        new_ctx = llama_context_ptr(llama_init_from_model(model.get(), ctx_params));
        if (new_ctx) break;                            

        if (try_batch <= MIN_BATCH) {          
            model.reset();
            throw std::runtime_error(
                "Error: Context creation failed even at n_batch="
                + std::to_string(MIN_BATCH));
        }

        std::cout << "[引擎] n_batch=" << try_batch
            << " 建 context 失败，降级到 " << (try_batch / 2) << " 重试\n";
        try_batch /= 2;
    }

    ctx = std::move(new_ctx);
    std::cout << "[引擎] 锁定 n_batch=" << llama_n_batch(ctx.get()) << "\n";
    vocab = llama_model_get_vocab(model.get());
}
void LModel::Uninstall_model() {
    ctx.reset();
    model.reset();
    vocab = nullptr;
    is_ctx_alive = false;
}