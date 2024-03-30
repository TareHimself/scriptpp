#include "vscript/frontend/Function.hpp"

#include <stdexcept>

#include "vscript/utils.hpp"
#include "vscript/frontend/Error.hpp"
#include "vscript/frontend/eval.hpp"
#include "vscript/frontend/List.hpp"
#include "vscript/frontend/Null.hpp"

namespace vs::frontend
{

    FunctionScope::FunctionScope(const std::weak_ptr<Function>& fn, const std::shared_ptr<ScopeLike>& calledFrom,
        const std::shared_ptr<ScopeLike>& scope, const std::vector<std::string>& argNames,
        const std::vector<std::shared_ptr<Object>>& args) : Scope(scope)
    {
        _fn = fn;
        _callerScope = calledFrom;
        _args = args;
        for(auto i = 0; i < argNames.size(); i++)
        {
            _argIndexes.insert({argNames[i],i});
        }
    }

    EScopeType FunctionScope::GetScopeType() const
    {
        return ST_Function;
    }

    std::string FunctionScope::ARGUMENTS_KEY = "__args__";

    std::shared_ptr<Object> FunctionScope::Find(const std::string& id, bool searchParent)
    {
        if(id == ARGUMENTS_KEY)
        {
            if(!Scope::Has(ARGUMENTS_KEY,false))
            {
                Create(ARGUMENTS_KEY,makeList(_args));
            }

            return makeReferenceWithId(id,cast<FunctionScope>(this->GetRef()),Scope::Find(ARGUMENTS_KEY,false));
        }
        
        if(_argIndexes.contains(id))
        {
            if(_args.size() > _argIndexes[id])
            {
                return _args[_argIndexes[id]];
            }
        }

        return Scope::Find(id);
    }

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
                                                   const std::vector<std::string>& argNames, const std::vector<std::shared_ptr<Object>>& args)
    {
        return makeObject<FunctionScope>(fn,callerScope,parent,argNames,args);
    }
    

    Function::Function(const std::shared_ptr<ScopeLike>& scope, const std::string& name, const std::vector<std::string>& args)
    {
        _scope = scope;
        _name = name;
        _args = args;
    }

    EObjectType Function::GetType() const
    {
        return OT_Function;
    }

    bool Function::ToBoolean() const
    {
        return Object::ToBoolean();
    }

    std::string Function::ToString() const
    {
        return "fn " + _name +  + "(" + join(_args,",") + ")";
    }

    std::shared_ptr<Object> Function::Call(const std::shared_ptr<ScopeLike>& callerScope,const std::vector<std::shared_ptr<Object>>& args )
    {
        std::vector<std::shared_ptr<Object>> callArgs = args;
        
        while(callArgs.size() < _args.size())
        {
            callArgs.emplace_back(makeNull());
        }
        
        auto fnScope =  makeFunctionScope(cast<Function>(this->GetRef()),callerScope,_scope,_args,callArgs);
        return HandleCall(fnScope);
    }
    
    RuntimeFunction::RuntimeFunction(const std::shared_ptr<ScopeLike>& scope,
        const std::shared_ptr<backend::FunctionNode>& function) : Function(scope,function->name,function->args)
    {
        _function = function;
    }

    std::shared_ptr<Object> RuntimeFunction::HandleCall(std::shared_ptr<FunctionScope>& scope)
    {
        return runScope(_function->body,scope);
    }

    NativeFunction::NativeFunction(const std::shared_ptr<ScopeLike>& scope, const std::string& name,
                                   const std::vector<std::string>& args, const NativeFunctionType& func) : Function(scope,name,args)
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
            throw makeError(scope,e.what());
        }
    }

    std::shared_ptr<RuntimeFunction> makeRuntimeFunction(const std::shared_ptr<ScopeLike>& scope,
                                                       const std::shared_ptr<backend::FunctionNode>& function, bool addToScope)
    {
        auto fn = makeObject<RuntimeFunction>(scope,function);
        if(scope && addToScope)
        {
            scope->Assign(function->name,fn);
        }
        return fn;
    }
    
    std::shared_ptr<NativeFunction> makeNativeFunction(const std::shared_ptr<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& args,
        const NativeFunctionType& nativeFunction,bool addToScope)
    {
        auto fn = makeObject<NativeFunction>(scope,name,args,nativeFunction);
        if(scope && addToScope)
        {
            scope->Assign(name,fn);
        }
        return fn;
    }
}
