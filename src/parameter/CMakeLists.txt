# Header-only library
add_library(methodverse-parameter INTERFACE)

target_include_directories(methodverse-parameter
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
          # pull includes from each dep target
        # $<TARGET_PROPERTY:methodverse_eigen_headers,INTERFACE_INCLUDE_DIRECTORIES>
        # $<TARGET_PROPERTY:methodverse_mpunits_headers,INTERFACE_INCLUDE_DIRECTORIES>
        # $<TARGET_PROPERTY:methodverse_fmt_headers,INTERFACE_INCLUDE_DIRECTORIES>
        # $<TARGET_PROPERTY:methodverse_boostmp11_headers,INTERFACE_INCLUDE_DIRECTORIES>
)

# target_compile_definitions(methodverse-parameter INTERFACE MP_UNITS_STD_FMT=fmt)

target_compile_features(methodverse-parameter INTERFACE cxx_std_23)

target_link_libraries(methodverse-parameter 
    INTERFACE 
        methodverse_eigen_headers 
        methodverse_mpunits_headers 
        methodverse_fmt_headers 
        methodverse_boostmp11_headers
)

target_include_directories(methodverse-parameter INTERFACE
    ${CMAKE_SOURCE_DIR}/external/mp-units/core/include
    ${CMAKE_SOURCE_DIR}/external/mp-units/systems/include
)

get_target_property(incs methodverse-parameter INCLUDE_DIRECTORIES)
message(STATUS "methodverse-parameter includes: ${incs}")