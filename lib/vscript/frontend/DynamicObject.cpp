#include "vscript/frontend/DynamicObject.hpp"

#include "vscript/utils.hpp"
#include "vscript/frontend/Null.hpp"

namespace vs::frontend
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

    std::shared_ptr<Object> DynamicObject::Get(const std::string& key)
    {
        return Find(key,false);
    }

    std::shared_ptr<Object> DynamicObject::Get(const std::shared_ptr<Object>& key)
    {
        if(key->GetType() == OT_String)
        {
            return Get(key->ToString());
        }

        return makeNull();
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

    void DynamicObject::Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val)
    {
        if(key->GetType() == OT_String)
        {
            Set(key->ToString(),val);
        }

        throw std::runtime_error("This object does not support none string keys");
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

    std::shared_ptr<Object> DynamicObject::Find(const std::string& id, bool searchParent)
    {
        if(_properties.contains(id))
        {
            return makeReferenceWithId(id,cast<DynamicObject>(this->GetRef()),_properties[id]);
        }

        if(searchParent && _outer)
        {
            return _outer->Find(id);
        }
        
        return makeNull();
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
        return OT_Dynamic;
    }

    std::string DynamicObject::ToString() const
    {
        return "<Dynamic Object " + std::to_string(GetAddress()) + ">";
    }

    bool DynamicObject::ToBoolean() const
    {
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
