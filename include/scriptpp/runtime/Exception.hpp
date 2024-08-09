#pragma once
#include "List.hpp"
#include "Object.hpp"
#include "Scope.hpp"
#include "String.hpp"
#include <optional>
namespace spp::runtime
{


    class Exception : public DynamicObject
    {
        std::shared_ptr<List> _callstack;
        std::shared_ptr<Object> _data;
        public:
        Exception(const std::shared_ptr<ScopeLike>& scope,const std::string& data,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});
        Exception(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& data,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});

        virtual void GenerateCallStack(const std::shared_ptr<ScopeLike>& scope,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});

        std::string ToString(const std::shared_ptr<ScopeLike>& scope = {}) const override;
    };

    struct ExceptionContainer : std::runtime_error
    {
        std::shared_ptr<Exception> exception{};
        ExceptionContainer(const std::shared_ptr<Exception>& inException);
    };
    

    ExceptionContainer makeException(const std::shared_ptr<ScopeLike>& scope,const std::string& data,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});
    ExceptionContainer makeException(const std::shared_ptr<ScopeLike>& scope,const std::shared_ptr<Object>& data,const std::optional<frontend::TokenDebugInfo>& debugInfo = {});
    
}
