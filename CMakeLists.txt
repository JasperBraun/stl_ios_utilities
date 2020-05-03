cmake_minimum_required(VERSION 3.15)

# main project
project(stl_ios_utilities)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

#[[
add_executable(<EXEC_NAME>
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/<SRC1>.cpp")
target_include_directories(<EXEC_NAME> PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/<LIB1>/include")
target_link_libraries(<EXEC_NAME> PRIVATE <LIB1>)
]]

#[[
add_library(<LIB_NAME>
        "${CMAKE_CURRENT_SOURCE_DIR}/src/<SRC1>.cpp")
target_include_directories(<LIB_NAME> PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/<LIB1>/include")
target_link_libraries(<LIB_NAME> PRIVATE <LIB1>)
]]

# unit tests
include(CTest)
project(<TEST_PROJECT_NAME>)

add_subdirectory(test)