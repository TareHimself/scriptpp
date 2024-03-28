#pragma once
#include <filesystem>

#include "DynamicObject.hpp"
#include "Scope.hpp"

namespace vs::frontend
{
    class Module;
    class ProgramScope;

    class Program : public DynamicObject
    {
        std::unordered_map<std::string,TSmartPtrType<Module>> _modules;
        std::filesystem::path _cwd = std::filesystem::current_path();

    protected:
        void OnRefSet() override;
    public:
        Program();

        

        virtual TSmartPtrType<Module> ImportModule(const std::string& id);

        virtual TSmartPtrType<Module> ModuleFromFile(const std::filesystem::path& path);

        virtual TSmartPtrType<Module> ImportModule(TSmartPtrType<FunctionScope>& scope, const std::string& id);

        TSmartPtrType<Object> Find(const std::string& id, bool searchParent) override;
    };
    

    TSmartPtrType<Program> makeProgram();
}
