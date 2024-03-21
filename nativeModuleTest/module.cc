#include <iostream>

#include "vscript/api.hpp"
#include "vscript/runtime/Function.hpp"
#include "vscript/runtime/Null.hpp"
using namespace vs::runtime;
int main(){
    
}

TSmartPtrType<Module> CreateModule(const TSmartPtrType<ProgramScope>& scope)
{
    auto newScope = makeModuleScope(scope);
    auto mod = makeModule(newScope);
    newScope->Create("test",vs::runtime::makeNativeFunction(newScope,"test",{},[](const TSmartPtrType<FunctionScope>& fs)
    {
        std::cout << "Hello from a native module function" << std::endl;
        return makeNull();
    }));
    return mod;
}

VS_API_MODULE(native_test,CreateModule)