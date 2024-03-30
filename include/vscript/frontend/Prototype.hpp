#pragma once
#include "DynamicObject.hpp"
#include "Function.hpp"
#include "Object.hpp"
#include "Scope.hpp"
#include "vscript/backend/ast.hpp"

namespace vs::frontend
{
    class Prototype : public DynamicObject
    {
        
        
    public:
        Prototype(const std::shared_ptr<ScopeLike>& scope);

        Prototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<NativeFunction>& func);
        
        std::string ToString() const override;
        bool ToBoolean() const override;
    };

    class RuntimePrototype : public Prototype
    {
        
        std::shared_ptr<backend::PrototypeNode> _prototype;

    public:
        RuntimePrototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<backend::PrototypeNode>& prototype);

        std::string ToString() const override;

        std::shared_ptr<Object> Create(std::shared_ptr<FunctionScope>& fnScope);
    };
    

    std::shared_ptr<RuntimePrototype> makePrototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<backend::PrototypeNode>& prototype);

    std::shared_ptr<Prototype> makePrototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<NativeFunction>& func);
}
