#include "AI/Loading_model.h"
#include <iostream>
#include "AI/VMmanager.h"
#include <vector>

using namespace std;
string LModel::Reasoning(vector<llama_token>& tokens,int max_new_tokens,float Modeltemperature, bool keep_ctx) {
    if (tokens.empty()) {
        throw std::invalid_argument("Reasoning: empty prompt");
    }
    const int n_ctx = static_cast<int>(llama_n_ctx(ctx.get()));
    const int SAFE_MARGIN = 10;
    if (static_cast<int>(tokens.size()) + SAFE_MARGIN >= n_ctx) {
        throw std::length_error(
            "Reasoning: prompt(" + std::to_string(tokens.size())
            + ") + margin(" + std::to_string(SAFE_MARGIN)
            + ") >= n_ctx(" + std::to_string(n_ctx) + ")");
    }
    if (max_new_tokens <= 0) {
        throw std::invalid_argument("Reasoning: max_new_tokens must be > 0");
    }

    int available_space = n_ctx - static_cast<int>(tokens.size());
    if (max_new_tokens > available_space) {
        max_new_tokens = available_space;
    }
    if (!keep_ctx) {
        llama_memory_clear(llama_get_memory(ctx.get()), true);
    }
    const int n_batch = static_cast<int>(llama_n_batch(ctx.get()));
    if (n_batch <= 0) {
        throw std::runtime_error("Reasoning: invalid n_batch");
    }
    for (size_t pos = 0; pos < tokens.size(); pos += static_cast<size_t>(n_batch)) {
        size_t chunk = std::min<size_t>(static_cast<size_t>(n_batch), tokens.size() - pos);
        llama_batch batch = llama_batch_get_one(tokens.data() + pos, static_cast<int32_t>(chunk));
        if (llama_decode(ctx.get(), batch) != 0) {
            throw std::runtime_error(
                "Prefill failed at pos=" + std::to_string(pos)
                + ", chunk=" + std::to_string(chunk)
                + ", total=" + std::to_string(tokens.size()));
        }
    }
    auto sparams = llama_sampler_chain_default_params();

    llama_sampler_ptr smpl(llama_sampler_chain_init(sparams));
    if (Modeltemperature <= 0.0f) {
        llama_sampler_chain_add(smpl.get(), llama_sampler_init_greedy());
    }
    else {
        llama_sampler_chain_add(smpl.get(), llama_sampler_init_penalties(llama_n_vocab(vocab), -1, 1.15f, 0.0f, 0.0f));
        llama_sampler_chain_add(smpl.get(), llama_sampler_init_top_k(40));
        llama_sampler_chain_add(smpl.get(), llama_sampler_init_top_p(0.9f, 1));
        llama_sampler_chain_add(smpl.get(), llama_sampler_init_temp(Modeltemperature));
        llama_sampler_chain_add(smpl.get(), llama_sampler_init_dist(Random_seed()));
    }
    string result_text = "";
    int n_generated = 0;
    int consecutive_qmarks = 0;
    while (n_generated < max_new_tokens) {
        llama_token new_token = llama_sampler_sample(smpl.get(), ctx.get(), -1);

        if (llama_vocab_is_eog(vocab, new_token)) {
            break;
        }
        char buf[512];
        int n = llama_token_to_piece(vocab, new_token, buf, sizeof(buf), 0, true);
        if (n > 0) {
            if (buf[0] == '?') {
                consecutive_qmarks++;
                if (consecutive_qmarks >= 5) {
                    cout << "\n[警告] 检测到连续问号死循环，强行中断生成！\n";
                    break; 
                }
            }
            else {
                consecutive_qmarks = 0; 
            }
            result_text.append(buf, n);
        }
        llama_batch next_batch = llama_batch_get_one(&new_token, 1);
        if (llama_decode(ctx.get(), next_batch) != 0) {
            if (n > 0) result_text.resize(result_text.size() - n);
            break;
        }
        n_generated++;
    }
    return result_text;
}