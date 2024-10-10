#pragma once
#include "Function.hpp"
#include "Object.hpp"
#include "Scope.hpp"

namespace spp::runtime
{

    class DynamicObject;
    
    struct ReservedDynamicFunctions
    {
        inline static std::string CALL = "__call__";
        inline static std::string GET = "__get__";
        inline static std::string SET = "__set__";
        inline static std::string CONSTRUCTOR = "__ctor__";
        inline static std::string ADD = "__add__"; 
        inline static std::string SUBTRACT = "__subtract__";
        inline static std::string DIVIDE = "__divide__";
        inline static std::string MULTIPLY = "__multiply__";
        inline static std::string MOD = "__mod__";
        inline static std::string TO_STRING = "__string__";
        inline static std::string TO_BOOLEAN = "__bool__";
        inline static std::string EQUAL = "__equal__";
        inline static std::string LESS = "__less__";
        inline static std::string GREATER = "__greater__";
        
        std::set<std::string> OVERRIDABLE_METHODS = {CALL,GET,SET,CONSTRUCTOR,ADD,SUBTRACT,DIVIDE,MULTIPLY,MOD,TO_STRING,TO_BOOLEAN,EQUAL,LESS,GREATER};
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

        virtual void Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val, const std::shared_ptr<ScopeLike>& scope);

        virtual std::shared_ptr<Object> Get(const std::string& key) const;

        virtual std::shared_ptr<Object> Get(const std::shared_ptr<Object>& key, const std::shared_ptr<ScopeLike>& scope) const;

        virtual bool HasOwn(const std::string& id) const;

        virtual bool HasOwn(const std::shared_ptr<Object>& key) const;

        bool Has(const std::string& id, bool searchParent = true) const override;

        void Assign(const std::string& id, const std::shared_ptr<Object>& var) override;
        void Create(const std::string& id, const std::shared_ptr<Object>& var) override;
        std::shared_ptr<Object> Find(const std::string& id, bool searchParent = true) const override;

        std::list<EScopeType> GetScopeStack() const override;
        bool HasScopeType(EScopeType type) const override;
        EScopeType GetScopeType() const override;
        EObjectType GetType() const override;

        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;

        template<typename T>
        using TNativeDynamicMemberFunction = std::shared_ptr<Object>(T::*)(std::shared_ptr<FunctionScope>&);

        template<typename T>
        using TNativeDynamicMemberFunctionConst = std::shared_ptr<Object>(T::*)(const std::shared_ptr<FunctionScope>&);
        
        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::string>& params,TNativeDynamicMemberFunction<T> func);

        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::string>& params,TNativeDynamicMemberFunctionConst<T> func);

        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::shared_ptr<frontend::ParameterNode>>& params,TNativeDynamicMemberFunction<T> func);

        template<typename T>
        void AddNativeMemberFunction(const std::string& name,T * instance,const std::vector<std::shared_ptr<frontend::ParameterNode>>& params,TNativeDynamicMemberFunctionConst<T> func);

        
        void AddLambda(const std::string& name,const std::vector<std::string>& args,const std::function<std::shared_ptr<Object>(std::shared_ptr<FunctionScope>&)>& func);

        std::shared_ptr<ScopeLike> GetOuter() const override;

        bool Equal(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const override;
        bool Less(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const override;
        bool Greater(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const override;
        std::shared_ptr<Object> Add(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Subtract(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Mod(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Divide(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Multiply(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;

        bool IsCallable() const override;

        size_t GetHashCode(const std::shared_ptr<ScopeLike>& scope) override;
    };

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance,const std::vector<std::string>& params,
        TNativeDynamicMemberFunction<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(_selfFunctionScope, name, params,std::bind(func,instance,std::placeholders::_1),false));
    }

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance, const std::vector<std::string>& params,
        TNativeDynamicMemberFunctionConst<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(_selfFunctionScope, name, params,std::bind(func,instance,std::placeholders::_1),false));
    }

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance,const std::vector<std::shared_ptr<frontend::ParameterNode>>& params,
        TNativeDynamicMemberFunction<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(_selfFunctionScope, name, params,std::bind(func,instance,std::placeholders::_1),false));
    }

    template <typename T>
    void DynamicObject::AddNativeMemberFunction(const std::string& name, T* instance, const std::vector<std::shared_ptr<frontend::ParameterNode>>& params,
        TNativeDynamicMemberFunctionConst<T> func)
    {
        DynamicObject::Set(name, makeNativeFunction(_selfFunctionScope, name, params,std::bind(func,instance,std::placeholders::_1),false));
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
