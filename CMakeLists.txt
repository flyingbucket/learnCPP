cmake_minimum_required(VERSION 3.16+)
project(learnAlgorithm C CXX)

# 全局输出目录
# 静态库 (.a) -> build/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 动态库 (.so) -> build/lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 默认可执行文件 -> build/bin
# 在examples，tests,benchmarks的具体配置中会根据实际需要覆盖此属性
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 基础配置
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

# 编译器与调试配置
option(USE_ASAN "Enable AddressSanitizer for debugging" ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fcolor-diagnostics)
    message(STATUS "Configuring for Clang: Precision diagnostics enabled.")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always -fmax-errors=5)
    message(STATUS "Configuring for GCC: Error limit and color enabled.")
endif()

if(USE_ASAN AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-fsanitize=address -g -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
    message(STATUS "AddressSanitizer: ENABLED")
else()
    message(STATUS "AddressSanitizer: DISABLED or UNSUPPORTED")
endif()

# 编译加速
find_program(MOLD_PATH mold)
if(MOLD_PATH)
    add_link_options("-fuse-ld=mold")
    message(STATUS "mold linker enabled.")
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "ccache enabled.")
endif()

# 添加子模块
# 核心库
add_subdirectory(src)

# Examples
option(BUILD_EXAMPLES "Build example targets" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
enable_testing()
option(BUILD_TESTS "Build test targets" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Benchmarks
option(BUILD_BENCHMARKS "Build benchmark targets" OFF)
option(IDE_SUPPORT "Enable all targets for clangd indexing" ON)
if(IDE_SUPPORT OR BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
