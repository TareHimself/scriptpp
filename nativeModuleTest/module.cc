#include <iostream>

#include "vscript/api.hpp"
#include "vscript/frontend/frontend.hpp"
using namespace vs::frontend;
int main(){
    
}

TSmartPtrType<Module> CreateModule(const TSmartPtrType<Program>& scope)
{
    auto mod = makeModule(scope);
    mod->AddLambda("test",{},[](const TSmartPtrType<FunctionScope>& fs)
    {
        std::cout << "Hello from a native module function" << std::endl;
        return makeNull();
    });
    return mod;
}

VS_API_MODULE(native_test,CreateModule)