cmake_minimum_required(VERSION 3.16+)
project(learnAlgorithm)

#== compiler setup==
option(USE_ASAN "Enable AddressSanitizer for debugging" ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fcolor-diagnostics)
    message(STATUS "Configuring for Clang: Precision diagnostics enabled.")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always -fmax-errors=5)
    message(STATUS "Configuring for GCC: Error limit and color enabled.")
endif()

if(USE_ASAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        # -g: 保留调试符号以便 ASan 打印行号
        # -fno-omit-frame-pointer: 确保 stack trace 的完整性
        add_compile_options(-fsanitize=address -g -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
        message(STATUS "AddressSanitizer: ENABLED (Recommended for development/testing)")
    else()
        message(WARNING "AddressSanitizer: NOT supported on this compiler.")
    endif()
else()
    message(STATUS "AddressSanitizer: DISABLED (Optimized for Benchmarking)")
endif()

# ==speed-up==
# mold linker
find_program(MOLD_PATH mold)
if(MOLD_PATH)
    message(STATUS "Found mold, enabling it for faster linking.")
    add_link_options("-fuse-ld=mold")
else()
    message(STATUS "mold not found, using system default linker.")
endif()
# ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache, enabling it for faster compilation.")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
else()
    message(STATUS "ccache not found, proceeding without it.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

# == basic config==
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==src==
macro(add_src_library target_name src_root)
    file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS "${src_root}/*.c")
    file(GLOB_RECURSE CXX_SOURCES CONFIGURE_DEPENDS "${src_root}/*.cpp")
    # 合并 C 和 C++ 源
    set(ALL_SOURCES ${C_SOURCES} ${CXX_SOURCES})

    if(ALL_SOURCES)
        add_library(${target_name} STATIC ${ALL_SOURCES})

        # 让这个库能访问你的公共头文件
        target_include_directories(${target_name} PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party
        )

        message(STATUS "Created library '${target_name}' with sources:")
        foreach(src ${ALL_SOURCES})
            message(STATUS "  - ${src}")
        endforeach()
    else()
        message(WARNING "No sources found in ${src_root} for target ${target_name}")
    endif()
endmacro()

add_src_library(impl_algo ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ==examples==
macro(add_dir_targets directory_path)
    file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${directory_path}/*.cpp" "${directory_path}/*.c")
    foreach(src_file ${SOURCE_FILES})
        # 获取文件名 (如: 01)
        get_filename_component(file_name ${src_file} NAME_WE)
        # 获取父目录名 (如: chapter1)
        get_filename_component(parent_dir ${src_file} DIRECTORY)
        get_filename_component(folder_name ${parent_dir} NAME)

        # 组合成唯一的 Target 名称: chapter1_01
        set(target_name "${folder_name}_${file_name}")

        if(NOT TARGET ${target_name})
            add_executable(${target_name} ${src_file})
            target_link_libraries(${target_name} PRIVATE impl_algo)
 
            # 保持构建产物的目录结构
            set_target_properties(${target_name} PROPERTIES 
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${folder_name}")
        endif()
    endforeach()
endmacro()
add_dir_targets(examples)
# ==tests==
enable_testing()
# 找到测试引擎文件
set(TEST_MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main.cpp")
add_library(catch2_main SHARED ${TEST_MAIN_SRC})
target_compile_definitions(catch2_main PRIVATE CATCH_CONFIG_MAIN)
target_precompile_headers(catch2_main PRIVATE "third_party/catch2/catch.hpp")

# 注册测试
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    # 排除 test_main 本身，避免把它自己注册成一个测试
    if(NOT ${test_name} STREQUAL "test_main")
        add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} PRIVATE catch2_main impl_algo)
        set_target_properties(${test_name} PROPERTIES BUILD_RPATH "$ORIGIN")
        add_test(NAME ${test_name} COMMAND ${test_name})
        message(STATUS "Speed-optimized target: ${test_name}")
    endif()
endforeach()

# == benchmarks ==
set(BENCH_MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/bench_main.cpp")
add_library(catch2_bench STATIC ${BENCH_MAIN_SRC})
# 确保宏定义正确传播
target_compile_definitions(catch2_bench PUBLIC CATCH_CONFIG_ENABLE_BENCHMARKING)

file(GLOB BENCH_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/bench_*.cpp")

foreach(bench_src ${BENCH_SOURCES})
    get_filename_component(bench_name ${bench_src} NAME_WE)
    
    # 【关键修改】排除 bench_main 本身，避免重复定义
    if(NOT ${bench_name} STREQUAL "bench_main")
        add_executable(${bench_name} ${bench_src})
        target_link_libraries(${bench_name} PRIVATE catch2_bench impl_algo)
        
        set_target_properties(${bench_name} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmarks"
        )
        # 建议直接在 target 上加优化开关，而不是 COMPILE_FLAGS（那是旧语法）
        target_compile_options(${bench_name} PRIVATE -O3)
    endif()
endforeach()
