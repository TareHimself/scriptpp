#include <iostream>
#include <fstream>
#include <filesystem>
#include "vscript/vscript.hpp"
using namespace vs;

void runRepl()
{
    auto program = runtime::makeProgramScope();
    std::string input;
    const auto scope = runtime::makeModuleScope(program);
    
    program->Assign("print",runtime::makeNativeFunction(program, "print", {},
    [](const runtime::TSmartPtrType<runtime::FunctionScope>& fnScope)
    {
    
        for(const auto &arg : fnScope->GetArgs())
        {
            std::cout << arg->ToString() << " ";
        }
        std::cout << std::endl;
    
        return runtime::makeNull();
    }));
    
    while(input != "quit")
    {
        try
        {
            std::getline(std::cin,input);
            if(input == "quit")
            {
                break;
            }
            
            if(!input.ends_with(";"))
            {
                input += ";";
            }
            std::list<parser::Token> tokens;
            tokenize(tokens,input);
            const auto ast = parser::parse(tokens);
            for(auto &statement : ast->statements)
            {
                if(const auto result = runtime::evalStatement(statement,scope.CastStatic<runtime::ScopeLike>()); result.IsValid())
                {
                    std::cout << result->ToString() << std::endl;
                }
            }
            
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
    
}

void profile(const std::string& name, const std::function<void()>& operation)
{
    const auto start = std::chrono::system_clock::now().time_since_epoch();
    operation();
    const auto diff = std::chrono::system_clock::now().time_since_epoch() - start;

    std::cout << "[profile] " << name + " -> " << std::chrono::duration_cast<std::chrono::milliseconds>(diff) << std::endl;
}

template<typename T>
T profile(const std::string& name, const std::function<T()>& operation)
{
    const auto start = std::chrono::system_clock::now().time_since_epoch();
    
    T r = operation();
    
    const auto diff = std::chrono::system_clock::now().time_since_epoch() - start;

    std::cout << "[profile] " << name + " -> " << std::chrono::duration_cast<std::chrono::milliseconds>(diff) << std::endl;

    return r;
}

int main()
{
    // runRepl();
    // return 0;
    //
    //
    try
    {
        parser::SourceStream contents;
        
        std::ifstream file("D:\\Github\\vscript\\list.vs", std::ios::binary);
        const std::string fileContent((std::istreambuf_iterator<char>(file)),
                                      std::istreambuf_iterator<char>());
        
        std::list<parser::Token> tokens;
        profile("tokenization",[&]
        {
            tokenize(tokens,fileContent);
        });
        
         const auto ast = profile<std::shared_ptr<parser::ModuleNode>>("ast",[=]
        {
            return parser::parse(tokens);
        });

        auto program = runtime::makeProgramScope();
    
        
        program->Assign("print",runtime::makeNativeFunction(program, "print", {},
                                                                [](runtime::TSmartPtrType<runtime::FunctionScope>& scope)
                                                                {
                                                                    for(const auto &arg : scope->GetArgs())
                                                                    {
                                                                        std::cout << arg->ToString() << " ";
                                                                    }
                                                                    std::cout << std::endl;
                                                        
                                                                    return runtime::makeNull();
                                                                }));
        program->Assign("input",runtime::makeNativeFunction(program, "input", {"prompt"},
                                                                [](runtime::TSmartPtrType<runtime::FunctionScope>& scope)
                                                                {
                                                                    if(auto prompt = scope->Find("prompt",false); prompt != runtime::makeNull())
                                                                    {
                                                                        std::cout << prompt->ToString() << std::endl;
                                                                    }
                                                                    
                                                                    std::string result;
                                                                    std::getline(std::cin,result);

                                                                    return runtime::makeString(result);
                                                                }));

        if(auto mod = api::importNative(R"(D:\Github\vscript\nativeModuleTest\build\Debug\native_module.vsnative)",program); mod.IsValid())
        {
            mod->GetScope()->Find("test").Cast<runtime::Function>()->Call();
        }
        const auto module = profile<runtime::TSmartPtrType<runtime::Module>>("eval",[&]
        {
            return evalModule(ast,runtime::makeModuleScope(program));
        });

        // if(const auto fn = std::dynamic_pointer_cast<runtime::Function>(module->GetScope()->Find("foo")))
        // {
        //     fn->Call();
        // }
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}