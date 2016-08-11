cmake_minimum_required(VERSION 3.0)

project(fsbb_tests CXX)

set( HEADERS_DIR ${CMAKE_SOURCE_DIR}/../include/ )

include_directories( ${HEADERS_DIR} )

set( INCLUDES
    ${HEADERS_DIR}fsbb_common.hpp
    ${HEADERS_DIR}fsbb_single.hpp
    ${HEADERS_DIR}fsbb_stacked.hpp
    ${HEADERS_DIR}fsbb_prefabs.hpp
)

add_executable( fsbb_tests ${INCLUDES} ${CMAKE_SOURCE_DIR}/src/fsbb_tests.cpp )
