build_dir := "build"
bin_dir   := "build/bin"
c         := "" 

# 默认指令
default:
    @just --list

setup:
    @# 1. 如果指定了 c，检查是否需要清理旧构建（编译器切换检测）
    @if [ -f "{{build_dir}}/CMakeCache.txt" ] && [ -n "{{c}}" ]; then \
        current=$(grep "CMAKE_CXX_COMPILER:" {{build_dir}}/CMakeCache.txt | grep -v "NOTFOUND" | grep -c "{{c}}"); \
        if [ "$current" -eq 0 ]; then \
            echo "Compiler change detected! Cleaning build directory..."; \
            rm -rf {{build_dir}}; \
        fi \
    fi
    @# 2. 执行配置（核心修复：只有在目录不存在，或者显式指定了编译器时才运行 cmake）
    @if [ ! -d "{{build_dir}}" ] || [ -n "{{c}}" ]; then \
        echo "Configuring CMake..."; \
        cmake -B {{build_dir}} \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo \
            {{ if c != "" { "-DCMAKE_CXX_COMPILER=" + c } else { "" } }}; \
    else \
        echo "Build directory exists. Skipping reconfiguration to preserve current compiler."; \
    fi

# 编译所有 (依赖 setup)
build: setup
    cmake --build {{build_dir}} -j $(nproc)

# 交互式选择并编译 (依赖 setup，而不是 build，避免全量编译)
build-p: setup
    @target=$(ls {{bin_dir}} | fzf --prompt="Please select target > ") && \
    cmake --build {{build_dir}} --target $target

# 清理
clean:
    rm -rf {{build_dir}}
    @echo "Build directory removed."


# 编译并运行特定 target (依赖 setup，确保环境就绪)
p target: setup
    @# 只编译这一个目标，速度更快
    cmake --build {{build_dir}} --target {{target}}
    @echo "----------------------------------------"
    @./{{bin_dir}}/{{target}}

# 交互式运行 (依赖 setup)
run: setup
    @target=$(ls {{bin_dir}} | fzf --prompt="Please select target > ") && \
    cmake --build {{build_dir}} --target $target && \
    ./{{bin_dir}}/$target


# 运行测试 (依赖 build，因为测试通常需要全量编译或者至少核心库编译)
test: build
    cd {{build_dir}} && ctest --output-on-failure

# 运行 Benchmark
bench target: setup
    @cmake --build {{build_dir}} --target {{target}}
    @echo "Running Benchmarks for {{target}}..."
    @./{{bin_dir}}/{{target}} "[benchmark]" --benchmark-samples 100

# 统计
stats:
    @echo "HPP files: $(find include -name "*.hpp" | wc -l)"
    @echo "CPP files: $(find examples tests -name "*.cpp" | wc -l)"
