cmake_minimum_required(VERSION 3.16+)
project(learnCPP)

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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# ==examples==
macro(add_dir_targets directory_path)
    file(GLOB SOURCE_FILES "${directory_path}/*.cpp")
    foreach(src_file ${SOURCE_FILES})
        get_filename_component(target_name ${src_file} NAME_WE)
        if(NOT TARGET ${target_name}) # 防止重复定义
            add_executable(${target_name} ${src_file})
            message(STATUS "Registered target: ${target_name}")
        endif()
    endforeach()
endmacro()

add_dir_targets(${CMAKE_CURRENT_SOURCE_DIR}/examples)


# ==tests==
enable_testing()
# 找到测试引擎文件
set(TEST_MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main.cpp")
add_library(catch2_main SHARED ${TEST_MAIN_SRC})
target_compile_definitions(catch2_main PRIVATE CATCH_CONFIG_MAIN)
target_precompile_headers(catch2_main PRIVATE "third_party/catch2/catch.hpp")

# 注册测试
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    # 排除 test_main 本身，避免把它自己注册成一个测试
    if(NOT ${test_name} STREQUAL "test_main")
        add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} PRIVATE catch2_main)
        set_target_properties(${test_name} PROPERTIES BUILD_RPATH "$ORIGIN")
        add_test(NAME ${test_name} COMMAND ${test_name})
        message(STATUS "Speed-optimized target: ${test_name}")
    endif()
endforeach()
