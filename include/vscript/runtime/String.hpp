#pragma once
#include "Object.hpp"

namespace vs::runtime
{
    class String : public Object
    {
        std::string _str;
    public:
        String(const std::string&str);
        EObjectType GetType() const override;
        std::string ToString() override;
        bool ToBoolean() const override;
        TSmartPtrType<Object> Add(const TSmartPtrType<Object>& other) override;
        TSmartPtrType<Object> Multiply(const TSmartPtrType<Object>& other) override;
    };

    TSmartPtrType<String> makeString(const std::string& str);
}
