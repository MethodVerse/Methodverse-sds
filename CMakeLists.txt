cmake_minimum_required(VERSION 3.20) 
project(MethodVerse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# ----------------------------------------------------------------------
# vendor Eigen, Boost.MP11, fmt, mp-units (header-only libraries)
# ----------------------------------------------------------------------
add_library(methodverse_eigen_headers INTERFACE)

target_include_directories(methodverse_eigen_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/eigen>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/eigen>  # <prefix>/include/eigen
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/eigen DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) # install to <prefix>/include

add_library(methodverse_boostmp11_headers INTERFACE)

target_include_directories(methodverse_boostmp11_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/mp11/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mp11>
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/mp11 DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) # install to <prefix>/include

add_library(methodverse_fmt_headers INTERFACE)

target_include_directories(methodverse_fmt_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/fmt/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/fmt>
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/fmt DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) # install to <prefix>/include

add_library(methodverse_mpunits_headers INTERFACE)

target_include_directories(methodverse_mpunits_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/mp-units/core/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/mp-units/systems/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mp-units/core/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/mp-units/systems/include>
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/mp-units DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}) # install to <prefix>/include

add_compile_definitions( MP_UNITS_USE_FMTLIB=1 ) # <= enable fmt support for mp-units

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
)

# ----------------------------------------------------------------------
# Tests
# ----------------------------------------------------------------------
enable_testing()

add_subdirectory(tst)