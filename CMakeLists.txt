cmake_minimum_required(VERSION 3.15)

project(stl_ios_utilities)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11")

add_library(stl_ios_utilities
        "${CMAKE_CURRENT_SOURCE_DIR}/src/delimited_row_parser.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/field_parser.cc")
target_include_directories(stl_ios_utilities PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(stl_ios_utilities
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)