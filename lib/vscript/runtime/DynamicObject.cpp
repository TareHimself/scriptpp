#include "vscript/runtime/DynamicObject.hpp"

#include "vscript/runtime/Null.hpp"

namespace vs::runtime
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

    TSmartPtrType<Object> OneLayerScopeProxy::Create(const std::string& id, const TSmartPtrType<Object>& var)
    {
        return _outer->Create(id,var);
    }

    TSmartPtrType<Object> OneLayerScopeProxy::Assign(const std::string& id, const TSmartPtrType<Object>& var)
    {
        return _outer->Assign(id,var);
    }

    TSmartPtrType<Object> OneLayerScopeProxy::Find(const std::string& id, bool searchParent)
    {
        return _outer->Find(id,false);
    }

    TSmartPtrType<Object>& OneLayerScopeProxy::FindRef(const std::string& id, bool searchParent)
    {
        return _outer->FindRef(id,false);
    }

    DynamicObject::DynamicObject(const TSmartPtrType<ScopeLike>& scope)
    {
        _scope = scope;
    }

    TSmartPtrType<Object>& DynamicObject::GetRef(const std::string& key)
    {
        return _scope->FindRef(key,false);
    }

    TSmartPtrType<Object>& DynamicObject::GetRef(const TSmartPtrType<Object>& key)
    {
        if(key->GetType() == OT_String)
        {
            return GetRef(key->ToString());
        }

        throw std::runtime_error("This object does not support none string keys");
    }

    TSmartPtrType<Object> DynamicObject::Get(const std::string& key)
    {
        return _scope->Find(key,false);
    }

    TSmartPtrType<Object> DynamicObject::Get(const TSmartPtrType<Object>& key)
    {
        if(key->GetType() == OT_String)
        {
            return GetRef(key->ToString());
        }

        return makeNull();
    }

    void DynamicObject::Set(const std::string& key,const TSmartPtrType<Object>& val)
    {
        if(_scope->Has(key))
        {
            _scope->Assign(key,val);
        }
        else
        {
            _scope->Create(key,val);
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

    bool DynamicObject::Has(const std::string& key) const
    {
        return _scope->Has(key);
    }

    TSmartPtrType<ScopeLike> DynamicObject::GetScope() const
    {
        return _scope;
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

    TSmartPtrType<DynamicObject> makeDynamic(const TSmartPtrType<ScopeLike>& scope)
    {
        return manage<DynamicObject>(scope);
    }

    TSmartPtrType<OneLayerScopeProxy> makeOneLayerScopeProxy(const TSmartPtrType<ScopeLike>& scope)
    {
        return manage<OneLayerScopeProxy>(scope);
    }
}
