cmake_minimum_required(VERSION 3.5)
project(ray_tracing_one_weekend_comp)

set(CMAKE_VERBOSE_MAKEFILE on)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

#add_compile_definitions(RLGL_SHOW_GL_DETAILS_INFO)

set(OPENGL_VERSION "4.3" CACHE STRING "")

list(APPEND HEADERS src/rt.h src/vec.h src/camera.h src/material.h src/primitive.h)

list(APPEND SOURCES src/main.cc)

add_executable(demo ${SOURCES} ${HEADERS})

set_target_properties(
    demo PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_subdirectory("raylib")

target_link_libraries(demo PUBLIC raylib)