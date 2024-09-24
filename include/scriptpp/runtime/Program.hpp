#pragma once
#include <filesystem>

#include "DynamicObject.hpp"
#include "Scope.hpp"

namespace spp::runtime
{
    class Module;
    class ProgramScope;

    class Program : public DynamicObject
    {
        std::unordered_map<std::string,std::shared_ptr<Module>> _modules;
        std::filesystem::path _cwd = std::filesystem::current_path();
    public:
        Program();

        void Init() override;
        virtual std::shared_ptr<Module> ImportModule(const std::string& id);

        virtual std::shared_ptr<Module> ModuleFromFile(const std::filesystem::path& path);

        virtual std::shared_ptr<Module> ImportModule(std::shared_ptr<FunctionScope>& scope, const std::string& id);

        virtual std::shared_ptr<Object> Import(const std::shared_ptr<FunctionScope>& scope);

        virtual std::shared_ptr<Object> GetCwd(const std::shared_ptr<FunctionScope>& scope);
        
        virtual std::shared_ptr<Object> Eval(const std::shared_ptr<FunctionScope>& scope);

        virtual std::shared_ptr<Object> Eval(std::string& expression);
        
        std::shared_ptr<Object> Find(const std::string& id, bool searchParent) const override;

        size_t GetHashCode(const std::shared_ptr<ScopeLike>& scope) override;
    };
    

    std::shared_ptr<Program> makeProgram();
}
