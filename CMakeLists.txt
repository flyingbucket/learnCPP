cmake_minimum_required(VERSION 3.10)
project(AlgorithmInCPP)

set(CMAKE_CXX_STANDARD 20)

# 将所有可执行文件统一输出到 build/bin 目录下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(pkgs/linklist)
# add_subdirectory(pkgs/merge_sort)
