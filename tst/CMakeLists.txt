include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(parameter_test parameter_test.cpp ../src/parameter.cpp)
target_include_directories(parameter_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${eigen_SOURCE_DIR})
target_link_libraries(parameter_test gtest_main)
add_test(NAME parameter_test COMMAND parameter_test)

add_executable(parameter_defs_test parameter_defs_test.cpp)
target_include_directories(parameter_defs_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${eigen_SOURCE_DIR})
target_link_libraries(parameter_defs_test gtest_main)
add_test(NAME parameter_defs_test COMMAND parameter_defs_test)

