#include "scriptpp/runtime/Prototype.hpp"

#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/eval.hpp"

namespace spp::runtime
{

    Prototype::Prototype(const std::shared_ptr<ScopeLike>& scope) : DynamicObject(scope)
    {
        
    }

    void Prototype::Init()
    {
        DynamicObject::Init();
        std::vector<std::shared_ptr<frontend::ParameterNode>> args{};

        if(auto ctor = cast<Function>(resolveReference(Get(ReservedDynamicFunctions::CONSTRUCTOR))))
        {
            args = ctor->GetParameters();
        }
        AddNativeMemberFunction(ReservedDynamicFunctions::CALL,this,args,&Prototype::Construct);
    }


    std::string Prototype::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "<Prototype>";
    }

    std::shared_ptr<Object> Prototype::Construct(std::shared_ptr<FunctionScope>& scope)
    {
        return CreateInstance(scope);
    }

    bool Prototype::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return true;
    }

    size_t Prototype::GetHashCode(const std::shared_ptr<ScopeLike>& scope)
    {
        return hashCombine(DynamicObject::GetHashCode(scope),GetAddress());
    }

    RuntimePrototype::RuntimePrototype(const std::shared_ptr<ScopeLike>& scope,
                                       const std::shared_ptr<frontend::PrototypeNode>& prototype) : Prototype(scope)
    {
        _prototype = prototype;
        
    }
    

    std::shared_ptr<DynamicObject> RuntimePrototype::CreateInstance(std::shared_ptr<FunctionScope>& scope)
    {
        auto dynamicObj = createDynamicFromPrototype(_prototype,cast<RuntimePrototype>(this->GetRef()));
        
        if(dynamicObj->Has(ReservedDynamicFunctions::CONSTRUCTOR,false))
        {
            if(auto ctor = cast<Function>(dynamicObj->Find(ReservedDynamicFunctions::CONSTRUCTOR,false)); ctor && ctor->GetType() == EObjectType::Function)
            {
                ctor->HandleCall(scope);
            }
        }

        return dynamicObj;
    }

    std::string RuntimePrototype::GetName() const
    {
        return _prototype->id;
    }


    std::shared_ptr<RuntimePrototype> makePrototype(const std::shared_ptr<ScopeLike>& scope,
                                                    const std::shared_ptr<frontend::PrototypeNode>& prototype)
    {
        
        return makeObject<RuntimePrototype>(makeScope(scope),prototype);
    }


}
