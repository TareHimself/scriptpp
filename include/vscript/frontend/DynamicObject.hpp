#pragma once
#include "Function.hpp"
#include "Object.hpp"
#include "Scope.hpp"

namespace vs::frontend
{

    class DynamicObject;
    
    struct ReservedDynamicFunctions
    {
        inline static std::string CALL = "__call__";
        inline static std::string CONSTRUCTOR = "__ctor__";
    };

    class OneLayerScopeProxy : public ScopeLike
    {
        TSmartPtrType<ScopeLike> _outer;
    public:
        explicit OneLayerScopeProxy(const TSmartPtrType<ScopeLike>& scope);

        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        EScopeType GetScopeType() const override;
        bool Has(const std::string& id, bool searchParent) const override;
        void Create(const std::string& id, const TSmartPtrType<Object>& var) override;
        void Assign(const std::string& id, const TSmartPtrType<Object>& var) override;
        TSmartPtrType<Object> Find(const std::string& id, bool searchParent) override;
        TSmartPtrType<ScopeLike> GetOuter() const override;
    };

    class DynamicObjectCallScope : public ScopeLike
    {
        Ref<DynamicObject> _outer;
    public:
        explicit DynamicObjectCallScope(const Ref<DynamicObject>& scope);

        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        EScopeType GetScopeType() const override;
        bool Has(const std::string& id, bool searchParent) const override;
        void Create(const std::string& id, const TSmartPtrType<Object>& var) override;
        void Assign(const std::string& id, const TSmartPtrType<Object>& var) override;
        TSmartPtrType<Object> Find(const std::string& id, bool searchParent) override;
        TSmartPtrType<ScopeLike> GetOuter() const override;
    };
    
    class DynamicObject : public Object, public ScopeLike
    {
    protected:
        TSmartPtrType<ScopeLike> _outer;
        TSmartPtrType<DynamicObjectCallScope> _callScope;
        std::unordered_map<std::string,TSmartPtrType<Object>> _properties;

        void OnRefSet() override;
    public:
        explicit DynamicObject(const TSmartPtrType<ScopeLike>& scope);
        
        virtual void Set(const std::string& key, const TSmartPtrType<Object>& val);

        virtual void Set(const TSmartPtrType<Object>& key, const TSmartPtrType<Object>& val);

        virtual TSmartPtrType<Object> Get(const std::string& key);

        virtual TSmartPtrType<Object> Get(const TSmartPtrType<Object>& key);

        bool Has(const std::string& id, bool searchParent = true) const override;

        void Assign(const std::string& id, const TSmartPtrType<Object>& var) override;
        void Create(const std::string& id, const TSmartPtrType<Object>& var) override;
        TSmartPtrType<Object> Find(const std::string& id, bool searchParent = true) override;

        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        EScopeType GetScopeType() const override;


        virtual TSmartPtrType<DynamicObjectCallScope> CreateCallScope();
        EObjectType GetType() const override;

        std::string ToString() const override;
        bool ToBoolean() const override;

        template<typename T>
        using TNativeDynamicMemberFunction = TSmartPtrType<Object>(T::*)(TSmartPtrType<FunctionScope>&);

        template<typename T>
        using TNativeDynamicMemberFunctionConst = TSmartPtrType<Object>(T::*)(const TSmartPtrType<FunctionScope>&);
        
        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::string>& args,TNativeDynamicMemberFunction<T> func);

        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::string>& args,TNativeDynamicMemberFunctionConst<T> func);

        
        void AddLambda(const std::string& name,const std::vector<std::string>& args,const std::function<TSmartPtrType<Object>(TSmartPtrType<FunctionScope>&)>& func);

        TSmartPtrType<ScopeLike> GetOuter() const override;
        
    };

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance,const std::vector<std::string>& args,
        TNativeDynamicMemberFunction<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(_callScope, name, args,std::bind(func,instance,std::placeholders::_1),false));
    }

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance, const std::vector<std::string>& args,
        TNativeDynamicMemberFunctionConst<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(_callScope, name, args,std::bind(func,instance,std::placeholders::_1),false));
    }

    class DynamicObjectReference : public Reference
    {
        std::string _id;
        TSmartPtrType<DynamicObject> _obj;
    public:
        DynamicObjectReference(const std::string& id,const TSmartPtrType<DynamicObject>& obj,const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<Object>& val);
        void Set(const TSmartPtrType<Object>& val) override;
    };


    TSmartPtrType<DynamicObject> makeDynamic(const TSmartPtrType<ScopeLike>& scope);
    
    TSmartPtrType<OneLayerScopeProxy> makeOneLayerScopeProxy(const TSmartPtrType<ScopeLike>& scope);
}
