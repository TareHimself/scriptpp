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
    
    class DynamicObject : public Object, public ScopeLike
    {
    protected:
        std::shared_ptr<ScopeLike> _outer;
        std::unordered_map<std::string,std::shared_ptr<Object>> _properties;
        std::shared_ptr<ScopeLike> _selfFunctionScope;
    public:

        void Init() override;
        explicit DynamicObject(const std::shared_ptr<ScopeLike>& scope);
        
        virtual void Set(const std::string& key, const std::shared_ptr<Object>& val);

        virtual void Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val);

        virtual std::shared_ptr<Object> Get(const std::string& key);

        virtual std::shared_ptr<Object> Get(const std::shared_ptr<Object>& key);

        bool Has(const std::string& id, bool searchParent = true) const override;

        void Assign(const std::string& id, const std::shared_ptr<Object>& var) override;
        void Create(const std::string& id, const std::shared_ptr<Object>& var) override;
        std::shared_ptr<Object> Find(const std::string& id, bool searchParent = true) override;

        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        EScopeType GetScopeType() const override;
        EObjectType GetType() const override;

        std::string ToString() const override;
        bool ToBoolean() const override;

        template<typename T>
        using TNativeDynamicMemberFunction = std::shared_ptr<Object>(T::*)(std::shared_ptr<FunctionScope>&);

        template<typename T>
        using TNativeDynamicMemberFunctionConst = std::shared_ptr<Object>(T::*)(const std::shared_ptr<FunctionScope>&);
        
        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::string>& args,TNativeDynamicMemberFunction<T> func);

        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::string>& args,TNativeDynamicMemberFunctionConst<T> func);

        
        void AddLambda(const std::string& name,const std::vector<std::string>& args,const std::function<std::shared_ptr<Object>(std::shared_ptr<FunctionScope>&)>& func);

        std::shared_ptr<ScopeLike> GetOuter() const override;
        
    };

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance,const std::vector<std::string>& args,
        TNativeDynamicMemberFunction<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(_selfFunctionScope, name, args,std::bind(func,instance,std::placeholders::_1),false));
    }

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance, const std::vector<std::string>& args,
        TNativeDynamicMemberFunctionConst<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(_selfFunctionScope, name, args,std::bind(func,instance,std::placeholders::_1),false));
    }

    class DynamicObjectReference : public Reference
    {
        std::string _id;
        std::shared_ptr<DynamicObject> _obj;
    public:
        DynamicObjectReference(const std::string& id,const std::shared_ptr<DynamicObject>& obj,const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& val);
        void Set(const std::shared_ptr<Object>& val) override;
    };


    std::shared_ptr<DynamicObject> makeDynamic(const std::shared_ptr<ScopeLike>& scope);
    
    
}
