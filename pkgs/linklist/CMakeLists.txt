# single link-list
add_executable(mainSLinkList mainSLinkList.cpp)
target_include_directories(mainSLinkList PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# double link-list
add_executable(mainDLinkList mainDLinkList.cpp)
target_include_directories(mainDLinkList PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
