#include "vscript/runtime/Program.hpp"

#include <fstream>

#include "vscript/api.hpp"
#include "vscript/utils.hpp"
#include "vscript/frontend/Tokenizer.hpp"
#include "vscript/runtime/Error.hpp"
#include "vscript/runtime/eval.hpp"
#include "vscript/runtime/Null.hpp"

namespace vs::runtime
{
    Program::Program() : DynamicObject({})
    {
    }

    void Program::Init()
    {
        DynamicObject::Init();
        AddLambda("import",{"moduleId"},[this](const std::shared_ptr<FunctionScope>& scope)
        {
            return Import(scope);
        });
        
        AddLambda("cwd",{},[this](const std::shared_ptr<FunctionScope>& scope)
        {
            return GetCwd(scope);
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

        if(absPath.extension() == ".vsnative")
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
        
        
        frontend::Tokenized tokens;
        tokenize(tokens,path);

        for(auto &token : tokens.GetList())
        {
            token.debugInfo.file = path.string();
        }

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
