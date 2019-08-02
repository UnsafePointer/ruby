cmake_minimum_required(VERSION 3.10.2)
project(yaml)

add_library(yaml src/Yaml.cpp)
target_include_directories(yaml PUBLIC include/)
