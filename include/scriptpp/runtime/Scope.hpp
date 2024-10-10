#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include "Object.hpp"
#include "scriptpp/frontend/Token.hpp"

namespace spp
{
    namespace runtime
    {
        class Function;
    }
}

namespace spp::runtime
{
    class Reference;
    
    enum EScopeType
    {
        ST_None,
        ST_Proxy,
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
        
        virtual void Assign(const std::string& id, const std::shared_ptr<Object>& var) = 0;
        
        virtual void Create(const std::string& id, const std::shared_ptr<Object>& var) = 0;

        virtual bool Has(const std::string& id, bool searchParent = true) const = 0;

        virtual std::shared_ptr<Object> Find(const std::string& id, bool searchParent = true) const = 0;

        virtual std::shared_ptr<ScopeLike> GetOuter() const = 0;
    };
    
    class Scope : public Object, public ScopeLike
    {
        std::unordered_map<std::string,std::shared_ptr<Object>> _data;
        std::shared_ptr<ScopeLike> _outer;
        std::list<EScopeType> _scopeStack;
        
    public:
        Scope();
        Scope(const std::shared_ptr<ScopeLike>& outer);
        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        EObjectType GetType() const override;
        
        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        EScopeType GetScopeType() const override;

        void Assign(const std::string& id, const std::shared_ptr<Object>& var) override;

        void Create(const std::string& id, const std::shared_ptr<Object>& var) override;

        bool Has(const std::string& id, bool searchParent = true) const override;

        std::shared_ptr<Object> Find(const std::string& id, bool searchParent = true) const override;

        std::shared_ptr<ScopeLike> GetOuter() const override;
    };

    class ScopeLikeProxy : public ScopeLike
    {
    public:
        EScopeType GetScopeType() const override;
        virtual std::shared_ptr<ScopeLike> GetActual() = 0;
        
    };

    class ScopeLikeProxyShared : public ScopeLikeProxy
    {
    protected:
        std::shared_ptr<ScopeLike> _scope;
    public:
        ScopeLikeProxyShared(const std::shared_ptr<ScopeLike>& scope);
        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        void Assign(const std::string& id, const std::shared_ptr<Object>& var) override;
        void Create(const std::string& id, const std::shared_ptr<Object>& var) override;
        bool Has(const std::string& id, bool searchParent) const override;
        std::shared_ptr<Object> Find(const std::string& id, bool searchParent) const override;
        std::shared_ptr<ScopeLike> GetOuter() const override;
        std::shared_ptr<ScopeLike> GetActual() override;
    };

    class ScopeLikeProxyWeak : public ScopeLikeProxy
    {
    protected:
        std::weak_ptr<ScopeLike> _scope;
    public:
        ScopeLikeProxyWeak(const std::weak_ptr<ScopeLike>& scope);
        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        void Assign(const std::string& id, const std::shared_ptr<Object>& var) override;
        void Create(const std::string& id, const std::shared_ptr<Object>& var) override;
        bool Has(const std::string& id, bool searchParent) const override;
        std::shared_ptr<Object> Find(const std::string& id, bool searchParent) const override;
        std::shared_ptr<ScopeLike> GetOuter() const override;
        std::shared_ptr<ScopeLike> GetActual() override;
    };

    // Used to track function calls
    class CallScope : public ScopeLikeProxyShared
    {
    protected:
        std::optional<frontend::TokenDebugInfo> _calledAt;
    public:
        CallScope(const std::optional<frontend::TokenDebugInfo>& calledAt,const std::shared_ptr<ScopeLike>& scope);
        std::string ToString() const;
    };

    class Reference : public Object
    {
    protected:
        std::shared_ptr<ScopeLike> _scope;
        std::shared_ptr<Object> _data;
    public:
        Reference(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& val);
        virtual std::shared_ptr<Object> Get() const;
        virtual void Set(const std::shared_ptr<Object>& val);

        EObjectType GetType() const override;
        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;
        bool IsCallable() const override;
    };

    class ReferenceWithId : public Reference
    {
    protected:
        std::string _id;
    public:
        ReferenceWithId(const std::string& id,const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& val);
        void Set(const std::shared_ptr<Object>& val) override;
    };

    class ReferenceWithSetter : public Reference
    {
    public:
        using SetterFn = std::function<void(const std::shared_ptr<ScopeLike>&,const std::shared_ptr<Object>&)>;
        ReferenceWithSetter(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& val,const SetterFn& fn);
        void Set(const std::shared_ptr<Object>& val) override;
    protected:
        SetterFn _fn;
    };

    class OneLayerScopeProxy : public ScopeLikeProxyShared
    {
        std::shared_ptr<ScopeLike> _outer;
    public:
        explicit OneLayerScopeProxy(const std::shared_ptr<ScopeLike>& scope);
        bool Has(const std::string& id, bool searchParent) const override;
        std::shared_ptr<Object> Find(const std::string& id, bool searchParent) const override;
    };

    std::shared_ptr<Reference> makeReference(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& val);

    std::shared_ptr<ReferenceWithId> makeReferenceWithId(const std::string& id,const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& val);

    std::shared_ptr<ReferenceWithSetter> makeReferenceWithSetter(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& val,const ReferenceWithSetter::SetterFn& fn);
    
    std::shared_ptr<Scope> makeScope(const std::shared_ptr<ScopeLike>& outer);

    std::shared_ptr<Scope> makeScope();

    std::shared_ptr<ScopeLikeProxyWeak> makeRefScopeProxy(const std::weak_ptr<ScopeLike>& scope);

    std::shared_ptr<CallScope> makeCallScope(const std::optional<frontend::TokenDebugInfo>& calledAt = {},const std::shared_ptr<ScopeLike>& scope = {});

    std::shared_ptr<Object> resolveReference(const std::shared_ptr<Object>& obj);

    std::shared_ptr<OneLayerScopeProxy> makeOneLayerScopeProxy(const std::shared_ptr<ScopeLike>& scope);
}
