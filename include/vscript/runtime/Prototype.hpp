#pragma once
#include "DynamicObject.hpp"
#include "Function.hpp"
#include "Object.hpp"
#include "Scope.hpp"
#include "vscript/parser/ast.hpp"

namespace vs::runtime
{
    class Prototype : public DynamicObject
    {
        
        
    public:
        Prototype(const TSmartPtrType<ScopeLike>& scope);

        Prototype(const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<NativeFunction>& func);
        
        std::string ToString() override;
        bool ToBoolean() const override;
    };

    class RuntimePrototype : public Prototype
    {
        
        std::shared_ptr<parser::PrototypeNode> _prototype;

    public:
        RuntimePrototype(const TSmartPtrType<ScopeLike>& scope,const std::shared_ptr<parser::PrototypeNode>& prototype);

        std::string ToString() override;

        TSmartPtrType<Object> Create(TSmartPtrType<FunctionScope>& fnScope);
    };
    

    TSmartPtrType<RuntimePrototype> makePrototype(const TSmartPtrType<ScopeLike>& scope,const std::shared_ptr<parser::PrototypeNode>& prototype);

    TSmartPtrType<Prototype> makePrototype(const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<NativeFunction>& func);
}
