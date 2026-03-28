set(BENCH_MAIN_SRC "bench_main.cpp")
add_library(catch2_bench OBJECT ${BENCH_MAIN_SRC})

target_include_directories(catch2_bench PUBLIC "${PROJECT_SOURCE_DIR}/third_party")
target_compile_definitions(catch2_bench PUBLIC CATCH_CONFIG_ENABLE_BENCHMARKING)

if(NOT MSVC)
    target_compile_options(catch2_bench PRIVATE -ffunction-sections -fdata-sections)
endif()

file(GLOB BENCH_SOURCES CONFIGURE_DEPENDS "bench_*.cpp")

foreach(bench_src ${BENCH_SOURCES})
    get_filename_component(bench_name ${bench_src} NAME_WE)

    if(NOT ${bench_name} STREQUAL "bench_main")
        add_executable(${bench_name} ${bench_src} $<TARGET_OBJECTS:catch2_bench>)
        target_link_libraries(${bench_name} PRIVATE impl_algo catch2_bench)
        if(NOT MSVC)
            target_link_options(${bench_name} PRIVATE -Wl,--gc-sections)
        endif()
        set_target_properties(${bench_name} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmarks"
        )
    endif()
endforeach()
