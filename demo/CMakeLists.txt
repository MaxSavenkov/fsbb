cmake_minimum_required(VERSION 3.0)

project(fsbb_demo CXX)

set( FSBB_HEADERS_DIR ${CMAKE_SOURCE_DIR}/../include/ )
set( SOURCES_DIR ${CMAKE_SOURCE_DIR}/src/ )
set( NYA_DIR ${CMAKE_SOURCE_DIR}/ext/nya-engine/ )

include_directories( ${HEADERS_DIR} )
include_directories( ${FSBB_HEADERS_DIR} )
include_directories( ${NYA_DIR} )

set( INCLUDES
    ${FSBB_HEADERS_DIR}fsbb_common.hpp
    ${FSBB_HEADERS_DIR}fsbb_single.hpp
    ${FSBB_HEADERS_DIR}fsbb_stacked.hpp
    ${FSBB_HEADERS_DIR}fsbb_prefabs.hpp
)

set( SOURCES
    ${SOURCES_DIR}app_context.h      
    ${SOURCES_DIR}common.h
    ${SOURCES_DIR}game.cpp
    ${SOURCES_DIR}game.h    
    ${SOURCES_DIR}game_state.h
    ${SOURCES_DIR}game_state_intro.h
    ${SOURCES_DIR}game_state_menu.h
    ${SOURCES_DIR}game_state_combat.h
    ${SOURCES_DIR}game_state_pause.h
    ${SOURCES_DIR}game_state_gameover.h
    ${SOURCES_DIR}game_state_victory.h
    ${SOURCES_DIR}game_state_context.h
    ${SOURCES_DIR}main.cpp
    ${SOURCES_DIR}shared_textures.cpp
    ${SOURCES_DIR}shared_textures.h
    ${SOURCES_DIR}ui_widgets.cpp
    ${SOURCES_DIR}ui_widgets.h
    ${SOURCES_DIR}viewer_camera.cpp
    ${SOURCES_DIR}viewer_camera.h
    ${SOURCES_DIR}visual_state.h
    ${SOURCES_DIR}visual_state_intro.h
    ${SOURCES_DIR}visual_state_menu.h
    ${SOURCES_DIR}visual_state_combat.h
    ${SOURCES_DIR}visual_state_pause.h
    ${SOURCES_DIR}visual_state_gameover.h
    ${SOURCES_DIR}visual_state_victory.h
    ${SOURCES_DIR}visual_state_context.h
)

source_group("fsbb" REGULAR_EXPRESSION .*fsbb_.*)
source_group("game_states" REGULAR_EXPRESSION .*game_state.*)
source_group("visual_states" REGULAR_EXPRESSION .*visual_state.*)
source_group("visual" FILES ${SOURCES_DIR}viewer_camera.cpp ${SOURCES_DIR}viewer_camera.h )
source_group("" FILES ${SOURCES_DIR}main.cpp ${SOURCES_DIR}game.cpp ${SOURCES_DIR}game.h ${SOURCES_DIR}common.h ${SOURCES_DIR}app_context.h)

add_subdirectory( ${NYA_DIR} )

add_executable( fsbb_demo WIN32 ${INCLUDES} ${SOURCES} )

target_link_libraries( fsbb_demo nya_engine opengl32.lib )