if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT FORCE_TESTS)
    message(STATUS "Release mode detected: Skipping tests by default.")
    set(BUILD_TESTS OFF CACHE BOOL "Build unit tests" FORCE)
else()
    option(BUILD_TESTS "Build unit tests" ON)
endif()

if(BUILD_TESTS)
    message(STATUS "Unit tests included in build.")

    set(TEST_MAIN_SRC "test_main.cpp")
    add_library(catch2_main SHARED ${TEST_MAIN_SRC})
    target_include_directories(catch2_main PRIVATE "${PROJECT_SOURCE_DIR}/third_party")
    target_compile_definitions(catch2_main PRIVATE CATCH_CONFIG_MAIN)
    target_precompile_headers(catch2_main PRIVATE "${PROJECT_SOURCE_DIR}/third_party/catch2/catch.hpp")

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

    foreach(test_src ${TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)

        if(NOT ${test_name} STREQUAL "test_main")
            add_executable(${test_name} ${test_src})
            target_link_libraries(${test_name} PRIVATE catch2_main impl_algo)

            # 统一输出路径和 RPATH
            set_target_properties(${test_name} PROPERTIES 
                BUILD_RPATH "$ORIGIN"
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test" 
            )
            # 注册到 ctest
            add_test(NAME ${test_name} COMMAND ${test_name})
        endif()
    endforeach()
else()
    message(STATUS "Unit tests are DISABLED for this build.")
endif()
