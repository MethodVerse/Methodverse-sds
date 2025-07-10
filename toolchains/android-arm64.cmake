# toolchains/android-arm64.cmake
#
# Purpose:
#   CMake toolchain file for cross-compiling Android native binaries targeting arm64 or x86_64.
#   This file is intended for use in CI pipelines and local builds.
#
# How it works:
#   - Uses environment variable ANDROID_NDK (set by CI or user) to locate the NDK.
#   - Sets up platform, ABI, and toolchain paths for Android cross-compilation.
#   - Automatically determines the host platform for the correct toolchain.
#   - Fails early if the NDK path is not set or invalid.
#
# Why use environment variables?
#   - Allows CI and developers to control which NDK version is used without editing this file.
#   - In CI, ANDROID_NDK is set automatically to the latest installed version.
#   - In local builds, users can set ANDROID_NDK to match their environment.
#

# Core platform settings
set(CMAKE_SYSTEM_NAME Android)
if(DEFINED ENV{ANDROID_API_LEVEL})
  set(CMAKE_SYSTEM_VERSION $ENV{ANDROID_API_LEVEL} CACHE STRING "Android API level")
else()
  set(CMAKE_SYSTEM_VERSION 29 CACHE STRING "Android API level")
endif()
set(CMAKE_ANDROID_ARCH_ABI x86_64)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Detect and validate NDK path
if(NOT DEFINED ANDROID_NDK AND DEFINED ENV{ANDROID_NDK})
   set(ANDROID_NDK $ENV{ANDROID_NDK})
endif()

if(NOT EXISTS "${ANDROID_NDK}")
  message(FATAL_ERROR "ANDROID_NDK not found or invalid. Set ANDROID_NDK environment variable.")
endif()

set(CMAKE_ANDROID_NDK "${ANDROID_NDK}" CACHE PATH "Path to Android NDK")
set(CMAKE_ANDROID_STL_TYPE c++_static)
set(CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION clang)

# Determine host platform tag
if(WIN32)
  set(ANDROID_HOST_TAG windows-x86_64)
elseif(APPLE)
  set(ANDROID_HOST_TAG darwin-x86_64)
elseif(UNIX)
  set(ANDROID_HOST_TAG linux-x86_64)
else()
  message(FATAL_ERROR "Unsupported host platform")
endif()

# Set compiler paths based on API and ABI
set(CMAKE_C_COMPILER   "${ANDROID_NDK}/toolchains/llvm/prebuilt/${ANDROID_HOST_TAG}/bin/aarch64-linux-android${CMAKE_SYSTEM_VERSION}-clang")
set(CMAKE_CXX_COMPILER "${ANDROID_NDK}/toolchains/llvm/prebuilt/${ANDROID_HOST_TAG}/bin/aarch64-linux-android${CMAKE_SYSTEM_VERSION}-clang++")