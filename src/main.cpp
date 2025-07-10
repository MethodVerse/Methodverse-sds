#include <iostream>
//#include "parameter_registry.h"
#include "parameter_defs.h"

// // test parameter.h and parameter_registry.h
// REGISTER_PARAMETER(TE, "TE");
// REGISTER_PARAMETER(TR, "TR");
// REGISTER_PARAMETER(FlipAngle, "Flip Angle");
// REGISTER_PARAMETER(SliceThickness, "Slice Thickness");
// REGISTER_PARAMETER(LoopCounterPE, "Loop Counter PE");
// REGISTER_PARAMETER(TestEnum, "Test Enum");    


// test parameter_defs.h
DEFINE_TYPED_PARAMETER(TE, "TE", double);
DEFINE_TYPED_PARAMETER(TR, "TR", double);

int main() {

    // test parameter_defs.h
    TE te({10.0, 11.0, 12.0});
    TR tr(20.0);
    double x = static_cast<double>(te);
    x = 0.5;
    te[1] = x;
    auto y = te[2];
    auto z = static_cast<double>(te);
    std::cout << "TE value: " << te[0] << ", TR value: " << tr[0] << std::endl;
    auto a = std::vector<int>{1,2,3} + 2;
    // // test parameter.h and parameter_registry.h
    // std::cout << "TE::Name is " << TE::name << std::endl;
    // std::cout << "TE::ID is " << TE::id << std::endl;
    // std::cout << "TR::Name is " << TR::name << std::endl;
    // std::cout << "TR::ID is " << TR::id << std::endl;
    // std::cout << "Hello, MethodVerse!" << std::endl;

    // std::cout << "LoopCounterPE info:" << std::endl;
    // std::cout << "Name: " << LoopCounterPE::name << std::endl;
    // std::cout << "ID: " << LoopCounterPE::id << std::endl;
    // std::cout << "Hash: " << LoopCounterPE::hash << std::endl;

    // TE te(10.0);
    // TR tr(20.0);
    // te.addObserver(&tr);
    // tr.setNotificationHandler<TE>([](const Parameter& notifier) {
    //     double x = notifier;
    //     std::cout << "TR notified by TE: " << x << std::endl;
    // });
    // te.notifyObservers();

    // enum class WhateverEnum {
    //     Value1,
    //     Value2,
    //     Value3
    // };

    // TestEnum testEnum;
    // testEnum.setEnum(WhateverEnum::Value1);
    
    // WhateverEnum enumValue = testEnum.getEnum<WhateverEnum>();
    // assert(enumValue == WhateverEnum::Value1);
    // // test parameter.h and parameter_registry.h
    
    return 0;
}