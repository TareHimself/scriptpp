#include "scriptpp/runtime/Module.hpp"

#include "scriptpp/utils.hpp"

namespace spp::runtime
{
    Module::Module(const std::shared_ptr<Program>& scope) : DynamicObject(makeRefScopeProxy(castStatic<ScopeLike>(scope)))
    {
    }

    EObjectType Module::GetType() const
    {
        return EObjectType::Module;
    }

    std::string Module::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "module";
    }

    bool Module::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return true;
    }

    EScopeType Module::GetScopeType() const
    {
        return ST_Module;
    }

    size_t Module::GetHashCode(const std::shared_ptr<ScopeLike>& scope)
    {
        return hashCombine(DynamicObject::GetHashCode(scope),GetAddress());
    }

    std::shared_ptr<Module> makeModule(const std::shared_ptr<Program>& scope)
    {
        return makeObject<Module>(scope);
    }
}
