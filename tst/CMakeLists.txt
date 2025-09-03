include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(parameterbase_test parameterbase_test.cpp)
target_include_directories(parameterbase_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${eigen_SOURCE_DIR} ${MP_UNITS_INCLUDE_DIR} ${boost_mp11_SOURCE_DIR}/include)
target_link_libraries(parameterbase_test gtest_main fmt::fmt)
add_test(NAME parameterbase_test COMMAND parameterbase_test)

add_executable(operation_policy_test operation_policy_test.cpp)
target_include_directories(operation_policy_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${eigen_SOURCE_DIR} ${MP_UNITS_INCLUDE_DIR} ${boost_mp11_SOURCE_DIR}/include)
target_link_libraries(operation_policy_test gtest_main fmt::fmt)
add_test(NAME operation_policy_test COMMAND operation_policy_test)