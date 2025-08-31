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

add_executable(parameterbase_test parameterbase_test.cpp)
target_include_directories(parameterbase_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${eigen_SOURCE_DIR} ${MP_UNITS_INCLUDE_DIR} ${boost_mp11_SOURCE_DIR}/include)
target_link_libraries(parameterbase_test gtest_main fmt::fmt)
add_test(NAME parameterbase_test COMMAND parameterbase_test)

add_executable(primitive_operators_test primitive_operators_test.cpp)
target_include_directories(primitive_operators_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${eigen_SOURCE_DIR} ${MP_UNITS_INCLUDE_DIR} ${boost_mp11_SOURCE_DIR}/include)
target_link_libraries(primitive_operators_test gtest_main fmt::fmt)
add_test(NAME primitive_operators_test COMMAND primitive_operators_test)

add_executable(primitive_operators_test1 primitive_operators_test1.cpp)
target_include_directories(primitive_operators_test1 PRIVATE ${CMAKE_SOURCE_DIR}/include ${eigen_SOURCE_DIR} ${MP_UNITS_INCLUDE_DIR} ${boost_mp11_SOURCE_DIR}/include)
target_link_libraries(primitive_operators_test1 gtest_main fmt::fmt)
add_test(NAME primitive_operators_test1 COMMAND primitive_operators_test1)