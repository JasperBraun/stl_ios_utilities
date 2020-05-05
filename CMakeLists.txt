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

add_library(stl_ios_utilities
        "${CMAKE_CURRENT_SOURCE_DIR}/src/delimited_row_parser.cc")
target_include_directories(stl_ios_utilities PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        #[["${CMAKE_CURRENT_SOURCE_DIR}/lib/<LIB1>/include"]])
#[[target_link_libraries(<LIB_NAME> PRIVATE <LIB1>)]]

set_target_properties(stl_ios_utilities
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

# unit tests
include(CTest)
project(stl_ios_utilities_test)

add_subdirectory(test)