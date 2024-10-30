cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключение Boost (все компоненты)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE
    "src/database_postgres.cpp"
    "boostbeast.hpp"
    )

add_executable(main ${SOURCE} main.cpp)

add_subdirectory(deps/libpqxx build-pqxx)
target_link_libraries(main 
    ${Boost_LIBRARIES} 
    pqxx
    )