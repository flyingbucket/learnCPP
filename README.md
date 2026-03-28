# learnAlgorithm

这是一个专注于基础数据结构与算法实现的练习项目，参考《王道2027数据结构》。
相关算法的实现采用纯c风格，极少使用c++特性，但为了方便测试和代码管理，将项目组织成c++项目的形式。

## 目录结构

- `include/`: 核心数据结构及算法的声明及部分通用容器的header only实现。
- `src/`: 核心数据结构及算法的实现
- `tests/`: 基于 Catch2 的单元测试代码。
- `examples/`: 各组件的简单应用示例以及部分课本习题求解代码。
- `benchmarks/`: 基准测试
- `third_party/`: 项目依赖库，测试框架Catch2。

## 环境配置

- 编译器(任选其一即可):
  - gcc套件(gcc,g++)
  - clang套件(clang,clang++)
- 工具链：
  - cmake(3.16+)
  - just
- 调试工具:
  - 使用gcc套件则安装libasan
  - 使用clang套件则安装llvm-asanitizer(一般直接安装完整的llvm开发工具)
- 编译加速(可选):
  - mold链接器
  - ccache缓存器

## 编译与运行

本项目使用 [just](https://github.com/casey/just) 作为命令运行器，封装了复杂的 CMake 指令，提供了 **Debug (开发调试)** 与 **Release (高性能比对)** 双轨并行的构建流。

### 核心指令速查

| 指令 | 说明 | 默认行为 / 参数 |
| :--- | :--- | :--- |
| `just setup` | 配置 CMake 环境 | `compiler=gcc`, `mode=debug` |
| `just build` | 编译当前项目 | `mode=debug` |
| `just run` | **交互式运行** | 汇总所有构建目录的可执行文件供 `fzf` 选择 |
| `just test` | 运行单元测试 | 强制在 Debug 模式下运行并开启 ASan |
| `just stats` | 代码行数统计 | 自动排除 `third_party` 依赖 |
| `just clean` | 清理构建目录 | 移除 `build` 与 `build_release` |

您也可以直接运行下面的命令查看上述just命令的具体使用方式

```bash
just
```

### 环境初始化

编译之前，首先执行配置命令：

```bash
just setup <compiler:gcc/clang> <debug/release>
```

第一个参数用于选择您使用的编译器，第二个参数选择debug或release会分别
在 `build/` (debug) 和 `build_release/` (release) 目录下生成构建配置。

```bash
just setup-all
```

这个命令会自动选择gcc编译器并同时将`build/`和`build_release/`设置好。

### 构建模式说明

项目针对不同场景对编译选项进行了深度定制：

- **Debug 模式**
  - **配置**：`just setup gcc debug`
  - **特性**：开启 `AddressSanitizer (ASan)` 内存检测、保留完整调试符号、启用单元测试。
  - **用途**：日常开发、Bug 排查、自动化测试。
- **Release 模式**
  - **配置**：`just setup gcc release`
  - **特性**：开启 `-O3` 最高级优化、关闭 ASan 以降低性能开销、禁用测试模块以纯净构建环境。
  - **用途**：运行 `benchmarks` 进行算法效率极限比对。

### 智能交互工作流

通过 `just run` 指令，你可以进入模糊搜索界面。该界面会根据路径自动识别并标注程序的运行模式：

- 若选择路径包含 `build_release`，则以 **[RELEASE]** 模式运行（针对 Benchmark 自动开启 100 次采样）。
- 若选择路径包含 `build`，则以 **[DEBUG]** 模式运行。

```bash
# 示例：使用 Clang 编译 Release 版本并运行
just setup clang release
just build release
just run  # 在 fzf 界面中输入关键字过滤
```
