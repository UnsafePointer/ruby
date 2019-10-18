cmake_minimum_required(VERSION 3.10.2)
project(ruby)

option(HANA "Compile with GDB support")

file(GLOB_RECURSE RUBY_SOURCES src/*.cpp)

include_directories(include)

add_subdirectory(imgui)
add_subdirectory(mini-yaml)

add_executable(ruby ${RUBY_SOURCES})
target_link_libraries(ruby imgui)
target_link_libraries(ruby yaml)
if (HANA)
    find_package(Threads)
    include_directories(hana/include)
    add_definitions(-DHANA)
    target_link_libraries(ruby ${CMAKE_CURRENT_SOURCE_DIR}/hana/libHana.a)
    target_link_libraries(ruby ${CMAKE_THREAD_LIBS_INIT})
endif(HANA)
set_property(TARGET ruby PROPERTY CXX_STANDARD 17)
target_compile_options(ruby PRIVATE -Werror -Wall -Wextra)
