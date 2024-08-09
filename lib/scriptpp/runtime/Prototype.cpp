#include "scriptpp/runtime/Prototype.hpp"

#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/eval.hpp"

namespace spp::runtime
{

    Prototype::Prototype(const std::shared_ptr<ScopeLike>& scope) : DynamicObject(scope)
    {
        
    }

    Prototype::Prototype(const std::shared_ptr<ScopeLike>& scope, const std::shared_ptr<NativeFunction>& func) : DynamicObject(scope)
    {
        DynamicObject::Set(ReservedDynamicFunctions::CALL,func);
    }

    std::string Prototype::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "<Prototype>";
    }

    bool Prototype::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return true;
    }

    RuntimePrototype::RuntimePrototype(const std::shared_ptr<ScopeLike>& scope,
                                       const std::shared_ptr<frontend::PrototypeNode>& prototype) : Prototype(scope)
    {
        _prototype = prototype;
        std::vector<std::string> args;
        for(auto &statement : _prototype->scope->statements)
        {
            if(statement->type == frontend::ENodeType::Function)
            {
                if(const auto fn = std::dynamic_pointer_cast<frontend::FunctionNode>(statement))
                {
                    if(fn->name == ReservedDynamicFunctions::CONSTRUCTOR)
                    {
                        args = fn->args;
                        break;
                    }
                }
            }
        }
        
        AddNativeMemberFunction(ReservedDynamicFunctions::CALL,this,args,&RuntimePrototype::Create);
    }

    std::string RuntimePrototype::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return "<Prototype : " + _prototype->id + ">";
    }

    std::shared_ptr<Object> RuntimePrototype::Create(std::shared_ptr<FunctionScope>& fnScope)
    {
        auto dynamicObj = createDynamicFromPrototype(_prototype,cast<RuntimePrototype>(this->GetRef()));
        
        if(dynamicObj->Has(ReservedDynamicFunctions::CONSTRUCTOR,false))
        {
            if(auto ctor = cast<Function>(dynamicObj->Find(ReservedDynamicFunctions::CONSTRUCTOR,false)); ctor && ctor->GetType() == OT_Function)
            {
                ctor->HandleCall(fnScope);
            }
        }

        return dynamicObj;
    }

    std::shared_ptr<RuntimePrototype> makePrototype(const std::shared_ptr<ScopeLike>& scope,
                                                  const std::shared_ptr<frontend::PrototypeNode>& prototype)
    {
        
        return makeObject<RuntimePrototype>(makeScope(scope),prototype);
    }

    std::shared_ptr<Prototype> makePrototype(const std::shared_ptr<ScopeLike>& scope,
        const std::shared_ptr<NativeFunction>& func)
    {
        return makeObject<Prototype>(makeScope(scope),func);
    }
}
