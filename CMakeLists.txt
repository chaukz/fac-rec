cmake_minimum_required(VERSION 3.16)
project(fac_rec LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(fac_rec
    src/main.cpp
    src/Capture.cpp
    src/Detector.cpp
)

target_include_directories(fac_rec PRIVATE include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(fac_rec PRIVATE ${OpenCV_LIBS})

target_compile_definitions(fac_rec PRIVATE MODELS_DIR="${CMAKE_SOURCE_DIR}/models")

add_executable(test_database tests/test_database.cpp src/FaceRecord.cpp)
target_include_directories(test_database PRIVATE include)