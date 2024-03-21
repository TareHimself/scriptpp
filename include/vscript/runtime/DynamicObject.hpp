#pragma once
#include "Function.hpp"
#include "Object.hpp"
#include "Scope.hpp"

namespace vs::runtime
{

    struct ReservedDynamicFunctions
    {
        inline static std::string CALL = "__call__";
        inline static std::string CONSTRUCTOR = "__ctor__";
    };

    class OneLayerScopeProxy : public ScopeLike
    {
        TSmartPtrType<ScopeLike> _outer;
    public:
        OneLayerScopeProxy(const TSmartPtrType<ScopeLike>& scope);

        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        EScopeType GetScopeType() const override;
        bool Has(const std::string& id, bool searchParent) const override;
        TSmartPtrType<Object> Create(const std::string& id, const TSmartPtrType<Object>& var) override;
        TSmartPtrType<Object> Assign(const std::string& id, const TSmartPtrType<Object>& var) override;
        TSmartPtrType<Object> Find(const std::string& id, bool searchParent) override;
        TSmartPtrType<Object>& FindRef(const std::string& id, bool searchParent) override;
    };
    
    class DynamicObject : public Object
    {
        TSmartPtrType<ScopeLike> _scope;
    public:

        DynamicObject(const TSmartPtrType<ScopeLike>& scope);
            
        virtual TSmartPtrType<Object>& GetRef(const std::string& key);

        virtual TSmartPtrType<Object>& GetRef(const TSmartPtrType<Object>& key);

        virtual TSmartPtrType<Object> Get(const std::string& key);

        virtual TSmartPtrType<Object> Get(const TSmartPtrType<Object>& key);

        virtual void Set(const std::string& key, const TSmartPtrType<Object>& val);

        virtual void Set(const TSmartPtrType<Object>& key, const TSmartPtrType<Object>& val);

        virtual bool Has(const std::string& key) const;

        virtual TSmartPtrType<ScopeLike> GetScope() const;

        EObjectType GetType() const override;

        std::string ToString() override;
        bool ToBoolean() const override;

        template<typename T>
        using TNativeDynamicMemberFunction = TSmartPtrType<Object>(T::*)(TSmartPtrType<FunctionScope>&);

        template<typename T>
        using TNativeDynamicMemberFunctionConst = TSmartPtrType<Object>(T::*)(const TSmartPtrType<FunctionScope>&);
        
        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::string>& args,TNativeDynamicMemberFunction<T> func);

        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::string>& args,TNativeDynamicMemberFunctionConst<T> func);

        // template<typename T>
        // void AddNativeMemberFunction(const std::string& name,T * ,const std::vector<std::string>& args,TNativeDynamicMemberFunctionC<T> func);
    };

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance,const std::vector<std::string>& args,
        TNativeDynamicMemberFunction<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(DynamicObject::GetScope(), name, args,std::bind(func,instance,std::placeholders::_1),false));
    }

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance, const std::vector<std::string>& args,
        TNativeDynamicMemberFunctionConst<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(DynamicObject::GetScope(), name, args,std::bind(func,instance,std::placeholders::_1),false));
    }


    TSmartPtrType<DynamicObject> makeDynamic(const TSmartPtrType<ScopeLike>& scope);

    TSmartPtrType<OneLayerScopeProxy> makeOneLayerScopeProxy(const TSmartPtrType<ScopeLike>& scope);
}
