#include "scriptpp/runtime/Function.hpp"

#include <stdexcept>

#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/Dictionary.hpp"
#include "scriptpp/runtime/Exception.hpp"
#include "scriptpp/runtime/eval.hpp"
#include "scriptpp/runtime/List.hpp"
#include "scriptpp/runtime/Null.hpp"

namespace spp::runtime
{

    FunctionScope::FunctionScope(const std::weak_ptr<Function>& fn,const std::shared_ptr<ScopeLike>& callScope,const std::shared_ptr<ScopeLike>& declarationScope,const std::vector<std::shared_ptr<frontend::ParameterNode>>& parameters,const std::unordered_map<std::string,std::shared_ptr<Object>>& args,const std::vector<std::shared_ptr<Object>>& positionalArgs): Scope(declarationScope)
    {
        _fn = fn;
        _callerScope = callScope;
        _arguments = args;
        _positionalArguments = positionalArgs;
        
        for (auto i = 0; i < parameters.size() && i < positionalArgs.size(); i++)
        {
            _arguments.insert_or_assign(std::to_string(i),_positionalArguments.at(i));
            _arguments.insert_or_assign(parameters.at(i)->name,_positionalArguments.at(i));
        } 
    }

    EScopeType FunctionScope::GetScopeType() const
    {
        return ST_Function;
    }

    std::shared_ptr<Object> FunctionScope::Find(const std::string& id, bool searchParent) const
    {
        if(_arguments.contains(id))
        {
            return _arguments.at(id);
        }
        return Scope::Find(id);
    }
    
    std::shared_ptr<Object> FunctionScope::FindArgument(const std::string& id,bool required)
    {
        if(!_arguments.contains(id)) throw makeException({},"Missing required argument : " + id);

        return _arguments.at(id);
    }

    std::shared_ptr<Object> FunctionScope::GetArgument(const uint32_t& index)
    {
        if(index >= _positionalArguments.size()) return makeNull();

        return _positionalArguments.at(index);
    }

    std::string FunctionScope::ARGUMENTS_KEY = "__args__";
    std::string FunctionScope::NAMED_ARGUMENTS_KEY = "__nargs__";

    std::string FunctionScope::THIS_KEY = "this";

    std::weak_ptr<Function> FunctionScope::GetFunction() const
    {
        return _fn;
    }

    std::unordered_map<std::string,std::shared_ptr<Object>> FunctionScope::GetNamedArgs() const
    {
        return _arguments;
    }

    std::vector<std::shared_ptr<Object>> FunctionScope::GetPositionalArgs() const
    {
        return _positionalArguments;
    }

    std::shared_ptr<ScopeLike> FunctionScope::GetCallerScope() const
    {
        return _callerScope;
    }

    std::shared_ptr<FunctionScope> makeFunctionScope(const std::weak_ptr<Function>& fn,
        const std::shared_ptr<ScopeLike>& callScope, const std::shared_ptr<ScopeLike>& declarationScope,
        const std::vector<std::shared_ptr<frontend::ParameterNode>>& parameters,
        const std::unordered_map<std::string, std::shared_ptr<Object>>& args,
        const std::vector<std::shared_ptr<Object>>& positionalArgs)
    {
        return makeObject<FunctionScope>(fn,callScope,declarationScope,parameters,args,positionalArgs); 
    }

    Function::Function(const std::shared_ptr<ScopeLike>& declarationScope, const std::string& name, const std::vector<std::shared_ptr<frontend::ParameterNode>>& params)
    {
        _declarationScope = declarationScope;
        _name = name;
        _params = params;
    }

    Function::Function(const std::shared_ptr<ScopeLike>& declarationScope, const std::string& name,
        const std::vector<std::string>& params)
    {
        _declarationScope = declarationScope;
        _name = name;
        _params.reserve(params.size());
        for (auto &param : params)
        {
            _params.push_back(std::make_shared<frontend::ParameterNode>(frontend::TokenDebugInfo{},param));
        }
    }

    EObjectType Function::GetType() const
    {
        return EObjectType::Function;
    }

    bool Function::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return Object::ToBoolean(scope);
    }

    std::string Function::ToString(const std::shared_ptr<ScopeLike>& scope) const
    {
        std::vector<std::string> strParams{};
        
        strParams.reserve(_params.size());
        
        for (auto &param : _params)
        {
            strParams.push_back(param->name);
        }
        
        return "fn " + _name +  + "(" + join(strParams,",") + ")";
    }

    std::shared_ptr<Object> Function::Call(const std::vector<std::shared_ptr<Object>>& positionalArgs, const std::unordered_map<std::string, std::shared_ptr<Object>>& namedArgs,const std::shared_ptr<ScopeLike>& callScope)
    {
        const auto myRef = cast<Function>(this->GetRef());
        auto fnScope =  makeFunctionScope(myRef,callScope ? callScope : makeCallScope(),_declarationScope,_params,namedArgs,positionalArgs);
        fnScope->Create(FunctionScope::ARGUMENTS_KEY,makeList(positionalArgs));
        fnScope->Create(FunctionScope::NAMED_ARGUMENTS_KEY,makeDictionary(namedArgs));
        fnScope->Create(FunctionScope::THIS_KEY,GetOwner());
        return HandleCall(fnScope);
    }

    std::shared_ptr<ScopeLike> Function::GetDeclarationScope() const
    {
        return _declarationScope;
    }

    bool Function::IsCallable() const
    {
        return true;
    }

    std::string Function::GetName() const
    {
        return _name;
    }

    std::vector<std::shared_ptr<frontend::ParameterNode>> Function::GetParameters() const
    {
        return _params;
    }

    void Function::SetOwner(const std::shared_ptr<Object>& ref)
    {
        _owner = ref;
    }

    std::shared_ptr<Object> Function::GetOwner() const
    {
        return _owner.lock();
    }

    RuntimeFunction::RuntimeFunction(const std::shared_ptr<ScopeLike>& scope,
                                     const std::shared_ptr<frontend::FunctionNode>& function) : Function(scope,function->name,function->params)
    {
        _function = function;
    }

    std::shared_ptr<Object> RuntimeFunction::HandleCall(std::shared_ptr<FunctionScope>& scope)
    {
        return runScope(_function->body,scope);
    }

    std::shared_ptr<Function> RuntimeFunction::Clone()
    {
        auto result = makeRuntimeFunction(GetDeclarationScope(),_function,false);
        result->SetOwner(GetOwner());
        return result;
    }

    NativeFunction::NativeFunction(const std::shared_ptr<ScopeLike>& scope, const std::string& name,
                                   const std::vector<std::string>& params, const NativeFunctionType& func) : Function(scope,name,params)
    {
        _func = func;
    }

    NativeFunction::NativeFunction(const std::shared_ptr<ScopeLike>& scope, const std::string& name,
        const std::vector<std::shared_ptr<frontend::ParameterNode>>& params, const NativeFunctionType& func) : Function(scope,name,params)
    {
        _func = func;
    }

    std::shared_ptr<Object> NativeFunction::HandleCall(std::shared_ptr<FunctionScope>& scope)
    {
        try
        {
            return _func(scope);
        }
        catch (std::exception& e)
        {
            throw makeException(scope,e.what());
        }
    }

    std::shared_ptr<Function> NativeFunction::Clone()
    {
        auto result = makeNativeFunction(GetDeclarationScope(),GetName(),GetParameters(),_func,false);
        result->SetOwner(GetOwner());
        return result;
    }

    std::shared_ptr<RuntimeFunction> makeRuntimeFunction(const std::shared_ptr<ScopeLike>& scope,
                                                         const std::shared_ptr<frontend::FunctionNode>& function, bool addToScope)
    {
        auto fn = makeObject<RuntimeFunction>(scope,function);
        if(scope && addToScope)
        {
            scope->Assign(function->name,fn);
        }
        return fn;
    }
    
    std::shared_ptr<NativeFunction> makeNativeFunction(const std::shared_ptr<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& params,
        const NativeFunctionType& nativeFunction,bool addToScope)
    {
        auto fn = makeObject<NativeFunction>(scope,name,params,nativeFunction);
        if(scope && addToScope)
        {
            scope->Assign(name,fn);
        }
        return fn;
    }

    std::shared_ptr<NativeFunction> makeNativeFunction(const std::shared_ptr<ScopeLike>& scope, const std::string& name,
        const std::vector<std::shared_ptr<frontend::ParameterNode>>& params, const NativeFunctionType& nativeFunction,
        bool addToScope)
    {
        auto fn = makeObject<NativeFunction>(scope,name,params,nativeFunction);
        if(scope && addToScope)
        {
            scope->Assign(name,fn);
        }
        return fn;
    }
}
