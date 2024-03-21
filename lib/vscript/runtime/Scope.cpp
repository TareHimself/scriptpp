#include "vscript/runtime/Scope.hpp"
#include <stdexcept>
#include "vscript/runtime/Function.hpp"
#include "vscript/runtime/Null.hpp"

namespace vs::runtime
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

    TSmartPtrType<Object> Scope::Assign(const std::string& id, const TSmartPtrType<Object>& var)
    {
        _data[id] = var;
        return var;
    }

    TSmartPtrType<Object> Scope::Create(const std::string& id, const TSmartPtrType<Object>& var)
    {
        _data[id] = var;
        return var;
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

    TSmartPtrType<Object>& Scope::FindRef(const std::string& id, const bool searchParent)
    {
        if(_data.contains(id))
        {
            return _data[id];
        }

        if(searchParent && _outer.IsValid())
        {
            return _outer->FindRef(id);
        }

        throw std::runtime_error(id + std::string(" has not been declared."));
    }

    TSmartPtrType<Object> Scope::Find(const std::string& id,bool searchParent)
    {
        if(_data.contains(id))
        {
            return _data[id];
        }

        if(searchParent && _outer.IsValid())
        {
            return _outer->Find(id);
        }

        return makeNull();
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

    TSmartPtrType<Scope> makeScope(const TSmartPtrType<ScopeLike>& outer)
    {
        return manage<Scope>(outer);
    }
    
    TSmartPtrType<Scope> makeScope()
    {
        return manage<Scope>();
    }

    TSmartPtrType<ProgramScope> makeProgramScope()
    {
        return manage<ProgramScope>();
    }
}
