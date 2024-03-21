#pragma once
#include <functional>
#include <memory>
#include <vector>

#include "Object.hpp"
#include "Scope.hpp"
#include "vscript/parser/ast.hpp"

namespace vs::runtime
{
    class FunctionScope : public Scope
    {
        std::vector<TSmartPtrType<Object>> _args;
        std::unordered_map<std::string,uint32_t> _argIndexes;
        TSmartPtrType<Object> _result;
    public:
        FunctionScope(const TSmartPtrType<ScopeLike>& scope,const std::vector<std::string>& argNames,const std::vector<TSmartPtrType<Object>>& args);

        EScopeType GetScopeType() const override;

        TSmartPtrType<Object> Find(const std::string& id, bool searchParent = true) override;
        TSmartPtrType<Object>& FindRef(const std::string& id, bool searchParent = true) override;

        virtual std::vector<TSmartPtrType<Object>> GetArgs() const;

    };

    TSmartPtrType<FunctionScope> makeFunctionScope(const TSmartPtrType<ScopeLike>& parent,const std::vector<std::string>& argNames,const std::vector<TSmartPtrType<Object>>& args);
    
    class Function : public Object
    {
        std::string _name;
        std::vector<std::string> _args;
        TSmartPtrType<ScopeLike> _scope;
    public:
        Function(const TSmartPtrType<ScopeLike>& scope,const std::string& name,const std::vector<std::string>& args);
        EObjectType GetType() const override;
        bool ToBoolean() const override;
        std::string ToString() override;

        virtual TSmartPtrType<Object> Call(const std::vector<TSmartPtrType<Object>>& args = {});

        template<typename ...TArgs, typename = std::enable_if_t<((std::is_convertible_v<TArgs, TSmartPtrType<Object>>) && ...)>>
        TSmartPtrType<Object> Call(TArgs... args);
        
        virtual TSmartPtrType<Object> HandleCall(TSmartPtrType<FunctionScope>& scope) = 0;
    };

    template <typename ... TArgs, typename>
    TSmartPtrType<Object> Function::Call(TArgs... args)
    {
        std::vector<TSmartPtrType<Object>> vecArgs;
        (vecArgs.push_back(args),...);
        return Call(vecArgs);
    }

    class RuntimeFunction : public Function
    {
        std::shared_ptr<parser::ScopeNode> _body;
    public:
        RuntimeFunction(const TSmartPtrType<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& args,const std::shared_ptr<parser::ScopeNode>& body);

        TSmartPtrType<Object> HandleCall(TSmartPtrType<FunctionScope>& scope) override;
    };

    using NativeFunctionType = std::function<TSmartPtrType<Object>(TSmartPtrType<FunctionScope>&)>;
    
    class NativeFunction : public Function
    {
        NativeFunctionType _func;
    public:
        NativeFunction(const TSmartPtrType<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& args,const NativeFunctionType& func);

        TSmartPtrType<Object> HandleCall(TSmartPtrType<FunctionScope>& scope) override;
    };
    
    TSmartPtrType<RuntimeFunction> makeRuntimeFunction(const TSmartPtrType<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& args,const std::shared_ptr<parser::ScopeNode>& body,bool addToScope = true);

    TSmartPtrType<NativeFunction> makeNativeFunction(const TSmartPtrType<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& args,const NativeFunctionType& nativeFunction,bool addToScope = true);
}
