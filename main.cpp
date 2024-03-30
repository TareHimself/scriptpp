#include <iostream>
#include <fstream>
#include <filesystem>
#include "vscript/vscript.hpp"
#include "vscript/frontend/Error.hpp"

using namespace vs;

void runRepl()
{
        auto program = frontend::makeProgram();

        program->AddLambda("print", {}, [](std::shared_ptr<frontend::FunctionScope>& scope)
        {
            for (const auto& arg : scope->GetArgs())
            {
                auto s = arg->ToString();
                std::cout << arg->ToString() << " ";
            }
            std::cout << std::endl;
        
            return frontend::makeNull();
        });
        
        program->AddLambda("input", {"prompt"}, [](std::shared_ptr<frontend::FunctionScope>& scope)
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
            backend::Tokenized tokens;
            tokenize(tokens, input,"<terminal>");
            const auto ast = backend::parse(tokens);
            for (auto& statement : ast->statements)
            {
                if (const auto result = frontend::evalStatement(statement, mod))
                {
                    std::cout << result->ToString() << std::endl;
                }
            }
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
        catch (std::shared_ptr<frontend::Error>& e)
        {
            std::cerr << e->ToString() << std::endl;
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
    
        auto program = frontend::makeProgram();

        program->AddLambda("print", {}, [](std::shared_ptr<frontend::FunctionScope>& scope)
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
        
        program->AddLambda("input", {"prompt"}, [](std::shared_ptr<frontend::FunctionScope>& scope)
        {
            if (const auto prompt = scope->Find("prompt", false); prompt != frontend::makeNull())
            {
                std::cout << prompt->ToString() << std::endl;
            }
        
            std::string result;
            std::getline(std::cin, result);
        
            return frontend::makeString(result);
        });

        const auto path = absolute(std::filesystem::path(args[0]));
        const auto module = program->ImportModule(path.string());
        
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (std::shared_ptr<frontend::Error>& e)
    {
        std::cerr << e->ToString() << std::endl;
    }
    return 0;
}
