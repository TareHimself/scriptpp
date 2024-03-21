#pragma once
#include <string>
#include <unordered_map>

#include "Object.hpp"

namespace vs::runtime
{
    enum EScopeType
    {
        ST_None,
        ST_Module,
        ST_Function,
        ST_Iteration,
        ST_Program
    };

    class ScopeLike
    {
    public:

        virtual std::list<EScopeType> GetScopeStack() const = 0;
        virtual bool HasScopeType(EScopeType type) const = 0;
        virtual EScopeType GetScopeType() const = 0;
        
        virtual TSmartPtrType<Object> Assign(const std::string& id, const TSmartPtrType<Object>& var) = 0;
        
        virtual TSmartPtrType<Object> Create(const std::string& id, const TSmartPtrType<Object>& var) = 0;

        virtual bool Has(const std::string& id, bool searchParent = true) const = 0;
        
        virtual TSmartPtrType<Object>& FindRef(const std::string& id,bool searchParent = true) = 0;

        virtual TSmartPtrType<Object> Find(const std::string& id,bool searchParent = true) = 0;
    };
    
    class Scope : public Object, public ScopeLike
    {
        std::unordered_map<std::string,TSmartPtrType<Object>> _data;
        TSmartPtrType<ScopeLike> _outer;
        std::list<EScopeType> _scopeStack;
        
    public:
        Scope();
        Scope(const TSmartPtrType<ScopeLike>& outer);
        std::string ToString() override;
        EObjectType GetType() const override;
        
        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        EScopeType GetScopeType() const override;
        
        TSmartPtrType<Object> Assign(const std::string& id, const TSmartPtrType<Object>& var) override;
        
        TSmartPtrType<Object> Create(const std::string& id, const TSmartPtrType<Object>& var) override;

        bool Has(const std::string& id, bool searchParent = true) const override;
        
        TSmartPtrType<Object>& FindRef(const std::string& id,bool searchParent = true) override;

        TSmartPtrType<Object> Find(const std::string& id,bool searchParent = true) override;
    };

    class ProgramScope : public Scope
    {
    public:
        
    };

    

    TSmartPtrType<Scope> makeScope(const TSmartPtrType<ScopeLike>& outer);

    TSmartPtrType<Scope> makeScope();

    TSmartPtrType<ProgramScope> makeProgramScope();
}
