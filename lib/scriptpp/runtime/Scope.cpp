#include "scriptpp/runtime/Scope.hpp"
#include <stdexcept>

#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/Function.hpp"
#include "scriptpp/runtime/Null.hpp"

namespace spp::runtime
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

    std::string Scope::ToString(const std::shared_ptr<ScopeLike>& scope) const
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

    std::shared_ptr<Object> Scope::Find(const std::string& id, bool searchParent) const
    {
        if(_data.contains(id))
        {
            return  makeReferenceWithId(id,cast<Scope>(this->GetRef()),_data.at(id));
        }

        if(searchParent && _outer)
        {
            return _outer->Find(id);
        }

        return {};
    }

    std::shared_ptr<ScopeLike> Scope::GetOuter() const
    {
        return _outer;
    }

    EScopeType ScopeLikeProxy::GetScopeType() const
    {
        return ST_Proxy;
    }

    ScopeLikeProxyShared::ScopeLikeProxyShared(const std::shared_ptr<ScopeLike>& scope)
    {
        _scope = scope;
    }

    std::list<EScopeType> ScopeLikeProxyShared::GetScopeStack() const
    {
        if(_scope)
        {
            return _scope->GetScopeStack();
        }
        return {};
    }

    bool ScopeLikeProxyShared::HasScopeType(EScopeType type) const
    {
        if(_scope)
        {
            return _scope->HasScopeType(type);
        }
        return false;
    }

    void ScopeLikeProxyShared::Assign(const std::string& id, const std::shared_ptr<Object>& var)
    {
        if(_scope)
        {
            return _scope->Assign(id,var);
        }
    }

    void ScopeLikeProxyShared::Create(const std::string& id, const std::shared_ptr<Object>& var)
    {
        if(_scope)
        {
            return _scope->Assign(id,var);
        }
    }

    bool ScopeLikeProxyShared::Has(const std::string& id, bool searchParent) const
    {
        if(_scope)
        {
            return _scope->Has(id,searchParent);
        }

        return false;
    }

    std::shared_ptr<Object> ScopeLikeProxyShared::Find(const std::string& id, bool searchParent) const
    {
        if(_scope)
        {
            return _scope->Find(id,searchParent);
        }

        return {};
    }

    std::shared_ptr<ScopeLike> ScopeLikeProxyShared::GetOuter() const
    {
        if(_scope)
        {
            return _scope->GetOuter();
        }

        return {};
    }

    std::shared_ptr<ScopeLike> ScopeLikeProxyShared::GetActual()
    {
        return _scope;
    }

    ScopeLikeProxyWeak::ScopeLikeProxyWeak(const std::weak_ptr<ScopeLike>& scope)
    {
        _scope = scope;
    }

    std::list<EScopeType> ScopeLikeProxyWeak::GetScopeStack() const
    {
        if(const auto s = _scope.lock())
        {
            return s->GetScopeStack();
        }

        return {};
    }

    bool ScopeLikeProxyWeak::HasScopeType(EScopeType type) const
    {
        if(const auto s = _scope.lock())
        {
            return s->HasScopeType(type);
        }

        return false;
    }

    void ScopeLikeProxyWeak::Assign(const std::string& id, const std::shared_ptr<Object>& var)
    {
        if(const auto s = _scope.lock())
        {
            return s->Assign(id,var);
        }
    }

    void ScopeLikeProxyWeak::Create(const std::string& id, const std::shared_ptr<Object>& var)
    {
        if(const auto s = _scope.lock())
        {
            return s->Create(id,var);
        }
    }

    bool ScopeLikeProxyWeak::Has(const std::string& id, bool searchParent) const
    {
        if(const auto s = _scope.lock())
        {
            return s->Has(id,searchParent);
        }

        return false;
    }

    std::shared_ptr<Object> ScopeLikeProxyWeak::Find(const std::string& id, bool searchParent) const
    {
        if(const auto s = _scope.lock())
    {
        return s->Find(id,searchParent);
    }

        return {};
    }

    std::shared_ptr<ScopeLike> ScopeLikeProxyWeak::GetOuter() const
    {
        if(const auto s = _scope.lock())
        {
            return s->GetOuter();
        }

        return {};
    }

    std::shared_ptr<ScopeLike> ScopeLikeProxyWeak::GetActual()
    {
        return _scope.lock();
    }

    CallScope::CallScope(const std::optional<frontend::TokenDebugInfo>& calledAt,
        const std::shared_ptr<ScopeLike>& scope) : ScopeLikeProxyShared(scope)
    {
        _calledAt = calledAt;
    }

    std::string CallScope::ToString() const
    {
        return _calledAt.has_value() ? _calledAt->ToString() : "<native>";
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
        return EObjectType::Scope;
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
        return EObjectType::Reference;
    }

    std::string Reference::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        return _data->ToString();
    }

    bool Reference::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return _data->ToBoolean();
    }

    bool Reference::IsCallable() const
    {
        return _data->IsCallable();
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

    std::shared_ptr<ScopeLikeProxyWeak> makeRefScopeProxy(const std::weak_ptr<ScopeLike>& scope)
    {
        return std::make_shared<ScopeLikeProxyWeak>(scope);
    }

    std::shared_ptr<CallScope> makeCallScope(const std::optional<frontend::TokenDebugInfo>& calledAt,
        const std::shared_ptr<ScopeLike>& scope)
    {
        return std::make_shared<CallScope>(calledAt,scope);
    }

    std::shared_ptr<Object> resolveReference(const std::shared_ptr<Object>& obj)
    {
        if(!obj)
        {
            return obj;
        }
        
        if(obj->GetType() == EObjectType::Reference)
        {
            return cast<Reference>(obj)->Get();
        }

        return obj;
    }

    OneLayerScopeProxy::OneLayerScopeProxy(const std::shared_ptr<ScopeLike>& scope) : ScopeLikeProxyShared(scope)
    {
        
    }
    
    bool OneLayerScopeProxy::Has(const std::string& id, bool searchParent) const
    {
        return ScopeLikeProxyShared::Has(id,false);
    }

    std::shared_ptr<Object> OneLayerScopeProxy::Find(const std::string& id, bool searchParent) const
    {
        return ScopeLikeProxyShared::Find(id,false);
    }

    std::shared_ptr<OneLayerScopeProxy> makeOneLayerScopeProxy(const std::shared_ptr<ScopeLike>& scope)
    {
        return std::make_shared<OneLayerScopeProxy>(scope);
    }
}
