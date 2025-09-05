# Header-only library
add_library(methodverse-parameter INTERFACE)

target_include_directories(methodverse-parameter
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(methodverse-parameter INTERFACE cxx_std_23)

target_link_libraries(methodverse-parameter 
    INTERFACE 
        methodverse_eigen_headers 
        methodverse_mpunits_headers 
        methodverse_fmt_headers 
        methodverse_boostmp11_headers
)
