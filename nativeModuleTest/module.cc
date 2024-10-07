#include <iostream>

#include "scriptpp/api.hpp"
#include "scriptpp/frontend/frontend.hpp"
using namespace spp::frontend;
int main(){
    
}

std::shared_ptr<Module> CreateModule(const std::shared_ptr<Program>& program)
{
    auto mod = makeModule(program);
    mod->AddLambda("test",{},[](const std::shared_ptr<FunctionScope>& fs)
    {
        std::cout << "Hello from a native module function" << std::endl;
        return makeNull();
    });

    
    return mod;
}

SPP_API_MODULE(native_test,CreateModule)