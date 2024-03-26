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
        Module(const TSmartPtrType<Program>& scope);
        EObjectType GetType() const override;
        std::string ToString() override;
        bool ToBoolean() const override;
        EScopeType GetScopeType() const override;
    };

    // class ModuleScope : public Scope
    // {
    // public:
    //     ModuleScope(const TSmartPtrType<Program>& scope);
    //     EScopeType GetScopeType() const override;
    // };

    TSmartPtrType<ModuleScope> makeModuleScope(const TSmartPtrType<Program>& scope);
    TSmartPtrType<Module> makeModule(const TSmartPtrType<ModuleScope>& scope);
}
