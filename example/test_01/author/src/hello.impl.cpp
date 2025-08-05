#include "hello.proj.hpp"
#include "hello.impl.hpp"

#include <iostream>

void sayHello() {
    std::cout << "Hello, World!" << std::endl;
    return;
}

TH_EXPORT_CPP_API_sayHello(sayHello);
