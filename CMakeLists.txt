cmake_minimum_required(VERSION 3.10)

project(HelloMethodVerse)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Fetch Boost.MP11 (header-only)
FetchContent_Declare(
  boost_mp11
  GIT_REPOSITORY https://github.com/boostorg/mp11.git
  GIT_TAG boost-1.86.0   # pick a stable tag, matches Boost release
)

FetchContent_MakeAvailable(boost_mp11)

# Fetch fmt library, which is needed for mp-units
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 11.0.2   # pick the latest stable release
)

# Make it available
FetchContent_MakeAvailable(fmt)

# Link it to your target
target_link_libraries(hello_methodverse PRIVATE fmt::fmt)

# Add include directory for your target(s)
target_include_directories(hello_methodverse PUBLIC ${boost_mp11_SOURCE_DIR}/include)

SET(MP_UNITS_INCLUDE_DIR 
    "${CMAKE_SOURCE_DIR}/external/mp-units/core/include"
    "${CMAKE_SOURCE_DIR}/external/mp-units/systems/include")

target_include_directories(hello_methodverse PUBLIC ${eigen_SOURCE_DIR} ${MP_UNITS_INCLUDE_DIR})

add_subdirectory(tst)