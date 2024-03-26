#include "vscript/frontend/DynamicObject.hpp"

#include "vscript/frontend/Null.hpp"

namespace vs::frontend
{
    OneLayerScopeProxy::OneLayerScopeProxy(const TSmartPtrType<ScopeLike>& scope)
    {
        _outer = scope;
    }

    std::list<EScopeType> OneLayerScopeProxy::GetScopeStack() const
    {
        return _outer->GetScopeStack();
    }

    bool OneLayerScopeProxy::HasScopeType(EScopeType type) const
    {
        return _outer->HasScopeType(type);
    }

    EScopeType OneLayerScopeProxy::GetScopeType() const
    {
        return _outer->GetScopeType();
    }

    bool OneLayerScopeProxy::Has(const std::string& id, bool searchParent) const
    {
        return _outer->Has(id,false);
    }

    void OneLayerScopeProxy::Create(const std::string& id, const TSmartPtrType<Object>& var)
    {
        return _outer->Create(id,var);
    }

    void OneLayerScopeProxy::Assign(const std::string& id, const TSmartPtrType<Object>& var)
    {
        return _outer->Assign(id,var);
    }

    TSmartPtrType<Object> OneLayerScopeProxy::Find(const std::string& id, bool searchParent)
    {
        return _outer->Find(id,false);
    }

    DynamicObjectCallScope::DynamicObjectCallScope(const Ref<DynamicObject>& scope)
    {
        _outer = scope;
    }

    std::list<EScopeType> DynamicObjectCallScope::GetScopeStack() const
    {
        if(auto obj = _outer.Reserve(); obj.IsValid())
        {
            return obj->GetScopeStack();
        }
        return {};
    }

    bool DynamicObjectCallScope::HasScopeType(EScopeType type) const
    {
        if(auto obj = _outer.Reserve(); obj.IsValid())
        {
            return obj->HasScopeType(type);
        }

        return false;
    }

    EScopeType DynamicObjectCallScope::GetScopeType() const
    {
        if(auto obj = _outer.Reserve(); obj.IsValid())
        {
            return obj->GetScopeType();
        }
        return ST_None;
    }

    bool DynamicObjectCallScope::Has(const std::string& id, bool searchParent) const
    {
        if(auto obj = _outer.Reserve(); obj.IsValid())
        {
            return obj->Has(id,searchParent);
        }

        return false;
    }

    void DynamicObjectCallScope::Create(const std::string& id, const TSmartPtrType<Object>& var)
    {
        if(auto obj = _outer.Reserve(); obj.IsValid())
        {
            return obj->Create(id,var);
        }
    }

    void DynamicObjectCallScope::Assign(const std::string& id, const TSmartPtrType<Object>& var)
    {
        if(auto obj = _outer.Reserve(); obj.IsValid())
        {
            return obj->Assign(id,var);
        }
    }

    TSmartPtrType<Object> DynamicObjectCallScope::Find(const std::string& id, bool searchParent)
    {
        if(auto obj = _outer.Reserve(); obj.IsValid())
        {
            return obj->Find(id,searchParent);
        }

        return makeNull();
    }

    DynamicObject::DynamicObject(const TSmartPtrType<ScopeLike>& scope)
    {
        _outer = scope;
    }

    TSmartPtrType<Object> DynamicObject::Get(const std::string& key)
    {
        return Find(key);
    }

    TSmartPtrType<Object> DynamicObject::Get(const TSmartPtrType<Object>& key)
    {
        if(key->GetType() == OT_String)
        {
            return Get(key->ToString());
        }

        return makeNull();
    }

    void DynamicObject::Set(const std::string& key,const TSmartPtrType<Object>& val)
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

    void DynamicObject::Set(const TSmartPtrType<Object>& key, const TSmartPtrType<Object>& val)
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

    void DynamicObject::Assign(const std::string& id, const TSmartPtrType<Object>& var)
    {
        _properties[id] = var;
    }

    void DynamicObject::Create(const std::string& id, const TSmartPtrType<Object>& var)
    {
        _properties.insert({id,var});
    }

    TSmartPtrType<Object> DynamicObject::Find(const std::string& id, bool searchParent)
    {
        if(_properties.contains(id))
        {
            return makeReference(this->ToRef().Reserve().Cast<DynamicObject>(),_properties[id]);
        }
        
        return makeNull();
    }

    std::list<EScopeType> DynamicObject::GetScopeStack() const
    {
        if(_outer.IsValid())
        {
            return _outer->GetScopeStack();
        }
        return {};
    }

    bool DynamicObject::HasScopeType(EScopeType type) const
    {
        if(_outer.IsValid())
        {
            return _outer->HasScopeType(type);
        }
        return false;
    }

    EScopeType DynamicObject::GetScopeType() const
    {
        return ST_None;
    }

    TSmartPtrType<DynamicObjectCallScope> DynamicObject::CreateCallScope()
    {
        return manage<DynamicObjectCallScope>(this->ToRef().Cast<DynamicObject>());
    }

    EObjectType DynamicObject::GetType() const
    {
        return OT_Dynamic;
    }

    std::string DynamicObject::ToString()
    {
        return "<Dynamic Object " + std::to_string(GetAddress()) + ">";
    }

    bool DynamicObject::ToBoolean() const
    {
        return true;
    }

    void DynamicObject::AddLambda(const std::string& name, const std::vector<std::string>& args,
        const std::function<TSmartPtrType<Object>(TSmartPtrType<FunctionScope>&)>& func)
    {
        DynamicObject::Set(name, makeNativeFunction(_callScope, name, args,func, false));
    }

    void DynamicObject::OnRefSet()
    {
        Object::OnRefSet();
        _callScope = CreateCallScope();
    }

    DynamicObjectReference::DynamicObjectReference(const std::string& id, const TSmartPtrType<DynamicObject>& obj,
                                                   const TSmartPtrType<ScopeLike>& scope, const TSmartPtrType<Object>& val) : Reference(scope,val)
    {
        _id = id;
        _obj = obj;
    }

    void DynamicObjectReference::Set(const TSmartPtrType<Object>& val)
    {
        Reference::Set(val);
        _obj->Set(_id,val);
    }

    TSmartPtrType<DynamicObject> makeDynamic(const TSmartPtrType<ScopeLike>& scope)
    {
        return manage<DynamicObject>(scope);
    }
    
    TSmartPtrType<OneLayerScopeProxy> makeOneLayerScopeProxy(const TSmartPtrType<ScopeLike>& scope)
    {
        return manage<OneLayerScopeProxy>(scope);
    }
    
}
