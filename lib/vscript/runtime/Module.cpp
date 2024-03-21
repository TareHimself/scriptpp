#include "vscript/runtime/Module.hpp"
namespace vs::runtime
{
    Module::Module(const TSmartPtrType<ModuleScope>& scope)
    {
        _scope = scope;
    }

    EObjectType Module::GetType() const
    {
        return OT_Module;
    }

    std::string Module::ToString()
    {
        return "module";
    }

    bool Module::ToBoolean() const
    {
        return true;
    }

    TSmartPtrType<Scope> Module::GetScope() const
    {
        return _scope.CastStatic<ModuleScope>();
    }

    ModuleScope::ModuleScope(const TSmartPtrType<ProgramScope>& scope) : Scope(scope)
    {
        
    }

    EScopeType ModuleScope::GetScopeType() const
    {
        return ST_Module;
    }

    TSmartPtrType<ModuleScope> makeModuleScope(const TSmartPtrType<ProgramScope>& scope)
    {
        return manage<ModuleScope>(scope);
    }

    TSmartPtrType<Module> makeModule(const TSmartPtrType<ModuleScope>& scope)
    {
        return manage<Module>(scope);
    }
}
