cmake_minimum_required(VERSION 3.10)

project(saros)

find_package(SQLite3 REQUIRED)

add_compile_options(-O3)

add_executable(parser main.cpp parse.cpp database.cpp socks.cpp)

target_link_libraries(parser SQLite3::SQLite3)
