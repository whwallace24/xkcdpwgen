cmake_minimum_required(VERSION 3.10)
project(practiceProgramArgs)

set(CMAKE_CXX_STANDARD 11)
add_executable(practiceProgramArgs main.cpp main.cpp)
include_directories(/usr/include)
link_directories(/usr/lib)
target_link_libraries(practiceProgramArgs sqlite3)