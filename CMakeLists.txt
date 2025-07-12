cmake_minimum_required(VERSION 3.10)

project(HelloMethodVerse)

set(CMAKE_CXX_STANDARD 23)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(hello_methodverse src/main.cpp src/parameter.cpp)

include(FetchContent)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        3.4.0
)
# Disable building of Eigen's tests, using populate instead of makeavailable.
FetchContent_Populate(eigen)

target_include_directories(hello_methodverse PUBLIC ${eigen_SOURCE_DIR})

add_subdirectory(tst)