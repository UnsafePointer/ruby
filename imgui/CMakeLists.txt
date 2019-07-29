cmake_minimum_required(VERSION 3.10.2)
project(imgui)

file(GLOB_RECURSE IMGUI_INCLUDE inlude/*.h)
file(GLOB_RECURSE IMGUI_SOURCES src/*.cpp)

add_subdirectory(glad)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(imgui ${IMGUI_INCLUDE} ${IMGUI_SOURCES})
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
target_include_directories(imgui PUBLIC include/)
target_link_libraries(imgui ${SDL2_LIBRARIES})
target_link_libraries(imgui glad)
