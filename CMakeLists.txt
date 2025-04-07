cmake_minimum_required(VERSION 3.18)
project(SDTS CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_executable(SDTS SDTS.cpp)
target_link_libraries(SDTS PRIVATE PkgConfig::GTK4)
