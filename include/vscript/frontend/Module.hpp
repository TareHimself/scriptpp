#pragma once

#include <string>
#include "Program.hpp"
#include "Scope.hpp"

namespace vs::frontend
{
    class ModuleScope;

    class Module : public DynamicObject
    {
        
    public:
        Module(const std::shared_ptr<Program>& scope);
        EObjectType GetType() const override;
        std::string ToString() const override;
        bool ToBoolean() const override;
        EScopeType GetScopeType() const override;
    };

    // class ModuleScope : public Scope
    // {
    // public:
    //     ModuleScope(const std::shared_ptr<Program>& scope);
    //     EScopeType GetScopeType() const override;
    // };

    // std::shared_ptr<ModuleScope> makeModuleScope(const std::shared_ptr<Program>& scope);
    std::shared_ptr<Module> makeModule(const std::shared_ptr<Program>& scope);
}
