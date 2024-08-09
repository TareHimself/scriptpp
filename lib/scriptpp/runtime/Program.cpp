#include "scriptpp/runtime/Program.hpp"

#include <fstream>

#include "scriptpp/api.hpp"
#include "scriptpp/utils.hpp"
#include "scriptpp/frontend/tokenizer.hpp"
#include "scriptpp/runtime/Exception.hpp"
#include "scriptpp/runtime/eval.hpp"
#include "scriptpp/runtime/Null.hpp"

namespace spp::runtime
{
    Program::Program() : DynamicObject({})
    {
    }

    void Program::Init()
    {
        DynamicObject::Init();

        // For when statements
        Set("else",makeBoolean(true));

        // import("foo.spp");
        AddLambda("import",{"moduleId"},[this](const std::shared_ptr<FunctionScope>& scope)
        {
            return Import(scope);
        });
        
        AddLambda("cwd",{},[this](const std::shared_ptr<FunctionScope>& scope)
        {
            return GetCwd(scope);
        });

        AddLambda("eval",{"expr"},[this](const std::shared_ptr<FunctionScope>& scope)
        {
            return Eval(scope);
        });

        Set("List",List::Prototype);
    }

    std::shared_ptr<Module> Program::ImportModule(const std::string& id)
    {
        if(_modules.contains(id))
        {
            return _modules[id];
        }

        const auto absPath = absolute(std::filesystem::path(id));
        auto targetPath = absPath.string();
        targetPath = targetPath.substr(0,targetPath.size() - absPath.extension().string().size());

        if(_modules.contains(targetPath))
        {
            return _modules[targetPath];
        }

        if(!std::filesystem::exists(absPath))
        {
            throw std::runtime_error("File does not exist");
        }

        if(absPath.extension() == ".sppn")
        {
            auto scope = cast<Program>(this->GetRef());
            auto native = api::importNative(absPath,scope);
            if(native)
            {
                _modules[targetPath] = native;
            }
            return native;
        }
        
        auto mod = ModuleFromFile(absPath);

        if(mod)
        {
            _modules[targetPath] = mod;
        }
        
        return mod;
    }

    std::shared_ptr<Module> Program::ModuleFromFile(const std::filesystem::path& path)
    {
        auto tokens = frontend::tokenize(path);

        const auto ast = parse(tokens);

        return evalModule(ast,cast<Program>(this->GetRef()));
    }

    std::shared_ptr<Module> Program::ImportModule(std::shared_ptr<FunctionScope>& scope, const std::string& id)
    {
        return ImportModule(id);
    }

    std::shared_ptr<Object> Program::Import(const std::shared_ptr<FunctionScope>& scope)
    {
        if(const auto mod = this->ImportModule(scope->Find("moduleId")->ToString(scope)))
        {
            return mod;
        }

        return makeNull();
    }

    std::shared_ptr<Object> Program::GetCwd(const std::shared_ptr<FunctionScope>& scope)
    {
        return makeString(std::string(_cwd.string()));
    }

    std::shared_ptr<Object> Program::Eval(const std::shared_ptr<FunctionScope>& scope)
    {
        auto txt = scope->FindArg("expr")->ToString();
        return Eval(txt);
    }

    std::shared_ptr<Object> Program::Eval(std::string& expression)
    {
        auto output = frontend::tokenize(expression,"<eval>");
        const auto ast = frontend::parse(output);

        auto self = cast<Program>(this->GetRef());
        
        auto mod = makeModule(self);

        std::shared_ptr<Object> result = makeNull();
        
        for (auto& statement : ast->statements)
        {
            result = evalStatement(statement, mod);
        }
        
        return result;
    }

    std::shared_ptr<Object> Program::Find(const std::string& id, bool searchParent) const
    {
        if(id == "program")
        {
            return this->GetRef();
        }
        
        return DynamicObject::Find(id, searchParent);
    }

    std::shared_ptr<Program> makeProgram()
    {
        return makeObject<Program>();
    }
}
