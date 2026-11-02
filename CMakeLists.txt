cmake_minimum_required(VERSION 3.21)
project(more-stickers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
qt_standard_project_setup()

enable_testing()
add_subdirectory(src/core)
add_subdirectory(tests)