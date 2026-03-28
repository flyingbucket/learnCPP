file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS "*.c")
file(GLOB_RECURSE CXX_SOURCES CONFIGURE_DEPENDS "*.cpp")
set(ALL_SOURCES ${C_SOURCES} ${CXX_SOURCES})

if(ALL_SOURCES)
    add_library(impl_algo STATIC ${ALL_SOURCES})

    # PUBLIC 使得链接 impl_algo 的 target 也能自动包含这些头文件目录
    target_include_directories(impl_algo PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/third_party
    )
else()
    message(WARNING "No sources found in src/ for target impl_algo")
endif()
