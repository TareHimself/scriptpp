#include "vscript/frontend/Scope.hpp"
#include <stdexcept>
#include "vscript/frontend/Function.hpp"
#include "vscript/frontend/Null.hpp"

namespace vs::frontend
{
    Scope::Scope()
    {
        _scopeStack = {GetScopeType()};
    };

    Scope::Scope(const TSmartPtrType<ScopeLike>& outer)
    {
        _outer = outer;
        if(_outer.IsValid())
        {
            _scopeStack = outer->GetScopeStack();
        }
        
        if(_scopeStack.empty() || GetScopeType() != _scopeStack.front())
        {
            _scopeStack.push_back(GetScopeType());
        }
    }

    std::string Scope::ToString()
    {
        return "scope";
    }

    void Scope::Assign(const std::string& id, const TSmartPtrType<Object>& var)
    {
        _data[id] = var;
    }

    void Scope::Create(const std::string& id, const TSmartPtrType<Object>& var)
    {
        _data[id] = var;
    }

    bool Scope::Has(const std::string& id, const bool searchParent) const
    {
        if(_data.contains(id))
        {
            return true;
        }

        if(searchParent && _outer.IsValid())
        {
            return _outer->Has(id);
        }

        return false;
    }

    TSmartPtrType<Object> Scope::Find(const std::string& id, bool searchParent)
    {
        if(_data.contains(id))
        {
            return  makeReferenceWithId(id,this->ToRef().Reserve().Cast<Scope>(),_data[id]);
        }

        if(searchParent && _outer.IsValid())
        {
            return _outer->Find(id);
        }

        return makeNull();
    }

    RefScopeProxy::RefScopeProxy(const Ref<ScopeLike>& scope)
    {
        _scope = scope;
    }

    std::list<EScopeType> RefScopeProxy::GetScopeStack() const
    {
        if(auto s = _scope.Reserve(); s.IsValid())
        {
            return s->GetScopeStack();
        }

        return {};
    }

    bool RefScopeProxy::HasScopeType(EScopeType type) const
    {
        if(auto s = _scope.Reserve(); s.IsValid())
        {
            return s->HasScopeType(type);
        }

        return false;
    }

    EScopeType RefScopeProxy::GetScopeType() const
    {
        if(auto s = _scope.Reserve(); s.IsValid())
        {
            return s->GetScopeType();
        }

        return ST_None;
    }

    void RefScopeProxy::Assign(const std::string& id, const TSmartPtrType<Object>& var)
    {
        if(auto s = _scope.Reserve(); s.IsValid())
        {
            return s->Assign(id,var);
        }
    }

    void RefScopeProxy::Create(const std::string& id, const TSmartPtrType<Object>& var)
    {
        if(auto s = _scope.Reserve(); s.IsValid())
        {
            return s->Create(id,var);
        }
    }

    bool RefScopeProxy::Has(const std::string& id, bool searchParent) const
    {
        if(auto s = _scope.Reserve(); s.IsValid())
        {
            return s->Has(id,searchParent);
        }

        return false;
    }

    TSmartPtrType<Object> RefScopeProxy::Find(const std::string& id, bool searchParent)
    {
        if(auto s = _scope.Reserve(); s.IsValid())
    {
        return s->Find(id,searchParent);
    }

        return makeNull();
    }

    Reference::Reference(const TSmartPtrType<ScopeLike>& scope, const TSmartPtrType<Object>& val)
    {
        _scope = scope;
        _data = val;
    }

    TSmartPtrType<Object> Reference::Get() const
    {
        return _data;
    }

    void Reference::Set(const TSmartPtrType<Object>& val)
    {
       _data = val;
    }

    EObjectType Scope::GetType() const
    {
        return OT_Scope;
    }
    
    std::list<EScopeType> Scope::GetScopeStack() const
    {
        return _scopeStack;
    }

    bool Scope::HasScopeType(const EScopeType type) const
    {
        return type == GetScopeType() || std::ranges::find(_scopeStack,type) != _scopeStack.end();
    }

    EScopeType Scope::GetScopeType() const
    {
        return EScopeType::ST_None;
    }

    EObjectType Reference::GetType() const
    {
        return OT_Reference;
    }

    std::string Reference::ToString()
    {
        return _data->ToString();
    }

    bool Reference::ToBoolean() const
    {
        return _data->ToBoolean();
    }

    ReferenceWithId::ReferenceWithId(const std::string& id, const TSmartPtrType<ScopeLike>& scope,
        const TSmartPtrType<Object>& val) : Reference(scope,val)
    {
        _id = id;
    }

    void ReferenceWithId::Set(const TSmartPtrType<Object>& val)
    {
        Reference::Set(val);
        _scope->Assign(_id,_data);
    }

    ReferenceWithSetter::ReferenceWithSetter(const TSmartPtrType<ScopeLike>& scope, const TSmartPtrType<Object>& val,
        const SetterFn& fn) : Reference(scope,val)
    {
        _fn = fn;
    }

    void ReferenceWithSetter::Set(const TSmartPtrType<Object>& val)
    {
        Reference::Set(val);
        _fn(_scope,val);
    }

    TSmartPtrType<Reference> makeReference(const TSmartPtrType<ScopeLike>& scope, const TSmartPtrType<Object>& val)
    {
        return manage<Reference>(scope,val);
    }

    TSmartPtrType<ReferenceWithId> makeReferenceWithId(const std::string& id, const TSmartPtrType<ScopeLike>& scope,
        const TSmartPtrType<Object>& val)
    {
        return manage<ReferenceWithId>(id,scope,val);
    }

    TSmartPtrType<ReferenceWithSetter> makeReferenceWithSetter(const TSmartPtrType<ScopeLike>& scope,
        const TSmartPtrType<Object>& val, const ReferenceWithSetter::SetterFn& fn)
    {
        return manage<ReferenceWithSetter>(scope,val,fn);
    }

    TSmartPtrType<Scope> makeScope(const TSmartPtrType<ScopeLike>& outer)
    {
        return manage<Scope>(outer);
    }
    
    TSmartPtrType<Scope> makeScope()
    {
        return manage<Scope>();
    }

    TSmartPtrType<RefScopeProxy> makeRefScopeProxy(const Ref<ScopeLike>& scope)
    {
        return manage<RefScopeProxy>(scope);
    }

    TSmartPtrType<Object> resolveReference(const TSmartPtrType<Object>& obj)
    {
        if(!obj.IsValid())
        {
            return obj;
        }
        
        if(obj->GetType() == OT_Reference)
        {
            return obj.Cast<Reference>()->Get();
        }

        return obj;
    }
}
