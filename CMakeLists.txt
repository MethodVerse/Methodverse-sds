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

FetchContent_MakeAvailable(eigen)

target_link_libraries(hello_methodverse PUBLIC Eigen3::Eigen)
target_include_directories(hello_methodverse PUBLIC ${eigen_SOURCE_DIR})

add_subdirectory(tst)