#include "vscript/frontend/Module.hpp"
namespace vs::frontend
{
    Module::Module(const TSmartPtrType<Program>& scope) : DynamicObject(makeRefScopeProxy(scope.CastStatic<ScopeLike>()))
    {
    }

    EObjectType Module::GetType() const
    {
        return OT_Module;
    }

    std::string Module::ToString() const
    {
        return "module";
    }

    bool Module::ToBoolean() const
    {
        return true;
    }

    EScopeType Module::GetScopeType() const
    {
        return ST_Module;
    }

    // ModuleScope::ModuleScope(const TSmartPtrType<Program>& scope) : Scope(makeRefScopeProxy(scope.CastStatic<ScopeLike>()))
    // {
    //     
    // }
    //
    // EScopeType ModuleScope::GetScopeType() const
    // {
    //     return ST_Module;
    // }
    //
    // TSmartPtrType<ModuleScope> makeModuleScope(const TSmartPtrType<Program>& scope)
    // {
    //     return manage<ModuleScope>(scope);
    // }

    TSmartPtrType<Module> makeModule(const TSmartPtrType<Program>& scope)
    {
        return manage<Module>(scope);
    }
}
