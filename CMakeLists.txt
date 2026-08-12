cmake_minimum_required(VERSION 3.16)

project(learnCPP CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# compile options
add_compile_options(
    -O2
    -Wall
    -Wextra
    -g
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-fcolor-diagnostics)
endif()


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Sanitizers 需要同时传入编译阶段和链接阶段
  add_compile_options(-fsanitize=address,undefined)
  add_link_options(-fsanitize=address,undefined)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE CPP_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/basics/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/stl/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/std/*.cpp"
)
foreach(FILE_PATH ${CPP_FILES})
  file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${FILE_PATH}")

  get_filename_component(DIR_NAME ${REL_PATH} DIRECTORY)
  get_filename_component(FILE_NAME ${REL_PATH} NAME_WE)

  if(DIR_NAME)
    string(REPLACE "/" "_" TARGET_NAME "${DIR_NAME}_${FILE_NAME}")
  else()
    set(TARGET_NAME "${FILE_NAME}")
  endif()

  add_executable(${TARGET_NAME} ${FILE_PATH})

  set_target_properties(${TARGET_NAME} PROPERTIES
        OUTPUT_NAME ${FILE_NAME}
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${DIR_NAME}"
    )

  message(STATUS "Added target: ${TARGET_NAME} -> bin/${DIR_NAME}/${FILE_NAME}")
endforeach()

