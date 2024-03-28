#include "vscript/frontend/Error.hpp"

namespace vs::frontend
{
    Error::Error(const TSmartPtrType<ScopeLike>& scope, const std::string& data,const std::optional<backend::TokenDebugInfo>& debugInfo) : DynamicObject({})
    {
        _callstack = makeList({});
        _data = makeString(data);
        Error::GenerateCallStack(scope,debugInfo);
    }

    Error::Error(const TSmartPtrType<ScopeLike>& scope, const TSmartPtrType<Object>& data,const std::optional<backend::TokenDebugInfo>& debugInfo) : DynamicObject({})
    {
        _callstack = makeList({});
        _data = data;
        Error::GenerateCallStack(scope,debugInfo);
    }

    void Error::GenerateCallStack(const TSmartPtrType<ScopeLike>& scope,const std::optional<backend::TokenDebugInfo>& debugInfo)
    {
        auto &callstackVec = _callstack->GetNative();
        
        if(debugInfo.has_value())
        {
            callstackVec.emplace_back(makeString(debugInfo.value().ToString()));
        }
        
        auto next = scope;
        
        while(next.IsValid())
        {
            if(auto asCallScope = next.Cast<CallScopeLayer>(); asCallScope.IsValid())
            {
                callstackVec.emplace_back(makeString(asCallScope->ToString()));
                if(asCallScope->GetScopeType() == ST_Function)
                {
                    if(auto asFnScope = asCallScope->GetScope().Cast<FunctionScope>(); asFnScope.IsValid())
                    {
                        next = asFnScope->GetCallerScope();
                        continue;
                    }
                }
            }
            
            if(next->GetScopeType() == ST_Function)
            {
                if(auto asFnScope = next.Cast<FunctionScope>(); asFnScope.IsValid())
                {
                    next = asFnScope->GetCallerScope();
                    continue;
                }
            }

            

            next = next->GetOuter();
        }

        Set("stack",_callstack);
        Set("data",_data);
    }

    std::string Error::ToString() const
    {
        std::string err;
        err += "Error: " + _data->ToString();

        for(auto &callstackVec = _callstack->GetNative(); auto &call : callstackVec)
        {
            err += "\n\t" + call->ToString();
        }
        
        return err;
    }

    TSmartPtrType<Error> makeError(const TSmartPtrType<ScopeLike>& scope, const std::string& data,const std::optional<backend::TokenDebugInfo>& debugInfo)
    {
        return manage<Error>(scope,data,debugInfo);
    }

    TSmartPtrType<Error> makeError(const TSmartPtrType<ScopeLike>& scope, const TSmartPtrType<Object>& data,const std::optional<backend::TokenDebugInfo>& debugInfo)
    {
        return manage<Error>(scope,data,debugInfo);
    }
}
