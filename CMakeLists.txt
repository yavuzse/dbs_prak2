cmake_minimum_required(VERSION 3.20)
project(dbs2_prak)

set(CMAKE_CXX_STANDARD 23)

include_directories("/opt/homebrew/Cellar/libpq/14.1/include")
link_libraries("/opt/homebrew/Cellar/libpq/14.1/lib/libpq.dylib")

add_executable(dbs2_prak main.cpp db.cpp db.h)
