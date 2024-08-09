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
                                                   const std::vector<std::string>& argNames, const std::vector<std::shared_ptr<Object>>& args)
    {
        return makeObject<FunctionScope>(fn,callerScope,parent,argNames,args);
    }
    

    Function::Function(const std::shared_ptr<ScopeLike>& declarationScope, const std::string& name, const std::vector<std::string>& args)
    {
        _declarationScope = declarationScope;
        _name = name;
        _args = args;
    }

    EObjectType Function::GetType() const
    {
        return OT_Function;
    }

    bool Function::ToBoolean(const std::shared_ptr<ScopeLike>& scope) const
    {
        return Object::ToBoolean(scope);
    }

    std::string Function::ToString(const std::shared_ptr<ScopeLike>& scope) const
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
        const auto myRef = cast<Function>(this->GetRef());
        auto fnScope =  makeFunctionScope(myRef,callerScope ? callerScope : makeCallScope({"<native>",0,0},myRef,{}),_declarationScope,_args,callArgs);
        fnScope->Create(FunctionScope::ARGUMENTS_KEY,makeList(args));
        return HandleCall(fnScope);
    }

    bool Function::IsCallable() const
    {
        return true;
    }

    RuntimeFunction::RuntimeFunction(const std::shared_ptr<ScopeLike>& scope,
                                     const std::shared_ptr<frontend::FunctionNode>& function) : Function(scope,function->name,function->args)
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
