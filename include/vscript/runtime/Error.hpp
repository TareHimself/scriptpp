#pragma once
#include "List.hpp"
#include "Object.hpp"
#include "Scope.hpp"
#include "String.hpp"
#include <optional>
namespace vs::runtime
{


    class Error : public DynamicObject
    {
        
        std::shared_ptr<List> _callstack;
        std::shared_ptr<Object> _data;
        public:
        Error(const std::shared_ptr<ScopeLike>& scope,const std::string& data,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});
        Error(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& data,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});

        virtual void GenerateCallStack(const std::shared_ptr<ScopeLike>& scope,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});

        std::string ToString(const std::shared_ptr<ScopeLike>& scope = {}) const override;

        
    };

    std::shared_ptr<Error> makeError(const std::shared_ptr<ScopeLike>& scope,const std::string& data,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});
    std::shared_ptr<Error> makeError(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& data,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});
}
