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
        void Loading_model(std::string& my_ais,std::string name);
        bool Store_memory(const std::string& File_path,int sep_id,const std::vector<llama_token>&tokens);
        std::vector<llama_token> Load_memory(const std::string& File_path, int dest_seq_id,int Quantity_limit);
        std::string Reasoning(std::vector<llama_token>& tokens, int max_new_tokens, float Modeltemperature,bool keep_ctx=false);
        void Uninstall_model();
        std::vector<llama_token> Dialogue(const std::string& Phrase);
};