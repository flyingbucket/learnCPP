debug_dir   := "build"
release_dir := "build_release"

# 默认操作：显示命令列表
default:
    @just --list

# --- 核心配置逻辑 ---

# 初始化/重新配置构建目录
# 用法: just setup (默认 gcc, debug)
# 用法: just setup clang release
setup compiler="gcc" mode="debug":
    #!/usr/bin/env bash
    set -e
    
    if [ "{{mode}}" == "release" ]; then
        target_dir="{{release_dir}}"
        build_type="Release"
        asan="OFF"
        # 核心：Release 模式默认关掉测试
        test_flag="OFF" 
        mode_msg="RELEASE (O3, no-tests)"
    else
        target_dir="{{debug_dir}}"
        build_type="Debug"
        asan="ON"
        test_flag="ON"
        mode_msg="DEBUG (ASan, with-tests)"
    fi

    if [ "{{compiler}}" == "clang" ]; then
            c_comp="clang"; cxx_comp="clang++"
        else
            c_comp="gcc"; cxx_comp="g++"
        fi

        echo "--- Configuring $mode_msg using $compiler ---"
    
    cmake -B "$target_dir" \
          -DCMAKE_BUILD_TYPE=$build_type \
          -DCMAKE_C_COMPILER=$c_comp \
          -DCMAKE_CXX_COMPILER=$cxx_comp \
          -DUSE_ASAN=$asan \
          -DBUILD_BENCHMARKS=ON \
          -DBUILD_TESTS=$test_flag \
          -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# 一键设置debug和release环境(使用gcc)
setup-all:
    @just setup gcc debug
    @just setup gcc release

# --- 编译与运行 ---

# 编译指定模式的所有目标
# 用法: just build (默认 debug)
build mode="debug":
    #!/usr/bin/env bash
    set -e
    target_dir=$([ "{{mode}}" == "release" ] && echo "{{release_dir}}" || echo "{{debug_dir}}")
    
    if [ ! -d "$target_dir" ]; then
        echo "Error: $target_dir not found. Running setup with gcc as default..."
        just setup gcc {{mode}}
    fi

    threads=$(nproc 2>/dev/null || echo 4)
    echo "--- Building ({{mode}}) using $threads threads ---"
    cmake --build "$target_dir" -j "$threads"

# 智能运行：汇总所有构建目录下的可执行文件供选择
run:
    #!/usr/bin/env bash
    set -e
    
    echo "Scanning executables in all build directories..."
    
    # 同时从 debug 和 release 目录搜寻可执行文件
    # 过滤掉一些 cmake 内部的辅助程序
    selected_path=$(find {{debug_dir}}/bin {{release_dir}}/bin {{debug_dir}}/test -type f -executable 2>/dev/null \
                    | grep -v "CMakeFiles" \
                    | fzf --prompt="Select Target to Run > " \
                          --preview "ls -lh {}" \
                          --header "Debug targets are in {{debug_dir}}, Release in {{release_dir}}")

    if [ -n "$selected_path" ]; then
        echo "------------------------------------------------"
        # 自动识别当前运行的是哪个版本
        [[ "$selected_path" == *"build_release"* ]] && echo "[MODE: RELEASE]" || echo "[MODE: DEBUG]"
        echo "Running: $selected_path"
        echo "------------------------------------------------"
        
        # 如果是 benchmark，自动带上常用参数
        if [[ "$selected_path" == *"benchmarks"* ]]; then
            "$selected_path" --benchmark-samples 100
        else
            "$selected_path"
        fi
    else
        echo "No target selected."
    fi

# 快捷测试：强制在 Debug 模式下运行
test:
    @just build debug
    @echo "--- Running Unit Tests (Debug + ASan) ---"
    @cd {{debug_dir}} && ctest --output-on-failure

# --- 实用工具 ---

clean:
    rm -rf {{debug_dir}} {{release_dir}} 
    @echo "All build directories removed."

stats:
    #!/usr/bin/env bash
    set -e
    echo "================ Code Statistics Overview ================"
    # (保持你原来的统计逻辑不变)
    if command -v cloc >/dev/null 2>&1; then
        cloc include src examples tests benchmarks --quiet --not-match-d=third_party
    fi
