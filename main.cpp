#include <iostream>
#include <fstream>
#include <filesystem>
#include "vscript/vscript.hpp"

using namespace vs;

void runRepl()
{
        auto program = frontend::makeProgram();

        program->AddLambda("print", {}, [](frontend::TSmartPtrType<frontend::FunctionScope>& scope)
        {
            for (const auto& arg : scope->GetArgs())
            {
                auto b = arg->ToBoolean();
                auto s = arg->ToString();
                std::cout << arg->ToString() << " ";
            }
            std::cout << std::endl;
        
            return frontend::makeNull();
        });
        
        program->AddLambda("input", {"prompt"}, [](frontend::TSmartPtrType<frontend::FunctionScope>& scope)
        {
            if (auto prompt = scope->Find("prompt", false); prompt != frontend::makeNull())
            {
                std::cout << prompt->ToString() << std::endl;
            }
        
            std::string result;
            std::getline(std::cin, result);
        
            return frontend::makeString(result);
        });
        
    std::string input;
    const auto mod = frontend::makeModule(program);

    program->AddLambda("print", {}, [](frontend::TSmartPtrType<frontend::FunctionScope>& scope)
        {
            for (const auto& arg : scope->GetArgs())
            {
                std::cout << arg->ToString() << " ";
            }
            std::cout << std::endl;

            return frontend::makeNull();
        });

    while (input != "quit")
    {
        try
        {
            std::getline(std::cin, input);
            if (input == "quit")
            {
                break;
            }

            if (!input.ends_with(";"))
            {
                input += ";";
            }
            std::list<backend::Token> tokens;
            tokenize(tokens, input);
            const auto ast = backend::parse(tokens);
            for (auto& statement : ast->statements)
            {
                if (const auto result = frontend::evalStatement(statement, mod);
                    result.IsValid())
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

    std::cout << "[profile] " << name + " -> " << std::chrono::duration_cast<std::chrono::milliseconds>(diff) <<
        std::endl;
}

template <typename T>
T profile(const std::string& name, const std::function<T()>& operation)
{
    const auto start = std::chrono::system_clock::now().time_since_epoch();

    T r = operation();

    const auto diff = std::chrono::system_clock::now().time_since_epoch() - start;

    std::cout << "[profile] " << name + " -> " << std::chrono::duration_cast<std::chrono::milliseconds>(diff) <<
        std::endl;

    return r;
}

int main()
{
    runRepl();
    return 0;
    // try
    // {
    
    //     auto program = frontend::makeProgram();

    //     program->AddLambda("print", {}, [](frontend::TSmartPtrType<frontend::FunctionScope>& scope)
    //     {
    //         for (const auto& arg : scope->GetArgs())
    //         {
    //             auto b = arg->ToBoolean();
    //             auto s = arg->ToString();
    //             std::cout << arg->ToString() << " ";
    //         }
    //         std::cout << std::endl;
        
    //         return frontend::makeNull();
    //     });
        
    //     program->AddLambda("input", {"prompt"}, [](frontend::TSmartPtrType<frontend::FunctionScope>& scope)
    //     {
    //         if (auto prompt = scope->Find("prompt", false); prompt != frontend::makeNull())
    //         {
    //             std::cout << prompt->ToString() << std::endl;
    //         }
        
    //         std::string result;
    //         std::getline(std::cin, result);
        
    //         return frontend::makeString(result);
    //     });
        
        
        
    //     // if (auto mod = program->ImportModule(R"(D:\Github\vscript\nativeModuleTest\build\Debug\native_module.vsnative)"); mod.IsValid())
    //     // {
    //     //     mod->GetScope()->Find("test").Cast<runtime::Function>()->Call();
    //     // }
        
    //     const auto module = program->ImportModule("D:\\Github\\vscript\\list.vs");
        
    //     // if(const auto fn = std::dynamic_pointer_cast<runtime::Function>(module->GetScope()->Find("foo")))
    //     // {
    //     //     fn->Call();
    //     // }
    // }
    // catch (std::runtime_error& e)
    // {
    //     std::cerr << e.what() << std::endl;
    // }
    return 0;
}
