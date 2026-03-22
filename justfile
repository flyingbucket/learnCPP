build_dir := "build"
bin_dir   := "build/bin"

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
	target=$(ls {{bin_dir}} 2>/dev/null | fzf --prompt="Please select target > ")
	if [ -n "$target" ]; then
		cmake --build {{build_dir}} --target "$target"
		./{{bin_dir}}/"$target"
	fi

# --- 测试与性能 ---

test compiler="":
	#!/usr/bin/env bash
	just build {{compiler}}
	cd {{build_dir}} && ctest --output-on-failure

bench target compiler="":
	#!/usr/bin/env bash
	just setup {{compiler}}
	cmake --build {{build_dir}} --target "{{target}}"
	echo "Running Benchmarks for {{target}}..."
	./{{bin_dir}}/{{target}} "[benchmark]" --benchmark-samples 100

# --- 工具 ---

clean:
	#!/usr/bin/env bash
	rm -rf {{build_dir}}
	echo "Build directory removed."

stats:
	#!/usr/bin/env bash
	hpp_count=$(find include -name "*.hpp" 2>/dev/null | wc -l)
	cpp_count=$(find examples tests -name "*.cpp" 2>/dev/null | wc -l)
	echo "HPP files: $hpp_count"
	echo "CPP files: $cpp_count"
