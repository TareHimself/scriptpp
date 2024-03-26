#pragma once
#include <string>
#include <unordered_map>
#include "Object.hpp"

namespace vs::frontend
{
    class Reference;
    
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
        
        virtual void Assign(const std::string& id, const TSmartPtrType<Object>& var) = 0;
        
        virtual void Create(const std::string& id, const TSmartPtrType<Object>& var) = 0;

        virtual bool Has(const std::string& id, bool searchParent = true) const = 0;

        virtual TSmartPtrType<Object> Find(const std::string& id, bool searchParent = true) = 0;
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

        void Assign(const std::string& id, const TSmartPtrType<Object>& var) override;

        void Create(const std::string& id, const TSmartPtrType<Object>& var) override;

        bool Has(const std::string& id, bool searchParent = true) const override;

        TSmartPtrType<Object> Find(const std::string& id, bool searchParent = true) override;
    };

    class RefScopeProxy : public ScopeLike
    {
    protected:
        Ref<ScopeLike> _scope;
    public:
        RefScopeProxy(const Ref<ScopeLike>& scope);
        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        EScopeType GetScopeType() const override;
        void Assign(const std::string& id, const TSmartPtrType<Object>& var) override;
        void Create(const std::string& id, const TSmartPtrType<Object>& var) override;
        bool Has(const std::string& id, bool searchParent) const override;
        TSmartPtrType<Object> Find(const std::string& id, bool searchParent) override;
    };

    class Reference : public Object
    {
    protected:
        TSmartPtrType<ScopeLike> _scope;
        TSmartPtrType<Object> _data;
    public:
        Reference(const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<Object>& val);
        virtual TSmartPtrType<Object> Get() const;
        virtual void Set(const TSmartPtrType<Object>& val);

        EObjectType GetType() const override;
        std::string ToString() override;
        bool ToBoolean() const override;
    };

    class ReferenceWithId : public Reference
    {
    protected:
        std::string _id;
    public:
        ReferenceWithId(const std::string& id,const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<Object>& val);
        void Set(const TSmartPtrType<Object>& val) override;
    };

    class ReferenceWithSetter : public Reference
    {
    public:
        using SetterFn = std::function<void(const TSmartPtrType<ScopeLike>&,const TSmartPtrType<Object>&)>;
        ReferenceWithSetter(const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<Object>& val,const SetterFn& fn);
        void Set(const TSmartPtrType<Object>& val) override;
    protected:
        SetterFn _fn;
    };

    TSmartPtrType<Reference> makeReference(const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<Object>& val);

    TSmartPtrType<ReferenceWithId> makeReferenceWithId(const std::string& id,const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<Object>& val);

    TSmartPtrType<ReferenceWithSetter> makeReferenceWithSetter(const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<Object>& val,const ReferenceWithSetter::SetterFn& fn);
    
    TSmartPtrType<Scope> makeScope(const TSmartPtrType<ScopeLike>& outer);

    TSmartPtrType<Scope> makeScope();

    TSmartPtrType<RefScopeProxy> makeRefScopeProxy(const Ref<ScopeLike>& scope);

    TSmartPtrType<Object> resolveReference(const TSmartPtrType<Object>& obj);

    
}
