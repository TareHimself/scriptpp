#include "vscript/frontend/Error.hpp"

#include "vscript/utils.hpp"

namespace vs::frontend
{
    Error::Error(const std::shared_ptr<ScopeLike>& scope, const std::string& data,const std::optional<backend::TokenDebugInfo>& debugInfo) : DynamicObject({})
    {
        _callstack = makeList({});
        _data = makeString(data);
        Error::GenerateCallStack(scope,debugInfo);
    }

    Error::Error(const std::shared_ptr<ScopeLike>& scope, const std::shared_ptr<Object>& data,const std::optional<backend::TokenDebugInfo>& debugInfo) : DynamicObject({})
    {
        _callstack = makeList({});
        _data = data;
        Error::GenerateCallStack(scope,debugInfo);
    }

    void Error::GenerateCallStack(const std::shared_ptr<ScopeLike>& scope,const std::optional<backend::TokenDebugInfo>& debugInfo)
    {
        auto &callstackVec = _callstack->GetNative();
        
        if(debugInfo.has_value())
        {
            callstackVec.emplace_back(makeString(debugInfo.value().ToString()));
        }
        
        auto next = scope;
        
        while(next)
        {
            if(next->GetScopeType() == ST_Proxy)
            {
                next = cast<ScopeLikeProxy>(next)->GetActual();
                continue;
            }
            
            if(const auto asCallScope = cast<CallScope>(next))
            {
                callstackVec.emplace_back(makeString(asCallScope->ToString()));
                if(asCallScope->GetScopeType() == ST_Function)
                {
                    if(const auto asFnScope = cast<FunctionScope>(asCallScope->GetActual()))
                    {
                        next = asFnScope->GetCallerScope();
                        continue;
                    }
                }
            }
            
            if(next->GetScopeType() == ST_Function)
            {
                if(const auto asFnScope = cast<FunctionScope>(next))
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

        for(const auto &callstackVec = _callstack->GetNative(); auto &call : callstackVec)
        {
            err += "\n\t" + call->ToString();
        }
        
        return err;
    }

    std::shared_ptr<Error> makeError(const std::shared_ptr<ScopeLike>& scope, const std::string& data,const std::optional<backend::TokenDebugInfo>& debugInfo)
    {
        return makeObject<Error>(scope,data,debugInfo);
    }

    std::shared_ptr<Error> makeError(const std::shared_ptr<ScopeLike>& scope, const std::shared_ptr<Object>& data,const std::optional<backend::TokenDebugInfo>& debugInfo)
    {
        return makeObject<Error>(scope,data,debugInfo);
    }
}
