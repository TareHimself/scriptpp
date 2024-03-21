#pragma once
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "Object.hpp"
#include "Scope.hpp"

namespace vs::runtime
{
    class ModuleScope;

    class Module : public Object
    {
        TSmartPtrType<ModuleScope> _scope;
    public:
        Module(const TSmartPtrType<ModuleScope>& scope);
        EObjectType GetType() const override;
        std::string ToString() override;
        bool ToBoolean() const override;
        TSmartPtrType<Scope> GetScope() const;
    };

    class ModuleScope : public Scope
    {
    public:
        ModuleScope(const TSmartPtrType<ProgramScope>& scope);
        EScopeType GetScopeType() const override;
    };

    TSmartPtrType<ModuleScope> makeModuleScope(const TSmartPtrType<ProgramScope>& scope);
    TSmartPtrType<Module> makeModule(const TSmartPtrType<ModuleScope>& scope);
}
