#pragma once

#include <string>
#include "Program.hpp"
#include "Scope.hpp"

namespace spp::runtime
{
    class ModuleScope;

    class Module : public DynamicObject
    {
        
    public:
        Module(const std::shared_ptr<Program>& scope);
        EObjectType GetType() const override;
        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;
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
