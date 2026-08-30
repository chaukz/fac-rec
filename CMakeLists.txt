cmake_minimum_required(VERSION 3.16)
project(fac_rec LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(fac_rec src/main.cpp)

target_include_directories(fac_rec PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(fac_rec PRIVATE ${OpenCV_LIBS})