#include "vscript/runtime/Function.hpp"

#include <stdexcept>

#include "vscript/utils.hpp"
#include "vscript/runtime/eval.hpp"
#include "vscript/runtime/Null.hpp"

namespace vs::runtime
{
    FunctionScope::FunctionScope(const TSmartPtrType<ScopeLike>& scope, const std::vector<std::string>& argNames,
        const std::vector<TSmartPtrType<Object>>& args) : Scope(scope)
    {
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

    TSmartPtrType<Object>& FunctionScope::FindRef(const std::string& id,bool searchParent)
    {
        if(_argIndexes.contains(id))
        {
            if(_args.size() > _argIndexes[id])
            {
                return _args[_argIndexes[id]];
            }
        }

        return Scope::FindRef(id);
    }

    TSmartPtrType<Object> FunctionScope::Find(const std::string& id,bool searchParent)
    {
        
        if(_argIndexes.contains(id))
        {
            if(_args.size() > _argIndexes[id])
            {
                return _args[_argIndexes[id]];
            }
        }

        return Scope::Find(id);
    }

    std::vector<TSmartPtrType<Object>> FunctionScope::GetArgs() const
    {
        return _args;
    }

    TSmartPtrType<FunctionScope> makeFunctionScope(const TSmartPtrType<ScopeLike>& parent,
        const std::vector<std::string>& argNames, const std::vector<TSmartPtrType<Object>>& args)
    {
        return manage<FunctionScope>(parent,argNames,args);
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

    std::string Function::ToString()
    {
        return "fn " + _name +  + "(" + join(_args,",") + ")";
    }

    TSmartPtrType<Object> Function::Call(const std::vector<TSmartPtrType<Object>>& args )
    {
        std::vector<TSmartPtrType<Object>> callArgs = args;
        
        while(callArgs.size() < _args.size())
        {
            callArgs.emplace_back(makeNull());
        }
        
        auto fnScope =  makeFunctionScope(_scope,_args,callArgs);
        return HandleCall(fnScope);
    }



    RuntimeFunction::RuntimeFunction(const TSmartPtrType<ScopeLike>& scope, const std::string& name,
        const std::vector<std::string>& args, const std::shared_ptr<parser::ScopeNode>& body): Function(scope,name,args)
    {
        _body = body;
    }

    TSmartPtrType<Object> RuntimeFunction::HandleCall(TSmartPtrType<FunctionScope>& scope)
    {
        return runScope(_body,scope);
    }
    
    NativeFunction::NativeFunction(const TSmartPtrType<ScopeLike>& scope, const std::string& name,
        const std::vector<std::string>& args, const NativeFunctionType& func) : Function(scope,name,args)
    {
        _func = func;
    }

    TSmartPtrType<Object> NativeFunction::HandleCall(TSmartPtrType<FunctionScope>& scope)
    {
        return _func(scope);
    }
    

    TSmartPtrType<RuntimeFunction> makeRuntimeFunction(const TSmartPtrType<ScopeLike>& scope,const std::string& name, const std::vector<std::string>& args,
                                                const std::shared_ptr<parser::ScopeNode>& body,bool addToScope)
    {
        auto fn = manage<RuntimeFunction>(scope,name,args,body);
        if(scope.IsValid() && addToScope)
        {
            scope->Assign(name,fn);
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
