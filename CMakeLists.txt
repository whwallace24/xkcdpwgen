cmake_minimum_required(VERSION 3.10)
project(xkcdpwgen)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_PREFIX_PATH "/usr/local/boost-1.69.0" ${OLD_CMAKE_PREFIX_PATH} )
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.69.0 COMPONENTS filesystem regex program_options)
add_executable(xkcdpwgen main.cpp)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(xkcdpwgen ${Boost_LIBRARIES})

include_directories(/usr/include)
link_directories(/usr/lib)
target_link_libraries(xkcdpwgen sqlite3)