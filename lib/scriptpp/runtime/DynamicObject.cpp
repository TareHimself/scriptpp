#include "scriptpp/runtime/DynamicObject.hpp"

#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/eval.hpp"
#include "scriptpp/runtime/Exception.hpp"
#include "scriptpp/runtime/Null.hpp"

namespace spp::runtime
{
    
    
    void DynamicObject::Init()
    {
        Object::Init();
        _selfFunctionScope = makeRefScopeProxy(cast<DynamicObject>(this->GetRef()));
    }

    DynamicObject::DynamicObject(const std::shared_ptr<ScopeLike>& scope)
    {
        _outer = scope;
    }

    std::shared_ptr<Object> DynamicObject::Get(const std::string& key) const
    {
        return Find(key,false);
    }

    std::shared_ptr<Object> DynamicObject::Get(const std::shared_ptr<Object>& key, const std::shared_ptr<ScopeLike>& scope) const
    {
        if(const auto impl = Get(ReservedDynamicFunctions::GET))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),key);
                }
            }
        }
        
        if(key->GetType() == EObjectType::String)
        {
            return Get(key->ToString());
        }

        return {};
    }

    bool DynamicObject::HasOwn(const std::string& id) const
    {
        return Has(id,false);
    }

    bool DynamicObject::HasOwn(const std::shared_ptr<Object>& key) const
    {
        if(key->GetType() == EObjectType::String)
        {
            return HasOwn(key->ToString());
        }

        return false;
    }

    void DynamicObject::Set(const std::string& key,const std::shared_ptr<Object>& val)
    {
        if(Has(key))
        {
            Assign(key,val);
        }
        else
        {
            Create(key,val);
        }
    }

    void DynamicObject::Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val, const std::shared_ptr<ScopeLike>& scope)
    {
        if(const auto impl = Get(ReservedDynamicFunctions::SET))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),key,val);
                    return;
                }
            }
        }
        
        if(key->GetType() == EObjectType::String)
        {
            Set(key->ToString(),val);
            return;
        }

        throw makeException(scope,"This object does not support none string keys");
    }

    bool DynamicObject::Has(const std::string& id, bool searchParent) const
    {
        return _properties.contains(id);
    }

    void DynamicObject::Assign(const std::string& id, const std::shared_ptr<Object>& var)
    {
        _properties[id] = var;
    }

    void DynamicObject::Create(const std::string& id, const std::shared_ptr<Object>& var)
    {
        _properties.insert({id,var});
    }

    std::shared_ptr<Object> DynamicObject::Find(const std::string& id, bool searchParent) const
    {
        if(_properties.contains(id))
        {
            return makeReferenceWithId(id,cast<DynamicObject>(this->GetRef()),_properties.at(id));
        }

        if(searchParent && _outer)
        {
            return _outer->Find(id);
        }
        
        return makeReferenceWithId(id,cast<DynamicObject>(this->GetRef()),makeNull());
    }

    std::list<EScopeType> DynamicObject::GetScopeStack() const
    {
        if(_outer)
        {
            return _outer->GetScopeStack();
        }
        return {};
    }

    bool DynamicObject::HasScopeType(EScopeType type) const
    {
        if(_outer)
        {
            return _outer->HasScopeType(type);
        }
        return false;
    }

    EScopeType DynamicObject::GetScopeType() const
    {
        return ST_None;
    }

    EObjectType DynamicObject::GetType() const
    {
        return EObjectType::Dynamic;
    }

    std::string DynamicObject::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        if(const auto impl = Get(ReservedDynamicFunctions::TO_STRING))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope))->ToString(scope);
                }
            }
        }
        return "<Dynamic Object " + std::to_string(GetAddress()) + ">";
    }

    bool DynamicObject::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        if(const auto impl = Get(ReservedDynamicFunctions::TO_BOOLEAN))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope))->ToBoolean(scope);
                }
            }
        }
        return true;
    }

    void DynamicObject::AddLambda(const std::string& name, const std::vector<std::string>& args,
        const std::function<std::shared_ptr<Object>(std::shared_ptr<FunctionScope>&)>& func)
    {
        DynamicObject::Set(name, makeNativeFunction(_selfFunctionScope, name, args,func, false));
    }

    std::shared_ptr<ScopeLike> DynamicObject::GetOuter() const
    {
        return _outer;
    }

    bool DynamicObject::Equal(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        if(const auto impl = Get(ReservedDynamicFunctions::EQUAL))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),other)->ToBoolean(scope);
                }
            }
        }
        
        return Object::Equal(other, scope);
    }

    bool DynamicObject::Less(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        if(const auto impl = Get(ReservedDynamicFunctions::LESS))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),other)->ToBoolean(scope);
                }
            }
        }
        
        return Object::Less(other, scope);
    }

    bool DynamicObject::Greater(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const
    {
        if(const auto impl = Get(ReservedDynamicFunctions::GREATER))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),other)->ToBoolean(scope);
                }
            }
        }
        
        return Object::Greater(other, scope);
    }

    std::shared_ptr<Object> DynamicObject::Add(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        if(const auto impl = Get(ReservedDynamicFunctions::ADD))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),other);
                }
            }
        }
        return Object::Add(other, scope);
    }

    std::shared_ptr<Object> DynamicObject::Subtract(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        if(const auto impl = Get(ReservedDynamicFunctions::SUBTRACT))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),other);
                }
            }
        }
        return Object::Subtract(other, scope);
    }

    std::shared_ptr<Object> DynamicObject::Mod(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        if(const auto impl = Get(ReservedDynamicFunctions::MOD))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),other);
                }
            }
        }
        return Object::Mod(other, scope);
    }

    std::shared_ptr<Object> DynamicObject::Divide(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        if(const auto impl = Get(ReservedDynamicFunctions::DIVIDE))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),other);
                }
            }
        }
        return Object::Divide(other, scope);
    }

    std::shared_ptr<Object> DynamicObject::Multiply(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope)
    {
        if(const auto impl = Get(ReservedDynamicFunctions::MULTIPLY))
        {
            if(const auto fn = resolveReference(impl); fn && fn->IsCallable())
            {
                if(auto [actualFn,callScope] = resolveCallable(fn,scope); actualFn)
                {
                    return actualFn->Call(makeCallScope({"<native>",0,0},actualFn,scope),other);
                }
            }
        }
        return Object::Multiply(other, scope);
    }

    bool DynamicObject::IsCallable() const
    {
        return HasOwn(ReservedDynamicFunctions::CALL);
    }


    DynamicObjectReference::DynamicObjectReference(const std::string& id, const std::shared_ptr<DynamicObject>& obj,
                                                   const std::shared_ptr<ScopeLike>& scope, const std::shared_ptr<Object>& val) : Reference(scope,val)
    {
        _id = id;
        _obj = obj;
    }

    void DynamicObjectReference::Set(const std::shared_ptr<Object>& val)
    {
        Reference::Set(val);
        _obj->Set(_id,val);
    }

    std::shared_ptr<DynamicObject> makeDynamic(const std::shared_ptr<ScopeLike>& scope)
    {
        return makeObject<DynamicObject>(scope);
    }
    
}
