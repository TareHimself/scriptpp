#include <iostream>
#include <fstream>
#include <filesystem>
#include "scriptpp/scriptpp.hpp"
#include "scriptpp/runtime/Exception.hpp"
#include "scriptpp/runtime/Number.hpp"

using namespace spp;

int64_t nowSeconds()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}


std::shared_ptr<runtime::Object> NowScriptPP(const std::shared_ptr<runtime::FunctionScope>& scope)
{
    return runtime::makeNumber(nowSeconds());
}

void runRepl()
{
        auto program = runtime::makeProgram();

        program->AddLambda("print", {}, [](const std::shared_ptr<runtime::FunctionScope>& scope)
        {
            for (const auto& arg : scope->GetPositionalArgs())
            {
                auto s = arg->ToString(scope);
                std::cout << arg->ToString(scope) << " ";
            }
            std::cout << '\n';
        
            return runtime::makeNull();
        });
        
        program->AddLambda("input", {"prompt"}, [](const std::shared_ptr<runtime::FunctionScope>& scope)
        {
            if (const auto prompt = scope->Find("prompt", false); prompt != runtime::makeNull())
            {
                std::cout << prompt->ToString(scope) << '\n';
            }
        
            std::string result;
            std::getline(std::cin, result);
        
            return runtime::makeString(result);
        });
        
    std::string input;
    const auto mod = runtime::makeModule(program);

    while (input != "quit")
    {
        try
        {
            std::getline(std::cin, input);
            if (input == "quit")
            {
                break;
            }

            input = trim(input);
            
            if (!input.empty() && !input.ends_with(";"))
            {
                input += ";";
            }
            auto tokens = frontend::tokenize(input,"<terminal>");
            const auto ast = frontend::parse(tokens);
            for (auto& statement : ast->statements)
            {
                if (const auto result = runtime::evalStatement(statement, mod))
                {
                    std::cout << result->ToString() << '\n';
                }
            }
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

void profile(const std::string& name, const std::function<void()>& operation)
{
    const auto start = std::chrono::system_clock::now().time_since_epoch();
    operation();
    const auto diff = std::chrono::system_clock::now().time_since_epoch() - start;

    std::cout << "[profile] " << name + " -> " << std::chrono::duration_cast<std::chrono::milliseconds>(diff) <<
        '\n';
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

int main(const int argc, char *argv[])
{
    std::vector<std::string> args;

    if (argc > 1) {
        args.assign(argv + 1, argv + argc);
    }

    if(args.empty())
    {
        runRepl();
        return 0;
    }
    
    try
    {
        const auto program = runtime::makeProgram();

        program->AddLambda("print", {}, [](const std::shared_ptr<runtime::FunctionScope>& scope)
        {
            for (const auto& arg : scope->GetPositionalArgs())
            {
                auto s = arg->ToString(scope);
                std::cout << arg->ToString(scope) << " ";
            }
            std::cout << '\n';
        
            return runtime::makeNull();
        });

        program->AddLambda("now",{},NowScriptPP);
        
        program->AddLambda("input", {"prompt"}, [](const std::shared_ptr<runtime::FunctionScope>& scope)
        {
            if (const auto prompt = scope->Find("prompt", false); prompt != runtime::makeNull())
            {
                std::cout << prompt->ToString(scope) << '\n';
            }
        
            std::string result;
            std::getline(std::cin, result);
        
            return runtime::makeString(result);
        });

        const auto path = absolute(std::filesystem::path(args[0]));
        const auto mod = program->ImportModule(path.string());
        
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
