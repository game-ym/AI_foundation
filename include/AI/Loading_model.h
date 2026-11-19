#pragma once
#include "llama/llama.h"
#include <string>
#include <atomic>
#include <chrono>
#include <cstdint>
#include "AI/AIConfig.h"
#include <vector>
#include "llama/llama-cpp.h"
class LModel {
    private:
        // 创建随机参数
        static uint32_t Random_seed() {
            static std::atomic<uint64_t> counter{ 0 };
            uint64_t t = static_cast<uint64_t>(
                std::chrono::steady_clock::now().time_since_epoch().count());
            uint64_t c = counter.fetch_add(1, std::memory_order_relaxed);

            uint64_t x = t ^ (c * 0x9E3779B97F4A7C15ULL); 
            x ^= x >> 33;
            x *= 0xff51afd7ed558ccdULL;
            x ^= x >> 33;

            uint32_t s = static_cast<uint32_t>(x);
            return s ? s : 1u;   
        }
        llama_model_ptr model;
        llama_context_ptr ctx;
        const llama_vocab* vocab;
    public:
        bool IsLoaded() const { return model != nullptr; }
        bool is_ctx_alive = false;
        // L2 记忆存储
        std::vector<uint8_t>Store_L2memory(int seq_id);
        // 读取 L2 记忆文件
        bool Load_L2memory(const std::vector<uint8_t>& kv_data,int seq_id);
        // 加载模型
        void Loading_model(std::string& my_ais,std::string name);
        // L3 记忆存储
        bool Store_L3memory(const std::string& File_path,int seq_id,const std::vector<llama_token>&tokens);
        // 读取 L3 记忆文件
        std::vector<llama_token> Load_L3memory(const std::string& File_path, int seq_id,int Quantity_limit);
        // 思考
        std::string Reasoning(std::vector<llama_token>& tokens, int max_new_tokens, float Modeltemperature,bool keep_ctx=false);
        // 卸载模型
        void Uninstall_model();
        std::vector<llama_token> Dialogue(const std::string& Phrase);
};