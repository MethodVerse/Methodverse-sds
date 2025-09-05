cmake_minimum_required(VERSION 3.20)  # 3.20+ recommended for modern FetchContent
project(MethodVerse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
include(GNUInstallDirs)
# ----------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------
# FetchContent_Declare(
#     eigen
#     GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
#     GIT_TAG        3.4.0
# )
# # Disable building of Eigen's tests, using populate instead of makeavailable.
# FetchContent_Populate(eigen)
# Header-only library wrapper for Eigen
add_library(methodverse_eigen_headers INTERFACE)

# Point it to the headers you vendored
target_include_directories(methodverse_eigen_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/eigen>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/eigen>  # <prefix>/include/eigen
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/eigen DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) # install to <prefix>/include

# # Fetch Boost.MP11 (header-only)
# FetchContent_Declare(
#   boost_mp11
#   GIT_REPOSITORY https://github.com/boostorg/mp11.git
#   GIT_TAG boost-1.86.0   # pick a stable tag, matches Boost release
# )
# FetchContent_MakeAvailable(boost_mp11)
add_library(methodverse_boostmp11_headers INTERFACE)

target_include_directories(methodverse_boostmp11_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/mp11/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mp11>
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/mp11 DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) # install to <prefix>/include

# Fetch fmt library, which is needed for mp-units
# FetchContent_Declare(
#   fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#   GIT_TAG 11.0.2   # pick the latest stable release
# )
# FetchContent_MakeAvailable(fmt)
add_library(methodverse_fmt_headers INTERFACE)
target_include_directories(methodverse_fmt_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/fmt/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/fmt>
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/fmt DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) # install to <prefix>/include

# mp-units (local copy, not FetchContent)
# SET(MP_UNITS_INCLUDE_DIR 
#     "${CMAKE_SOURCE_DIR}/external/mp-units/core/include"
#     "${CMAKE_SOURCE_DIR}/external/mp-units/systems/include")
add_library(methodverse_mpunits_headers INTERFACE)
target_include_directories(methodverse_mpunits_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/mp-units/core/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/mp-units/systems/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mp-units/core/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mp-units/systems/include>
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/mp-units DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) # install to <prefix>/include
# target_compile_definitions(methodverse_mpunits_headers INTERFACE MP_UNITS_NO_FMT=1)
# add_compile_definitions(MP_UNITS_NO_FMT=1)
# target_compile_definitions(methodverse_mpunits_headers INTERFACE 
#     #MP_UNITS_NO_FMT=1
#     MP_UNITS_NO_STD_FORMAT=1
#     MP_UNITS_NO_FMTLIB=1
# ) # <= current safe config

add_compile_definitions(
  #MP_UNITS_DISABLE_IOSTREAM=1  # no operator<<
  #MP_UNITS_DISABLE_FMT=1       # no std::format/fmtlib support
  MP_UNITS_USE_FMTLIB=1
)
# target_link_libraries(methodverse_mpunits_headers INTERFACE methodverse_fmt_headers)

# ----------------------------------------------------------------------
# Subsystems
# ----------------------------------------------------------------------

# Parameter library (header-only)
add_subdirectory(src/parameter)

# ----------------------------------------------------------------------
# Executable
# ----------------------------------------------------------------------
add_executable(hello_methodverse src/main.cpp)

# Link dependencies (target-based)
target_link_libraries(hello_methodverse
    PRIVATE
        methodverse-parameter   # your header-only lib
        # methodverse_fmt_headers                # fmt target from FetchContent
        # methodverse_mpunits_headers                # mp-units header-only lib
        # methodverse_eigen_headers           # Eigen header-only lib
        # methodverse_boostmp11_headers             # Boost.MP11 header-only lib
)

get_target_property(incs hello_methodverse INCLUDE_DIRECTORIES)
message(STATUS "hello_methodverse includes: ${incs}")

# # Add dependencies' include dirs
# target_include_directories(hello_methodverse
#     PRIVATE
#         ${eigen_SOURCE_DIR}
#         ${MP_UNITS_INCLUDE_DIR}
# 	      ${boost_mp11_SOURCE_DIR}/include
# )

# ----------------------------------------------------------------------
# Tests
# ----------------------------------------------------------------------
enable_testing()
add_subdirectory(tst)