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

add_executable(test_database tests/test_database.cpp src/FaceRecord.cpp src/FaceDatabase.cpp)
target_include_directories(test_database PRIVATE include)

find_package(dlib REQUIRED)

add_executable(test_recognition tests/test_recognition.cpp src/Detector.cpp src/Encoder.cpp)
target_include_directories(test_recognition PRIVATE include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_recognition PRIVATE ${OpenCV_LIBS} dlib::dlib)
target_compile_definitions(test_recognition PRIVATE MODELS_DIR="${CMAKE_SOURCE_DIR}/models")


add_executable(test_snapshot tests/test_snapshot.cpp src/Capture.cpp)
target_include_directories(test_snapshot PRIVATE include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_snapshot PRIVATE ${OpenCV_LIBS})

add_executable(test_chip_debug tests/test_chip_debug.cpp src/Detector.cpp)
target_include_directories(test_chip_debug PRIVATE include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_chip_debug PRIVATE ${OpenCV_LIBS} dlib::dlib)
target_compile_definitions(test_chip_debug PRIVATE MODELS_DIR="${CMAKE_SOURCE_DIR}/models")

add_executable(test_landmarks tests/test_landmarks.cpp src/Capture.cpp src/Detector.cpp)
target_include_directories(test_landmarks PRIVATE include ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_landmarks PRIVATE ${OpenCV_LIBS} dlib::dlib)
target_compile_definitions(test_landmarks PRIVATE MODELS_DIR="${CMAKE_SOURCE_DIR}/models")