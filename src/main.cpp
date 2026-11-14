#include <iostream>
#include "AI/AIConfig.h"
#include <windows.h>
#include <vector>
#include "AI/Readfilesize.h"
#include <fstream>
#include "nlohmann/json.hpp" 
#include "llama/llama.h"
#include "AI/Loading_model.h"
#include "AI/Modelselector.h"
#include "AI/Thinkingprocess.h"
#include "llama/llama-cpp.h"

using namespace std;

int main() {
    // 1. 初始化
    SetConsoleOutputCP(CP_UTF8);
    SetDllDirectoryW(L"dlls");
    llama_backend_init();

   
    try {
        vector<AIConfig> my_ais;
        AIname(my_ais); // 读取配置

        if (my_ais.empty()) {
            throw runtime_error("读取 config.json 失败，或者配置为空！请检查工作目录。");
        }

        // 2. 声明三体与牺牲名单
        LModel AI_Main, AI_Thinker, AI_Critic;
        // 牺牲顺序：优先杀 Critic，然后 Thinker，最后 Main
        vector<LModel*> sacrifice_list = { &AI_Critic, &AI_Thinker, &AI_Main };

        // 用户需求
        string user_input = "帮我写个泡沫循环排序";

        string main_output = "";

        // ==========================================
        // 【阶段一】唤醒主脑 (Main) —— 负责任务拆解
        // ==========================================
        cout << "[系统] 正在加载主脑..." << endl;

        // 【核心修复 2：补回主脑的加载代码】
        Modelselector::Startmodel(AI_Main, my_ais, sacrifice_list, "Main");

        if (!AI_Main.is_ctx_alive) {
            string full_prompt = "<|im_start|>system\n你是一个项目经理。请把用户的需求包装成给程序员的清晰的目标，只允许发送指令，不允许生成代码，如果你输出代码的话你就会被解雇。请遵守以上规则<|im_end|>\n"
                "<|im_start|>user\n" + user_input + "<|im_end|>\n<|im_start|>assistant\n";  
            vector<llama_token> main_tokens = AI_Main.Dialogue(full_prompt);
            // 告诉底层：这次不要清空，喂完了标记为“脑子活着”
            main_output = AI_Main.Reasoning(main_tokens, 512, 0.7f, false);
            AI_Main.is_ctx_alive = true;
        }
        else {
            string full_prompt = "<|im_start|>user\n" + user_input + "<|im_end|>\n<|im_start|>assistant\n";
            vector<llama_token> main_tokens = AI_Main.Dialogue(full_prompt);
            main_output = AI_Main.Reasoning(main_tokens, 512, 0.7f, true);

        }
        cout << "[主脑] 任务拆解完成。" << endl;
        cout << "AI1:" << main_output << endl;

        // ==========================================
        // 【阶段二 & 三】思考与反思闭环
        // ==========================================
        cout << "\n[系统] 正在启动思考闭环..." << endl;

        // 传参：思考模型、审查模型、配置、名单、主脑的指令
        Status final_result = Thinkingprocess::Run(AI_Thinker, AI_Critic, my_ais, sacrifice_list, main_output, 3);

        string final_presentation = "";

        // ==========================================
        // 【阶段四】主脑重新接管，进行最终包装
        // ==========================================
        if (!final_result.error_msg.empty()) {
            final_presentation = "[系统错误] 思考过程发生异常：" + final_result.error_msg;
            cout << "\n" << final_presentation << endl;
        }
        else if (!final_result.is_passed) {
            final_presentation = "抱歉，代码在重试 3 轮后仍未通过审查，请查看底层的最后一份代码和审查意见。";
            cout << "\n" << final_presentation << endl;
        }
        else {
            cout << "\n[系统] 正在唤醒主脑进行最终交付包装..." << endl;
            // 注意：这一步会触发 Modelselector 把 Critic 踢掉，重新加载 Main 模型！
            Modelselector::Startmodel(AI_Main, my_ais, sacrifice_list, "Main");

            if (AI_Main.is_ctx_alive) {
                // 主脑脑子还在！只需要喂最终代码
                string delta_prompt = "<|im_start|>user\n程序员写好了：\n" + final_result.final_Think + "\n请向用户汇报。<|im_end|>\n<|im_start|>assistant\n";
                vector<llama_token> wrap_tokens = AI_Main.Dialogue(delta_prompt);
                final_presentation = AI_Main.Reasoning(wrap_tokens, 512, 0.6f, true); // 保留上下文
            }
            else {
                // 主脑被踢过，脑子空了，重新喂全量
                string full_wrap = "<|im_start|>system\n你是一个项目经理...<|im_end|>\n"
                    "<|im_start|>user\n" + user_input + "<|im_end|>\n"
                    "<|im_start|>user\n程序员写好了：\n" + final_result.final_Think + "\n请向用户汇报。<|im_end|>\n<|im_start|>assistant\n";
                vector<llama_token> wrap_tokens = AI_Main.Dialogue(full_wrap);
                final_presentation = AI_Main.Reasoning(wrap_tokens, 512, 0.6f, false);
            }
            cout << "\n[主脑] 最终回复包装完成。" << endl;
            cout << "================ 最终呈现给用户的回复 ================\n" << final_presentation << "\n======================================================" << endl;
            cout << "================ 源代码 ================\n" << final_result.final_Think << "\n======================================================" << endl;
        }

        // ==========================================
        // 【最终展示】全部写进文件
        // ==========================================
        ofstream out("E:/AI_foundation/final_result.txt");
        out << "===== 用户的原始需求 =====\n" << user_input << "\n\n";
        out << "===== 【阶段一：主脑拆解】 =====\n" << main_output << "\n\n";

        if (!final_result.error_msg.empty() || !final_result.is_passed) {
            out << "===== 【异常/失败记录】 =====\n" << final_presentation << "\n\n";
            out << "===== 【最后一次代码与反思】 =====\n" << final_result.final_Think << "\n\n审查意见：\n" << final_result.final_Reflection << "\n";
        }
        else {
            out << "思考轮次:" << final_result.total_rounds << endl;
            out << "===== 【最终交付给用户的回复（主脑包装版）】 =====\n" << final_presentation << "\n\n";
            out << "===== 【底层经过"<<final_result.total_rounds<<"轮审查的纯净代码】 =====\n" << final_result.final_Think << "\n";
        }
        out.close();

        cout << "完成！请打开 final_result.txt 查看结果" << endl;

    }
    catch (const std::exception& e) {
        // 捕获所有 C++ 异常并打印
        cerr << "\n[致命错误] 程序捕获到异常: " << e.what() << endl;
    }

    system("pause");
    llama_backend_free();
    return 0;
}