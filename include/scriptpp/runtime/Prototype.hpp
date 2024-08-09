#pragma once
#include "DynamicObject.hpp"
#include "Function.hpp"
#include "Object.hpp"
#include "Scope.hpp"
#include "scriptpp/frontend/parser.hpp"

namespace spp::runtime
{
    class Prototype : public DynamicObject
    {
        
        
    public:
        Prototype(const std::shared_ptr<ScopeLike>& scope);

        Prototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<NativeFunction>& func);
        
        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;
    };

    class RuntimePrototype : public Prototype
    {
        
        std::shared_ptr<frontend::PrototypeNode> _prototype;

    public:
        RuntimePrototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<frontend::PrototypeNode>& prototype);

        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;

        std::shared_ptr<Object> Create(std::shared_ptr<FunctionScope>& fnScope);
    };
    

    std::shared_ptr<RuntimePrototype> makePrototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<frontend::PrototypeNode>& prototype);

    std::shared_ptr<Prototype> makePrototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<NativeFunction>& func);
}
