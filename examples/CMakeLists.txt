file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "*.cpp" "*.c")

foreach(src_file ${SOURCE_FILES})
    get_filename_component(file_name ${src_file} NAME_WE)
    get_filename_component(parent_dir ${src_file} DIRECTORY)
    get_filename_component(folder_name ${parent_dir} NAME)

    set(target_name "${folder_name}_${file_name}")

    if(NOT TARGET ${target_name})
        add_executable(${target_name} ${src_file})
        target_link_libraries(${target_name} PRIVATE impl_algo)

        # 覆盖全局设置，将其按文件夹归类输出到 build/bin/chapter_xx 下
        set_target_properties(${target_name} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${folder_name}")
    endif()
endforeach()
