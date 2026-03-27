build_dir := "build"
bin_dir   := "build/bin"
bench_dir := "build_bench"

# show these info 
default:
	@just --list

setup compiler="":
    #!/usr/bin/env bash
    set -e
    c_val="{{compiler}}"
    
    if [ -n "$c_val" ]; then
        if [ "$c_val" = "clang" ]; then
            c_comp="clang"; cxx_comp="clang++"
        elif [ "$c_val" = "gcc" ]; then
            c_comp="gcc"; cxx_comp="g++"
        else
            c_comp="$c_val"; cxx_comp="$c_val"
        fi
        echo "Using C compiler:   $c_comp"
        echo "Using CXX compiler: $cxx_comp"
    else
        echo "Using default system compilers (no override)."
    fi

    if [ ! -d "{{build_dir}}" ] || [ -n "$c_val" ]; then
        echo "Configuring CMake..."
        args=(-B "{{build_dir}}" -DCMAKE_BUILD_TYPE=RelWithDebInfo)
        [ -n "$c_comp" ] && args+=("-DCMAKE_C_COMPILER=$c_comp")
        [ -n "$cxx_comp" ] && args+=("-DCMAKE_CXX_COMPILER=$cxx_comp")
        cmake "${args[@]}"
    else
        echo "Build directory exists. Skipping reconfiguration."
    fi


build compiler="":
	#!/usr/bin/env bash
	just setup {{compiler}}
	threads=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
	cmake --build {{build_dir}} -j "$threads"

build-p compiler="":
	#!/usr/bin/env bash
	just setup {{compiler}}
	target=$(ls {{bin_dir}} 2>/dev/null | fzf --prompt="Please select target > ")
	if [ -n "$target" ]; then
		cmake --build {{build_dir}} --target "$target"
	fi

p target compiler="":
	#!/usr/bin/env bash
	just setup {{compiler}}
	cmake --build {{build_dir}} --target "{{target}}"
	echo "----------------------------------------"
	./{{bin_dir}}/{{target}}

run compiler="":
    #!/usr/bin/env bash
    just setup {{compiler}}

    selected_path=$(find {{bin_dir}} -executable -type f | fzf --prompt="Select target > ")

    if [ -n "$selected_path" ]; then
        target=$(basename "$selected_path")
        echo "--- Building $target ---"
        cmake --build {{build_dir}} --target "$target"
        echo "--- Running $target ---"
        "$selected_path"
    fi

# --- 测试与性能 ---
test compiler="":
	#!/usr/bin/env bash
	just build {{compiler}}
	cd {{build_dir}} && ctest --output-on-failure

bench compiler="" asan_toggle="no-asan":
    #!/usr/bin/env bash
    set -e
    
    # 1. 处理 ASan 开关逻辑
    if [ "{{asan_toggle}}" = "asan" ]; then
        asan_flag="ON"
        mode_msg="ASan: ON (Debug Mode)"
    else
        asan_flag="OFF"
        mode_msg="ASan: OFF (Performance Mode)"
    fi

    # 2. 配置环境 (如果目录不存在，或显式指定了参数，则重新配置)
    c_val="{{compiler}}"
    if [ ! -d "{{bench_dir}}" ] || [ -n "$c_val" ] || [ -n "{{asan_toggle}}" ]; then
        echo "--- Configuring Benchmark Build ($mode_msg) ---"
        args=(-B "{{bench_dir}}" -DCMAKE_BUILD_TYPE=Release -DUSE_ASAN=$asan_flag -DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
        
        if [ "$c_val" = "clang" ]; then
            args+=("-DCMAKE_C_COMPILER=clang" "-DCMAKE_CXX_COMPILER=clang++")
        elif [ "$c_val" = "gcc" ]; then
            args+=("-DCMAKE_C_COMPILER=gcc" "-DCMAKE_CXX_COMPILER=g++")
        fi
        cmake "${args[@]}"
    fi

    # 3. 交互式选择源码
    selected_cpp=$(find benchmarks -name "bench_*.cpp" ! -name "bench_main.cpp" | fzf --prompt="Select Benchmark Source ($mode_msg) > ")

    if [ -n "$selected_cpp" ]; then
        target=$(basename "$selected_cpp" .cpp)
        threads=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

        echo "--- Building $target ---"
        cmake --build {{bench_dir}} --target "$target" -j "$threads"

        echo "------------------------------------------------"
        echo "Running: $target"
        ./{{bench_dir}}/bin/benchmarks/"$target" --benchmark-samples 100
    else
        echo "No target selected."
    fi
clean:
    #!/usr/bin/env bash
    rm -rf {{build_dir}}
    rm -rf {{bench_dir}}
    echo "Cleanup complete: '{{build_dir}}' and '{{bench_dir}}' removed."

stats:
    #!/usr/bin/env bash
    set -e
    echo "================ Code Statistics Overview ================"
    
    # include 中的各个子模块 (Header files)
    echo "(include/):"
    for dir in include/*/; do
        module=$(basename "$dir")
        count=$(find "$dir" -type f \( -name "*.hpp" -o -name "*.h" \) | wc -l)
        printf "  %-12s: %2d files\n" "$module" "$count"
    done

    # src 中的实现文件 (Source files)
    echo -e "\n(src/):"
    for dir in src/*/; do
        module=$(basename "$dir")
        count=$(find "$dir" -type f \( -name "*.cpp" -o -name "*.c" \) | wc -l)
        printf "  %-12s: %2d files\n" "$module" "$count"
    done

    # 统计应用层代码
    echo -e "\nOther files:"
    examples_count=$(find examples -type f \( -name "*.cpp" -o -name "*.c" \) | wc -l)
    tests_count=$(find tests -type f \( -name "*.cpp" -o -name "*.c" \) | wc -l)
    
    printf "  %-12s: %2d files\n" "Examples" "$examples_count"
    printf "  %-12s: %2d files\n" "Tests" "$tests_count"
    
    # 总计行数统计
    if command -v cloc >/dev/null 2>&1; then
        echo -e "\n========= Lines of code =========:"
        cloc include src examples tests --quiet --not-match-d=third_party
    fi
