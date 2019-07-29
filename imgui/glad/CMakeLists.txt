cmake_minimum_required(VERSION 3.10.2)
project(glad)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include/)
target_link_libraries(glad dl)
