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
        
        void Init() override;
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;

        size_t GetHashCode(const std::shared_ptr<ScopeLike>& scope) override;

        std::string ToString(const std::shared_ptr<ScopeLike>&) const override;

        std::shared_ptr<Object> Construct(std::shared_ptr<FunctionScope>& scope);
        virtual std::shared_ptr<DynamicObject> CreateInstance(std::shared_ptr<FunctionScope>& scope) = 0;
        virtual std::string GetName() const = 0;
    };

    class RuntimePrototype : public Prototype
    {
        
        std::shared_ptr<frontend::PrototypeNode> _prototype;

    public:
        RuntimePrototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<frontend::PrototypeNode>& prototype);
        
        std::shared_ptr<DynamicObject> CreateInstance(std::shared_ptr<FunctionScope>& scope) override;

        std::string GetName() const override;
    };
    
    std::shared_ptr<RuntimePrototype> makePrototype(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<frontend::PrototypeNode>& prototype);
}
