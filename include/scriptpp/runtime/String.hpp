#pragma once
#include "DynamicObject.hpp"
#include "Object.hpp"

namespace spp::runtime
{
    class String : public DynamicObject
    {
        std::string _str;
    public:
        void Init() override;
        String(const std::string&str);
        EObjectType GetType() const override;
        std::string ToString(const std::shared_ptr<ScopeLike>& scope) const override;
        bool ToBoolean(const std::shared_ptr<ScopeLike>& scope) const override;
        bool Equal(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) const override;
        std::shared_ptr<Object> Add(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Multiply(const std::shared_ptr<Object>& other, const std::shared_ptr<ScopeLike>& scope) override;
        std::shared_ptr<Object> Split(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Size(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Trim(const std::shared_ptr<FunctionScope>& fnScope);
        std::shared_ptr<Object> Get(const std::shared_ptr<Object>& key) const override;
        void Set(const std::shared_ptr<Object>& key, const std::shared_ptr<Object>& val) override;
        
    };

    std::shared_ptr<String> makeString(const std::string& str);
}
