# AI_foundation
# AI_foundation (端侧多智能体协作操作系统基座)

这是一个15岁纯手搓(好吧，用了点AI) C++ (基于 llama.cpp) 编写的**微型 AI 操作系统基座**。项目致力于在极低显存（如 8GB VRAM）的受限硬件环境下，实现多模型（MoE）、多智能体（Multi-Agent）的动态调度与闭环自我进化。

## 核心架构

1. **资源调度层 (`VMManager` + `Modelselector`)**
   - 实时监控显存水位，基于“时间换空间”策略，实现模型的动态强拆卸载与防抖复用。
   - 精确计算 KV Cache 字节数，动态分配 `n_ctx`，彻底杜绝 OOM。
2. **引擎封装层 (`LModel`)**
   - 纯 C++ RAII 管理模型生命周期。
   - 支持分块 Prefill，自带底层计算溢出熔断（防 AMD Vulkan 后端 FP16 问号死循环）。
3. **多智能体业务层 (`ThinkingProcess`)**
   - 包含 Main (主脑拆解)、Thinker (专家写码)、Critic (反思审查)。
   - 实现了完整的“生成 -> 审查 -> 打回重写”闭环，支持用户手动加钟。
4. **分级记忆库 (`MemoryManager` - 开发中)**
   - 构建 L1(显存) -> L2(内存LRU) -> L3(硬盘) 三级缓存架构。
   - 支持 `<Memory>` 标签注入原汁原味的历史上下文。

## 编译与运行

- **环境要求**: Windows / Linux (推荐 Arch), CMake, C++17。
- **硬件要求**: 支持 Vulkan 或 CUDA 的 GPU (本项目在 AMD RX 590 GME 8GB 极限测试通过)。
- **模型准备**: 请将 `.gguf` 格式的模型放入 `models/` 目录，并在 `config.json` 中配置路径。

## 路线图 (Roadmap)
- [x] 动态显存调度与防崩护盾 (v0.1.0)
- [ ] 三级分层记忆库与语义检索 (v0.2.0)
- [ ] HTTP API 与 Web UI 流式输出 (v1.0.0)
- [ ] 云端微调与 `<Instruction>` 结构化输出
