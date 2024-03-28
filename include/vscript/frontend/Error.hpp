#pragma once
#include "List.hpp"
#include "Object.hpp"
#include "Scope.hpp"
#include "String.hpp"
#include <optional>
namespace vs::frontend
{


    class Error : public DynamicObject
    {
        
        TSmartPtrType<List> _callstack;
        TSmartPtrType<Object> _data;
        public:
        Error(const TSmartPtrType<ScopeLike>& scope,const std::string& data,const std::optional<backend::TokenDebugInfo>& debugInfo = {});
        Error(const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<Object>& data,const std::optional<backend::TokenDebugInfo>& debugInfo = {});

        virtual void GenerateCallStack(const TSmartPtrType<ScopeLike>& scope,const std::optional<backend::TokenDebugInfo>& debugInfo = {});

        std::string ToString() const override;

        
    };

    TSmartPtrType<Error> makeError(const TSmartPtrType<ScopeLike>& scope,const std::string& data,const std::optional<backend::TokenDebugInfo>& debugInfo = {});
    TSmartPtrType<Error> makeError(const TSmartPtrType<ScopeLike>& scope,const TSmartPtrType<Object>& data,const std::optional<backend::TokenDebugInfo>& debugInfo = {});
}
