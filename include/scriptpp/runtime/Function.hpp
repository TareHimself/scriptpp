#pragma once
#include <functional>
#include <memory>
#include <vector>

#include "Object.hpp"
#include "Scope.hpp"
#include "scriptpp/frontend/parser.hpp"

namespace spp::runtime
{
    class Function;
    
    class FunctionScope : public Scope
    {
        std::vector<std::shared_ptr<Object>> _args{};
        std::unordered_map<std::string,uint32_t> _argIndexes{};
        std::shared_ptr<Object> _result{};
        std::weak_ptr<Function> _fn{};
        std::shared_ptr<ScopeLike> _callerScope{};
        std::shared_ptr<ScopeLike> _ownerScope{};
        
    public:

        static std::string ARGUMENTS_KEY;
        
        FunctionScope(const std::weak_ptr<Function>& fn,const std::shared_ptr<ScopeLike>& calledFrom,const std::shared_ptr<ScopeLike>& scope,const std::vector<std::shared_ptr<frontend::ParameterNode>>& parameters,const std::vector<std::shared_ptr<Object>>& args);

        EScopeType GetScopeType() const override;

        std::shared_ptr<Object> Find(const std::string& id, bool searchParent = true) const override;

        std::shared_ptr<Object> FindArg(const std::string& id);

        std::shared_ptr<Object> GetArg(const uint32_t& index);

        virtual std::weak_ptr<Function> GetFunction() const;

        virtual std::vector<std::shared_ptr<Object>> GetArgs() const;

        std::shared_ptr<ScopeLike> GetCallerScope() const;

    };

    std::shared_ptr<FunctionScope> makeFunctionScope(const std::weak_ptr<Function>& fn,const std::shared_ptr<ScopeLike>& callerScope,const std::shared_ptr<ScopeLike>& parent,const std::vector<std::string>& argNames,const std::vector<std::shared_ptr<Object>>& args);
    
    class Function : public Object
    {
        std::string _name;
         std::vector<std::shared_ptr<frontend::ParameterNode>> _params;
        std::shared_ptr<ScopeLike> _declarationScope;
    public:
        Function(const std::shared_ptr<ScopeLike>& declarationScope,const std::string& name,const  std::vector<std::shared_ptr<frontend::ParameterNode>>& params);
        Function(const std::shared_ptr<ScopeLike>& declarationScope,const std::string& name,const  std::vector<std::string>& params);
        EObjectType GetType() const override;
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;
        std::string ToString(const std::shared_ptr<ScopeLike>& scope = {}) const override;

        virtual std::shared_ptr<Object> Call(const std::shared_ptr<ScopeLike>& callerScope,const std::vector<std::shared_ptr<Object>>& args = {});

        template<typename ...TArgs, typename = std::enable_if_t<((std::is_convertible_v<TArgs, std::shared_ptr<Object>>) && ...)>>
        std::shared_ptr<Object> Call(const std::shared_ptr<ScopeLike>& callerScope,TArgs... args);

        // virtual std::shared_ptr<Object> CallFromNative(const std::shared_ptr<ScopeLike>& callerScope,const std::vector<std::shared_ptr<Object>>& args = {});
        //
        // template<typename ...TArgs, typename = std::enable_if_t<((std::is_convertible_v<TArgs, std::shared_ptr<Object>>) && ...)>>
        // std::shared_ptr<Object> CallFromRuntime(const std::shared_ptr<ScopeLike>& callerScope,TArgs... args);
        
        virtual std::shared_ptr<Object> HandleCall(std::shared_ptr<FunctionScope>& scope) = 0;

        std::shared_ptr<ScopeLike> GetDeclarationScope() const;

        bool IsCallable() const override;
        
    };

    template <typename ... TArgs, typename>
    std::shared_ptr<Object> Function::Call(const std::shared_ptr<ScopeLike>& callerScope,TArgs... args)
    {
        std::vector<std::shared_ptr<Object>> vecArgs;
        (vecArgs.push_back(args),...);
        return Call(callerScope,vecArgs);
    }

    class RuntimeFunction : public Function
    {
        
        std::shared_ptr<frontend::FunctionNode> _function;
    public:
        RuntimeFunction(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<frontend::FunctionNode>& function);

        std::shared_ptr<Object> HandleCall(std::shared_ptr<FunctionScope>& scope) override;
    };

    using NativeFunctionType = std::function<std::shared_ptr<Object>(std::shared_ptr<FunctionScope>&)>;
    
    class NativeFunction : public Function
    {
        NativeFunctionType _func;
    public:
        NativeFunction(const std::shared_ptr<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& params,const NativeFunctionType& func);
        NativeFunction(const std::shared_ptr<ScopeLike>& scope,const std::string& name, const std::vector<std::shared_ptr<frontend::ParameterNode>>& params,const NativeFunctionType& func);

        std::shared_ptr<Object> HandleCall(std::shared_ptr<FunctionScope>& scope) override;
    };
    
    std::shared_ptr<RuntimeFunction> makeRuntimeFunction(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<frontend::FunctionNode>& function,bool addToScope = true);

    std::shared_ptr<NativeFunction> makeNativeFunction(const std::shared_ptr<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& params,const NativeFunctionType& nativeFunction,bool addToScope = true);
    std::shared_ptr<NativeFunction> makeNativeFunction(const std::shared_ptr<ScopeLike>& scope,const std::string& name, const std::vector<std::shared_ptr<frontend::ParameterNode>>& params,const NativeFunctionType& nativeFunction,bool addToScope = true);
}

