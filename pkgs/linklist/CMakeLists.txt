# single link-list
add_executable(mainSLinkList mainSLinkList.cpp)
target_include_directories(mainSLinkList PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# doubly link-list
add_executable(mainDLinkList mainDLinkList.cpp)
target_include_directories(mainDLinkList PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# cicular doubly link-list
add_executable(mainCDLinkList mainCDLinkList.cpp)
target_include_directories(mainCDLinkList PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
