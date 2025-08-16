#include <iostream>
//#include "parameter_registry.h"
#include "parameter_defs.h"

// test parameter_defs.h
DEFINE_TYPED_PARAMETER(TE, "TE", double, "ms");
DEFINE_TYPED_PARAMETER(TR, "TR", double, "ms");

int main() {

    TE te({10.0, 11.0, 12.0});
    TR tr(20.0);
    std::cout << "Hello, MethodVerse!" << std::endl;
     
    return 0;
}