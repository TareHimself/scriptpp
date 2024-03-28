#include "vscript/frontend/Function.hpp"

#include <stdexcept>

#include "vscript/utils.hpp"
#include "vscript/frontend/Error.hpp"
#include "vscript/frontend/eval.hpp"
#include "vscript/frontend/List.hpp"
#include "vscript/frontend/Null.hpp"

namespace vs::frontend
{

    FunctionScope::FunctionScope(const Ref<Function>& fn, const TSmartPtrType<ScopeLike>& calledFrom,
        const TSmartPtrType<ScopeLike>& scope, const std::vector<std::string>& argNames,
        const std::vector<TSmartPtrType<Object>>& args) : Scope(scope)
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

    TSmartPtrType<Object> FunctionScope::Find(const std::string& id, bool searchParent)
    {
        if(id == ARGUMENTS_KEY)
        {
            if(!Scope::Has(ARGUMENTS_KEY,false))
            {
                Create(ARGUMENTS_KEY,makeList(_args));
            }

            return makeReferenceWithId(id,this->ToRef().Reserve().Cast<FunctionScope>(),Scope::Find(ARGUMENTS_KEY,false));
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

    Ref<Function> FunctionScope::GetFunction() const
    {
        return _fn;
    }

    std::vector<TSmartPtrType<Object>> FunctionScope::GetArgs() const
    {
        return _args;
    }

    TSmartPtrType<ScopeLike> FunctionScope::GetCallerScope() const
    {
        return _callerScope;
    }

    TSmartPtrType<FunctionScope> makeFunctionScope(const Ref<Function>& fn,const TSmartPtrType<ScopeLike>& callerScope,const TSmartPtrType<ScopeLike>& parent,
                                                   const std::vector<std::string>& argNames, const std::vector<TSmartPtrType<Object>>& args)
    {
        return manage<FunctionScope>(fn,callerScope,parent,argNames,args);
    }
    

    Function::Function(const TSmartPtrType<ScopeLike>& scope, const std::string& name, const std::vector<std::string>& args)
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

    TSmartPtrType<Object> Function::Call(const TSmartPtrType<ScopeLike>& callerScope,const std::vector<TSmartPtrType<Object>>& args )
    {
        std::vector<TSmartPtrType<Object>> callArgs = args;
        
        while(callArgs.size() < _args.size())
        {
            callArgs.emplace_back(makeNull());
        }
        
        auto fnScope =  makeFunctionScope(this->ToRef().Cast<Function>(),callerScope,_scope,_args,callArgs);
        return HandleCall(fnScope);
    }
    
    RuntimeFunction::RuntimeFunction(const TSmartPtrType<ScopeLike>& scope,
        const std::shared_ptr<backend::FunctionNode>& function) : Function(scope,function->name,function->args)
    {
        _function = function;
    }

    TSmartPtrType<Object> RuntimeFunction::HandleCall(TSmartPtrType<FunctionScope>& scope)
    {
        return runScope(_function->body,scope);
    }

    NativeFunction::NativeFunction(const TSmartPtrType<ScopeLike>& scope, const std::string& name,
                                   const std::vector<std::string>& args, const NativeFunctionType& func) : Function(scope,name,args)
    {
        _func = func;
    }

    TSmartPtrType<Object> NativeFunction::HandleCall(TSmartPtrType<FunctionScope>& scope)
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

    TSmartPtrType<RuntimeFunction> makeRuntimeFunction(const TSmartPtrType<ScopeLike>& scope,
                                                       const std::shared_ptr<backend::FunctionNode>& function, bool addToScope)
    {
        auto fn = manage<RuntimeFunction>(scope,function);
        if(scope.IsValid() && addToScope)
        {
            scope->Assign(function->name,fn);
        }
        return fn;
    }
    
    TSmartPtrType<NativeFunction> makeNativeFunction(const TSmartPtrType<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& args,
        const NativeFunctionType& nativeFunction,bool addToScope)
    {
        auto fn = manage<NativeFunction>(scope,name,args,nativeFunction);
        if(scope.IsValid() && addToScope)
        {
            scope->Assign(name,fn);
        }
        return fn;
    }
}
