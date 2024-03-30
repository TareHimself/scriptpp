#include "vscript/frontend/Scope.hpp"
#include <stdexcept>

#include "vscript/utils.hpp"
#include "vscript/frontend/Function.hpp"
#include "vscript/frontend/Null.hpp"

namespace vs::frontend
{
    Scope::Scope()
    {
        _scopeStack = {GetScopeType()};
    };

    Scope::Scope(const std::shared_ptr<ScopeLike>& outer)
    {
        _outer = outer;
        if(_outer)
        {
            _scopeStack = outer->GetScopeStack();
        }
        
        if(_scopeStack.empty() || GetScopeType() != _scopeStack.front())
        {
            _scopeStack.push_back(GetScopeType());
        }
    }

    std::string Scope::ToString() const
    {
        return "scope";
    }

    void Scope::Assign(const std::string& id, const std::shared_ptr<Object>& var)
    {
        _data[id] = var;
    }

    void Scope::Create(const std::string& id, const std::shared_ptr<Object>& var)
    {
        _data[id] = var;
    }

    bool Scope::Has(const std::string& id, const bool searchParent) const
    {
        if(_data.contains(id))
        {
            return true;
        }

        if(searchParent && _outer)
        {
            return _outer->Has(id);
        }

        return false;
    }

    std::shared_ptr<Object> Scope::Find(const std::string& id, bool searchParent)
    {
        if(_data.contains(id))
        {
            return  makeReferenceWithId(id,cast<Scope>(this->GetRef()),_data[id]);
        }

        if(searchParent && _outer)
        {
            return _outer->Find(id);
        }

        return makeNull();
    }

    std::shared_ptr<ScopeLike> Scope::GetOuter() const
    {
        return _outer;
    }

    RefScopeProxy::RefScopeProxy(const std::weak_ptr<ScopeLike>& scope)
    {
        _scope = scope;
    }

    std::list<EScopeType> RefScopeProxy::GetScopeStack() const
    {
        if(const auto s = _scope.lock())
        {
            return s->GetScopeStack();
        }

        return {};
    }

    bool RefScopeProxy::HasScopeType(EScopeType type) const
    {
        if(const auto s = _scope.lock())
        {
            return s->HasScopeType(type);
        }

        return false;
    }

    EScopeType RefScopeProxy::GetScopeType() const
    {
        if(const auto s = _scope.lock())
        {
            return s->GetScopeType();
        }

        return ST_None;
    }

    void RefScopeProxy::Assign(const std::string& id, const std::shared_ptr<Object>& var)
    {
        if(const auto s = _scope.lock())
        {
            return s->Assign(id,var);
        }
    }

    void RefScopeProxy::Create(const std::string& id, const std::shared_ptr<Object>& var)
    {
        if(const auto s = _scope.lock())
        {
            return s->Create(id,var);
        }
    }

    bool RefScopeProxy::Has(const std::string& id, bool searchParent) const
    {
        if(const auto s = _scope.lock())
        {
            return s->Has(id,searchParent);
        }

        return false;
    }

    std::shared_ptr<Object> RefScopeProxy::Find(const std::string& id, bool searchParent)
    {
        if(const auto s = _scope.lock())
    {
        return s->Find(id,searchParent);
    }

        return makeNull();
    }

    std::shared_ptr<ScopeLike> RefScopeProxy::GetOuter() const
    {
        if(const auto s = _scope.lock())
        {
            return s->GetOuter();
        }

        return {};
    }

    CallScope::CallScope(const backend::TokenDebugInfo& calledAt, const std::shared_ptr<Function>& called,
        const std::shared_ptr<ScopeLike>& scope)
    {
        _calledAt = calledAt;
        _called = called;
        _scope = scope;
    }

    std::list<EScopeType> CallScope::GetScopeStack() const
    {
        return _scope->GetScopeStack();
    }

    bool CallScope::HasScopeType(EScopeType type) const
    {
        return _scope->HasScopeType(type);
    }

    EScopeType CallScope::GetScopeType() const
    {
        return _scope->GetScopeType();
    }

    void CallScope::Assign(const std::string& id, const std::shared_ptr<Object>& var)
    {
        _scope->Assign(id,var);
    }

    void CallScope::Create(const std::string& id, const std::shared_ptr<Object>& var)
    {
        _scope->Create(id,var);
    }

    bool CallScope::Has(const std::string& id, bool searchParent) const
    {
        return _scope->Has(id,searchParent);
    }

    std::shared_ptr<Object> CallScope::Find(const std::string& id, bool searchParent)
    {
        return _scope->Find(id,searchParent);
    }

    std::shared_ptr<ScopeLike> CallScope::GetOuter() const
    {
        return _scope->GetOuter();
    }

    std::string CallScope::ToString() const
    {
        return join({_called->ToString(),_calledAt.ToString()}," @ ");
    }

    std::shared_ptr<ScopeLike> CallScope::GetScope() const
    {
        return _scope;
    }

    Reference::Reference(const std::shared_ptr<ScopeLike>& scope, const std::shared_ptr<Object>& val)
    {
        _scope = scope;
        _data = val;
    }

    std::shared_ptr<Object> Reference::Get() const
    {
        return _data;
    }

    void Reference::Set(const std::shared_ptr<Object>& val)
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

    std::string Reference::ToString() const
    {
        return _data->ToString();
    }

    bool Reference::ToBoolean() const
    {
        return _data->ToBoolean();
    }

    ReferenceWithId::ReferenceWithId(const std::string& id, const std::shared_ptr<ScopeLike>& scope,
        const std::shared_ptr<Object>& val) : Reference(scope,val)
    {
        _id = id;
    }

    void ReferenceWithId::Set(const std::shared_ptr<Object>& val)
    {
        Reference::Set(val);
        _scope->Assign(_id,_data);
    }

    ReferenceWithSetter::ReferenceWithSetter(const std::shared_ptr<ScopeLike>& scope, const std::shared_ptr<Object>& val,
        const SetterFn& fn) : Reference(scope,val)
    {
        _fn = fn;
    }

    void ReferenceWithSetter::Set(const std::shared_ptr<Object>& val)
    {
        Reference::Set(val);
        _fn(_scope,val);
    }

    std::shared_ptr<Reference> makeReference(const std::shared_ptr<ScopeLike>& scope, const std::shared_ptr<Object>& val)
    {
        return makeObject<Reference>(scope,val);
    }

    std::shared_ptr<ReferenceWithId> makeReferenceWithId(const std::string& id, const std::shared_ptr<ScopeLike>& scope,
        const std::shared_ptr<Object>& val)
    {
        return makeObject<ReferenceWithId>(id,scope,val);
    }

    std::shared_ptr<ReferenceWithSetter> makeReferenceWithSetter(const std::shared_ptr<ScopeLike>& scope,
        const std::shared_ptr<Object>& val, const ReferenceWithSetter::SetterFn& fn)
    {
        return makeObject<ReferenceWithSetter>(scope,val,fn);
    }

    std::shared_ptr<Scope> makeScope(const std::shared_ptr<ScopeLike>& outer)
    {
        return makeObject<Scope>(outer);
    }
    
    std::shared_ptr<Scope> makeScope()
    {
        return makeObject<Scope>();
    }

    std::shared_ptr<RefScopeProxy> makeRefScopeProxy(const std::weak_ptr<ScopeLike>& scope)
    {
        return std::make_shared<RefScopeProxy>(scope);
    }

    std::shared_ptr<CallScope> makeCallScope(const backend::TokenDebugInfo& calledAt,const std::shared_ptr<Function>& called,const std::shared_ptr<ScopeLike>& scope)
    {
        return std::make_shared<CallScope>(calledAt,called,scope);
    }

    std::shared_ptr<Object> resolveReference(const std::shared_ptr<Object>& obj)
    {
        if(!obj)
        {
            return obj;
        }
        
        if(obj->GetType() == OT_Reference)
        {
            return cast<Reference>(obj)->Get();
        }

        return obj;
    }

    OneLayerScopeProxy::OneLayerScopeProxy(const std::shared_ptr<ScopeLike>& scope)
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

    void OneLayerScopeProxy::Create(const std::string& id, const std::shared_ptr<Object>& var)
    {
        return _outer->Create(id,var);
    }

    void OneLayerScopeProxy::Assign(const std::string& id, const std::shared_ptr<Object>& var)
    {
        return _outer->Assign(id,var);
    }

    std::shared_ptr<Object> OneLayerScopeProxy::Find(const std::string& id, bool searchParent)
    {
        return _outer->Find(id,false);
    }

    std::shared_ptr<ScopeLike> OneLayerScopeProxy::GetOuter() const
    {
        return _outer->GetOuter();
    }

    std::shared_ptr<OneLayerScopeProxy> makeOneLayerScopeProxy(const std::shared_ptr<ScopeLike>& scope)
    {
        return std::make_shared<OneLayerScopeProxy>(scope);
    }
}
