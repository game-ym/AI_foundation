#include "AI/Thinkingprocess.h"
#include "AI/Modelselector.h" 
#include <vector>
#include <iostream>

using namespace std;

Status Thinkingprocess::Run(LModel& thinker, LModel& critic, vector<AIConfig>& my_ais, vector<LModel*>& sacrifice_list, const string& task_prompt, int max_retries) {
    Status result;
    result.is_passed = false;
    result.total_rounds = 0;

    string current_Think = "";
    string current_Reflection = "";
    int Round = 1;
    while (Round <= max_retries) {
        result.total_rounds = Round;
        cout << "\n[系统] 第 " << Round << " 轮思考开始..." << endl;

        Modelselector::Startmodel(thinker, my_ais, sacrifice_list, "Thinker");

        if (Round == 1) {
            current_Think = "<|im_start|>system\n你是一个资深C++程序员。请直接编写完整且能运行的C++代码，绝对不要输出任何思考过程。<|im_end|>\n"
                "<|im_start|>user\n" + task_prompt + "<|im_end|>\n<|im_start|>assistant\n<think>\n\n</think>\n\n```cpp\n";
        }
        else {
            string temp_prompt = "<|im_start|>system\n你是一个资深C++程序员。请根据审查员的意见修改代码，输出修改后的完整代码，绝对不要输出任何思考过程。<|im_end|>\n"
                "<|im_start|>user\n原代码：\n" + current_Think + "\n\n审查意见：\n" + current_Reflection + "<|im_end|>\n<|im_start|>assistant\n<think>\n\n</think>\n\n```cpp\n";
            current_Think = temp_prompt;
        }

        try {
            vector<llama_token> thinker_tokens = thinker.Dialogue(current_Think);
            string raw_thinker_output = thinker.Reasoning(thinker_tokens, 2048, 0.3f);

            current_Think = "```cpp\n" + raw_thinker_output;
            size_t think_end = current_Think.find("</think>");
            if (think_end != string::npos) {
                current_Think = current_Think.substr(think_end + 8);
            }
            cout << "[思考] 第 " << Round << " 轮代码编写完成。" << endl;
        }
        catch (const std::exception& e) {
            result.error_msg = string("Thinker 运行异常: ") + e.what();
            return result;
        }

        cout << "[系统] 正在切换到反思模型..." << endl;
        Modelselector::Startmodel(critic, my_ais, sacrifice_list, "Critic");

        string critic_prompt = "<|im_start|>system\n你是一个严苛的代码审查员。请直接输出审查结果，绝对不要输出思考过程。如果代码完全正确没有漏洞，请只输出 [PASS]；如果存在语法、逻辑漏洞，请输出 [FAIL] 并给出修改意见。<|im_end|>\n"
            "<|im_start|>user\n请审查以下代码：\n" + current_Think + "<|im_end|>\n<|im_start|>assistant\n<think>\n\n</think>\n\n";

        try {
            vector<llama_token> critic_tokens = critic.Dialogue(critic_prompt);
            string raw_critic_output = critic.Reasoning(critic_tokens, 512, 0.3f);

            current_Reflection = raw_critic_output;
            size_t c_think_end = current_Reflection.find("</think>");
            if (c_think_end != string::npos) {
                current_Reflection = current_Reflection.substr(c_think_end + 8);
            }
            cout << "[反思] 第 " << Round << " 轮审查结果：\n" << current_Reflection << endl;
        }
        catch (const std::exception& e) {
            result.error_msg = string("Critic 运行异常: ") + e.what();
            return result;
        }

        bool pass = false;
        if (current_Reflection.find("[PASS]") != string::npos) {
            pass = true;
        }
        else if (current_Reflection.find("[FAIL]") != string::npos ||
            current_Reflection.find("错误") != string::npos ||
            current_Reflection.find("漏洞") != string::npos ||
            current_Reflection.find("越界") != string::npos) {
            pass = false;
        }
        else {
            if (current_Reflection.find("?") != string::npos || current_Reflection.length() < 10) {
                pass = false;
            }
            else {
                pass = true;
            }
        }
        if (pass) {
            result.is_passed = true;
            cout << "\n[系统] 恭喜！代码在 " << Round << " 轮后通过审查！" << endl;
            break;
        }
        else {
            cout << "\n[系统] 审查未通过，准备打回重写..." << endl;
            if (Round == max_retries) {
                cout << "Maximum rounds have been reached." << endl;
                cout << "Whether to add rounds(y:n): ";
                char Judgment;
                cin >> Judgment;
                cin.ignore(10000, '\n'); 

                if (Judgment == 'y' || Judgment == 'Y') {
                    max_retries += 3;
                    cout << "\nRounds have been added: " << max_retries << endl;
                }
                else {
                    cout << "Okay, I respect your choice." << endl;
                    break;
                }
            }
        }
        Round++;
    }

    result.final_Think = current_Think;
    result.final_Reflection = current_Reflection;

    if (!result.is_passed) {
        cout << "\n[系统] 达到最大重试次数，强制结束。请以最后一份代码为准。" << endl;
    }
    return result;
}