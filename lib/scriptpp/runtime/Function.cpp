#include "scriptpp/runtime/Function.hpp"

#include <stdexcept>

#include "scriptpp/utils.hpp"
#include "scriptpp/runtime/Exception.hpp"
#include "scriptpp/runtime/eval.hpp"
#include "scriptpp/runtime/List.hpp"
#include "scriptpp/runtime/Null.hpp"

namespace spp::runtime
{

    FunctionScope::FunctionScope(const std::weak_ptr<Function>& fn, const std::shared_ptr<ScopeLike>& calledFrom,
        const std::shared_ptr<ScopeLike>& scope, const std::vector<std::shared_ptr<frontend::ParameterNode>>& parameters,
        const std::vector<std::shared_ptr<Object>>& args) : Scope(scope)
    {
        _fn = fn;
        _callerScope = calledFrom;
        _args = args;
        
        for(auto i = 0; i < parameters.size(); i++)
        {
            _argIndexes.insert({parameters[i]->name,i});
        }
    }

    EScopeType FunctionScope::GetScopeType() const
    {
        return ST_Function;
    }

    std::shared_ptr<Object> FunctionScope::Find(const std::string& id, bool searchParent) const
    {
        if(id == ARGUMENTS_KEY)
        {
            if(!Scope::Has(ARGUMENTS_KEY,false))
            {
                
            }

            return makeReferenceWithId(id,cast<FunctionScope>(this->GetRef()),Scope::Find(ARGUMENTS_KEY,false));
        }
        
        if(_argIndexes.contains(id))
        {
            if(_args.size() > _argIndexes.at(id))
            {
                return _args[_argIndexes.at(id)];
            }
        }

        return Scope::Find(id);
    }
    
    std::shared_ptr<Object> FunctionScope::FindArg(const std::string& id)
    {
        if(_argIndexes.contains(id))
        {
            if(_args.size() > _argIndexes.at(id))
            {
                return _args[_argIndexes.at(id)];
            }
        }

        return makeNull();
    }

    std::shared_ptr<Object> FunctionScope::GetArg(const uint32_t& index)
    {
        if(index >= _args.size()) return makeNull();
        return _args[index];
    }

    std::string FunctionScope::ARGUMENTS_KEY = "__args__";

    std::weak_ptr<Function> FunctionScope::GetFunction() const
    {
        return _fn;
    }

    std::vector<std::shared_ptr<Object>> FunctionScope::GetArgs() const
    {
        return _args;
    }

    std::shared_ptr<ScopeLike> FunctionScope::GetCallerScope() const
    {
        return _callerScope;
    }

    std::shared_ptr<FunctionScope> makeFunctionScope(const std::weak_ptr<Function>& fn,const std::shared_ptr<ScopeLike>& callerScope,const std::shared_ptr<ScopeLike>& parent,
                                                   const std::vector<std::shared_ptr<frontend::ParameterNode>>& parameters, const std::vector<std::shared_ptr<Object>>& args)
    {
        return makeObject<FunctionScope>(fn,callerScope,parent,parameters,args);
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

    std::shared_ptr<Object> Function::Call(const std::shared_ptr<ScopeLike>& callerScope,const std::vector<std::shared_ptr<Object>>& args )
    {

        
        std::vector<std::shared_ptr<Object>> callArgs = args;
        
        // Resolve default arguments and pass null for missing arguments
        if(callArgs.size() != _params.size() && callArgs.size() < _params.size())
        {
            callArgs.reserve(_params.size());
            for(auto i = callArgs.size(); i < _params.size(); i++)
            {
                if(_params[i]->defaultValue)
                {
                    auto defaultVal = evalExpression(_params[i]->defaultValue,_declarationScope);
                    callArgs.push_back(resolveReference(defaultVal));
                }
                else
                {
                    callArgs.push_back(makeNull());
                }
                
            }
        }
        
        const auto myRef = cast<Function>(this->GetRef());
        auto fnScope =  makeFunctionScope(myRef,callerScope ? callerScope : makeCallScope({"<native>",0,0},myRef,{}),_declarationScope,_params,callArgs);
        fnScope->Create(FunctionScope::ARGUMENTS_KEY,makeList(args));
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

    RuntimeFunction::RuntimeFunction(const std::shared_ptr<ScopeLike>& scope,
                                     const std::shared_ptr<frontend::FunctionNode>& function) : Function(scope,function->name,function->params)
    {
        _function = function;
    }

    std::shared_ptr<Object> RuntimeFunction::HandleCall(std::shared_ptr<FunctionScope>& scope)
    {
        return runScope(_function->body,scope);
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
