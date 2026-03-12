cmake_minimum_required(VERSION 3.10)
project(learnCPP)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

enable_testing()

# 找到测试引擎文件
set(TEST_MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_main.cpp")

# 修改注册测试的循环
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    
    # 排除 test_main 本身，避免把它自己注册成一个测试
    if(NOT ${test_name} STREQUAL "test_main")
        # 关键点：将 test_main.cpp 和当前测试源文件一起编译
        add_executable(${test_name} ${test_src} ${TEST_MAIN_SRC})
        add_test(NAME ${test_name} COMMAND ${test_name})
        message(STATUS "Speed-optimized target: ${test_name}")
    endif()
endforeach()
